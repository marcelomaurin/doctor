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

//*************** Descricao do Produto *********************
//Versao do produto 
#define Versao  "0"  //Controle de Versao do Firmware
#define Release "2" //Controle Revisao do Firmware

#define MAXCICLO 90000
#define MAXSPEED 12


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

//Avancos dos motores de passo
int passoA;
int passoB;
int passoC;

/*Posicao de servos*/
long int posSERVA;
long int posSERVB;
long int posSERVC;



int flgFIMA1;
int flgFIMA2;
int flgFIMB1;
int flgFIMB2;
int flgFIMC1;
int flgFIMC2;

int lastflgFIMA1 = -1;
int lastflgFIMA2 = -1;
int lastflgFIMB1 = -1;
int lastflgFIMB2 = -1;
int lastflgFIMC1 = -1;
int lastflgFIMC2 = -1;

/*Posicao local do Modulo1*/
long LOCAL_DEFMOD1 = 0;
/*Posicao local do Modulo2*/
long LOCAL_DEFMOD2 = 0;
/*Posicao local do Modulo3*/
long LOCAL_DEFMOD3 = 0;
/*Posicao local do Modulo4*/
long LOCAL_DEFMOD4 = 0;

int FOPEPag = 0; //Controle de Pagina da Funcao
int FOPEMAXPag = 2; //Tres Paginas para Funcoes
//int FOPERMAXPag = 2; //tres Paginas para Operacoes

int FUNCPag = 0; //Controle de Pagina da Funcao
int FUNCMAXPag = 2; //Tres Paginas para Funcoes
//int OPERMAXPag = 2; //tres Paginas para Operacoes



char Produto[20] = { "Doctor - Betha"};

long long int contciclo = 0; //Contador de Ciclos de Repeticao


//****************** SD Card ******************
Sd2Card card;
SdVolume volume;
File root; //Pasta root
File farquivo; //Arquivo de gravacao
char ArquivoTrabalho[80]; //Arquivo de trabalho a ser carregado

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
char IP[40];



//http://www.aranacorp.com/en/control-a-stepper-motor-with-arduino/
double stepsPerRevolution = 2048;

// Here's the new syntax when using SoftwareSerial (e.g. Arduino Uno) ----
// If using hardware serial instead, comment out or remove these lines:



/*Definicoes de variaveis*/
bool flgBeep = false; //Aviso sonoro de temperatura

// note names and their corresponding half-periods
//'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C'};
byte names[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956};

//Buffer
char Buffer[500]; //Buffer de Teclado

/*Estrutura de etiqueta*/
typedef struct ETIQUETA {
  char titulo[20];
  char linha01[40];
  char linha02[40];
  char barra[20];
} ETIQUETA;

/*Variavel de etiqueta*/
ETIQUETA etiqueta;


/* **************************************************
 * Pinout 
 ****************************************************/
#include "SoftwareSerial.h"
#define TX_PIN 6 //Arduino transmit  YELLOW WIRE  labeled RX on printer
#define RX_PIN 5 //Arduino receive   GREEN WIRE   labeled TX on printer
#define GND_PIN 7 //Arduino receive   GREEN WIRE   labeled TX on printer

#define IN1_01  34 //Motor Passo01
#define IN2_01  35 //Motor Passo01
#define IN3_01  36 //Motor Passo01
#define IN4_01  37 //Motor Passo01

#define speakerPin A13 //Speaker
#define DHT22Pin A9 //DHT22
#define pinSD  4

#define pinFIMA1 26 /*Fim de curso*/
#define pinFIMA2 27
#define pinFIMB1 28
#define pinFIMB2 29
#define pinFIMC1 30
#define pinFIMC2 31

#define pinRELE01 42
#define pinRELE02 44



//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define DHTTYPE DHT11   // DHT 11

DHT dht(DHT22Pin, DHTTYPE);

//SoftwareSerial NextionSerial(pinNextionRX, pinNextionTX); // RX, TX

Stepper Passo01(stepsPerRevolution, IN1_01, IN3_01, IN2_01, IN4_01);  // Pin inversion to make the library work


/*Funcoes predefinidas*/

