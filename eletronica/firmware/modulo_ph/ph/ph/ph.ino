#include <Arduino.h>
#include <Stepper.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>

// Definição dos endereços na EEPROM para cada valor (cada float ocupa 4 bytes)
#define EEPROM_ADDR_PH7   0
#define EEPROM_ADDR_PH4   4
#define EEPROM_ADDR_PH10  8

#define TEMPODELAY 5000


// Mapa de pinos
#define pin_IN1 A0       // Pino IN1 do ULN2003
#define pin_IN2 A1       // Pino IN2 do ULN2003
#define pin_IN3 A2       // Pino IN3 do ULN2003
#define pin_IN4 A3       // Pino IN4 do ULN2003
#define pin_FIMCURSO 5   // Pino do sensor de fim de curso
#define pin_TEMP A4
#define  pin_PH A5
#define OFFSET 0.8


// Definição dos pinos para a SoftwareSerial
#define RX_PIN 9
#define TX_PIN 11




#define pin_ONEWIRE A4   // Pino para o barramento 1-Wire (temperatura)

//2048
#define PASSOS_POR_VOLTA 128 // Número total de passos para o motor de passo ULN2003

#define VERSAO "0.9"          // Versão do módulo

#define TEMPO_DEBOUNCE 50     // Tempo para debounce (em ms)

OneWire oneWire(pin_TEMP);
DallasTemperature sensors(&oneWire);


// Instância da SoftwareSerial
SoftwareSerial serialSoftware(RX_PIN, TX_PIN);

long NUM_PASSOS = 400;         // Número de passos por comando
long FIM_CURSO = 9800;       //Posicao final do curso


char buffer[300] ={0};           // Buffer para dados recebidos da serial
size_t buffer_pos = 0; // Posição atuRETCARRO al no buffer

//Flags de controle
bool flagMoverLong = false;    // Controle de movimento Longo
bool flagMoverDir = false;    // Controle de movimento para a direita
bool flagMoverEsq = false;    // Controle de movimento para a esquerda
bool flagRetCarro = false; // Controle de retorno do carro
bool flagMoverFimCursoEsq = false; // Novo flag para o comando MOVERFIMCURSOESQ

bool flgComando = false; // Flag que indica se um comando está em execução

int estadoAtualFimCurso = -1;

int contadorpassosDados = 0;          // Contador de passos dados
int passosDados = 0;          // Contador de passos dados
int estadoAnteriorFimCurso = 255; // Inicializa com HIGH
unsigned long ultimoTempoFimCurso = 0; // Controle de tempo para debounce
// Adicionado flag flg_PH
bool flg_PH = false; // Inicia o flag como falso

int flgCallModulo = 0; //Chamada de modulo de CallModulo

float Etemp = 0; //Temperatura
float pHValue = 0; //PH



// Cria uma instância do motor de passo
Stepper motor(PASSOS_POR_VOLTA, pin_IN1, pin_IN3, pin_IN2, pin_IN4);


const float VREF = 5.0;  // Tensão de referência do Arduino
const int ADC_RES = 1023; // Resolução do ADC (10 bits)

float calibracao_ph7 = 4.15;   // Tensão obtida em solução de calibração pH 7
float calibracao_ph4 = 4.71;   // Tensão obtida em solução de calibração pH 4
float calibracao_ph10 = 3.75;  // Tensão obtida em solução de calibração pH 10

//Funcoes
void MoverLongEsq();
void MoverLongDir();
void MoverEsq();
void MoverDir();
void MoverRetCarro();
void MoverFimCursoEsq();
void WaitModulo();
void RetModulo();
void FinalizaCOMANDO();
void salvarCalibracoes();
void carregarCalibracoes();


// -----------------------------------------------------------------------

// Função para salvar os valores de calibração na EEPROM
void salvarCalibracoes() {
  EEPROM.put(EEPROM_ADDR_PH7, calibracao_ph7);
  EEPROM.put(EEPROM_ADDR_PH4, calibracao_ph4);
  EEPROM.put(EEPROM_ADDR_PH10, calibracao_ph10);
  EnviarSerialLn("Calibrações salvas na EEPROM.");
}

