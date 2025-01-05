#include <Stepper.h> 
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>

//#include "NextionHandler.h"  // Inclus�o do header para o Nextion
#include <stdio.h>
#include <string.h>
#include "DHT.h"
#include "Adafruit_Thermal.h"
#include <Servo.h>

//#include "adalogo.h"
//#include "adaqrcode.h"

//*************** Descricao do Produto *********************
//Versao do produto 
#define Versao  "0"  //Controle de Versao do Firmware
#define Release "5" //Controle Revisao do Firmware
char Produto[20] = { "Doctor - Betha"};

#define MAXCICLO 90000
#define MAXSPEED 64
#define MOVPASSOS 50//Nro de passos por vez

// Definições
#define ANGULO_MAXIMO 5 // Máximo de graus a mover por vez
#define DELAY_TEMPO 50  // Tempo em milissegundos entre cada incremento

// Instâncias dos servos
Servo ServoBraco01;
Servo ServoBraco02;
Servo ServoBraco03;
Servo ServoBraco04;

// Variáveis globais para posições
int posBracoAtual[4] = {90, 90, 90, 90}; // Posições atuais dos servos
int posBracoDestino[4] = {90, 90, 90, 90}; // Posições finais desejadas

/* **************************************************
 * Pinout 
 ****************************************************/
#include "SoftwareSerial.h"
#define Br01_PIN 7 //Braco robotico servo 01
#define Br02_PIN 6  //Braco robotico servo 02
#define Br03_PIN 5  //Braco robotico servo 03
#define Br04_PIN 4  //Braco robotico servo 04

#define speakerPin 13 //Speaker
#define DHT22Pin A9 //DHT22
#define pinSD  53


#define pinFIMA1 22 /*Fim de curso*/
#define pinFIMA2 23
#define pinFIMB1 24
#define pinFIMB2 25

#define IN1_01  27 //Motor Passo01 27
#define IN2_01  26 //Motor Passo01 26
#define IN3_01  29 //Motor Passo01 29
#define IN4_01  28 //Motor Passo01 28



#define pinFIMC1 30
#define pinFIMC2 31


#define pinRELE01 32
#define pinRELE02 33


#define pinSDA 20 //Pino SDA para I2C
#define pinSCL 21 //Pino SCL para I2C

#define pinLoc01 22 //Pino de localiza��o
#define pinLoc02 23 //Pino de localiza��o
#define pinLoc03 24 //Pino de localiza��o
#define pinLoc04 25 //Pino de localiza��o
#define pinLoc05 26 //Pino de localiza��o
#define pinLoc06 27 //Pino de localiza��o
#define pinLoc07 28 //Pino de localiza��o
#define pinLoc08 29 //Pino de localiza��o



//http://www.aranacorp.com/en/control-a-stepper-motor-with-arduino/
double stepsPerRevolution = 128;


//Flags de Controle
bool OperflgLeitura = false;
byte flgEscape = 0; //Controla Escape
byte flgEnter = 0; //Controla Escape
byte flgTempo = 0; //Controle de Tempo e Temperatura
// Variável para controlar o estado do motor
bool flgmotorAtivo = false;

//Buffer do Teclado
char customKey;

// Variáveis globais para posições
int posBraco[4] = {0, 0, 0, 0}; // Posições iniciais dos servos

float h, oldh ; //Humidade
float t, oldt; //Temperatura
float f , oldf;



//Avancos dos motores de passo
int passoA;
int passoB;
int passoC;

/*Posicao de servos*/
long int posSERVA = 0;
long int posFIMSERVA = 0;
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





long long int contciclo = 0; //Contador de Ciclos de Repeticao


//****************** SD Card ******************
Sd2Card card;
SdVolume volume;
File root; //Pasta root
File farquivo; //Arquivo de gravacao
char ArquivoTrabalho[80]; //Arquivo de trabalho a ser carregado





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





//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
#define DHTTYPE DHT11   // DHT 11

DHT dht(DHT22Pin, DHTTYPE);

//SoftwareSerial NextionSerial(pinNextionRX, pinNextionTX); // RX, TX