void ImprimeEtiqueta();
void Beep();
void Sound(char serByte);
void WellComeConsole();
void WellComeBluetooth();
void KeyCMD();//Comando de entrada 
float Reset();
void LstDir(File dir, int numTabs);
void MAN();
void DeleteFile(File root, char *sMSG1); //Deleta arquivo do SD
void LOAD(File root, char *sMSG1); //Carrega a aplicação para o SD
int LOADLoop(File root, char *Info);
void LOADLeituras();
void LOADLEARQUIVO(char *Arquivo);
void LOADAnalisa(File * farquivo);
void LOADKeyCMD(File * farquivo);
void LOADRename(String filename);
void LOADCancela(String filename);
void LOADBloco(File * farquivo, char *Info);
void LOADFIMARQUIVO();
void MostraTemperatura();
float Run(String Arquivo);
void MOPERACAO();
void FOPEImprime();
void MCONFIG();
void Le_DHT22();
void NextionShow(char* info1);
void NextionFieldText(char *field,char *value);
void NextionMensage(String info);
void NextionMensageStop(String info);
void RetConsole();
void NextionWAITESC();
void NextionMensageSTOP(String info);
void Rele01(bool Value);
void Rele02(bool Value);
void MovePassoA_Dir();
void MovePassoA_Esq();
void RetornaServos();
long fLOCAL_DEFMOD(int modulo);
void fDEFMOD(int modulo,char *MSG1);
void gravaDEFMOD(int modulo, long valor);
void ImprimeInvertido(char info[]);
void ImprimeGrande(char info[]);
void ImprimeMedio(char info[]);
void ImprimePequeno(char info[]);
void ImprimeBarra(char info[]);
void ImprimeAvanco();
void Report(); //Imprime relatorio
void retirac(char *nome, char *nova, char tira);


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
  pinMode(pinFIMA1, INPUT);
  pinMode(pinFIMA2, INPUT);
  pinMode(pinFIMB1, INPUT);
  pinMode(pinFIMB2, INPUT);
  pinMode(pinFIMC1, INPUT);
  pinMode(pinFIMC2, INPUT);
}

void Start_RELES()
{
  pinMode(pinRELE01, OUTPUT);
  pinMode(pinRELE02, OUTPUT);
  digitalWrite(pinRELE01, HIGH);
  digitalWrite(pinRELE02, HIGH);
}

void Start_Serial()
{
  Serial.begin(9600); 
}

void Start_Bluetooth()
{
  Serial2.begin(9600);
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
  Serial3.begin(9600);
  NextionShow("Splah"); //Chamando tela splash
  char info[40];
  memset(info,'\0',sizeof(info));
  sprintf(info,"%s.%s",Versao,Release);
  NextionFieldText("versao",info);

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
  //ImprimeAvanco();
  //ImprimeInvertido(" Bem vindo ao Doctor");
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
  
  Serial3.println(" ");
  Serial3.println("MAN para ajuda");

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
  MostraTemperatura();
  Serial.println("MAN para ajuda");
  //Serial.print(f);

  Serial.println(" ");
 

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
    
    //ImprimeMedio("Rede Local:");
    //ImprimeMedio(Ethernet.localIP());
    char Info[40];
    memset(Info,'\0',sizeof(Info));
    memset(IP,'\0',sizeof(Info));
   // sprintf(IP,"%s.%s.%s.%s",myIPAddress[0],myIPAddress[1],myIPAddress[2],myIPAddress[3]);
    NextionFieldText("ip",IP);
    // Imprime(0,Info);
    //printer.println(Info);
    sprintf(Info,"Port:23");
    Serial.println(Info);
    // Imprime(1,Info);
    //ImprimeMedio(Info);
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

  Serial.println("Calibração Eixo X");
  
  while (!flgFIMA1 )
  {
      MovePassoA_Esq(3);
      Le_FimCurso();
  }
  Serial.println("Fim de calibração");
  
  
}

void MovePasso01()
{
//  stepper.step(10);
}

