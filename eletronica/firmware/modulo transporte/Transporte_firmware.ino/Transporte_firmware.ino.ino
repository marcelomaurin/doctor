/*------------------------------------------------------------------------
  Example sketch for Adafruit Thermal Printer library for Arduino.
  Demonstrates a few text styles & layouts, bitmap printing, etc.

  IMPORTANT: DECLARATIONS DIFFER FROM PRIOR VERSIONS OF THIS LIBRARY.
  This is to support newer & more board types, especially ones that don't
  support SoftwareSerial (e.g. Arduino Due).  You can pass any Stream
  (e.g. Serial1) to the printer constructor.  See notes below.
  ------------------------------------------------------------------------*/
#include <Stepper.h> 
#include <Ethernet.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <stdio.h>
#include <string.h>
#include "DHT.h"
#include "Adafruit_Thermal.h"
#include "adalogo.h"
#include "adaqrcode.h"

//Flags de Controle
bool OperflgLeitura = false;
byte flgEscape = 0; //Controla Escape
byte flgEnter = 0; //Controla Escape
byte flgTempo = 0; //Controle de Tempo e Temperatura

//Buffer do Teclado
char customKey;

//Ethernet flags
bool flgRServer = false; //Controle de retorno Server
bool flgRClient = false; //Controle de retorno do cliente
bool flgEthernetErro = true; //Verifica se houve erro de start

float h ;
float t; 
float f ;

int flgFIM01;
int flgFIM02;
int flgFIM03;
int flgFIM04;
int flgFIM05;
int flgFIM06;

int FOPEPag = 0; //Controle de Pagina da Funcao
int FOPEMAXPag = 2; //Tres Paginas para Funcoes
//int FOPERMAXPag = 2; //tres Paginas para Operacoes

int FUNCPag = 0; //Controle de Pagina da Funcao
int FUNCMAXPag = 2; //Tres Paginas para Funcoes
//int OPERMAXPag = 2; //tres Paginas para Operacoes

//*************** Descricao do Produto *********************
//Versao do produto 
char Versao = '0';  //Controle de Versao do Firmware
char Release = '1'; //Controle Revisao do Firmware
char Produto[20] = { "Doctor - Betha"};

float contciclo = 0; //Contador de Ciclos de Repeticao
const float maxciclo  = 9000;

//****************** SD Card ******************
Sd2Card card;
SdVolume volume;
File root; //Pasta root
File farquivo; //Arquivo de gravacao
char ArquivoTrabalho[40]; //Arquivo de trabalho a ser carregado

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

IPAddress ip(192, 168, 1, 177);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

EthernetServer server(23);


EthernetClient client;
EthernetClient Serverclient;
EthernetServer serverport(23);


//http://www.aranacorp.com/en/control-a-stepper-motor-with-arduino/
double stepsPerRevolution = 2048;

// Here's the new syntax when using SoftwareSerial (e.g. Arduino Uno) ----
// If using hardware serial instead, comment out or remove these lines:

#define MAXSPEED 17

/*Definicoes de variaveis*/
bool flgBeep = false; //Aviso sonoro de temperatura

// note names and their corresponding half-periods
//'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C'};
byte names[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956};

//Buffer
char Buffer[40]; //Buffer de Teclado


/* **************************************************
 * Pinout 
 ****************************************************/
#include "SoftwareSerial.h"
#define TX_PIN 6 //Arduino transmit  YELLOW WIRE  labeled RX on printer
#define RX_PIN 5 //Arduino receive   GREEN WIRE   labeled TX on printer
#define GND_PIN 7 //Arduino receive   GREEN WIRE   labeled TX on printer

#define IN1_01  52 //Motor Passo01
#define IN2_01  51 //Motor Passo01
#define IN3_01  50 //Motor Passo01
#define IN4_01  49 //Motor Passo01

#define speakerPin A13 //Speaker
#define DHT22Pin A9 //DHT22
#define pinSD  4

