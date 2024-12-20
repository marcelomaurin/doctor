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



// Sensores
#define pin_PH A5        // Pino do sensor de pH
#define pin_TEMP A6      // Pino do sensor de temperatura
#define pin_ONEWIRE A4   // Pino para o barramento 1-Wire (temperatura)

//2048
#define PASSOS_POR_VOLTA 128 // Número total de passos para o motor de passo ULN2003

#define VERSAO "0.4"          // Versão do módulo
//const char VERSAO[] PROGMEM = "0.4";


#define TEMPO_DEBOUNCE 50     // Tempo para debounce (em ms)


long NUM_PASSOS = 50;         // Número de passos por comando
long FIM_CURSO = 17000;       //Posicao final do curso


char buffer[100] ={0};           // Buffer para dados recebidos da serial
size_t buffer_pos = 0; // Posição atual no buffer

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





// Função para enviar dados para a Serial padrão e SoftwareSerial
void EnviarSerial(const String& mensagem) {
    Serial.print(mensagem);
    
}

// Função para enviar dados para a Serial padrão e SoftwareSerial
void EnviarSerial(int info) {
    Serial.print(info);
    
}

void EnviarSerialLn(const String& mensagem) {
    Serial.println(mensagem);
    
}

void EnviarSerialLn(int info) {
    Serial.println(info);
    
}


// Função para ativar a comunicação serial
void Start_serial() {
    Serial.begin(9600);
    
    
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
    EnviarSerialLn(VERSAO); // VERSAO já está armazenado em PROGMEM
    EnviarSerialLn(F("===================================="));
}


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
    EnviarSerialLn(F("MAN;             - Exibe esta lista de comandos."));
    EnviarSerialLn(F("====================================="));
}