Stepper Passo01(stepsPerRevolution, IN1_01, IN3_01, IN2_01, IN4_01);  // Pin inversion to make the library work


/*Funcoes predefinidas*/
void ativaMotores(); 
void desativaMotores();
void Retorna_Servos();
void Move_Servo(int eixo, int posicao);
void Carrega_Movimento(int eixo, int posicao);
void Move_Servo_Completo(int eixo, int posicao);
void Move_Servo_Incremental() ;
void ImprimeEtiqueta();
void Beep();
void Sound(char serByte);
void WellComeConsole();

void KeyCMD();//Comando de entrada 
float Reset();
void LstDir(File dir, int numTabs);
void MAN();
void DeleteFile(File root, char *sMSG1); //Deleta arquivo do SD
void LOAD(File root, char *sMSG1); //Carrega a aplica��o para o SD
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
//void NextionWAITESC();
//void NextionMensageSTOP(String info);
void showPageId(); 
void Rele01(bool Value);
void Rele02(bool Value);

void MovePassoA_Dir();
void MovePassoA_Esq(int passo);
void PosicaoCentral();
void RetornaCarro();
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
void Le_FimCurso();


//SoftwareSerial mySerial(RX_PIN, TX_PIN); // Declare SoftwareSerial obj first
//Adafruit_Thermal printer(&mySerial);     // Pass addr to printer constructor
Adafruit_Thermal printer(&Serial3);     // Pass addr to printer constructor
// Then see setup() function regarding serial & printer begin() calls.

// Here's the syntax for hardware serial (e.g. Arduino Due) --------------
// Un-comment the following line if using hardware serial:

//Adafruit_Thermal printer(&Serial1);      // Or Serial2, Serial1, etc.

// -----------------------------------------------------------------------


// Função para ativar os motores
void ativaMotores() 
{
  if (!flgmotorAtivo) 
  {
    ServoBraco01.attach(Br01_PIN);
    ServoBraco02.attach(Br02_PIN);
    ServoBraco03.attach(Br03_PIN);
    ServoBraco04.attach(Br04_PIN);
    flgmotorAtivo = true;
    Serial.println("Motores ativados.");
  }
}

// Função para desativar os motores
void desativaMotores() 
{
  if (flgmotorAtivo) {
    ServoBraco01.detach();
    ServoBraco02.detach();
    ServoBraco03.detach();
    ServoBraco04.detach();
    flgmotorAtivo = false;
    Serial.println("Motores desativados.");
  }
}

// Função para inicializar os servos
void Start_Servos() {
  flgmotorAtivo = false;
  Serial.println("Inicializando servos...");
  ativaMotores();

  flgmotorAtivo = true;
  Serial.println("Servos inicializados nas posições padrão.");
  Retorna_Servos();
}

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
  
  Serial2.begin(9600); // Inicializa a Serial2 para comunicação
  Serial2.println('MAN');
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
  Serial1.begin(9600);
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
  Serial3.begin(9600);  // Initialize SoftwareSerial
  //Serial1.begin(19200); // Use this instead if using hardware serial
  printer.begin();        // Init printer (same regardless of serial type)
  //ImprimeAvanco();
  //ImprimeInvertido(" Bem vindo ao Doctor");
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

// Função para calibrar os servos
void Calibra_Servos() {
  if(flgmotorAtivo = false) 
  {
      ativaMotores();
  }
  Serial.println("Calibrando servos...");
  PosicaoCentral();
 
  Move_Servo_Completo(2, 150);
  delay(1000);
  Move_Servo_Completo(2, 1);
  Move_Servo_Completo(1, 1);
  delay(1000);
  Move_Servo_Completo(1, 100);
  delay(1000);
  Move_Servo_Completo(1, 1);
  Move_Servo_Completo(3, 140);
  delay(1000);
  Move_Servo_Completo(3, 1);
  Move_Servo_Completo(4, 50);
  delay(1000);
  Move_Servo_Completo(4, 169);
  RetornaCarro();
  Serial.println("Servos calibrados.");
}