#define pinFIM01 22 /*Fim de curso*/
#define pinFIM02 23
#define pinFIM03 24
#define pinFIM04 25
#define pinFIM05 26
#define pinFIM06 27



//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define DHTTYPE DHT11   // DHT 11

DHT dht(DHT22Pin, DHTTYPE);

//SoftwareSerial NextionSerial(pinNextionRX, pinNextionTX); // RX, TX

Stepper Passo01(stepsPerRevolution, IN1_01, IN3_01, IN2_01, IN4_01);  // Pin inversion to make the library work


/*Funcoes predefinidas*/
void MovePasso01_Dir();
void MovePasso01_Esq();
void ImprimeEtiqueta();
void Beep();
void Sound(char serByte);
void WellComeConsole();
void WellComeBluetooth();
void KeyCMD();//Comando de entrada 
float Reset();
void LstDir(File dir, int numTabs);
void MAN();
void LOAD(File root, char sMSG1[20]); //Carrega a aplicação para o SD
void LOADLoop(File root, char Info[40]);
void LOADLeituras();
void LOADAnalisa();
void LOADKeyCMD();
void LOADRemoveTemp();
void LOADCancela();
void LOADBloco(char Info[40]);
void LOADFIMARQUIVO();
void LOADCOPYTEMP();
float Run(String Arquivo);
void MOPERACAO();
void FOPEImprime();
void MCONFIG();
void Le_DHT22();
void NextionShow(String info);
void NextionFieldText(String field,String value);
void NextionMensage(String info);
void RetConsole();

SoftwareSerial mySerial(RX_PIN, TX_PIN); // Declare SoftwareSerial obj first
Adafruit_Thermal printer(&mySerial);     // Pass addr to printer constructor
// Then see setup() function regarding serial & printer begin() calls.

// Here's the syntax for hardware serial (e.g. Arduino Due) --------------
// Un-comment the following line if using hardware serial:

//Adafruit_Thermal printer(&Serial1);      // Or Serial2, Serial3, etc.

// -----------------------------------------------------------------------

void Start_FIMDECURSO()
{
  Serial.println("configurando Fim de curso");
  pinMode(pinFIM01, OUTPUT);
  pinMode(pinFIM02, OUTPUT);
  pinMode(pinFIM03, OUTPUT);
  pinMode(pinFIM04, OUTPUT);
  pinMode(pinFIM05, OUTPUT);
  pinMode(pinFIM06, OUTPUT);
}

void Start_Serial()
{
  Serial.begin(9600); 
}

void Start_Bluetooth()
{
  Serial3.begin(9600);
  WellComeBluetooth();
}

void Start_Motor01()
{
  Serial.println("Motor de passo sendo iniciado");
  Passo01.setSpeed(MAXSPEED);  
}

void Start_DHT22()
{
  Serial.println("Sensor de temperatura sendo iniciado");
  dht.begin();
}

//Inicializa Speaker
void Speak_Start()
{
  Serial.println("Speaker Starting...");
  pinMode(speakerPin, OUTPUT);
  Serial.println("Speaker OK");
}

void Start_SD()
{
  Serial.print("Inicializando SD Card.");
  SD.begin(pinSD);
  if (!card.init(SPI_HALF_SPEED, pinSD)) {
    Serial.println("initialization failed");
    NextionMensage("initialization failed");
    return;
  }
  else
  {
    Serial.println("SD Card Iniciado");
  }
}

void Start_Nextion()
{
  Serial.println("Ativando Nextion Display");
  // set the data rate for the SoftwareSerial port
  Serial2.begin(9600);
  NextionShow("Splah"); //Chamando tela splash

}

void Start_Printer()
{
  // This line is for compatibility with the Adafruit IotP project pack,
  // which uses pin 7 as a spare grounding point.  You only need this if
  // wired up the same way (w/3-pin header into pins 5/6/7):
  //pinMode(GND_PIN, OUTPUT); digitalWrite(GND_PIN, LOW);

  // NOTE: SOME PRINTERS NEED 9600 BAUD instead of 19200, check test page.
  mySerial.begin(9600);  // Initialize SoftwareSerial
  //Serial1.begin(19200); // Use this instead if using hardware serial
  printer.begin();        // Init printer (same regardless of serial type)
}