// Função para carregar os valores de calibração da EEPROM
void carregarCalibracoes() {
  EEPROM.get(EEPROM_ADDR_PH7, calibracao_ph7);
  EEPROM.get(EEPROM_ADDR_PH4, calibracao_ph4);
  EEPROM.get(EEPROM_ADDR_PH10, calibracao_ph10);
  EnviarSerialLn("Calibrações carregadas da EEPROM:");
  EnviarSerial("calibracao_ph7: ");
  EnviarSerialLn(calibracao_ph7);
  EnviarSerial("calibracao_ph4: ");
  EnviarSerialLn(calibracao_ph4);
  EnviarSerial("calibracao_ph10: ");
  EnviarSerialLn(calibracao_ph10);
}


void FinalizaCOMANDO()
{
  flgComando = false;
  EnviarSerialLn(" ");
  EnviarSerialLn("flgComando=false");
  //delay(100);
}

// Função para enviar dados para a Serial padrão e SoftwareSerial
void EnviarSerial(const String& mensagem) {
    Serial.print(mensagem);
    serialSoftware.print(mensagem);
    
}

// Função para enviar dados para a Serial padrão e SoftwareSerial
void EnviarSerial(int info) {
    Serial.print(info);
    serialSoftware.print(info);
    
}

void EnviarSerialLn(const String& mensagem) {
    Serial.println(mensagem);
    serialSoftware.println(mensagem);
    delay(100);
    
}

void EnviarSerialLn(int info) {
    Serial.println(info);
    serialSoftware.println(info);
    delay(100);
}

void EnviarSerial(float info) {
    Serial.print(info, 3);            // Mostra com 3 casas decimais
    serialSoftware.print(info, 3);    // Também na SoftwareSerial
    delay(100);
}

void EnviarSerialLn(float info) {
    Serial.println(info, 3);            // Mostra com 3 casas decimais
    serialSoftware.println(info, 3);    // Também na SoftwareSerial
    delay(100);
}


// Função para ativar a comunicação serial
void Start_serial() {
    Serial.begin(9600);
    serialSoftware.begin(9600);
    
}

// Função para configurar o sensor de fim de curso
void ConfiguraSensorFim() {
    pinMode(pin_FIMCURSO, INPUT_PULLUP);
    EnviarSerialLn("Sensor de fim de curso configurado.");
}

// Função chamada em caso de erro
void ERRO_SISTEMA(const char* mensagem) {
    EnviarSerialLn(F("=== ERRO NO SISTEMA ==="));
    EnviarSerialLn(mensagem);
    EnviarSerialLn(F("======================="));
}

// Função de boas-vindas
void WellCome() {
    EnviarSerialLn(F(" "));
    EnviarSerialLn(F("===================================="));
    EnviarSerialLn(F("Modulo de controle de PH do sistema DOCTOR"));
    EnviarSerialLn(F("Autor: Marcelo Maurin Martins"));
    EnviarSerial(F("Versao: "));
    EnviarSerialLn(VERSAO); // VERSAO já esta armazenado em PROGMEM
    EnviarSerialLn(F("===================================="));
}


