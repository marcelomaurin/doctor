/* Arduino Nano required */

#include <Wire.h>

#include "max6675.h"
#include <EEPROM.h>
#include <SimpleDHT.h>



#define DHTTYPE    DHT22     // DHT 22 (AM2302)

#define MAXSIZEBUFF 200
#define R 8.314
#define F 96485
#define N 1 // Número de elétrons transferidos na reação do H+ (hidrogênio)
#define VERSAO "1.0"

//Pinout
#define SO  12
#define CS  10
#define CLK  3
#define PINPH  A0
#define PINDHT22 A2


char buffer[MAXSIZEBUFF]; // buffer de dados

MAX6675 sensor(CLK, CS, SO);


SimpleDHT22 dht22(PINDHT22);



float valor_calibracao = 21.34; // Fator de calibração

int contagem = 0;           // Variável de contagem
float soma_tensao = 0;      // Variável para soma de tensão
float media = 0;            // Variável que calcula a media
float entrada_A0;           // Variável de leitura do pino A0
float tensao;               // Variável para conversão em tensão
unsigned long tempo;        // Float tempo
float valor_pH;             // PH
float Temperatura;          // Temperatura
float media_analogica = 0;


void Start_DHT()
{

}

void Start_PH() {
  pinMode(PINPH, INPUT);
}

void Start_Serial() {
  Serial.begin(115200);
}

void WellCome()
{
  Serial.println("Leitor de PH");
  Serial.println("Desenvolvido como trabalho de graduação");
  Serial.println("FATEC Ribeirão Preto - SP");
  Serial.print("Versão do Firmware:");
  Serial.println(VERSAO);
  Serial.println("Iniciando carga do software");
 
}


// Função para armazenar um valor float na EEPROM e atribuí-lo à variável valor_calibracao
void Start_EEPROM() {
  // Lê o valor da EEPROM e o atribui à variável valor_calibracao
  EEPROM.get(0, valor_calibracao);

  Serial.print("Valor de calibração armazenado: ");
  Serial.println(valor_calibracao);
}

void Start_Ambiente() {
  memset(buffer, '\0', MAXSIZEBUFF); // Inicializa buffer de dados
  // Coleta valor de calibração
  Start_EEPROM();
}

void setup() {
  Start_Serial();

  WellCome();
  Start_Ambiente();
  Start_PH();  
  Start_DHT();
  delay(2000);

  Serial.println("Inicializacao finalizada.");
}

void Grava_ValorCalibracao(float valor) {
  // Armazena o valor float na EEPROM
  EEPROM.put(0, valor);
}

void Le_Temperatura() {
  Temperatura = sensor.readCelsius();
  Serial.print("Graus C = ");
  Serial.println(Temperatura);
}

float calcularInclinacao(float temperaturaCelsius) {
    float temperaturaKelvin = temperaturaCelsius + 273.15;
    return (R * temperaturaKelvin) / (F * N); // Calcula a inclinação para pH
} 

void Le_DHT()
{
 // read without samples.
  // @remark We use read2 to get a float data, such as 10.1*C
  //    if user doesn't care about the accurate data, use read to get a byte data, such as 10*C.
  float temperature = 0;
  float humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht22.read2(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT22 failed, err="); Serial.print(SimpleDHTErrCode(err));
    Serial.print(","); Serial.println(SimpleDHTErrDuration(err)); delay(2000);
    return;
  }
  
  Serial.print("Sample OK: ");
  Serial.print((float)temperature); Serial.print(" *C, ");
  Serial.print((float)humidity); Serial.println(" RH%");
  
}

void Le_PH() {
    soma_tensao = 0;  // Inicia soma_tensão em 0
    contagem = 0;     // Inicia a contagem em 0

    while (contagem < 10) {
        entrada_A0 = analogRead(PINPH);               // Lê a entrada analógica
        media_analogica = (entrada_A0 + media_analogica) / 2;

       
       
        contagem++;                                   // Soma 1 à variável de contagem

        delay(100);                                   // Aguarda para próxima leitura
    }
    tensao = (media_analogica * 5.0) / 1024.0 /6;    // Converte em tensão, o valor lido
                      
    Serial.print("tensao: ");
    Serial.println(tensao);

    // Calcula o pH básico
    float pH_basico = -5.70 * tensao + valor_calibracao;
    Serial.print("pH_basico: ");
    Serial.println(pH_basico);
    float inclinacao = calcularInclinacao(Temperatura);
    valor_pH = (tensao - 7) / inclinacao; // Calcula valor de pH usando a inclinação corrigida
    Serial.print("pH corrigido: ");
    Serial.println(valor_pH);
}



void Le_Serial() {
  static String inputString = "";         // String para armazenar os dados de entrada
  char carac;

  while (Serial.available() > 0) {       // Se houver dados disponíveis na serial
    carac = (char)Serial.read();         // Lê o próximo caractere

    if (carac != '\n') {                 // Se não for o fim da linha
      inputString += carac;              // Adiciona o caractere à string de entrada
    } else {                             // Se for o fim da linha
      if (inputString.startsWith("calibrar:")) { // Verifica se a string começa com "calibrar:"
        String valorString = inputString.substring(9); // Extrai o valor após "calibrar:"
        float valor = valorString.toFloat();           // Converte a string para float
        Grava_ValorCalibracao(valor);                  // Chama a função para gravar o valor
        Serial.print("Novo valor de calibração: ");
        Serial.println(valor);
      }
      inputString = "";                  // Limpa a string de entrada para a próxima leitura
    }
  }
}

void loop() {
  Le_PH();
  Le_DHT();
  Le_Temperatura();
  Le_Serial();
 

  // Aguarda para próxima leitura
  delay(2500);
}