// Função para retornar todos os servos à posição inicial
void Retorna_Servos() {
  if(flgmotorAtivo = false) 
  {
      ativaMotores();
  }
  Serial.println("Retornando todos os servos para a posição inicial...");
  
  
  Move_Servo_Completo(2, 1);
  Move_Servo_Completo(3, 1);
  
  Move_Servo_Completo(4, 169);

  Move_Servo_Completo(1, 1);
  
  Serial.println("Todos os servos retornaram à posição inicial.");
}

// Função para carregar o movimento para os servos
void Carrega_Movimento(int eixo, int posicao) 
{
  if(flgmotorAtivo = false) 
  {
      ativaMotores();
  }
  if (eixo < 1 || eixo > 4) 
  {
    Serial.println("Eixo inválido! Use valores entre 1 e 4.");
    return;
  }
  posicao = constrain(posicao, 0, 180); // Limita a posição entre 0 e 180 graus
  posBracoDestino[eixo - 1] = posicao; // Define o destino para o eixo
  Serial.print("Movimento carregado para o Servo ");
  Serial.print(eixo);
  Serial.print(": posição desejada -> ");
  Serial.println(posicao);
}


// Função para mover qualquer servo
void Move_Servo(int eixo, int posicao)
{
  if(flgmotorAtivo = false) 
  {
      ativaMotores();
  }
  posicao = constrain(posicao, 0, 180); // Limita a posição entre 0 e 180 graus

  switch (eixo) {
    case 1:
      ServoBraco01.write(posicao);
      posBraco[0] = posicao;
      Serial.print("Servo 01 movido para a posição: ");
      Serial.println(posicao);
      break;
    case 2:
      ServoBraco02.write(posicao);
      posBraco[1] = posicao;
      Serial.print("Servo 02 movido para a posição: ");
      Serial.println(posicao);
      break;
    case 3:
      ServoBraco03.write(posicao);
      posBraco[2] = posicao;
      Serial.print("Servo 03 movido para a posição: ");
      Serial.println(posicao);
      break;
    case 4:
      ServoBraco04.write(posicao);
      posBraco[3] = posicao;
      Serial.print("Servo 04 movido para a posição: ");
      Serial.println(posicao);
      break;
    default:
      Serial.println("Eixo inválido! Use valores entre 1 e 4.");
      break;
  }
}


// Move o servo diretamente até a posição final, em incrementos de ANGULO_MAXIMO
void Move_Servo_Completo(int eixo, int posicao) 
{
  if(flgmotorAtivo = false) 
  {
      ativaMotores();
  }
  posicao = constrain(posicao, 0, 180); // Limita a posição entre 0 e 180 graus
  posBracoDestino[eixo - 1] = posicao; // Atualiza a posição desejada

  while (posBracoAtual[eixo - 1] != posBracoDestino[eixo - 1]) {
    int direcao = (posBracoDestino[eixo - 1] > posBracoAtual[eixo - 1]) ? 1 : -1;
    posBracoAtual[eixo - 1] += direcao * min(ANGULO_MAXIMO, abs(posBracoDestino[eixo - 1] - posBracoAtual[eixo - 1]));
    switch (eixo) {
      case 1:
        ServoBraco01.write(posBracoAtual[0]);
        break;
      case 2:
        ServoBraco02.write(posBracoAtual[1]);
        break;
      case 3:
        ServoBraco03.write(posBracoAtual[2]);
        break;
      case 4:
        ServoBraco04.write(posBracoAtual[3]);
        break;
    }
    Serial.print("Servo ");
    Serial.print(eixo);
    Serial.print(" movido para posição: ");
    Serial.println(posBracoAtual[eixo - 1]);

    delay(DELAY_TEMPO); // Aguarda entre os incrementos
  }

  Serial.print("Servo ");
  Serial.print(eixo);
  Serial.print(" chegou na posição final: ");
  Serial.println(posicao);
}