void WellComeBluetooth()
{

  Serial3.println(" ");
  Serial3.println("Bem vindo");
  Serial3.println("DOCTOR - Automacao Laboratorial");
  Serial3.println("Projeto Doctor");
  Serial3.print("Versao:");
  Serial3.print(Versao);
  Serial3.print(".");
  Serial3.println(Release);
  Serial3.print(F("Humidity: "));
  Serial3.print(h);
  Serial3.print(F("%  Temperature: "));
  Serial3.print(t);
  Serial3.print(F("°C "));
  Serial3.println(" ");
  Serial3.println("MAN para ajuda");
  NextionShow("Menu");
}

void WellComeConsole()
{

  Serial.println(" ");
  Serial.println("Bem vindo");
  Serial.println("DOCTOR - Automacao Laboratorial");
  Serial.println("Projeto Doctor");
  Serial.print("Versao:");
  Serial.print(Versao);
  Serial.print(".");
  Serial.println(Release);
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.println("MAN para ajuda");
  //Serial.print(f);

  Serial.println(" ");
  NextionShow("Menu");
}

//Inicia o Servidor
void Service_Start()
{
  //CLS();
 // Imprime(2, "Ethernet");
 // Imprime(3, "Detectando IP...");

  Serial.println("Ethernet start");
  Serial.println("Detectando IP...");
  if (Ethernet.begin(mac) == 0)
  {
    delay(1000);
    Serial.println("Ethernet Erro:");
    Serial.println("Nao detectou IP");
    //Imprime(2, "Ethernet Erro:");
    //Imprime(3, "Nao detectou IP");
    
    // initialize the ethernet device not using DHCP:
    //Ethernet.begin(mac, ip, gateway, subnet);
    Ethernet.begin(mac, ip);
    flgEthernetErro = true;
  }
  else
  {
    //CLS();
    delay(1000);
    IPAddress myIPAddress = Ethernet.localIP();
    Serial.print("IP:");
    Serial.println( Ethernet.localIP());
    //sprintf(Info,"IP:%s.%s.%s.%s",myIPAddress[0],myIPAddress[1],myIPAddress[2],myIPAddress[3]);
    // Imprime(0,Info);
    // sprintf(Info,"Port:23");
    // Imprime(1,Info);
    //delay(2000);
    // start listening for clients
    delay(1000);
    Serial.println("Inicializando Serviço Porta 23 ...");
    serverport.begin();
    Serial.println("Servico Iniciado.");
    flgEthernetErro = false; //Ativa Ethernet
  }
}

/*Rotina que posiciona o equipamento no local correto*/
void Calibracao()
{
  Serial.println("Iniciando Calibração");
  NextionShow("CALIB"); //Chamando tela calibração
  
  
}

void setup() {
 
  Start_Serial();
  Serial.println("Starting modules...");
  Speak_Start();
  Beep(); 
  Start_Nextion();
  Start_Bluetooth();
  Start_Motor01();
  Start_Printer();

  Start_DHT22();
  Start_FIMDECURSO();

  Start_SD();
  Service_Start();

  // Font options
  //MovePasso01();
  //ImprimeEtiqueta();
  Le_DHT22();

  Calibracao(); /*Calibração do equipamento*/
  WellComeConsole(); 
  Reset();  
}



//Toca um som
void Sound(char serByte)
{
  int count = 0;

  for (count = 0; count <= 8; count++) { // look for the note
    if (names[count] == serByte) {  // ahh, found it
      for ( int i = 0; i < 20; i++ ) { // play it for 50 cycles
        digitalWrite(speakerPin, HIGH);
        delayMicroseconds(tones[count]);
        digitalWrite(speakerPin, LOW);
        delayMicroseconds(tones[count]);
      }
    }
  }
}