void HelpComandos() {
    EnviarSerialLn(F("=== LISTA DE COMANDOS DISPONÍVEIS ==="));
    EnviarSerialLn(F("MOVERDIR         - Move o motor para a direita."));
    EnviarSerialLn(F("MOVERLONGDIR     - Move o motor longamente para a direita."));
    EnviarSerialLn(F("MOVERESQ         - Move o motor para a esquerda."));
    EnviarSerialLn(F("MOVERLONGESQ     - Move o motor longamente para a esquerda."));
    EnviarSerialLn(F("MOVERFIMCURSOESQ - Move o motor ate o fim de curso a esquerda."));
    EnviarSerialLn(F("RETCARRO         - Retorna o carro ao ponto inicial."));
    EnviarSerialLn(F("LEITURA_PH       - Exibe o valor de pH atual."));
    EnviarSerialLn(F("LEITURA_TEMP     - Exibe a temperatura atual."));
    EnviarSerialLn(F("ATIVA_PH         - Ativa a leitura continua de pH."));
    EnviarSerialLn(F("DESATIVA_PH      - Desativa a leitura continua de pH."));
    EnviarSerialLn(F("CALIBRAR         - Inicia a calibracao geral."));
    EnviarSerialLn(F("CLBPH4           - Inicia a calibracao com solucao de pH 4."));
    EnviarSerialLn(F("CLBPH7           - Inicia a calibracao com solucao de pH 7."));
    EnviarSerialLn(F("CLBPH10          - Inicia a calibracao com solucao de pH 10."));
    EnviarSerialLn(F("ENXAGUE          - Inicia o processo de enxague."));
    EnviarSerialLn(F("CALLMODULO       - Inicia o processo de analise."));
    EnviarSerialLn(F("WAITMODULO       - Mostra se processo acabou."));
    EnviarSerialLn(F("RETMODULO        - Mostra resultado obtido."));
    EnviarSerialLn(F("MAN              - Exibe esta lista de comandos."));
    EnviarSerialLn(F("====================================="));
    FinalizaCOMANDO();
}


// Função para verificar o estado do fim de curso com debounce
void LerFimCurso() {
    estadoAtualFimCurso = digitalRead(pin_FIMCURSO);
    if (millis() - ultimoTempoFimCurso > TEMPO_DEBOUNCE) {
        if (estadoAtualFimCurso != estadoAnteriorFimCurso) {
            estadoAnteriorFimCurso = estadoAtualFimCurso;
            ultimoTempoFimCurso = millis();

            EnviarSerial(F("FLGFIMM1="));
            EnviarSerialLn(estadoAtualFimCurso?'ON':'OFF');

            if (estadoAtualFimCurso == LOW) {
                //ERRO_SISTEMA("Fim de curso acionado. Parando o motor.");
                Serial.println("Fim de curso acionado. Parando o motor.");
                flagMoverDir = false;
                flagMoverLong = false;
                passosDados = 0;
                contadorpassosDados = 0;
                FinalizaCOMANDO();
            } else {
                EnviarSerialLn(F("Fim de curso liberado. Movimento permitido."));
            }
        }
    }
}

void LerTemp() {
    sensors.requestTemperatures(); // Solicita a leitura do sensor
    Etemp = sensors.getTempCByIndex(0); // Obtém a temperatura em graus Celsius

    // Se a leitura falhar, DS18B20 retorna -127°C
    if (Etemp == -127.0) {
        Serial.println("Erro na leitura do sensor DS18B20!");
        return;
    }

    float Etemp = 23.56;  // Exemplo de valor
    char tempString[10];  // Buffer para a string
    
    // Converte o float para string com largura mínima de 4 e 2 casas decimais
    dtostrf(Etemp, 4, 2, tempString);
    
    EnviarSerial("Temperatura: ");
    EnviarSerial(tempString);
    EnviarSerialLn(" °C");
}

float CorrecaoParabolica(float phMedido) {
    float a = 0.0583;
    float b = -0.8667;
    float c = 0.3333;

    // Proteção para garantir que está no intervalo 0 a 14
    if (phMedido < 0) phMedido = 0;
    if (phMedido > 14) phMedido = 14;

    float ajuste = a * phMedido * phMedido + b * phMedido + c;
    // somente valores positivos
    if(ajuste<0) 
    {
      ajuste = 0;
    }
    return ajuste;
}

float CalcularOffset(float phMedido) {
    // Limita primeiro!
    if (phMedido < 0) phMedido = 0;
    if (phMedido > 14) phMedido = 14;

    float m = 0.0050;
    float b = 0.020;

    float offset = m * phMedido + b + CorrecaoParabolica(phMedido);
    if(offset>0) 
    {
      offset = 0;
    }
    Serial.print("offset: ");
    Serial.println(offset, 3);
    return offset;
}