// Função para verificar o estado do fim de curso com debounce
void LerFimCurso() {
    int estadoAtualFimCurso = digitalRead(pin_FIMCURSO);
    if (millis() - ultimoTempoFimCurso > TEMPO_DEBOUNCE) {
        if (estadoAtualFimCurso != estadoAnteriorFimCurso) {
            estadoAnteriorFimCurso = estadoAtualFimCurso;
            ultimoTempoFimCurso = millis();

            EnviarSerial(F("Estado atual do fim de curso: "));
            EnviarSerialLn(estadoAtualFimCurso);

            if (estadoAtualFimCurso == LOW) {
                ERRO_SISTEMA("Fim de curso acionado. Parando o motor.");
                flagMoverDir = false;
                flagMoverLong = false;
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


void CalibrarPH4() {
    EnviarSerialLn(F("Iniciando calibração com solução pH 4..."));

    // Ler o valor do sensor de pH
    int leituraPH = analogRead(pin_PH);
    float phVoltage = leituraPH * (5.0 / 1023.0);

    // Ajustar o coeficiente de calibração para pH 4
    float calibPH4 = 4.0 / phVoltage; // Exemplo: ajuste o valor conforme necessário

    EnviarSerial(F("Tensão medida: "));
    EnviarSerialLn(phVoltage);
    EnviarSerial(F("Coeficiente ajustado para pH 4: "));
    EnviarSerialLn(calibPH4);

    EnviarSerialLn(F("Calibração para pH 4 concluída."));
}

void CalibrarPH7() {
    EnviarSerialLn(F("Iniciando calibração com solução pH 7..."));

    // Ler o valor do sensor de pH
    int leituraPH = analogRead(pin_PH);
    float phVoltage = leituraPH * (5.0 / 1023.0);

    // Ajustar o coeficiente de calibração para pH 7
    float calibPH7 = 7.0 / phVoltage; // Exemplo: ajuste o valor conforme necessário

    EnviarSerial(F("Tensão medida: "));
    EnviarSerialLn(phVoltage);
    EnviarSerial(F("Coeficiente ajustado para pH 7: "));
    EnviarSerialLn(calibPH7);

    EnviarSerialLn(F("Calibração para pH 7 concluída."));
}

void CalibrarPH10() {
    EnviarSerialLn(F("Iniciando calibração com solução pH 10..."));

    // Ler o valor do sensor de pH
    int leituraPH = analogRead(pin_PH);
    float phVoltage = leituraPH * (5.0 / 1023.0);

    // Ajustar o coeficiente de calibração para pH 10
    float calibPH10 = 10.0 / phVoltage; // Exemplo: ajuste o valor conforme necessário

    EnviarSerial(F("Tensão medida: "));
    EnviarSerialLn(phVoltage);
    EnviarSerial(F("Coeficiente ajustado para pH 10: "));
    EnviarSerialLn(calibPH10);

    EnviarSerialLn(F("Calibração para pH 10 concluída."));
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

    EnviarSerialLn("Processo de enxágue concluído.");
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
    } else if (comando.indexOf("ATIVA_PH") != -1) {
        flg_PH = true; // Ativa a leitura de pH
        EnviarSerialLn(F("Leitura de pH ativada."));
    } else if (comando.indexOf("DESATIVA_PH") != -1) {
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
    }  else if (comando.indexOf("CALIBRAR") != -1) {
        EnviarSerialLn(F("Comando CALIBRAR recebido."));
        ConfiguraSensorFim();
        // Insira aqui a funcionalidade para calibrar o sistema
        EnviarSerialLn(F("Iniciando calibração geral..."));
        // Exemplo: Alguma lógica de calibração pode ser adicionada
    } else if (comando.indexOf("CALIBRARPH4") != -1) {
        EnviarSerialLn(F("Comando CALIBRARPH4 recebido."));
        CalibrarPH4();
        // Insira aqui a funcionalidade específica para calibrar o sensor de pH
        EnviarSerialLn(F("Iniciando calibração do sensor de pH..."));
        // Exemplo: Alguma lógica de calibração de pH pode ser adicionada
    } else if (comando.indexOf("CALIBRARPH7") != -1) {
        EnviarSerialLn(F("Comando CALIBRARPH7 recebido."));
        CalibrarPH7();
        // Insira aqui a funcionalidade específica para calibrar o sensor de pH
        EnviarSerialLn(F("Iniciando calibração do sensor de pH..."));
        // Exemplo: Alguma lógica de calibração de pH pode ser adicionada
    } else if (comando.indexOf("CALIBRARPH10") != -1) {
        EnviarSerialLn(F("Comando CALIBRARPH10 recebido."));
        CalibrarPH10();
        // Insira aqui a funcionalidade específica para calibrar o sensor de pH
        EnviarSerialLn(F("Iniciando calibração do sensor de pH..."));
        // Exemplo: Alguma lógica de calibração de pH pode ser adicionada
    }  else if (comando.indexOf("ENXAGUE") != -1) {
        EnviarSerialLn(F("Comando ENXAGUE recebido."));
        Enxaguar();
    } else if (comando.indexOf("MAN") != -1) {
        HelpComandos(); // Chama a função de ajuda
    } else {
        ERRO_SISTEMA("Comando desconhecido recebido.");
    }
}

// Função para processar o buffer
void ProcessaBuffer() {
    for (size_t i = 0; i < buffer_pos; i++) {
        if (buffer[i] == '\n') { // Encontrou o delimitador de comando
            buffer[i] = '\0'; // Substituir '\n' por '\0' para criar uma string válida
            ChamaComandos(buffer); // Processar o comando

            // Deslocar o restante do buffer
            memmove(buffer, &buffer[i + 1], buffer_pos - i - 1);
            buffer_pos -= i + 1;
            i = -1; // Reiniciar o loop para processar o próximo comando
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

    EnviarSerial("Posição absoluta: ");
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
    //if (buffer.indexOf('\n') != -1) {
    ProcessaBuffer();
    //}

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
        size_t len = strlen(buffer);
        if (len < sizeof(buffer) - 1) {
            buffer[len] = c;
            Serial.print(c);
            buffer[len + 1] = '\0';
        }
    }
}


// Função principal de leitura
void Ler() {
    LeSerial();
    ProcessaBuffer();
    LerFimCurso();
    // Chama Ler_ph apenas se o flag estiver ativo
    if (flg_PH) {
        Ler_ph();
    }
}

// Configuração inicial
void setup() {
    Start_serial();
    Serial.println(F("Sistema inicializando..."));
    ConfiguraSensorFim();    
    

    // Movimentação inicial (opcional)
    EnviarSerialLn(F("Iniciando movimento para a direita..."));
    flagMoverDir = true;
    passosDados = 0;
    flagRetCarro = true;
    flagMoverLong = true;
    WellCome();
    Serial.println('Feito!');
}

// Loop principal
void loop() {

    Ler();
    Analisar();
}