void ImprimeEtiqueta() {
   // Test inverse on & off
  //printer.inverseOn();
  printer.setSize('M');        // Set type size, accepts 'S', 'M', 'L'
  //printer.justify('C');
  printer.println(F("   Etiqueta Doctor   "));
  printer.inverseOff();
  printer.setSize('S');
  printer.println(F("An.Solic: Cultura Bacteria."));
  // Barcode examples:
  // CODE39 is the most common alphanumeric barcode:
  printer.printBarcode("00234", CODE39);
  printer.setBarcodeHeight(100);
  // Print UPC line on product barcodes:
  //printer.printBarcode("00234", UPC_A);
  printer.sleep();      // Tell printer to sleep

  printer.wake();       // MUST wake() before printing again, even if reset
  printer.setDefault(); // Restore printer to defaults
  printer.println(' ');  
}

void MovePasso01_Dir()
{
    // 1 rotation counterclockwise:  
    Serial.println("counterclockwise");    
    Passo01.step(stepsPerRevolution);  
    //delay(1000); 
}

void MovePasso01_Esq()
{
  // 1 rotation clockwise: 
  Serial.println("clockwise");  
  Passo01.step(-stepsPerRevolution); 
  //delay(1000); 
}

//Carrega a aplicação para o SD
void LOAD(File root, char sMSG1[20])
{
  //Imprime(1, "Carregando APP...");

  //Copia arquivo
  sprintf(ArquivoTrabalho, "%s", sMSG1);
  //Imprime(2, ArquivoTrabalho);

  //Realiza operação de Loop
  LOADLoop(root, ArquivoTrabalho);

}

//Analisa Entrada de Informacoes de Entrada
void LOADAnalisa()
{
  LOADKeyCMD(); //Analisa o que esta na entrada do buffer de teclado
  //LOADKeyCMDBluetooth(); //Analisa o que esta na entrada do buffer do bluetooth
}


void LOADLoop(File root, char Info[40])
{
  //Tenta carregar arquivo temp.out para ser temporario
  File farquivo = SD.open("temp.out", FILE_WRITE);
  //Limpa os buffer
  if (!farquivo)
  {
    //Imprime(1, "Erro SD");
   // Imprime(2, "Nao pode abrir SD");
  }
  else
  {
    flgEscape = false; //Sai quando verdadeiro
    //Loop
    while (!flgEscape)
    {
      LOADLeituras();
      //Realiza analise das informações encontradas
      LOADAnalisa();
      //arquivo.println("Leitura Potenciometro: ");
    }
  }
  farquivo.close();
  //Apaga o Arquivo Temporario
  LOADRemoveTemp();
}

//Realiza Leitura do arquivo
void LOADLeituras()
{
  //Inicializa Temperatura
  //Le_Temperatura();
  //Le_Teclado();
  Le_Serial();
  //Le_Bluetooth();
  Chk_Beep();
}


//Apaga o arquivo temporario
void LOADCancela()
{
  LOADRemoveTemp();
  flgEscape = true;
}

//Apaga Arquivo Temporario
void LOADRemoveTemp()
{
  SD.remove("temp.out");
}

void LOADBloco(char Info[40])
{
  farquivo.print(Info);
}

//Finaliza o arquivo temporario copiando para arquivo final
void LOADFIMARQUIVO()
{
  //Imprime(1, "Carregando Arquivo  ");
  //Imprime(2, "Copiando Arquivo    ");
  //Copia o temporario para o definitivo
  LOADCOPYTEMP();
  LOADRemoveTemp();
  flgEscape = true;
}

void LOADCOPYTEMP()
{
  // open the file named ourfile.txt
  File FOrigem = SD.open("temp.out");
  File FDestino = SD.open(ArquivoTrabalho, FILE_WRITE);

  // if the file is available, read the file
  if (FOrigem)
  {
    while (FOrigem.available())
    {
      //Serial.write();
      FDestino.write(FOrigem.read());
    }
    FOrigem.close();
    FDestino.close();
  }
}