float LerPH(float temperatura) {
    const int numAmostras = 20; 
    long somaLeituras = 0;
    delay(TEMPODELAY);

    for (int i = 0; i < numAmostras; i++) {  
        int leitura = analogRead(pin_PH);
        somaLeituras += leitura;
        delay(500);
    }

    float valorMedio = somaLeituras / float(numAmostras); 
    float tensao = (valorMedio * 5.0) / 1023.0; 

    Serial.print("Voltagem: ");
    Serial.println(tensao, 3);

    // Verifica se os valores de calibração são válidos
    if (calibracao_ph7 == 0.0 || calibracao_ph4 == 0.0 || calibracao_ph10 == 0.0) {
        Serial.println("Erro: Calibração inválida!");
        return -1.0;
    }

    // Determina a inclinação entre os pontos de calibração
    float slope = (10.0 - 4.0) / (calibracao_ph10 - calibracao_ph4); 
    float intercept = 7.0 - (slope * calibracao_ph7);

    // Cálculo do pH
    float phMedido = slope * tensao + intercept;
    Serial.print("phMedido: ");
    Serial.println(phMedido, 3);

    // Compensação de temperatura
    //float phCorrigido = (phMedido + ((Etemp - 25.0) / 10.0) * 0.025);
    float phCorrigido = (phMedido + ((Etemp - 25.0) / 10.0) * 0.03);
    Serial.print("phCorrigido1: ");
    Serial.println(phCorrigido, 3);
    phCorrigido = phCorrigido+CalcularOffset(phMedido);
    

    //Serial.print("pH Medido:");
    //Serial.println(phMedido, 2);
    
    Serial.print("Temperliq:");
    Serial.println(temperatura);
    Serial.print("PH Medido:");
    Serial.println(phCorrigido, 2);

    return phCorrigido;
}




float CalibrarPH4() {
    const int numAmostras = 20; 
    int somaLeituras = 0;
    delay(TEMPODELAY);
    for (int i = 0; i < numAmostras; i++) {  
        int leitura = analogRead(pin_PH);
        somaLeituras += leitura; // Soma os valores corretamente
        Serial.print("Leitura ");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.println(leitura);
        delay(500);
    }

    float valorMedio = somaLeituras / float(numAmostras); // Calcula a média correta
    float tensao = (valorMedio * 5.0) / 1023.0; // Ajuste do divisor ADC

    Serial.print("Média das Leituras: ");
    Serial.println(valorMedio);
    
    Serial.print("Voltagem Final: ");
    Serial.println(tensao, 3); // Mostra 3 casas decimais para maior precisão

    calibracao_ph4 = tensao; 
    salvarCalibracoes();
    return tensao;
}


float CalibrarPH7() {
    const int numAmostras = 20; 
    int somaLeituras = 0;
    delay(TEMPODELAY);
    for (int i = 0; i < numAmostras; i++) {  
        int leitura = analogRead(pin_PH);
        somaLeituras += leitura; // Soma os valores corretamente
        Serial.print("Leitura ");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.println(leitura);
        delay(500);
    }

    float valorMedio = somaLeituras / float(numAmostras); // Calcula a média correta
    float tensao = (valorMedio * 5.0) / 1023.0; // Conversão para tensão corrigida

    Serial.print("Média das Leituras: ");
    Serial.println(valorMedio);
    
    Serial.print("Voltagem Final: ");
    Serial.println(tensao, 3); // Mostra 3 casas decimais para maior precisão

    calibracao_ph7 = tensao; 
    salvarCalibracoes();
    return tensao;
}


float CalibrarPH10() {
    const int numAmostras = 6; 
    int somaLeituras = 0;
    delay(TEMPODELAY);
    for (int i = 0; i < numAmostras; i++) {  
        int leitura = analogRead(pin_PH);
        somaLeituras += leitura; // Soma os valores corretamente
        Serial.print("Leitura ");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.println(leitura);
        delay(500);
    }

    float valorMedio = somaLeituras / float(numAmostras); // Calcula a média correta
    float tensao = (valorMedio * 5.0) / 1023.0; // Ajuste do divisor ADC

    Serial.print("Média das Leituras: ");
    Serial.println(valorMedio);
    
    Serial.print("Voltagem Final: ");
    Serial.println(tensao, 3); // Mostra 3 casas decimais para maior precisão

    calibracao_ph10 = tensao; 
    salvarCalibracoes();
    return tensao;
}


