#include <Arduino.h>
#include <Stepper.h>
#include <DallasTemperature.h>
#include <OneWire.h>

// Mapa de pinos
const int pin_IN1 = A0;      // Pino IN1 do ULN2003
const int pin_IN2 = A1;      // Pino IN2 do ULN2003
const int pin_IN3 = A2;      // Pino IN3 do ULN2003
const int pin_IN4 = A3;      // Pino IN4 do ULN2003
const int pin_FIMCURSO = 5;  // Pino do sensor de fim de curso

const int pin_PH = A5;       // Pino do sensor de pH
const int pin_TEMP = A6;     // Pino do sensor de temperatura
const int pin_ONEWIRE = A4;   // Pino para o barramento 1-Wire (temperatura)
//2048
#define PASSOS_POR_VOLTA 128 // Número total de passos para o motor de passo ULN2003
#define VERSAO "0.3"          // Versão do módulo
#define TEMPO_DEBOUNCE 50     // Tempo para debounce (em ms)


long NUM_PASSOS = 50;         // Número de passos por comando
long FIM_CURSO = 18000;       //Posicao final do curso

String buffer = "";           // Buffer para dados recebidos da serial

//Flags de controle
bool flagMoverLong = false;    // Controle de movimento Longo
bool flagMoverDir = false;    // Controle de movimento para a direita
bool flagMoverEsq = false;    // Controle de movimento para a esquerda
bool flagRetCarro = false; // Controle de retorno do carro


int contadorpassosDados = 0;          // Contador de passos dados
int passosDados = 0;          // Contador de passos dados
int estadoAnteriorFimCurso = 255; // Inicializa com HIGH
unsigned long ultimoTempoFimCurso = 0; // Controle de tempo para debounce
// Adicionado flag flg_PH
bool flg_PH = false; // Inicia o flag como falso



// Instâncias
OneWire oneWire(pin_ONEWIRE);
DallasTemperature sensors(&oneWire);

// Cria uma instância do motor de passo
Stepper motor(PASSOS_POR_VOLTA, pin_IN1, pin_IN3, pin_IN2, pin_IN4);

// Função para ativar a comunicação serial
void Start_serial() {
    Serial.begin(9600);
    Serial.println("Serial ativada!");
}

// Função para configurar o sensor de fim de curso
void ConfiguraSensorFim() {
    pinMode(pin_FIMCURSO, INPUT_PULLUP);
    Serial.println("Sensor de fim de curso configurado.");
}

// Função chamada em caso de erro
void ERRO_SISTEMA(const char* mensagem) {
    Serial.println("=== ERRO NO SISTEMA ===");
    Serial.println(mensagem);
    Serial.println("=======================");
}

// Função de boas-vindas
void WellCome() {
    Serial.println();
    Serial.println("====================================");
    Serial.println("Modulo de controle de PH do sistema DOCTOR");
    Serial.println("Autor: Marcelo Maurin Martins");
    Serial.print("Versao: ");
    Serial.println(VERSAO);
    Serial.println("====================================");
}

// Função para verificar o estado do fim de curso com debounce
void LerFimCurso() {
    int estadoAtualFimCurso = digitalRead(pin_FIMCURSO);

    if (millis() - ultimoTempoFimCurso > TEMPO_DEBOUNCE) {
        if (estadoAtualFimCurso != estadoAnteriorFimCurso) {
            estadoAnteriorFimCurso = estadoAtualFimCurso;
            ultimoTempoFimCurso = millis();

            Serial.print("Estado atual do fim de curso: ");
            Serial.println(estadoAtualFimCurso);

            if (estadoAtualFimCurso == LOW) {
                ERRO_SISTEMA("Fim de curso acionado. Parando o motor.");
                flagMoverDir = false;
                flagMoverEsq = false;
                flagMoverLong = false;
                passosDados = 0;
                contadorpassosDados = 0;
            } else {
                Serial.println("Fim de curso liberado. Movimento permitido.");
            }
        }
    }
}

// Função para ler o sensor de pH
float LerPH() {
    int leitura = analogRead(pin_PH);
    float phVoltage = leitura * (5.0 / 1023.0);
    float phValue = -5.70 * phVoltage + 21.34; // Ajuste conforme calibração
    return phValue;
}

// Função para ler o sensor de temperatura
float LerTemp() {
    int leitura = analogRead(pin_TEMP);
    float tempVoltage = leitura * (5.0 / 1023.0);
    float tempValue = tempVoltage * 10.0; // Ajuste conforme calibração
    return tempValue;
}

// Função para leitura de pH e temperatura com múltiplas amostras
void Ler_ph() {

    if (!flg_PH) {
        return; // Retorna imediatamente se o flag estiver desativado
    }
    float phTot = 0;
    float temTot = 0;

    // Obtendo 10 amostras com atraso de 10ms entre cada uma
    for (int x = 0; x < 10; x++) {
        phTot += analogRead(pin_PH);
        temTot += analogRead(pin_TEMP);
        delay(10);
    }

    float temAvg = temTot / 10.0;
    float phAvg = phTot / 10.0;
    float temVoltage = temAvg * (5000.0 / 1023.0); // Convertendo leitura para mV
    float phVoltage = phAvg * (5.0 / 1023.0); // Convertendo leitura para mV

    sensors.requestTemperatures(); // Obtendo temperaturas dos sensores 1-Wire
    float Etemp = temVoltage * 0.1; // Convertendo mV para °C
    float pHValue = phVoltage * 0.0178 + 0.1; // Ajustar os valores conforme calibração
    float Wtemp = sensors.getTempCByIndex(0);
    float TempDif = fabs(Etemp - Wtemp); // Diferença absoluta entre temperaturas

    // Exibindo valores via Serial
    Serial.print("pH calculado: ");
    Serial.println(pHValue);
    Serial.print("Temperatura (Etemp): ");
    Serial.println(Etemp);
    Serial.print("Temperatura (1-Wire): ");
    Serial.println(Wtemp);
    Serial.print("Diferenca de temperatura: ");
    Serial.println(TempDif);
}