//Comando de entrada do Teclado
void LOADKeyCMD()
{
  bool resp = false;

  //incluir busca /n

  if (strchr (Buffer, '\n') != 0)
  {
    Serial.print("Comando:");
    Serial.println(Buffer);

    //Funcao Cancela dados
    if (strcmp( Buffer, "CANCELA;\n") == 0)
    {
      LOADCancela();
      resp = true;
    }

    //Funcao Carrega Bloco
    if (strcmp( Buffer, "BLOCO=") == 0)
    {
      //FUNCMDefault();
      char Info[40];
      sprintf(Info, "0000000000000000");
      LOADBloco(Info);
      resp = true;
    }

    //Funcao Carrega Bloco
    if (strcmp( Buffer, "FIMARQUIVO;\n") == 0)
    {
      LOADFIMARQUIVO();
      resp = true;
    }

    if (resp == false)
    {
      Serial.print("Comando:");
      Serial.print(Buffer);
      Serial.println("Cmd n reconhecido");
      //Imprime(3, "Cmd n reconhecido");
      //strcpy(BufferKeypad,'\0');
      memset(Buffer, 0, sizeof(Buffer));
    }
    else
    {
      //strcpy(BufferKeypad,'\0');
      memset(Buffer, 0, sizeof(Buffer));
    }
    //RetConsole(); //Retorno de Comando de Console
  }
}

//Funcoes FUNC
void FOPEImprime()
{
  //CLS();
  switch (FUNCPag)
  {
    case 0:
      {
        //Imprime(0, "********SETUP********");
        //Imprime(1, "0 - STATUS");
        //Imprime(2, "1 - SD CARD");
        //Imprime(3, "PgUp,PgDn,Nro sel");
        break;
      }
    case 1:
      {
        //Imprime(0, "********SETUP********");
        //Imprime(1, "2 - REDE");
        //Imprime(2, "3 - BLUETOOTH");
        //Imprime(3, "PgUp,PgDn, Nro sel");
        break;
      }
  } //end switch
}

//Menu de Funcoes (loop)
void MCONFIG()
{
  //Define modo Pagina inicio Funcoes
  FUNCPag = 0;
  flgEscape = 0; //reseta Escape
  //FUNCImprime();
  while (1)
  {
    //FUNCLeituras();
    if (flgEscape != 0)
    {
      break;
    }

  }
  //FuncLeituras();
  //Devolve o controle para o principal

  //RetConsole();
}

//Ativa Flag de Escape
void FOPEMDefault()
{
  flgEscape = 255;
}


//Comando de entrada do Teclado
void FOPEKeyCMD()
{
  bool resp = false;

  //incluir busca /n

  if (strchr (Buffer, '\n') != 0)
  {
    Serial.print("Comando:");
    Serial.println(Buffer);

    //Funcao MDEFAULT
    if (strcmp( Buffer, "MDEFAULT\n") == 0)
    {
      FOPEMDefault();
      resp = true;
      flgBeep = false;
    }

    //Funcao MAN
    if (strcmp( Buffer, "MAN\n") == 0)
    {
      //FOPEMan();
      resp = true;
    }

    //Funcao PAGEUP
    if (strcmp( Buffer, "PAGEUP\n") == 0)
    {
      //FOPEPAGEUp();
      resp = true;
    }


    //Funcao Status
    if (strcmp( Buffer, "0\n") == 0)
    {
      //FUNCCALIBRAR();
      //WellCome();
      resp = true;
    }

    //Funcao Digitacao Manual
    if (strcmp( Buffer, "1\n") == 0)
    {
      //FUNCDIGMANUAL();
      //root = card.open("/");
      //LstDir(root, 0);
      //FuncInfoSD();
      resp = true;
    }

    
   
    //Bluettoth
    if (strcmp( Buffer, "4\n") == 0)
    {
      //FUNCAJUSTEFINO();
      resp = true;
    }

    //Funcao PAGEDOWN
    if (strcmp( Buffer, "PAGEDOWN\n") == 0)
    {
      //FOPEPAGEDown();
      resp = true;
    }


    if (resp == false)
    {
      Serial.print("Comando:");
      Serial.print(Buffer);
      Serial.println("Cmd n reconhecido");
      //Imprime(3, "Cmd n reconhecido");
      //strcpy(BufferKeypad,'\0');
      memset(Buffer, 0, sizeof(Buffer));
    }
    else
    {
      //strcpy(BufferKeypad,'\0');
      memset(Buffer, 0, sizeof(Buffer));
    }
    //RetConsole(); //Retorno de Comando de Console
  }
}