void Enxaguar() {
    EnviarSerialLn(F("Iniciando processo de enxágue..."));

    // Exemplo de lógica para o processo de enxágue:
    // 1. Movimenta o motor para frente e para trás algumas vezes
    // 2. Pausa entre os movimentos
    for (int i = 0; i < 3; i++) {
        EnviarSerial(F("Enxágue etapa "));
        EnviarSerialLn(i + 1);

        // Movimento para a direita
        EnviarSerialLn(F("Movendo para a direita..."));
        for (int passos = 0; passos < 50; passos++) {
            MoverMotor(1, true); // 1 passo para a direita
            delay(10);          // Pequeno atraso entre os passos
        }

        // Movimento para a esquerda
        EnviarSerialLn(F("Movendo para a esquerda..."));
        for (int passos = 0; passos < 50; passos++) {
            MoverMotor(1, false); // 1 passo para a esquerda
            delay(10);           // Pequeno atraso entre os passos
        }

        delay(1000); // Pausa entre as etapas
    }
    FinalizaCOMANDO();
    EnviarSerialLn("Processo de enxágue concluído.");
}


void MoverLongEsq()
{
  EnviarSerialLn(F("Comando MOVERESQ recebido."));
  flagMoverEsq = true;
  flagMoverLong = false;
  flagMoverDir = false;
  flagRetCarro = false;
  flagMoverFimCursoEsq = false;
  passosDados = 0;
}

void MoverDir()
{
  EnviarSerialLn(F("Comando MOVERDIR recebido."));
  flagMoverDir = true;
  flagMoverLong = false;
  passosDados = 0;
}

void MoverLongDir()
{
  EnviarSerialLn(F("Comando MOVERDIR recebido."));
  flagMoverDir = true;
  flagMoverLong = true;
  passosDados = 0;
}


/*
void MoverLongEsq()
{
  EnviarSerialLn(F("Comando MOVERESQ recebido."));
  flagMoverEsq = true;
  flagMoverLong = true;
  passosDados = 0;
}
*/

void MoverEsq()
{
    EnviarSerialLn(F("Comando MOVERESQ recebido."));
    flagMoverEsq = true;
    flagMoverLong = false;
    passosDados = 0;
}

void MoverRetCarro()
{
        EnviarSerialLn(F("ALERTA:Comando RETCARRO recebido."));
        flagMoverFimCursoEsq = false;
        flagRetCarro = true; // Ativa o movimento de retorno
        flagMoverDir = true;
        flagMoverLong = true;
        flagMoverEsq = false;
        passosDados = 0;
}



void MoverFimCursoEsq()
{
  EnviarSerialLn(F("Comando MOVERFIMCURSOESQ recebido."));
  flagMoverFimCursoEsq = true;
  flagMoverEsq = true; // Garante que o flagMoverEsq esteja desligado
  flagMoverLong = true;
  flagMoverDir = false;
  flagRetCarro = false;
  passosDados = 0;
}

void WaitModulo()
{
  EnviarSerialLn(F("Comando WAITMODULO recebido."));
  if(flgCallModulo==1) 
  {
    EnviarSerialLn(F("flgCallModulo=ATIVO"));
  } else
  {
    EnviarSerialLn(F("flgCallModulo=INATIVO"));
  }
}

void RetModulo()
{
  EnviarSerialLn(F("Comando RETMODULO recebido."));
  if(flgCallModulo==0) 
  {
        // Exibindo valores via Serial
        EnviarSerial(F("pH calculado: "));
        EnviarSerialLn(pHValue);
        EnviarSerial(F("Temperatura (Etemp): "));
        EnviarSerialLn(Etemp);
  }
}