// Move todos os servos em uma única etapa incremental
void Move_Servo_Incremental() 
{

  if(flgmotorAtivo = false) 
  {
      ativaMotores();
  }
  bool movimentoRestante = false;

  for (int eixo = 0; eixo < 4; eixo++) {
    if (posBracoAtual[eixo] != posBracoDestino[eixo]) {
      int direcao = (posBracoDestino[eixo] > posBracoAtual[eixo]) ? 1 : -1;
      posBracoAtual[eixo] += direcao * min(ANGULO_MAXIMO, abs(posBracoDestino[eixo] - posBracoAtual[eixo]));
      
      switch (eixo + 1) {
        case 1:
          ServoBraco01.write(posBracoAtual[0]);
          break;
        case 2:
          ServoBraco02.write(posBracoAtual[1]);
          break;
        case 3:
          ServoBraco03.write(posBracoAtual[2]);
          break;
        case 4:
          ServoBraco04.write(posBracoAtual[3]);
          break;
      }
      Serial.print("Servo ");
      Serial.print(eixo + 1);
      Serial.print(" movido para posição: ");
      Serial.println(posBracoAtual[eixo]);
      
      movimentoRestante = true; // Ainda há movimento para ser concluído
    }
  }

  //if (!movimentoRestante) {
    //Serial.println("Todos os servos alcançaram suas posições finais.");
  //}

  delay(DELAY_TEMPO); // Aguarda entre os incrementos
}

/*Rotina que posiciona o equipamento no local correto*/
void Calibracao()
{
  Serial.println("Iniciando Calibra��o");
  NextionShow("CALIB"); //Chamando tela calibra��o

  Serial.println("Retornando carro");
  
  while (!flgFIMA1 )
  {
      MovePassoA_Esq(MOVPASSOS);
      Le_FimCurso();
  }
  Serial.println("Avancando carro");
  while (!flgFIMA2 )
  {
      MovePassoA_Dir(MOVPASSOS);
      Le_FimCurso();
  }
  
  Serial.println("Retornando carro");
  while (!flgFIMA1 )
  {
      MovePassoA_Esq(1);
      Le_FimCurso();
  }
  Serial.println("Fim de calibracao");
  
}


void PosicaoCentral()
{
  Serial.println("Indo para Posicao Central");
  MovePassoA_Esq(MOVPASSOS);
  //Le_FimCurso();

  Serial.println("Move posicao Central carro");
  RetornaCarro();
 
  MovePassoA_Dir((int)posFIMSERVA / 2);
  //Le_FimCurso();
  
}



void MovePasso01()
{
//  stepper.step(10);
}

void Start_I2C() 
{
//  Wire.begin(pinSDA, pinSCL);
}

void setup() {
 
  Start_Serial();
  Start_Nextion();
  NextionShow("Splah");
  delay(100);
  Serial.println("Starting modules...");
  Speak_Start();  
  Beep(); 
  Start_Printer();
  Start_FIMDECURSO();
 

  Start_Motor01();

  Start_RELES();

  Start_DHT22(); 
  
  

  Start_SD();
  
  
  Start_I2C(); //Start de comunica��o I2C
  Start_Servos();
  

  // Font options
  //MovePasso01();
  //ImprimeEtiqueta();
  Le_DHT22();
  Reset();
  NextionShow("CALIB");  
  delay(100);
  //Calibracao(); /*Calibra��o do equipamento*/
  
  //Calibra_Servos();
  
  delay(100);
  NextionShow("Menu");
  desativaMotores();
  ImprimeStart();
  Sound('a');
  Sound('b');
  Sound('c');
  WellComeConsole(); 
  
}

void sendStringToI2C(int address, char* data) {
  Wire.beginTransmission(address);
  Wire.write(data);
  Wire.endTransmission();
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
  printer.println(' ');
}

void ImprimeReset()
{
  printer.wake(); // MUST wake() before printing again, even if reset  
  printer.setDefault(); // Restore printer to defaults
  printer.println(' ');  
}