/*Menu de Configuracoes*/
//Leituras do Modo Funcoes
void FOPELeituras()
{
  //FOPELe_Teclado(); //Porque as funcoes do teclado mudam durante o programa
  Le_Serial(); //Le dados do Serial
  //Precisa implementar o tempo em ciclos para leitura
  //Le_Temperatura();
  FOPEKeyCMD();
}


/*Menu de Operacao*/
//MOPERACAO
//Menu de Funcoes (loop)
void MOPERACAO()
{
  //Define modo Pagina inicio Funcoes
  FOPEPag = 0;
  flgEscape = 0; //reseta Escape
  FOPEImprime();
  while (1)
  {
    FOPELeituras();
    if (flgEscape != 0)
    {
      break;
    }

  }
  FOPELeituras();
  //Devolve o controle para o principal

  //RetConsole();
}


//Emula Beep de Alarme
void Beep()
{
  
    Serial.println('Beep');
    Sound('a');
    Sound('c');
    //delay(500);
    //Imprime(2, "BEEP - ESC p/ Parar ");
  
}

//Emula Beep de Alarme
void Chk_Beep()
{
  if ( flgBeep == true)
  {
    Serial.println('Beep');
    Sound('a');
    Sound('c');
    //delay(500);
    //Imprime(2, "BEEP - ESC p/ Parar ");
  }
}


void NextionShow(String info)
{
  char strFF = 0xFF;
  Serial2.print("page "+info+String(strFF)+String(strFF)+String(strFF));  
  delay(100);  
}

void NextionFieldText(String field,String value)
{
char strFF = 0xFF;  
  String cmd;
  
  cmd = field+".txt=\""+value+"\""+String(strFF)+String(strFF)+String(strFF);
  Serial.print(cmd);  
  Serial2.print(cmd);  
}

void NextionMensage(String info)
{
  char strFF = 0xFF;
  Serial2.print("page MSG"+String(strFF)+String(strFF)+String(strFF));  
  delay(100);
  String cmd;
  
  cmd = "MSGtxt.txt=\""+info+"\""+String(strFF)+String(strFF)+String(strFF);
  Serial.print(cmd);  
  Serial2.print(cmd);
}


//Reseta todas as entradas para o valor padrão
float Reset()
{
  Serial.println('Reset rodou');
  flgBeep = false;
  /* incluir sub comandos de inicialização*/

  

  //Zera buffer teclado
  //memset(Linha,0,sizeof(Linha));
  WellComeConsole();

}