void ChamaComandos(const String& comando) {
    if (comando.indexOf("MOVERDIR") != -1) {
        EnviarSerialLn(F("CMD OK"));
        flgComando = true;
        MoverDir();
    } else if (comando.indexOf("MOVERLONGDIR") != -1) {
        EnviarSerialLn(F("CMD OK"));
        flgComando = true;
        MoverLongDir();
    } else if (comando.indexOf("MOVERESQ") != -1) {
        EnviarSerialLn(F("CMD OK"));
        flgComando = true;
        MoverEsq();
    } else if (comando.indexOf("MOVERLONGESQ") != -1) {
        EnviarSerialLn(F("CMD OK"));
        flgComando = true;
        MoverLongEsq();
    } else if (comando.indexOf("LEITURA_PH") != -1) {
        EnviarSerialLn(F("CMD OK"));
        flgComando = true;
        pHValue = LerPH(Etemp);
        EnviarSerial(F("PH:"));        
        
        EnviarSerialLn(pHValue);
        EnviarSerial(F("IMPRIMIR=PH:"));
        EnviarSerialLn(pHValue);        
        FinalizaCOMANDO();
    } else if (comando.indexOf("LEITURA_TEMP") != -1) {
        EnviarSerialLn(F("CMD OK"));
        flgComando = true;
        EnviarSerial(F("Temperatura: "));
        LerTemp();
        EnviarSerialLn(Etemp);
        EnviarSerial(F("IMPRIMIR=Temperatura:"));
        EnviarSerialLn(Etemp);
    } else if (comando.indexOf("ATIVA_PH") != -1) {
        EnviarSerialLn(F("CMD OK"));
        flgComando = true;
        flg_PH = true; // Ativa a leitura de pH
        EnviarSerialLn(F("Leitura de pH ativada."));
        FinalizaCOMANDO();
    } else if (comando.indexOf("DESATIVA_PH") != -1) {
        EnviarSerialLn(F("CMD OK"));
        flgComando = true;
        EnviarSerialLn(F("CMD OK"));
        flg_PH = false; // Desativa a leitura de pH
        EnviarSerialLn(F("Leitura de pH desativada."));
        FinalizaCOMANDO();
    } else if (comando.indexOf("MOVERLONGESQ") != -1) {
        EnviarSerialLn(F("CMD OK"));
        EnviarSerialLn(F("CMD OK"));
        flgComando = true;
        MoverLongEsq();
    }  else if (comando.indexOf("MOVERFIMCURSOESQ") != -1) {        
        EnviarSerialLn(F("CMD OK"));
        flgComando = true;
        MoverFimCursoEsq();
    } else if (comando.indexOf("RETCARRO") != -1) {
        EnviarSerialLn(F("CMD OK"));
        flgComando = true;
        EnviarSerialLn(F("Iniciando retorno de carro."));
        MoverRetCarro();
    }  else if (comando.indexOf("CALIBRAR") != -1) {
        EnviarSerialLn(F("CMD OK"));
        flgComando = true;
        EnviarSerialLn(F("Comando CALIBRAR recebido."));
        //ConfiguraSensorFim();
        Calibrar();
        // Insira aqui a funcionalidade para calibrar o sistema
        EnviarSerialLn(F("Iniciando calibração geral..."));
        // Exemplo: Alguma lógica de calibração pode ser adicionada
    } else if (comando.indexOf("CLBPH4") != -1) {
        EnviarSerialLn(F("CMD OK"));
        flgComando = true;
        EnviarSerialLn(F("Comando CALIBRARPH4 recebido."));
        // Insira aqui a funcionalidade específica para calibrar o sensor de pH
        EnviarSerialLn(F("Iniciando calibração do sensor de pH..."));
        CalibrarPH4();
        FinalizaCOMANDO();
    } else if (comando.indexOf("CLBPH7") != -1) {
        EnviarSerialLn(F("CMD OK"));
        flgComando = true;
        EnviarSerialLn(F("Comando CALIBRARPH7 recebido."));
        EnviarSerialLn(F("Iniciando calibração do sensor de pH..."));
        CalibrarPH7();
        FinalizaCOMANDO();
        // Exemplo: Alguma lógica de calibração de pH pode ser adicionada
    } else if (comando.indexOf("CLBPH10") != -1) {
        EnviarSerialLn(F("CMD OK"));
        flgComando = true;
        EnviarSerialLn(F("Comando CALIBRARPH10 recebido."));
        EnviarSerialLn(F("Iniciando calibração do sensor de pH..."));
        CalibrarPH10();     
        FinalizaCOMANDO();
        // Exemplo: Alguma lógica de calibração de pH pode ser adicionada
    }  else if (comando.indexOf("ENXAGUE") != -1) {
        EnviarSerialLn(F("CMD OK"));
        flgComando = true;
        EnviarSerialLn(F("Comando ENXAGUE recebido."));
        Enxaguar();
    } else if (comando.indexOf("MAN") != -1) {
        flgComando = true;
        HelpComandos(); // Chama a função de ajuda
        FinalizaCOMANDO();
    } else if (comando.indexOf("CALLMODULO") != -1) {
        EnviarSerialLn(F("CMD OK"));
        flgComando = true;
        EnviarSerialLn(F("Comando CALLMODULO recebido."));
        MoverFimCursoEsq();
        EnviarSerialLn(F("Posicionando sensor"));
        flgCallModulo = 1;
        EnviarSerialLn(F("flgCallModulo=ATIVO"));
        
    } else if (comando.indexOf("WAITMODULO") != -1) {
        EnviarSerialLn(F("CMD OK"));
        flgComando = true;
        WaitModulo();
        FinalizaCOMANDO();      
        
    } else if (comando.indexOf("RETMODULO") != -1) {
        flgComando = true;
        //EnviarSerialLn(F("Comando RETMODULO recebido."));
        RetModulo();
    } else {        
        //ERRO_SISTEMA("Comando desconhecido recebido.");
        //Serial.println(F("Comando desconhecido recebido."));
        EnviarSerial(F("Comando desconhecido recebido"));
        EnviarSerialLn(comando);
    }
    //FinalizaCOMANDO();
}