// Função para processar comandos serial
void ChamaComandos(const String& comando) {
    if (comando.indexOf("MOVERDIR;") != -1) {
        Serial.println("Comando MOVERDIR recebido.");
        flagMoverDir = true;
        flagMoverLong = false;
        passosDados = 0;
    } else if (comando.indexOf("MOVERLONGDIR;") != -1) {
        Serial.println("Comando MOVERDIR recebido.");
        flagMoverDir = true;
        flagMoverLong = true;
        passosDados = 0;
    } else if (comando.indexOf("MOVERESQ;") != -1) {
        Serial.println("Comando MOVERESQ recebido.");
        flagMoverEsq = true;
        flagMoverLong = false;
        passosDados = 0;
    } else if (comando.indexOf("MOVERLONGESQ;") != -1) {
        Serial.println("Comando MOVERESQ recebido.");
        flagMoverEsq = true;
        flagMoverLong = true;
        passosDados = 0;
    } else if (comando.indexOf("LEITURA_PH;") != -1) {
        Serial.print("Valor de pH: ");
        Serial.println(LerPH());
    } else if (comando.indexOf("LEITURA_TEMP;") != -1) {
        Serial.print("Temperatura: ");
        Serial.println(LerTemp());
    } else if (comando.indexOf("ATIVA_PH;") != -1) {
        flg_PH = true; // Ativa a leitura de pH
        Serial.println("Leitura de pH ativada.");
    } else if (comando.indexOf("DESATIVA_PH;") != -1) {
        flg_PH = false; // Desativa a leitura de pH
        Serial.println("Leitura de pH desativada.");
    } else if (comando.indexOf("RETCARRO;") != -1) {
        Serial.println("Comando RETCARRO recebido.");
        flagRetCarro = true; // Ativa o movimento de retorno
        flagMoverDir = true;
        flagMoverEsq = false;
        flagMoverLong = true;
    } else {
        ERRO_SISTEMA("Comando desconhecido recebido.");
    }
}

// Função para processar o buffer
void ProcessaBuffer() {
    int pos = buffer.indexOf('\n');
    if (pos != -1) {
        String comando = buffer.substring(0, pos);
        buffer = buffer.substring(pos + 1);
        ChamaComandos(comando);
    }
}

// Função para movimentar o motor
void MoverMotor(int passos, bool horario) {
    if (!flagMoverDir && !flagMoverEsq) {
        return; // Interrompe o movimento se nenhum flag estiver ativo
    }
    if(FIM_CURSO >=contadorpassosDados)
    {
        //motor.setSpeed(15); // Define a velocidade do motor
        motor.setSpeed(75); // Define a velocidade do motor
        motor.step(horario ? passos : -passos);
        passosDados += abs(passos);
        contadorpassosDados = contadorpassosDados + (horario ? -passos : passos);
    }
    //Serial.print("Passos dados: ");    
    //Serial.println(passosDados);
    Serial.print("Posicao absoluta: ");
    Serial.println(contadorpassosDados);
}

// Função para mover para a direita
void MoverDireita() {
    if (passosDados >= NUM_PASSOS) {
        Serial.println("Movimento para a direita concluído.");
        flagMoverDir = false;
        passosDados = 0;
        return;
    }
    if(flagMoverLong)
    {
      MoverMotor(100, true);
    } else {
      MoverMotor(1, true);
    }
    
}

// Função para mover para a esquerda
void MoverEsquerda() {
    if (passosDados >= NUM_PASSOS) {
        Serial.println("Movimento para a esquerda concluído.");
        flagMoverEsq = false;
        passosDados = 0;
        return;
    }
    if(flagMoverLong)
    {
      MoverMotor(100, false);
    } else {
      MoverMotor(1, false);
    }
    
}

// Função para analisar comandos
void Analisar() {
    if (buffer.indexOf('\n') != -1) {
        ProcessaBuffer();
    }

    if (flagMoverDir) {
        MoverDireita();
    }
    if (flagMoverEsq) {
        MoverEsquerda();
    }
    if (flagRetCarro) {
      if (digitalRead(pin_FIMCURSO) == LOW) {
          Serial.println("Fim de curso acionado. Movimento interrompido.");
          flagRetCarro = false;
          flagMoverLong = false;
          passosDados = 0;
      } else {
          if(flagMoverDir)
          {
            passosDados = 0;
            flagMoverLong = true; 
            flagMoverDir = true;
            Serial.println("Retornando carro...");
          }         
      }
    }
}



// Função principal de leitura
void Ler() {
    LerFimCurso();
    // Chama Ler_ph apenas se o flag estiver ativo
    if (flg_PH) {
        Ler_ph();
    }
}

// Configuração inicial
void setup() {
    Start_serial();
    ConfiguraSensorFim();
    WellCome();
    Serial.println("Sistema inicializado.");

    // Movimentação inicial (opcional)
    Serial.println("Iniciando movimento para a direita...");
    flagMoverDir = true;
    passosDados = 0;
    flagRetCarro = true;
    flagMoverLong = true;
}

// Loop principal
void loop() {
    if (Serial.available()) {
        char c = Serial.read();
        buffer += c;
    }
    Ler();
    Analisar();
}