void setup() {
 
  Start_Serial();
  Serial.println("Starting modules...");
  Speak_Start();  
  Beep(); 
  Start_Printer();
  Start_FIMDECURSO();
  Start_Nextion();
  Start_Bluetooth();
  Start_Motor01();

  Start_RELES();

  Start_DHT22(); 

  Start_SD();
  Service_Start();

  // Font options
  //MovePasso01();
  //ImprimeEtiqueta();
  Le_DHT22();
  Reset();  
  Calibracao(); /*Calibração do equipamento*/
  WellComeConsole(); 
  NextionShow("Menu");
  delay(100);
  NextionShow("Menu");
 

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


void Rele01(bool Value)
{
  //pinrele01
  if (Value == true)
  {
    digitalWrite(pinRELE01, HIGH);
  }
  else
  {
    digitalWrite(pinRELE01, LOW);
  }
}

void Rele02(bool Value)
{
  //pinrele01
  if (Value == true)
  {
    digitalWrite(pinRELE02, HIGH);
  }
  else
  {
    digitalWrite(pinRELE02, LOW);
  }
}

void ImprimeInvertido(char info[])
{
  printer.inverseOn();
  printer.setSize('M');        // Set type size, accepts 'S', 'M', 'L'
  printer.justify('C');
  printer.println(info);
  printer.inverseOff();
  printer.setSize('S');
  printer.inverseOff();
}

void ImprimeGrande(char info[])
{
  printer.setSize('L');
  printer.println(info);
  printer.setSize('S');
}

void ImprimeMedio(char info[])
{

  printer.setSize('M');
  printer.println(info);
  printer.setSize('S');
}


void ImprimePequeno(char info[])
{

  printer.setSize('S');
  printer.println(info);
  printer.setSize('S');
}

void ImprimeBarra(char info[])
{

  printer.setSize('S');
  printer.printBarcode(info, CODE39);
  printer.setBarcodeHeight(100);
  printer.setSize('S');
  printer.sleep();      // Tell printer to sleep 
}

void ImprimeAvanco()
{
  
  printer.println(' ');
  printer.println(' ');
}

void ImprimeReset()
{
  printer.wake(); // MUST wake() before printing again, even if reset  
  printer.setDefault(); // Restore printer to defaults
  printer.println(' ');  
}


void ImprimeEtiqueta() 
{
  // Test inverse on & off
  ImprimeInvertido(etiqueta.titulo);  
  ImprimeMedio(etiqueta.linha01);  
  ImprimeBarra(etiqueta.barra);
  ImprimeAvanco();
}

void ImprimeHello() 
{
  // Test inverse on & off
  ImprimeInvertido(etiqueta.titulo);  
  ImprimeMedio(etiqueta.linha01);  
  ImprimeMedio(etiqueta.linha02);  
  //ImprimeBarra(etiqueta.barra);
}

void Report()
{
  memset(&etiqueta,'\0',sizeof(ETIQUETA));
  sprintf(etiqueta.titulo,"DOCTOR");
  sprintf(etiqueta.linha01,"Linha 01");
  sprintf(etiqueta.linha02,"Linha 02");
  sprintf(etiqueta.barra,"12345");
  ImprimeEtiqueta();
  //ImprimeAvanco();
  
}

void MovePassoA_Dir(int passo)
{
  
    // 1 rotation counterclockwise:  
    //Serial.println("counterclockwise");    
    Passo01.step(passo);  
    posSERVA = posSERVA+ passo;
    Serial.print("POSSERVA=");  
    Serial.println(posSERVA);
    //delay(1000); 
}

void MovePassoA_Esq(int passo)
{
  // 1 rotation clockwise: 
 
  Passo01.step(-passo); 
  posSERVA = posSERVA- passo;
  Serial.print("POSSERVA=");  
  Serial.println(posSERVA);
  //delay(1000); 
}

void RetornaServos()
{
  
  while(!flgFIMA1)
  {      
    MovePassoA_Esq(passoA);
    Le_FimCurso();
  }
  
}


int analise_DEFMOD(int modulo, char linha[])
{
  char *posicao=NULL;
  char *fimdelinha=NULL;
  char param[255];
  Serial.print("Linha:");
  Serial.println(linha);
  posicao=strstr(linha,"LOCAL_DEFMOD=");
  if(posicao!=NULL)
  {
          posicao=strstr(linha,"=");
          posicao= posicao +1;
          fimdelinha = strstr(linha,"\0");
          memset(param,'\0',sizeof(param)); /*inicializa param*/
          strcpy(param,posicao);
          Serial.print("LOCAL_DEFMOD=");
          Serial.print(modulo);
          Serial.print(",");
          Serial.println(param);
          
          char *ptr;
          long valor;
          valor = strtol(param,&ptr,10);
          Serial.println("Fim2 fLOCAL_DEFMOD");
          return valor;
  }
   else
  {
          //arquivo.close();
          return 0;
  } 
}

/*Carrega configuração do modulo local*/
long int fLOCAL_DEFMOD(int modulo)
{
  File arquivo;
  int tamanho =0;
  char linha[255];
  char param[255];
  char *posicao=NULL;
  char *fimdelinha=NULL;
  int flgfinded = 0;
  char info;
  long valor = 0;
  
  Serial.println("Inicio fLOCAL_DEFMOD");
  if(modulo==1) flgfinded  = SD.exists("DEFMOD1.CFG");
  if(modulo==2) flgfinded  = SD.exists("DEFMOD2.CFG");
  if(modulo==3) flgfinded  = SD.exists("DEFMOD3.CFG");
  if(modulo==4) flgfinded  = SD.exists("DEFMOD4.CFG");
  Serial.print("flgfinded:");
  Serial.println(flgfinded);
  if(flgfinded==1){
    if(modulo==1)   arquivo = SD.open("DEFMOD1.CFG");
    if(modulo==2)   arquivo = SD.open("DEFMOD2.CFG");
    if(modulo==3)   arquivo = SD.open("DEFMOD3.CFG");
    if(modulo==4)   arquivo = SD.open("DEFMOD4.CFG");
    Serial.println("Abriu arquivo");
    memset(linha,'\0',sizeof(linha));
    while (arquivo.available()) 
    {      
      info = arquivo.read();
      //Serial.print("info:");
      //Serial.println(info);
      

      if(info=='\n')
      {
        //Serial.print("linha:");
        //Serial.println(linha);
        valor = analise_DEFMOD(modulo,linha);
        arquivo.close();
        return valor;
      } else
      {
        sprintf(linha,"%s%c",linha,info);
        
      }
    }      
  } else {
    arquivo.close();
    return 0;
  }
  
  arquivo.close();
  Serial.println("Fim fLOCAL_DEFMOD");
  return 0;
}


void removeDEFMOD(int modulo)
{
  if(modulo==1)
  {
    if(SD.exists("DEFMOD1.CFG")) SD.remove("DEFMOD1.CFG");
  }
  if(modulo==2)
  {
    if(SD.exists("DEFMOD2.CFG")) SD.remove("DEFMOD2.CFG");
  }
  if(modulo==3)
  {   
    if(SD.exists("DEFMOD3.CFG")) SD.remove("DEFMOD3.CFG");
  }
  if(modulo==4)
  {
    if(SD.exists("DEFMOD4.CFG")) SD.remove("DEFMOD4.CFG");
  }
}

void gravaDEFMOD(int modulo, long valor)
{
  File arquivo;
  char info[255];
  memset(info,'\0',sizeof(info));
  Serial.println("gravaDEFMOD");
  sprintf(info,"LOCAL_DEFMOD=%i\n",valor);
  removeDEFMOD(modulo);
  if(modulo==1)   arquivo = SD.open("DEFMOD1.CFG", FILE_WRITE);
  if(modulo==2)   arquivo = SD.open("DEFMOD2.CFG", FILE_WRITE);
  if(modulo==3)   arquivo = SD.open("DEFMOD3.CFG", FILE_WRITE);
  if(modulo==4)   arquivo = SD.open("DEFMOD4.CFG", FILE_WRITE);
  //arquivo.write("DEFMOD=",7);
  arquivo.println(info);
  arquivo.close();
}

void fDEFMOD(int modulo,char *MSG1)
{
  char *ptr;
  long local = 0;

  local = strtol(MSG1,&ptr,10);
  Serial.println("fDEFMOD");
  if (modulo==1) LOCAL_DEFMOD1 = local;
  if (modulo==2) LOCAL_DEFMOD2 = local;
  if (modulo==3) LOCAL_DEFMOD3 = local;
  if (modulo==4) LOCAL_DEFMOD4 = local;
  gravaDEFMOD(modulo,local);
  Serial.print("LOCAL_DEFMOD");
  Serial.print(modulo);
  Serial.print("=");
  Serial.println(local);
}

//Carrega a aplicação para o SD
void LOAD(File root, char *sMSG1)
{
 
  //Imprime(1, "Carregando APP...");
  Serial.println("Carregando APP..");
  NextionShow("LOAD");
 
  //Realiza operação de Loop
  LOADLoop(root, sMSG1);
  NextionShow("Menu");

}

//DeleteFile
void DeleteFile(File root, char *sMSG1)
{
 
  //Imprime(1, "Carregando APP...");
  Serial.println("DeleteFile..");
  //NextionShow("LOAD");
 
  //Realiza operação de Loop
  //LOADLoop(root, sMSG1);
  //NextionShow("Menu");
  if (SD.exists(sMSG1)) 
  {
   SD.remove(sMSG1);
   Serial.println("Alert! File Deleted!");
  } else {
    Serial.println("Alert! File not exist!");
  }
  

}


void LOADLEARQUIVO(char *Arquivo)
{
  File arquivo;
  Serial.println("Carregando APP..");
  NextionShow("LOAD");
  Serial.print("Arquivo=");
  Serial.println(Arquivo);
  arquivo = SD.open(Arquivo);
  //arquivo = SD.open("ARQUIVO.REC");
  Serial.println("Teste1");
  // if the file is available, read the file
  //if(arquivo)
  {
    Serial.println("Lendo...");
    char info = ' ';
    while(arquivo.available())
    {
      //Serial.write();
      if (info =='\n')
      {
        Serial.print("READY=");
      }
      info = arquivo.read();
      Serial.print(info);
     
    }
    Serial.println("Fechou");
    arquivo.close();
  }
  
}

//Analisa Entrada de Informacoes de Entrada
void LOADAnalisa(File * farquivo)
{
  LOADKeyCMD(farquivo); //Analisa o que esta na entrada do buffer de teclado
  //LOADKeyCMDBluetooth(); //Analisa o que esta na entrada do buffer do bluetooth
}

void retirac(char *nome, char *nova, char tira)
{
    int i=0;
    while (*nome != '\0'){
        if (*nome == tira)
            nova [i] = '\0';
        else
            nova[i] = *nome;
        ++i;
        ++nome;
    }
    nova [i] = '\0';
}

int LOADLoop(File root, char *Info)
{

  memset(Buffer,'\0',sizeof(Buffer)); /*Zera buffer*/
  Serial.println("Iniciando rotina LOADLoop");
  //Tenta carregar arquivo t para ser temporario
  Serial.print("LOADLoop Info:");
  Serial.println(Info);
  
  //farquivo = SD.open("temp.out", FILE_WRITE);
  //File farquivo = SD.open(String(Info), FILE_WRITE);
  farquivo = SD.open(Info, FILE_WRITE);
  //Limpa os buffer
  if (!farquivo)
  {
    Serial.println("Erro SD");
    Serial.print("Nao pode abrir SD:");
    Serial.println(Info);
    //NextionMessage("Nao pode abrir SD");
    //Imprime(1, "Erro SD");
    // Imprime(2, "Nao pode abrir SD");
    return -1;
  }
  else
  {
    Serial.println("Iniciou bloco de gravação do arquivo");
    flgEscape = false; //Sai quando verdadeiro
    //Loop
    while (!flgEscape)
    {
      LOADLeituras();
      //Realiza analise das informações encontradas
      LOADAnalisa(&farquivo);      
      //arquivo.println("Leitura Potenciometro: ");
    }
    Serial.println("Finalizou bloco de gravação do arquivo");
  }
  farquivo.close();
  Serial.println("Fechou arquivo ");
  //Apaga o Arquivo Temporario
  //LOADRename(Info);
   //LOADCOPYTEMP(ArquivoFinal);
   //LOADRemoveTemp();
  return 0;
}

//Realiza Leitura do arquivo
void LOADLeituras()
{
  //Inicializa Temperatura
  //Le_Temperatura();
  //Le_Teclado();
  Le_Serial();
  Le_Serial3();
  //Le_Bluetooth();
  Chk_Beep();
  //Le_DHT22();
  Le_FimCurso(); /*Leitura de fim de curso*/
}


//Apaga o arquivo temporario
void LOADCancela(String filename)
{
  //LOADRemoveArquivo(filename);
  //SD.remove("temp.out");
  flgEscape = true;
}

//Apaga Arquivo Temporario
void LOADRename(String filename)
{
  if (SD.exists("temp.out")) 
  {
    //SD.rename("temp.out",filename);
    // open the file named ourfile.txt
    Serial.println("Iniciando copia de arquivo temp.out"); 
    File FOrigem = SD.open("temp.out");
    char filename[40];
    sprintf(filename,"%s",ArquivoTrabalho);
    File FDestino = SD.open(filename, FILE_WRITE);
    FDestino.close();
    //FDestino = SD.open(ArquivoTrabalho, FILE_WRITE);
    FDestino = SD.open("receita.rec", FILE_WRITE); /*Bug nao resolvido*/
    char info;
    long cont;
    // if the file is available, read the file
    if (FOrigem)
    {
      Serial.println("Abriu origem");
      for(cont = 0;cont<=FOrigem.size();cont++)
      {
        
        info = FOrigem.read();
        Serial.print(info);
        FDestino.write(info);
      }
    
    }
    Serial.println("Finalizando copia de arquivo temp.out"); 
    FOrigem.close();
    FDestino.close();
    SD.remove("temp.out");
  }
}

//Apaga Arquivo Temporario
void LOADRemoveTemp()
{
  SD.remove("temp.out");
}

//Copia o Arquivo Temporario para o Arquivo definitivo
void LOADCOPYTEMP(char *Info)
{
  // open the file named ourfile.txt
  char filename[40];
  sprintf(filename,"%s",ArquivoTrabalho);
  Serial.print("filename:");
  Serial.println(filename);
  File FOrigem = SD.open("temp.out");
  File FDestino = SD.open(filename, FILE_WRITE);
  Serial.print("Arquivo nome:");
  Serial.println(Info);
  //File FDestino = SD.open(Info, FILE_WRITE);

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

void LOADBloco(File *farquivo, char *Info)
{
  Serial.print("Gravou:");
  Serial.println(Info);
  farquivo->print(Info);
}

//Finaliza o arquivo temporario copiando para arquivo final
void LOADFIMARQUIVO()
{
  Serial.println("Fim do Arquivo");
  //Imprime(1, "Carregando Arquivo  ");
  //Imprime(2, "Copiando Arquivo    ");
  //Copia o temporario para o definitivo
  //LOADCOPYTEMP();
  //LOADRemoveTemp();
  flgEscape = true;
}

/*
void LOADCOPYTEMP()
{
  Serial.print("Copiando arquivo para ");
  Serial.println(ArquivoTrabalho);
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
  Serial.println("Finalizou copia");
}

*/

//Comando de entrada do Teclado
void LOADKeyCMD(File * farquivo)
{
  bool resp = false;

  //incluir busca /n

  if (strchr(Buffer,'\n') == 0)
  {  
    //Serial.println("Sem fim de linha");    
  } else
  {
    Serial.print("Comando:");
    Serial.println(Buffer);

    //Funcao Cancela dados
    if (strcmp( Buffer, "CANCELA\n") == 0)
    {
      LOADCancela(ArquivoTrabalho);
      resp = true;
    }

    //Funcao Carrega Bloco
    if (strstr( Buffer, "BLOCO=") != 0)
    {
      //FUNCMDefault();
      char *Info;
      Info = strstr(Buffer,"=")+1;     
      //Serial.print("Bloco Dados:"); 
      //Serial.println(Info);
      LOADBloco(farquivo, Info); /*Grava comando*/
      resp = true;
    }

    //Funcao Carrega Bloco
    if (strstr( Buffer, "FIMARQUIVO\n") != 0)
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
      memset(Buffer, '\0', sizeof(Buffer));
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
  Le_Serial3();
  //Le_DHT22();
  Le_FimCurso(); /*Leitura de fim de curso*/
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


void NextionShow(char* info1)
{
  char strFF = 0xFF;
  char cmd[40];
  char *pos;
  Serial.print("Info:");
  Serial.println(info1);
  pos =strstr(info1,'\n');
  
  Serial.print("Info:");
  Serial.println(info1);
  memset(cmd,'\0',sizeof(cmd));

  sprintf(cmd,"page %s%c%c%c",info1,strFF,strFF,strFF);  
  Serial.println(cmd);
  Serial3.print(cmd);  
  delay(100);  
}

void NextionFieldText(char *field,char *value)
{
  char strFF = 0xFF;  
  //String cmd;
  char cmd[40];
  memset(cmd,'\0',sizeof(cmd));
  sprintf(cmd,"%s.txt=\"%s\"%c%c%c",field,value,strFF,strFF,strFF);  
  //cmd = field+".txt=\""+value+"\""+String(strFF)+String(strFF)+String(strFF);
  Serial.println(cmd); 
  Serial3.println(cmd);  
}

void NextionWAITESC()
{
  
}

void NextionMensage(String info)
{
  char strFF = 0xFF;
  Serial3.print("page MSG"+String(strFF)+String(strFF)+String(strFF));  
  delay(100);
  String cmd;
  
  cmd = "MSGtxt.txt=\""+info+"\""+String(strFF)+String(strFF)+String(strFF);
  Serial.println(cmd);  
  Serial3.print(cmd);
}

void NextionMensageSTOP(String info)
{
  char strFF = 0xFF;
  Serial3.print("page MSG"+String(strFF)+String(strFF)+String(strFF));  
  delay(100);
  String cmd;
  
  cmd = "MSGtxt.txt=\""+info+"\" "+String(strFF)+String(strFF)+String(strFF);
  Serial.println(cmd);  
  Serial3.print(cmd);
  NextionWAITESC();
}



//Reseta todas as entradas para o valor padrão
float Reset()
{
  memset(Buffer,'\0',sizeof(Buffer));
  Serial.println("RESET");
  contciclo = 0;
  /*Inicializa Modulos*/
  LOCAL_DEFMOD1=0;
  LOCAL_DEFMOD2=0;
  LOCAL_DEFMOD3=0;
  LOCAL_DEFMOD4=0;
  Serial.println('Reset rodou');
  flgBeep = false;
  /* incluir sub comandos de inicialização*/
  passoA = 10;
  passoB = 10;
  passoC = 10;

  RetornaServos();

  /*Posicao de servo motores*/
  posSERVA = 0;
  Serial.print("POSSERVA=");  
  Serial.println(posSERVA);
  posSERVB = 0;
  Serial.print("POSSERVB=");  
  Serial.println(posSERVB);
  posSERVC = 0;
  Serial.print("POSSERVC=");  
  Serial.println(posSERVC);

  LOCAL_DEFMOD1=fLOCAL_DEFMOD(1);
  LOCAL_DEFMOD2=fLOCAL_DEFMOD(2);
  LOCAL_DEFMOD3=fLOCAL_DEFMOD(3);
  LOCAL_DEFMOD4=fLOCAL_DEFMOD(4);
  Serial.print("LOCAL_DEFMOD1=");
  Serial.println(LOCAL_DEFMOD1);
  Serial.print("LOCAL_DEFMOD2=");
  Serial.println(LOCAL_DEFMOD2);
  Serial.print("LOCAL_DEFMOD3=");
  Serial.println(LOCAL_DEFMOD3);
  Serial.print("LOCAL_DEFMOD4=");
  Serial.println(LOCAL_DEFMOD4);
  Serial.println("Fim de RESET");
  
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
    if (!entry.isDirectory()) {
    Serial.print("FILENAME:");
    Serial.println(entry.name());
    }
    /*
    if (entry.isDirectory()) {
      Serial.println("/");
      LstDir(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    */
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
  Serial.println("RELE01 - ACIONAMENTO RELE01");
  Serial.println("RELE02 - ACIONAMENTO RELE02");
  Serial.println("DELFILE - REMOVE ARQUIVO");
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
  if (strchr(Buffer,'\n') != 0)
  {
    Serial.print("Comando:");
    Serial.println(Buffer);  

    //Inicio
    if (strcmp( Buffer, "BEGIN\n") == 0)
    {
      Serial.println("BEGIN");
      Reset();
      resp = true;
    }

    //FIM
    if (strcmp( Buffer, "END\n") == 0)
    {
      Serial.println("END");
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

    //void Report()
    //MAN
    if (strcmp( Buffer, "REPORT\n") == 0)
    {
      //Serial.println(Temperatura);
      Report();
      resp = true;
    }

    //Load - Carrega arquivo no SD
    if (strstr( Buffer,"LOAD=") != 0)
    {
      char *MSG1;
      char Filename[80];
      memset(Filename,'\0',sizeof(Filename));
      MSG1 = strstr(Buffer,"=")+1;
      char *fim = strchr(Buffer,'\n');
      long posicao = strlen(Buffer);
      if (fim !=0)  posicao = (fim-MSG1);
      Serial.print("Posicao:");
      Serial.println(posicao);
      strncpy(Filename,MSG1,posicao);
      
      
      Serial.println("Iniciou Load");
      
      //Imprime(0, MSG1);
      Serial.print("Arquivo:");
      Serial.println(Filename);
      //root = card.open(MSG1);
      root = SD.open("/");
      LOAD(root, Filename);

      resp = true;
    }

    //DELETEFILE - Deleta arquivo do SD
    if (strstr( Buffer,"DELFILE=") != 0)
    {
      char *MSG1;
      char Filename[80];
      memset(Filename,'\0',sizeof(Filename));
      MSG1 = strstr(Buffer,"=")+1;
      char *fim = strchr(Buffer,'\n');
      long posicao = strlen(Buffer);
      if (fim !=0)  posicao = (fim-MSG1);
      Serial.print("Posicao:");
      Serial.println(posicao);
      strncpy(Filename,MSG1,posicao);
      
      
      Serial.println("Iniciou DeleteFile");
      
      //Imprime(0, MSG1);
      Serial.print("Arquivo:");
      Serial.println(Filename);
      //root = card.open(MSG1);
      root = SD.open("/");
      DeleteFile(root, Filename);

      resp = true;
    }

    //DEFMOD1=
    if (strstr( Buffer,"DEFMOD=") != 0)
    {
      char *IGUAL;
      char *virgula;
      char *fim;
      int nro =0;
      char *ptr;
      char param01[20];
      Serial.println("DEFMOD");
      Serial.print("Buffer:");
      Serial.println(Buffer);
      IGUAL = strstr(Buffer,"=");
      virgula = strstr(Buffer,",");
      if(virgula!=NULL)
      {
        fim = strstr(Buffer,"\n");
        memset(param01,'\n',sizeof(param01));
        
        strncpy(param01,(char*)(IGUAL+1),virgula-(IGUAL+1));
        //strncpy(virgula, (char*)(virgula+1),(fim-(virgula+1)));
        nro = strtol(param01,&ptr,10);
        Serial.print("NRO:");
        Serial.println(nro);
        Serial.print("param02:");
        Serial.println(virgula+1);
      
        fDEFMOD(nro,virgula+1);
        resp = true;
      }
      else {
        Serial.println("Comando mau formado");
        resp = false;
      }
    }



    if (strstr( Buffer, "SHOW=") != 0)
    {
      char sMSG1[40];
      char sMSG2[40];
      Serial.println("Achou SHOW");
      char *posequ = strstr( Buffer, "=");
      char *posend = strstr( Buffer, "\n");
      
      Serial.print("POSEQU=");
      Serial.println(posequ);
      if(posequ != 0)
      {
        posequ ++;
        
        memset(sMSG1,'\0',sizeof(sMSG1));
        
        strncpy(sMSG1, posequ,(posend)-posequ);
        Serial.print("sMSG1=");
        Serial.println(sMSG1);
        NextionShow(sMSG1);
        
        resp = true;
      }
    }

    //NextionFieldText
    if (strstr( Buffer, "NTFLD:") != 0)
    {
      Serial.println("Achou NEXTFIELD");

      /*Achando valores de parametros*/
      char sFIELD[40];
      memset(sFIELD,'\0',sizeof(sFIELD));
      char sVALUE[40];
      memset(sVALUE,'\0',sizeof(sFIELD));

      char *posdois = strchr( Buffer, ':');
      char *posequ = strchr( Buffer, '=');
      char *posend = strchr( Buffer, '\n');
      
      Serial.print("POSEQU=");
      Serial.println(posequ);
      if(posequ != 0)
      {
        posequ ++;            
        int tamanho = (posequ-posdois)-2;
        strncpy(sFIELD, posdois+1,tamanho);
        tamanho = (posdois-posend)-2;
        strncpy(sVALUE, posequ,tamanho);
        
        Serial.print("sFIELD=");
        Serial.println(sFIELD);
        Serial.print("sVALUE=");
        Serial.println(sVALUE);
        //NextionShow(sMSG1);
        NextionFieldText(sFIELD,sVALUE);
        
        resp = true;
      }
    }

    if (strstr( Buffer, "MENSAGEM=") != 0)
    {
      char sMSG1[20];
      char sMSG2[20];
      Serial.println("Achou MENSAGEM");
      char *posequ = strstr( Buffer, "=");
      Serial.print("POSEQU=");
      Serial.println(posequ);
      if(posequ != 0)
      {
        posequ ++;
        memset(sMSG1,'\0',sizeof(sMSG1));
        strcpy(sMSG1, posequ);
        Serial.print("sMSG1=");
        Serial.println(posequ);
        NextionMensage(String(posequ));
        
        resp = true;
      }
    }
    
    if (strstr( Buffer, "MSGSTOP=") != 0)
    {
      char sMSG1[20];
      char sMSG2[20];
      Serial.println("Achou MENSAGEM");
      char *posequ = strstr( Buffer, "=");
      Serial.print("POSEQU=");
      Serial.println(posequ);
      if(posequ != 0)
      {
        posequ ++;
        memset(sMSG1,'\0',sizeof(sMSG1));
        strcpy(sMSG1, posequ);
        Serial.print("sMSG1=");
        Serial.println(posequ);
        NextionMensageSTOP(String(posequ));
        
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

    if (strncmp("RELE01=", Buffer, 6) == 0)
    {
      char Info[20];
      strcpy(Info, &Buffer[6]);
      //Imprime(1, "BOMBA               ");
      //Imprime(2, "                    ");
      //Imprime(2, Info);
      Serial.println(Info);
      if (strstr(Info, "ON\n") == 0)
      {
        //Aciona o RELE01
        Rele01(true);
      }
      else
      {
        //Abre o RELE01
        Rele01(false);
      }
      resp = true;
    }
    
    if (strncmp("RELE02=", Buffer, 6) == 0)
    {
      char Info[20];
      strcpy(Info, &Buffer[6]);
      //Imprime(1, "RE               ");
      //Imprime(2, "                    ");
      //Imprime(2, Info);
      Serial.println(Info);
      if (strstr(Info, "ON\n") == 0)
      {
        //Aciona o RELE02
        Rele02(true);
      }
      else
      {
        //Abre o RELE02
        Rele02(false);
      }
      resp = true;
    }

    //SENSOR
    if(strstr( Buffer, "SENSOR\n")!= 0)
    {
      MostraTemperatura();
      resp = true;
    }

 

    //BeepMsg
    if (strcmp( Buffer, "BEEPMSG=") == 0)
    {
      //Serial.print("Temperatura:");
      //Serial.print(Temperatura);
      Serial.println(";");
      resp = true;
    }

    //BeepMsg
    if (strstr( Buffer, "READ=") != 0)
    {
      char sMSG1[40];
      char sMSG2[40];
      char *poscr;
      Serial.println("Achou MENSAGEM");
      char *posequ = strstr( Buffer, "=");
      Serial.print("POSEQU=");
      Serial.println(posequ);
      if(posequ != 0)
      {
        posequ++;
        poscr = strstr(sMSG1,"\n");
        if (poscr!=0)
        {
          Serial.println("Teste4");
          memset(sMSG1,'\0',sizeof(sMSG1));
          strncpy(sMSG1, posequ,int(poscr-posequ)-2);
        } else {
          Serial.println("Teste3");
          strcpy(sMSG1, posequ);  
        }
        Serial.print("sMSG1=");
        Serial.println(posequ);
        LOADLEARQUIVO(posequ);
        
        resp = true;
      }
      
      
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

//Le registro do Serial3
void Le_Serial3()
{
  char key;
  
  while (Serial3.available() > 0)
  {
    
    key = Serial3.read();

    if (key != 0)
    {
      
      Serial3.print(key);
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

  char temperatura[10];
  memset(temperatura,'\0',sizeof(temperatura));
  //sprintf(temperatura,"%f",t);
  dtostrf(t, 3, 2, temperatura);  // 8 char min total width, 6 after decimal
  //Serial.print("TEMPERATURA:");
  //Serial.println(temperatura);
  NextionFieldText("temp",temperatura);
  memset(temperatura,'\0',sizeof(temperatura));
  //sprintf(temperatura,"%f",t);
  dtostrf(h, 4, 2, temperatura);  // 8 char min total width, 6 after decimal
  Serial.print("Humidade:");
  Serial.println(temperatura);
  delay(100);
  NextionFieldText("hum",temperatura);
  delay(100);
  NextionFieldText("hum",temperatura);
}

void Le_FimCurso()
{  
  flgFIMA1 = digitalRead(pinFIMA1)!=HIGH?HIGH:LOW;  
  flgFIMA2 = digitalRead(pinFIMA2)!=HIGH?HIGH:LOW;
  flgFIMB1 = digitalRead(pinFIMB1)!=HIGH?HIGH:LOW;
  flgFIMB2 = digitalRead(pinFIMB2)!=HIGH?HIGH:LOW;
  flgFIMC1 = digitalRead(pinFIMC1)!=HIGH?HIGH:LOW;
  flgFIMC2 = digitalRead(pinFIMC2)!=HIGH?HIGH:LOW;  
  if(lastflgFIMA1!=flgFIMA1)
  {
     Serial.print("flgFIMA1=");  Serial.println(flgFIMA1==HIGH?"ON":"OFF");
     lastflgFIMA1 = flgFIMA1;
  }
  if(lastflgFIMA2!=flgFIMA2)
  {
    Serial.print("flgFIMA2=");  Serial.println(flgFIMA2==HIGH?"ON":"OFF");
    lastflgFIMA2 = flgFIMA2;
  }
}

void Leituras()
{
  //Le_Teclado();
  Le_Serial();
  Le_Serial3();
  //Le_DHT22();
  Le_FimCurso();
 

  Chk_Beep();
}

void MostraTemperatura()
{
  Serial.print("TEMPERATURA=");
  Serial.println(t);
  Serial.print("HUMIDADE=");
  Serial.println(h);
}

//Imprime Retorno de Console
void RetConsole()
{
  //Serial ÃƒÆ’Ã‚Â© padrao para todos os retornos
  Serial.println("$>");
}

void loop() 
{
  Leituras();
  Analisa();
  contciclo = contciclo+1;
  
  if  (contciclo > MAXCICLO) //Modulo de contagem de ciclos
  {
    Le_DHT22();
    //MostraTemperatura
    contciclo = 0;
  }
  
  
}