// Função para processar o buffer
void ProcessaBuffer() {
    size_t len = strlen(buffer);
    for (size_t i = 0; i < len; i++) {
        if (buffer[i] == '\n') {
            Serial.print("Achou comando:");
            Serial.println(buffer);
            ChamaComandos(buffer);

            // Desloca o resto do buffer
            memmove(buffer, &buffer[i + 1], len - i - 1);
            buffer[len - i - 1] = '\0';
            i = -1; 
            len = strlen(buffer); // Recalcula tamanho após o memmove
        }
    }
}



// Função para movimentar o motor
void MoverMotor(int passos, bool horario) {
    if (!flagMoverDir && !flagMoverEsq) {
        return; // Interrompe o movimento se nenhum flag estiver ativo
    }

    // Verificar se o fim de curso foi acionado apenas para o movimento à direita
    if (digitalRead(pin_FIMCURSO) == LOW && horario) { // Se movendo para a direita
        EnviarSerialLn(F("Fim de curso acionado. Movimento interrompido."));
        flagMoverDir = false;
        flagMoverLong = false;
        flagRetCarro = false;
        flagMoverFimCursoEsq = false;
        passosDados = 0;
        EnviarSerialLn(F("flgCallModulo=INATIVO"));
        flgCallModulo = 0;
        FinalizaCOMANDO();
        return;
    }

    // Movimento permitido
    if (contadorpassosDados <= FIM_CURSO || !horario) {
        motor.setSpeed(75); // Define a velocidade do motor
        motor.step(horario ? passos : -
        passos);
        passosDados += abs(passos);
        contadorpassosDados += (horario ? -passos : +passos);
    } else {
      // Movimento permitido
      if (flagRetCarro) {
          motor.setSpeed(75); // Define a velocidade do motor
          motor.step(horario ? passos : -
          passos);
          passosDados += abs(passos);
          contadorpassosDados += (horario ? -passos : +passos);
      }
    }

    EnviarSerial("POSSERVM=");
    EnviarSerialLn(contadorpassosDados);
}


