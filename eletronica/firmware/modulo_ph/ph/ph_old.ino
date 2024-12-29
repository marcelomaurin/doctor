#include <Arduino.h>
#include <Stepper.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <SoftwareSerial.h>

// Mapa de pinos
#define pin_IN1 A0       // Pino IN1 do ULN2003
#define pin_IN2 A1       // Pino IN2 do ULN2003
#define pin_IN3 A2       // Pino IN3 do ULN2003
#define pin_IN4 A3       // Pino IN4 do ULN2003
#define pin_FIMCURSO 5   // Pino do sensor de fim de curso

// Pinos da SoftwareSerial
#define pin_RX 3         // RX da SoftwareSerial
#define pin_TX 4         // TX da SoftwareSerial

// Sensores
#define pin_PH A5        // Pino do sensor de pH
#define pin_TEMP A6      // Pino do sensor de temperatura
#define pin_ONEWIRE A4   // Pino para o barramento 1-Wire (temperatura)

//2048
#define PASSOS_POR_VOLTA 128 // Número total de passos para o motor de passo ULN2003

#define VERSAO "0.5"          // Versão do módulo

#define TEMPO_DEBOUNCE 50     // Tempo para debounce (em ms)


long NUM_PASSOS = 50;         // Número de passos por comando
long FIM_CURSO = 17000;       //Posicao final do curso


String buffer = "";           // Buffer para dados recebidos da serial

//Flags de controle
bool flagMoverLong = false;    // Controle de movimento Longo
bool flagMoverDir = false;    // Controle de movimento para a direita
bool flagMoverEsq = false;    // Controle de movimento para a esquerda
bool flagRetCarro = false; // Controle de retorno do carro
bool flagMoverFimCursoEsq = false; // Novo flag para o comando MOVERFIMCURSOESQ



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

SoftwareSerial softSerial(pin_RX, pin_TX); // SoftwareSerial nos pinos 2 (RX) e 3 (TX)



// Função para enviar dados para a Serial padrão e SoftwareSerial
void EnviarSerial(const String& mensagem) {
    Serial.print(mensagem);
    softSerial.print(mensagem);
}

// Função para enviar dados para a Serial padrão e SoftwareSerial
void EnviarSerial(int info) {
    Serial.print(info);
    softSerial.print(info);
}

void EnviarSerialLn(const String& mensagem) {
    Serial.println(mensagem);
    softSerial.println(mensagem);
}

void EnviarSerialLn(int info) {
    Serial.println(info);
    softSerial.println(info);
}


// Função para ativar a comunicação serial
void Start_serial() {
    Serial.begin(9600);
    softSerial.begin(9600);
    
}

// Função para configurar o sensor de fim de curso
void ConfiguraSensorFim() {
    pinMode(pin_FIMCURSO, INPUT_PULLUP);
    EnviarSerialLn(F("Sensor de fim de curso configurado."));
}

// Função chamada em caso de erro
void ERRO_SISTEMA(const char* mensagem) {
    EnviarSerialLn(F("=== ERRO NO SISTEMA ==="));
    EnviarSerialLn(mensagem);
    EnviarSerialLn(F("======================="));
}

// Função de boas-vindas
void WellCome() {
    EnviarSerialLn(" ");
    EnviarSerialLn(F("===================================="));
    EnviarSerialLn(F("Modulo de controle de PH do sistema DOCTOR"));
    EnviarSerialLn(F("Autor: Marcelo Maurin Martins"));
    EnviarSerial(F("Versao: "));
    EnviarSerialLn(VERSAO);
    EnviarSerialLn(F("===================================="));
}

// Função para exibir os comandos disponíveis
void HelpComandos() {
    EnviarSerialLn(F("=== LISTA DE COMANDOS DISPONÍVEIS ==="));
    EnviarSerialLn(F("MOVERDIR;         - Move o motor para a direita."));
    EnviarSerialLn(F("MOVERLONGDIR;     - Move o motor longamente para a direita."));
    EnviarSerialLn(F("MOVERESQ;         - Move o motor para a esquerda."));
    EnviarSerialLn(F("MOVERLONGESQ;     - Move o motor longamente para a esquerda."));
    EnviarSerialLn(F("MOVERFIMCURSOESQ; - Move o motor até o fim de curso à esquerda."));
    EnviarSerialLn(F("RETCARRO;         - Retorna o carro ao ponto inicial."));
    EnviarSerialLn(F("LEITURA_PH;       - Exibe o valor de pH atual."));
    EnviarSerialLn(F("LEITURA_TEMP;     - Exibe a temperatura atual."));
    EnviarSerialLn(F("ATIVA_PH;         - Ativa a leitura contínua de pH."));
    EnviarSerialLn(F("DESATIVA_PH;      - Desativa a leitura contínua de pH."));
    EnviarSerialLn(F("MAN;              - Exibe esta lista de comandos."));
    EnviarSerialLn(F("====================================="));
}