//Imprime Diretorios
void LstDir(File dir, int numTabs)
{
  Serial.println("Lista de aplicacoes do SD");
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      LstDir(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}

void MAN()
{
  //Device Console
  Serial.println("Projeto Doctor");
  Serial.print("Versao:");
  Serial.print(Versao);
  Serial.print(".");
  Serial.println(Release);
  Serial.println("INICIO - Inicia bloco");
  Serial.println("FIM - Finaliza bloco");
  Serial.println("BEEP - Emite som");
  Serial.println("BEEPMSG - Mensagem com SOM");
  Serial.println("SOUND - Toca som");
  Serial.println("ESC - ESCAPE");
  Serial.println("MAN - AUXILIO MANUAL");
  Serial.println("LSTDIR - LISTA DIRETORIO");
  Serial.println("LOAD - CARREGA ARQUIVO");
  Serial.println("RUN - RODA SCRIPT");
  Serial.println("MENSAGEM - Mostra mensagem");
  Serial.println("RESET - RESETA O AMBIENTE");
  Serial.println("MOPERACAO - MODO OPERACIONAL");
  Serial.println("MOPERACAO - MODO CONFIG");
  Serial.println(" ");
}


//RUN - Roda Programa de Scripts
float Run(String Arquivo)
{
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  Serial.println('Inicializando RUN');
  Serial.print('Carregando:');
  Serial.println(Arquivo);
  File dataFile = SD.open(Arquivo, FILE_READ);
  char Linha[255];
  char Info;
  while (dataFile.available())
  {
    Info = dataFile.read();
    //Adicionando informação ao Info
    sprintf(Linha, "%s%c", Linha, Info);

    //Verifica se o ultimo foi enter ai analisa a linha
    if (Info == '\n')
    {
      Serial.print('Executando:');
      Serial.println(Linha);
      sprintf(Buffer, "%s", Linha);
      //Limpa Info
      memset(Linha, 0, sizeof(Linha));
    }
  }
  dataFile.close();
  // print to the serial port too:
  Serial.println('Finalizou Script');
}

//Comando de entrada do Teclado
void KeyCMD()
{
  bool resp = false;

  //incluir busca /n
  if (strchr (Buffer, '\n') != 0)
  {
    Serial.print("Comando:");
    Serial.println(Buffer);

  

    //Inicio
    if (strcmp( Buffer, "INICIO\n") == 0)
    {
      Serial.println("Inicio");
      Reset();
      resp = true;
    }

    //FIM
    if (strcmp( Buffer, "FIM\n") == 0)
    {
      Serial.println("Fim");
      Reset();
      resp = true;
    }

    //RESET
    if (strcmp( Buffer, "RESET\n") == 0)
    {
      Serial.println("Reset");
      Reset();
      resp = true;
    }

    //Beep
    if (strcmp( Buffer, "BEEP\n") == 0)
    {
      Serial.println("Beep");
      flgBeep = true;
      resp = true;
    }

    
    //SOUND
    if (strcmp( Buffer, "SOUND\n") == 0)
    {
      Serial.println("SOUND");
      //flgBeep = true;
      Sound('a');
      Sound('b');
      Sound('c');
      resp = true;
    }

    //Beep
    if (strcmp( Buffer, "ESC\n") == 0)
    {
      Serial.println("ESC");
      flgBeep = false;
      resp = true;
    }


    //LstDir - Lista o Diretorio
    if (strcmp( Buffer, "LSTDIR\n") == 0)
    {
      char sMSG1[16];
      strncpy(sMSG1, Buffer, 7);
      //Imprime(1, "LSTDIR           ");
      //Imprime(2, "                 ");
      //root = card.open(sMSG1);
      root = SD.open("/");
      LstDir(root, 0);

      resp = true;
    }

    //MAN
    if (strcmp( Buffer, "MAN\n") == 0)
    {
      //Serial.println(Temperatura);
      MAN();
      resp = true;
    }

    //Load - Carrega arquivo no SD
    if (strncmp( "LOAD=",Buffer, 5) == 0)
    {
      char sMSG1[16];
      //strncpy(sMSG1, BufferKeypad, 7);
      strncpy(sMSG1, &Buffer[6], strlen(Buffer) - 6);
      //Imprime(0, sMSG1);
      //root = card.open(sMSG1);
      root = SD.open("/");
     LOAD(root, sMSG1);

      resp = true;
    }


    if (strstr( Buffer, "MENSAGEM=") != 0)
    {
      char sMSG1[20];
      char sMSG2[20];
      Serial.println("Achou MENSAGEM");
      char *posequ = strstr( Buffer, '=');
      Serial.print("POSEQU=");
      Serial.println(posequ);
      if(posequ != 0)
      {
        memset(sMSG1,'\0',sizeof(sMSG1));
        strcpy(sMSG1, posequ);
        Serial.print("sMSG1=");
        Serial.println(sMSG1);
        NextionMensage(String(sMSG1));
        
        resp = true;
      }
    }

    //Run(String Arquivo)c
    //Roda o script
    if (strcmp( Buffer, "RUN(") == 0)
    {
      char sMSG1[20];
      char sMSG2[20];
      int posfim = strcmp( Buffer, ");");
      if (posfim == 0)
      {
        strncpy(sMSG1, &Buffer[5], posfim - 5);
        Run(sMSG1);
        //Imprime(1, sMSG1);
        //Imprime(2, sMSG1);
        resp = true;
      }
    }

 

    //BeepMsg
    if (strcmp( Buffer, "BEEPMSG=") == 0)
    {
      //Serial.print("Temperatura:");
      //Serial.print(Temperatura);
      Serial.println(";");
      resp = true;
    }


    //MOPERACAO
    if (strcmp( Buffer, "MOPERACAO\n") == 0)
    {
      MOPERACAO();
      resp = true;
    }

    //MCONFIG Menu de Setup
    if (strcmp( Buffer, "MCONFIG\n") == 0)
    {
      MCONFIG();
      resp = true;
    }
    //MDEFAULT
    if (strcmp( Buffer, "MDEFAULT\n") == 0)
    {
      //MCONFIG();
      Serial.println("Ja em Default");
      //Imprime(3, "Ja em Default");
      resp = true;
    }

   

    //Verifica se houve comando valido
    if (resp == false)
    {
      Serial.print("Comando:");
      Serial.println(Buffer);
      Serial.println("Cmd nao reconhecido");
      //Imprime(3, "Cmd n reconhecido");
      //strcpy(BufferKeypad,'\0');
      memset(Buffer, 0, sizeof(Buffer));
    }
    else
    {
      //strcpy(BufferKeypad,'\0');
      memset(Buffer, 0, sizeof(Buffer));
    }
  }

}

//Analisa Entrada de Informacoes de Entrada
void Analisa()
{
  KeyCMD(); //Analisa o que esta na entrada do buffer
}

//Le registro do Serial
void Le_Serial()
{
  char key;
  while (Serial.available() > 0)
  {
    key = Serial.read();

    if (key != 0)
    {
      Serial.print(key);
      //BufferKeypad += key;
      sprintf(Buffer, "%s%c", Buffer, key);
    }
  }
}

void Le_DHT22()
{
  
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
   h = dht.readHumidity();
  // Read temperature as Celsius (the default)
   t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
   f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    //Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
}

void Le_FimCurso()
{
  flgFIM01 = digitalRead(pinFIM01)==LOW?LOW:digitalRead(pinFIM01);
  flgFIM02 = digitalRead(pinFIM02)==LOW?LOW:digitalRead(pinFIM02);
  flgFIM03 = digitalRead(pinFIM03)==LOW?LOW:digitalRead(pinFIM03);
  flgFIM04 = digitalRead(pinFIM04)==LOW?LOW:digitalRead(pinFIM04);
  flgFIM05 = digitalRead(pinFIM05)==LOW?LOW:digitalRead(pinFIM05);
  flgFIM06 = digitalRead(pinFIM06)==LOW?LOW:digitalRead(pinFIM06);
  
}

void Leituras()
{
  //Le_Teclado();
  Le_Serial();
  Le_DHT22();

  Chk_Beep();
}

//Imprime Retorno de Console
void RetConsole()
{
  //Serial ÃƒÆ’Ã‚Â© padrao para todos os retornos
  Serial.println("$>");
}

void loop() {
  Leituras();
  Analisa();
  //Modulo de contagem de ciclos
  if (contciclo >= maxciclo)
  {
    contciclo = 0;
  }
  contciclo++;
  
}