// Função para mover para a direita
void MoverDireita() {
    if (passosDados >= NUM_PASSOS) {
        EnviarSerialLn(F("Movimento para a direita concluído."));
        flagMoverDir = false;
        passosDados = 0;
        return;
    }
    if(flagMoverLong)
    {
      MoverMotor(NUM_PASSOS, true);
    } else {
      MoverMotor(1, true);
    }
    
}

void MoverEsquerda() {
    // Verifica o estado do fim de curso apenas se o flagMoverFimCursoEsq estiver ativo
    if (flagMoverEsq) 
    { 
        if((FIM_CURSO <= contadorpassosDados)) 
        {
          EnviarSerialLn(F("Movimento para a esquerda concluído."));
          if (flgCallModulo == 1)
          {
            EnviarSerialLn(F("Iniciando Leitura de Devices."));
            flg_PH = 1;
          }
          //MMM
          if(flagMoverFimCursoEsq)
          {
            FinalizaCOMANDO();
          }
          flagMoverEsq = false;
          flagMoverFimCursoEsq  = false;
        } else if ((passosDados >= NUM_PASSOS)&(!flagMoverFimCursoEsq )) 
        {
            EnviarSerialLn(F("Movimento para a esquerda concluído."));
            flagMoverEsq = false;
            passosDados = 0;
        } else 
        {            
            if(flagMoverLong)
            {
              MoverMotor(NUM_PASSOS, false);
            } else 
            {
              MoverMotor(1, false);
            }
        }
    }
}
void Calibrar()
{
    MoverRetCarro();
}

// Função para analisar comandos
void Analisar() {
    ProcessaBuffer();
    

    if (flagMoverDir) {
        MoverDireita();
    }
    if (flagMoverEsq) {
        MoverEsquerda();
    }
    if (flagRetCarro) {
      if (digitalRead(pin_FIMCURSO) == LOW) {
          EnviarSerialLn(F("Fim de curso acionado. Movimento interrompido."));
          flagRetCarro = false;
          flagMoverLong = false;
          passosDados = 0;
      } else 
      {
          if(flagMoverDir)
          {
            
            if(!flagMoverFimCursoEsq) 
            {
              EnviarSerialLn(passosDados);
              if(passosDados> NUM_PASSOS)
              {
                passosDados = -100;
              
                flagMoverLong = true; 
                flagMoverDir = true;
              } else {
                //EnviarSerialLn("entrou aqui");
                passosDados = 0;
                flagMoverLong = true; 
                flagMoverDir = true;
              }
              //EnviarSerialLn(F("Retornando carro..."));
            } else
            {              
              EnviarSerialLn(F("terminou"));
              FinalizaCOMANDO();  
            }
          }         
      }
    }
}

void LeSerial() {
    while (Serial.available()) {
        char c = Serial.read();
        size_t len = strlen(buffer); 
        if (len < sizeof(buffer) - 1) {
            buffer[len] = c;
            buffer[len + 1] = '\0';
        }
    }
    while (serialSoftware.available()) {
        char c = serialSoftware.read();
        size_t len = strlen(buffer);
        if (len < sizeof(buffer) - 1) {
            buffer[len] = c;
            buffer[len + 1] = '\0';
        }
    }
}


// Função principal de leitura
void Ler() {
    LeSerial();
    
    LerFimCurso();
    LerTemp();
    // Chama Ler_ph apenas se o flag estiver ativo
    if (flg_PH) {
        LerPH(Etemp);
    }
}

// Configuração inicial
void setup() {
    Start_serial();
    Serial.println(F("Sistema inicializando..."));
           
    
    ConfiguraSensorFim(); 
    WellCome();
    Calibrar();
    carregarCalibracoes();
    Serial.println('Feito!');
}

// Loop principal
void loop() 
{
    Ler();
    Analisar();
}