void ImprimeStart() 
{
  // Test inverse on & off
 //ImprimeInvertido("DOCTOR");  
 ImprimeGrande("Doctor");
 ImprimeMedio("===========");
 ImprimePequeno(" ");
 ImprimePequeno("Equipamento DOCTOR do TCC da FATEC-RP");
 ImprimePequeno("Inicializacao do equipamento concluido");
 ImprimePequeno(" ");
 char info[40];
 memset(info,'\0',sizeof(info));
 sprintf(info,"Versao:%s.%s",Versao,Release);
 
 ImprimeMedio(info);  
 //ImprimeBarra(etiqueta.barra);
 ImprimeAvanco();
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

// Fun��o para definir o modo de impress�o (normal ou de teste)
void setPrintMode(uint8_t mode) {
  printer.sleep(); // Colocar a impressora em modo de espera antes de enviar os comandos
  if (mode == 1) {
    printer.test();
  } else {
    printer.normal();
  }
}

// Fun��o para definir o tamanho do papel (largura e altura em pontos)
void setPaperSize(uint8_t width, uint8_t height) {
  printer.sleep(); // Colocar a impressora em modo de espera antes de enviar os comandos
  printer.setSize('M'); // Configurar o tamanho da fonte para m�dio
  printer.feed(1); // Avan�ar uma linha
 // printer.writeBytes(27, 87, width, height, 0); // Enviar comando para definir o tamanho do papel
}

// Fun��o para cortar o papel (parcial ou totalmente)
void cutPaper(uint8_t mode) {
  printer.sleep(); // Colocar a impressora em modo de espera antes de enviar os comandos
  if (mode == 1) {
    //printer.partialCut(); // Cortar parcialmente
  } else {
    printer.feed(3); // Avan�ar tr�s linhas
   // printer.fullCut(); // Cortar completamente
  }
}

// Fun��o para imprimir um c�digo QR (tamanho em pontos, n�vel de corre��o de erro, dados)
void printQR(uint8_t size, uint8_t correction, const char* data) {
  printer.sleep(); // Colocar a impressora em modo de espera antes de enviar os comandos
  printer.setSize('M'); // Configurar o tamanho da fonte para m�dio
  printer.feed(1); // Avan�ar uma linha
 // printer.printBarcode(data, QR, size, correction); // Enviar comando para imprimir o c�digo QR
}



void MovePassoA_Dir(int passo)
{
  
    // 1 rotation counterclockwise:  
    //Serial.println("counterclockwise");    
    Passo01.step(passo);  
    posSERVA = posSERVA+ passo;
    Serial.print("POSSERVA=");  
    Serial.println(posSERVA);
    //delay(100); 
}

void MovePassoA_Esq(int passo)
{
  // 1 rotation clockwise: 
 
  Passo01.step(-passo); 
  posSERVA = posSERVA- passo;
  Serial.print("POSSERVA=");  
  Serial.println(posSERVA);
  //delay(100); 
}

void RetornaCarro()
{
  Le_FimCurso();
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

/*Carrega configura��o do modulo local*/
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

//Carrega a aplica��o para o SD
void LOAD(File root, char *sMSG1)
{
 
  //Imprime(1, "Carregando APP...");
  Serial.println("Carregando APP..");
  NextionShow("LOAD");
 
  //Realiza opera��o de Loop
  LOADLoop(root, sMSG1);
  NextionShow("Menu");

}

//DeleteFile
void DeleteFile(File root, char *sMSG1)
{
 
  //Imprime(1, "Carregando APP...");
  Serial.println("DeleteFile..");
  NextionShow("LOAD");
 
  //Realiza opera��o de Loop
  LOADLoop(root, sMSG1);
  NextionShow("Menu");
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
    Serial.println("Iniciou bloco de grava��o do arquivo");
    flgEscape = false; //Sai quando verdadeiro
    //Loop
    while (!flgEscape)
    {
      LOADLeituras();
      //Realiza analise das informa��es encontradas
      LOADAnalisa(&farquivo);      
      //arquivo.println("Leitura Potenciometro: ");
    }
    Serial.println("Finalizou bloco de grava��o do arquivo");
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
  Le_Serial1();
  
  
  Chk_Beep();
  Le_DHT22();
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
  Le_Serial1();
  
  
  
  
  Le_DHT22();
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

/*Captura a pagina em que o nextion esta*/
void showPageId() {
  char pageId[10];
  Serial1.print("sendme\n"); // Enviar o comando "sendme" para solicitar o ID da p�gina atual
  delay(10);
  while (Serial1.available() > 0) { // Esperar at� que haja dados dispon�veis na serial
    char c = Serial1.read();
    if (c == 0xFF) { // Verificar se � um byte de in�cio de mensagem
      int i = 0;
      while (Serial1.available() > 0 && i < sizeof(pageId) - 1) { // Ler o ID da p�gina at� o final da mensagem
        c = Serial1.read();
        if (c == 0xFF) { // Verificar se � um byte de in�cio de mensagem (pode ocorrer dentro da mensagem)
          i = 0;
        } else if (c == '\n') { // Verificar se � o final da mensagem
          pageId[i] = '\0'; // Adicionar terminador de string ao final do ID da p�gina
          break;
        } else {
          pageId[i] = c;
          i++;
        }
      }
      Serial.print("ID da pagina atual: ");
      Serial.println(pageId);
    }
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
  Serial1.print(cmd);  
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
  Serial1.print(cmd);  
}

void NextionWAITESC()
{
  
}

void NextionMensage(String info)
{
  char strFF = 0xFF;
  Serial1.print("page MSG"+String(strFF)+String(strFF)+String(strFF));  
  delay(100);
  String cmd;
  
  cmd = "MSGtxt.txt=\""+info+"\""+String(strFF)+String(strFF)+String(strFF);
  Serial.println(cmd);  
  Serial1.print(cmd);
}

void NextionMensageSTOP(String info)
{
  char strFF = 0xFF;
  Serial1.print("page MSG"+String(strFF)+String(strFF)+String(strFF));  
  delay(100);
  String cmd;
  
  cmd = "MSGtxt.txt=\""+info+"\" "+String(strFF)+String(strFF)+String(strFF);
  Serial.println(cmd);  
  Serial1.print(cmd);
  NextionWAITESC();
}



//Reseta todas as entradas para o valor padr�o
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
  /* incluir sub comandos de inicializa��o*/
  passoA = 10;
  passoB = 10;
  passoC = 10;

  RetornaCarro();

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
  Serial.println("SENDMSG=DEV,MSG - Envia uma mensagem para uma Serial:");
  Serial.println("   DEV: Dispositivo (1 = Serial2)");
  Serial.println("   MSG: Mensagem a ser enviada, termina antes do '\\n'");  
  Serial.println("MENSAGEM - Mostra mensagem");
  Serial.println("RESET - RESETA O AMBIENTE");
  Serial.println("MOPERACAO - MODO OPERACIONAL");
  Serial.println("MOPERACAO - MODO CONFIG");
  Serial.println("RELE01 - ACIONAMENTO RELE01");
  Serial.println("RELE02 - ACIONAMENTO RELE02");
  Serial.println("DELFILE - REMOVE ARQUIVO");
  Serial.println("MOVESERVO=NRO_SERVO,ANGULO - Move um servo para o ângulo especificado");
  Serial.println("MOVEDIR=NRO_PASSOS - Move o motor de passo para a direita");
  Serial.println("MOVEESQ=NRO_PASSOS - Move o motor de passo para a esquerda");
  Serial.println("POSFIMSERVA - Informa Posicao final do carro ");
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
    //Adicionando informa��o ao Info
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

    // MOVEDIR=NRO_PASSOS
    if (strstr(Buffer, "MOVEDIR=") != NULL)
    {
      char *paramStart = strstr(Buffer, "=");
      if (paramStart != NULL)
      {
        paramStart++; // Avança para depois do '='
        int passos = atoi(paramStart);
        MovePassoA_Dir(passos);
        resp = true;
        Serial.print("Comando MOVEDIR executado: ");
        Serial.print(passos);
        Serial.println(" passos para a direita.");
      }
    }

    // MOVEESQ=NRO_PASSOS
    if (strstr(Buffer, "MOVEESQ=") != NULL)
    {
      char *paramStart = strstr(Buffer, "=");
      if (paramStart != NULL)
      {
        paramStart++; // Avança para depois do '='
        int passos = atoi(paramStart);
        MovePassoA_Esq(passos);
        resp = true;
        Serial.print("Comando MOVEESQ executado: ");
        Serial.print(passos);
        Serial.println(" passos para a esquerda.");
      }
    }


    // Comando MOVESERVO
    if (strstr(Buffer, "MOVESERVO") != NULL) 
    {
      char *paramStart = strstr(Buffer, "=");
      if (paramStart != NULL) {
        paramStart++; // Avança para depois do '='
        int servo, angle;
        if (sscanf(paramStart, "%d,%d", &servo, &angle) == 2) {
          Carrega_Movimento(servo, angle);
          resp = true;
          Serial.print("Comando MOVESERVO executado: Servo ");
          Serial.print(servo);
          Serial.print(", Ângulo ");
          Serial.println(angle);
        } else {
          Serial.println("Erro: parâmetros inválidos para MOVESERVO. Use MOVESERVO=NRO_SERVO,ANGULO");
        }
      }
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

    // Implementação do comando SENDMSG
    if (strstr(Buffer, "SENDMSG=") != NULL) 
    {
      char *paramStart = strstr(Buffer, "=");
      if (paramStart != NULL) 
      {
          paramStart++; // Avança para depois do '='
          char dev[10];
          char mensagem[200];
          memset(dev, 0, sizeof(dev));
          memset(mensagem, 0, sizeof(mensagem));

          // Analisa os parâmetros DEV e MSG
          if (sscanf(paramStart, "%[^,],%[^\n]", dev, mensagem) == 2) 
          {
              int dispositivo = atoi(dev);

              if (dispositivo == 1) {
                  EnviaParaSerial2(mensagem);
                  //EnviaParaSerial2("\n");
                  resp = true;
              } else 
              {
                  Serial.println("Dispositivo inválido ou não implementado!");
              }
          } else 
          {
              Serial.println("Formato do comando inválido. Use SENDMSG=DEV,MSG");
          }
      }
    }

    if (strstr( Buffer, "MENSAGEM=") != 0)
    {
      char sMSG1[20];
      char sMSG2[20];
      char posequ[20];
      memset(posequ,'\0',sizeof(posequ));
      Serial.println("Achou MENSAGEM");
      // Encontrar o caractere '=' em Buffer
      char *posicao = strstr(Buffer, "=");
      if (posicao != NULL) {
        // Copiar a partir de '=' para posequ
        strcpy(posequ, posicao);
        Serial.println("Valor copiado para posequ:");
        Serial.println(posequ);
        Serial.print("sMSG1=");
        Serial.println(posequ);
        NextionMensage(String(posequ));
        NextionShow('MSG');
        delay(100);
        NextionFieldText('MSGtxt',posequ);
        resp = true;
      } else {
        // Caso não encontre '='
        posequ[0] = '\0'; // Inicializa como string vazia
        Serial.println("Caractere '=' não encontrado no Buffer.");
        resp = false;
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

    //POSFIMSERVA - FIM de curso posicao
    if (strcmp( Buffer, "POSFIMSERVA\n") == 0)
    {
      Serial.print("POSFIMSERVA=");
      Serial.println(posFIMSERVA);
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



void Le_Serial1() {
    char key;
    static int index = 0;
    static char buffer[10];

    while (Serial1.available() > 0) {
        key = Serial1.read();

        // Armazena os bytes lidos no buffer
        buffer[index++] = key;

        // Verifica se recebemos a sequ�ncia completa (3 bytes iniciais + ID da p�gina + 3 bytes finais)
        if (index >= 7) {
            // Verifica se a mensagem corresponde � mudan�a de p�gina
            if (buffer[0] == 0x00 && buffer[1] == 0x00 && buffer[2] != 0xFF) {
                // Extra�mos o ID da p�gina (4� byte)
                int pageId = buffer[2];
                Serial.print("Tela mudada para ID: ");
                Serial.println(pageId);

                // Se voc� quiser exibir o nome da tela, pode associar o ID da p�gina ao nome
                switch (pageId) {
                    case 0:
                        Serial.println("Tela: Home");
                        break;
                    case 1:
                        Serial.println("Tela: Configuracoes");
                        break;
                    // Adicione outros casos para suas telas conforme necess�rio
                    default:
                        Serial.println("Tela: Desconhecida");
                        break;
                }
            }

            // Reinicia o �ndice do buffer para capturar a pr�xima mensagem
            index = 0;
        }
    }
}




// Função para enviar mensagem para Serial2
void EnviaParaSerial2(const char* mensagem) {
    Serial2.println(mensagem);
    //Serial2.print("\n\r");
    Serial.print("Mensagem enviada para Serial2: ");
    Serial.println(mensagem);
}


void Le_Serial2() 
{
  // Buffer local, fica dentro da função.
  // A cada vez que você entra na função, ele inicia vazio.
  char bufferprint[1000];  // Ajuste o tamanho conforme necessário
  size_t pos = 0;         // Índice de escrita

  // Enquanto houver dados na Serial2, faça a leitura
  while (Serial2.available() > 0) 
  {
    char key = Serial2.read();
    Serial.print(key);
    // Ignora '\r' (caractere de retorno de carro).
    // Em muitos casos (Windows), as quebras de linha chegam como "\r\n"
    if (key == '\r') {
      continue; 
    }

    // Se chegar '\n', significa que finalizamos uma linha
    if (key == '\n') {
      // Finaliza a string em C, colocando \0
      bufferprint[pos] = '\0';
      
      // Chama a função que espera um char*
      //ImprimeMedio(bufferprint);
      //ImprimePequeno(bufferprint);

      // Exemplos de debug:
      Serial.println();
      Serial.print("MODULO1:");
      //Serial.println(bufferprint);

      // Zera o índice do buffer para começar uma nova linha
      pos = 0;
    }
    else 
    {
     
      // Se ainda houver espaço no buffer
      if (pos < (sizeof(bufferprint) - 1)) {
        bufferprint[pos] = key;  // Armazena o caractere
        pos++;
        
        // Opcional: imprimir no Serial
        //Serial.print(key); 
      } 
      else 
      {
        // Caso o buffer esteja cheio, trate como achar melhor.
        // Aqui, vamos: 
        // 1) Fechar a string
        // 2) Enviar para ImprimeMedio
        // 3) Avisar overflow
        // 4) Resetar o buffer
        
//MODULO1:pH calculado: 0

//MODULO1:Temperatura (Etemp): 5

        ImprimePequeno(bufferprint);
        bufferprint[pos] = '\0';
        Serial.println("\n[WARN] Buffer cheio, resetando...");
        pos = 0;
      }
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
  if(oldt != t)
  {
    Serial.print("TEMPERATURA:");
    Serial.println(temperatura);
    NextionFieldText("temp",temperatura);
    oldt = t;
  }
  //sprintf(temperatura,"%f",h);
  char humidade[10];
  memset(humidade,'\0',sizeof(humidade));
  dtostrf(h, 3, 2, humidade);  // 8 char min total width, 6 after decimal
  if(oldh!=h)
  {
    Serial.print("Humidade:");
    Serial.println(humidade);
    //delay(100);
    NextionFieldText("hum",humidade);
    oldh = h;
  }
  
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
     posSERVA = 0;
     lastflgFIMA1 = flgFIMA1;
  }
  if(lastflgFIMA2!=flgFIMA2)
  {
    Serial.print("flgFIMA2=");  Serial.println(flgFIMA2==HIGH?"ON":"OFF");
    posFIMSERVA = posSERVA;
    Serial.print("POSFIMSERVA=");
    Serial.println(posFIMSERVA);
    lastflgFIMA2 = flgFIMA2;
  }
}






void Leituras()
{
  //Le_Teclado();
  Le_Serial();
  Le_Serial1();
  Le_Serial2();
  
  Le_DHT22();
  Le_FimCurso();
  Chk_Beep();
  //Serial.print('.');
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
  //Serial ÃƒÂ© padrao para todos os retornos
  Serial.println("$>");
}

void loop() 
{
  Leituras();
  Move_Servo_Incremental(); // Executa movimentos incrementais para todos os servos
  Analisa();
  contciclo = contciclo+1;
  
  if  (contciclo > MAXCICLO) //Modulo de contagem de ciclos
  {
    Le_DHT22();
    //MostraTemperatura
    contciclo = 0;
  }
  
  
}