// Função para verificar o estado do fim de curso com debounce
void LerFimCurso() {
    int estadoAtualFimCurso = digitalRead(pin_FIMCURSO);

    if (millis() - ultimoTempoFimCurso > TEMPO_DEBOUNCE) {
        if (estadoAtualFimCurso != estadoAnteriorFimCurso) {
            estadoAnteriorFimCurso = estadoAtualFimCurso;
            ultimoTempoFimCurso = millis();

            EnviarSerial("Estado atual do fim de curso: ");
            EnviarSerialLn(estadoAtualFimCurso);

            if (estadoAtualFimCurso == LOW) {
                ERRO_SISTEMA("Fim de curso acionado. Parando o motor.");
                if(flagMoverDir)
                {
                  flagMoverDir = false;
                  //flagMoverEsq = false;
                  flagMoverLong = false;
                }
                passosDados = 0;
                contadorpassosDados = 0;
            } else {
                EnviarSerialLn(F("Fim de curso liberado. Movimento permitido."));
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
    EnviarSerial(F("pH calculado: "));
    EnviarSerialLn(pHValue);
    EnviarSerial(F("Temperatura (Etemp): "));
    EnviarSerialLn(Etemp);
    EnviarSerial(F("Temperatura (1-Wire): "));
    EnviarSerialLn(Wtemp);
    EnviarSerial(F("Diferenca de temperatura: "));
    EnviarSerialLn(TempDif);
}

void ChamaComandos(const String& comando) {
    if (comando.indexOf("MOVERDIR") != -1) {
        EnviarSerialLn(F("Comando MOVERDIR recebido."));
        flagMoverDir = true;
        flagMoverLong = false;
        passosDados = 0;
    } else if (comando.indexOf("MOVERLONGDIR") != -1) {
        EnviarSerialLn(F("Comando MOVERDIR recebido."));
        flagMoverDir = true;
        flagMoverLong = true;
        passosDados = 0;
    } else if (comando.indexOf("MOVERESQ") != -1) {
        EnviarSerialLn(F("Comando MOVERESQ recebido."));
        flagMoverEsq = true;
        flagMoverLong = false;
        passosDados = 0;
    } else if (comando.indexOf("MOVERLONGESQ") != -1) {
        EnviarSerialLn(F("Comando MOVERESQ recebido."));
        flagMoverEsq = true;
        flagMoverLong = true;
        passosDados = 0;
    } else if (comando.indexOf("LEITURA_PH") != -1) {
        EnviarSerial(F("Valor de pH: "));
        EnviarSerialLn(LerPH());
    } else if (comando.indexOf("LEITURA_TEMP") != -1) {
        EnviarSerial(F("Temperatura: "));
        EnviarSerialLn(LerTemp());
    } else if (comando.indexOf("ATIVA_PH;") != -1) {
        flg_PH = true; // Ativa a leitura de pH
        EnviarSerialLn(F("Leitura de pH ativada."));
    } else if (comando.indexOf("DESATIVA_PH;") != -1) {
        flg_PH = false; // Desativa a leitura de pH
        EnviarSerialLn(F("Leitura de pH desativada."));
    } else if (comando.indexOf("MOVERLONGESQ") != -1) {
        EnviarSerialLn(F("Comando MOVERESQ recebido."));
        flagMoverEsq = true;
        flagMoverLong = false;
        flagMoverDir = false;
        flagRetCarro = false;
        flagMoverFimCursoEsq = false;
        passosDados = 0;
    }  else if (comando.indexOf("MOVERFIMCURSOESQ") != -1) {
        EnviarSerialLn(F("Comando MOVERFIMCURSOESQ recebido."));
        flagMoverFimCursoEsq = true;
        flagMoverEsq = true; // Garante que o flagMoverEsq esteja desligado
        flagMoverLong = true;
        flagMoverDir = false;
        flagRetCarro = false;
        passosDados = 0;
    } else if (comando.indexOf("RETCARRO") != -1) {
        EnviarSerialLn(F("Comando RETCARRO recebido."));
        flagMoverFimCursoEsq = false;
        flagRetCarro = true; // Ativa o movimento de retorno
        flagMoverDir = true;
        flagMoverLong = true;
        flagMoverEsq = false;
        passosDados = 0;
    } else if (comando.indexOf("MAN") != -1) {
        HelpComandos(); // Chama a função de ajuda
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

    // Verificar se o fim de curso foi acionado apenas para o movimento à direita
    if (digitalRead(pin_FIMCURSO) == LOW && horario) { // Se movendo para a direita
        EnviarSerialLn(F("Fim de curso acionado. Movimento interrompido."));
        flagMoverDir = false;
        flagMoverLong = false;
        flagRetCarro = false;
        flagMoverFimCursoEsq = false;
        passosDados = 0;
        return;
    }

    // Movimento permitido
    if (contadorpassosDados <= FIM_CURSO || !horario) {
        motor.setSpeed(75); // Define a velocidade do motor
        motor.step(horario ? passos : -
        passos);
        passosDados += abs(passos);
        contadorpassosDados += (horario ? -passos : +passos);
    }

    EnviarSerial(F("Posição absoluta: "));
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
      MoverMotor(100, true);
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
              MoverMotor(100, false);
            } else 
            {
              MoverMotor(1, false);
            }
        }
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
          EnviarSerialLn(F("Fim de curso acionado. Movimento interrompido."));
          flagRetCarro = false;
          flagMoverLong = false;
          passosDados = 0;
      } else {
          if(flagMoverDir)
          {
            passosDados = 0;
            flagMoverLong = true; 
            flagMoverDir = true;
            EnviarSerialLn(F("Retornando carro..."));
          }         
      }
    }
}


void LeSerial()
{
   if (Serial.available()) {
        char c = Serial.read();
        buffer += c;
    }
}

// Função principal de leitura
void Ler() {
    LeSerial();
    LerFimCurso();
    // Chama Ler_ph apenas se o flag estiver ativo
    if (flg_PH) {
        Ler_ph();
    }
}

// Configuração inicial
void setup() {
    Start_serial();
    Serial.println("Sistema inicializando...");
    ConfiguraSensorFim();    
    

    // Movimentação inicial (opcional)
    EnviarSerialLn(F("Iniciando movimento para a direita..."));
    flagMoverDir = true;
    passosDados = 0;
    flagRetCarro = true;
    flagMoverLong = true;
    WellCome();
}

// Loop principal
void loop() {
   
    Ler();
    Analisar();
}
