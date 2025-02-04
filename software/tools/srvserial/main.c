#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8101
#define BUF_SIZE 1024
#define MAX_CLIENTS 10


// Variáveis globais
int serial_fd;
int client_fds[MAX_CLIENTS];
int client_count = 0;

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t serial_mutex  = PTHREAD_MUTEX_INITIALIZER;

/* ===================== FUNÇÕES DE SETUP ===================== */

/* Configura a porta serial (ex: 9600 8N1) e retorna seu descritor */
int SetupSerial(const char *device) {
    int fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if(fd < 0) {
        perror("Erro ao abrir a porta serial");
        return -1;
    }
    
    struct termios options;
    if(tcgetattr(fd, &options) != 0) {
        perror("Erro ao obter atributos da porta serial");
        close(fd);
        return -1;
    }
    
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);
    
    options.c_cflag &= ~PARENB;   // Sem paridade
    options.c_cflag &= ~CSTOPB;   // 1 stop bit
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;       // 8 bits de dados
    options.c_cflag |= CREAD | CLOCAL; // Habilita leitura, ignora controle de modem
    
    options.c_iflag = 0;
    options.c_oflag = 0;
    options.c_lflag = 0;
    
    tcflush(fd, TCIFLUSH);
    if(tcsetattr(fd, TCSANOW, &options) != 0) {
        perror("Erro ao configurar a porta serial");
        close(fd);
        return -1;
    }
    
    return fd;
}

/* Configura e retorna um socket TCP já preparado para receber conexões */
int SetupTCPServer(int port) {
    int server_fd;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Erro ao criar o socket");
        exit(EXIT_FAILURE);
    }
    
    int opt = 1;
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Erro em setsockopt");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    
    if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Erro no bind");
        exit(EXIT_FAILURE);
    }
    
    if(listen(server_fd, 3) < 0) {
        perror("Erro no listen");
        exit(EXIT_FAILURE);
    }
    
    printf("Servidor TCP ouvindo na porta %d\n", port);
    return server_fd;
}

/* ===================== FUNÇÕES DE LEITURA/ESCRITA ===================== */

/* Lê dados da porta serial (SerialRead) */
int SerialRead(char *buffer, int bufsize) {
    int n = read(serial_fd, buffer, bufsize);
    if(n < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
        perror("Erro ao ler da porta serial");
    }
    return n;
}


/* Escreve dados na porta serial (SerialWrite)
   - Bufferiza os dados até encontrar um '\n'
   - Quando encontra, envia todos os dados acumulados (inclusive o '\n')
   - Em caso de estouro do buffer, emite uma mensagem de erro e reseta o buffer */
void SerialWrite(const char *data, int size) {
    // Buffer estático e índice para manter os dados acumulados
    static char commandBuffer[BUF_SIZE];
    static int commandBufferIndex = 0;

    pthread_mutex_lock(&serial_mutex);

    for (int i = 0; i < size; i++) {
        char c = data[i];

        // Se houver espaço no buffer, copia o caractere
        if (commandBufferIndex < BUF_SIZE - 1) {  // Deixa espaço para o terminador '\0', se necessário
            commandBuffer[commandBufferIndex++] = c;
        } else {
            // Buffer cheio sem ter recebido '\n'. Aqui você pode decidir se envia os dados parciais
            // ou apenas descarta-os e reinicia o buffer. Neste exemplo, vamos emitir um erro e limpar o buffer.
            fprintf(stderr, "Buffer overflow na SerialWrite. Comando descartado.\n");
            commandBufferIndex = 0;
            // Se desejar, pode retornar imediatamente:
            // pthread_mutex_unlock(&serial_mutex);
            // return;
        }

        // Se encontrou o caractere de nova linha, envia o comando acumulado
        if (c == '\n') {
            // Opcional: adiciona terminador nulo para imprimir como string (sem afetar o write)
            commandBuffer[commandBufferIndex] = '\0';

            // Imprime o comando que será enviado à serial (para debug)
            printf("Serial Write: %s", commandBuffer);

            // Envia os dados acumulados para a porta serial
            int written = write(serial_fd, commandBuffer, commandBufferIndex);
            if (written < 0) {
                perror("Erro ao escrever na porta serial");
            }

            // Limpa o buffer para a próxima linha
            commandBufferIndex = 0;
        }
    }

    pthread_mutex_unlock(&serial_mutex);
}

/* Lê dados de um cliente (ClientRead) */
int ClientRead(int client_fd, char *buffer, int bufsize) {
    int n = recv(client_fd, buffer, bufsize, 0);
    if(n < 0) {
        perror("Erro ao ler dados do cliente");
    }
    return n;
}

/* Envia dados para todos os clientes conectados (ClientWrite) */
void ClientWrite(const char *data, int size) {
    pthread_mutex_lock(&clients_mutex);
    for(int i = 0; i < client_count; i++){
        int sent = send(client_fds[i], data, size, 0);
        if(sent < 0) {
            perror("Erro ao enviar para o cliente");
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

/* ===================== FUNÇÕES DE MANIPULAÇÃO DE CLIENTES ===================== */

/* Adiciona um novo cliente à lista */
void add_client(int client_fd) {
    pthread_mutex_lock(&clients_mutex);
    if(client_count < MAX_CLIENTS) {
        client_fds[client_count++] = client_fd;
    } else {
        printf("Número máximo de clientes atingido.\n");
        close(client_fd);
    }
    pthread_mutex_unlock(&clients_mutex);
}

/* Remove um cliente da lista */
void remove_client(int client_fd) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++){
        if(client_fds[i] == client_fd) {
            for(int j = i; j < client_count - 1; j++){
                client_fds[j] = client_fds[j+1];
            }
            client_count--;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

/* ===================== LOOP E HANDLER ===================== */

/* Loop principal de leitura da porta serial.
   Ao ler dados, chama ClientWrite para encaminhar aos clientes */
void *SerialLoop(void *arg) {
    (void)arg;
    char buffer[BUF_SIZE];
    
    while(1) {
        int n = SerialRead(buffer, BUF_SIZE);
        if(n > 0) {
            printf("Recebido da serial (%d bytes): %.*s\n", n, n, buffer);
            ClientWrite(buffer, n);
        }
        // Pausa breve para evitar loop acelerado
        usleep(10000);
    }
    return NULL;
}

/* Handler para cada cliente: lê dados enviados e os direciona à porta serial */
void *ClientHandler(void *arg) {
    int client_fd = *(int *)arg;
    free(arg);
    char buffer[BUF_SIZE];
    
    while(1) {
        int n = ClientRead(client_fd, buffer, BUF_SIZE);
        if(n > 0) {
            // Encaminha o comando para a porta serial
            SerialWrite(buffer, n);
        } else if(n == 0) {
            printf("Cliente desconectado.\n");
            close(client_fd);
            remove_client(client_fd);
            break;
        } else {  // n < 0
            close(client_fd);
            remove_client(client_fd);
            break;
        }
    }
    return NULL;
}

/* Loop principal do servidor TCP: aceita conexões e cria threads para cada cliente */
void *TCPServerLoop(void *arg) {
    (void)arg;
    int server_fd = SetupTCPServer(PORT);
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    
    while(1) {
        int client_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if(client_fd < 0) {
            perror("Erro no accept");
            continue;
        }
        printf("Novo cliente conectado: %s:%d\n",
               inet_ntoa(address.sin_addr), ntohs(address.sin_port));
        
        add_client(client_fd);
        
        int *pclient = malloc(sizeof(int));
        if(pclient == NULL) {
            perror("Erro ao alocar memória");
            close(client_fd);
            continue;
        }
        *pclient = client_fd;
        
        pthread_t tid;
        if(pthread_create(&tid, NULL, ClientHandler, pclient) != 0) {
            perror("Erro ao criar thread para o cliente");
            free(pclient);
            close(client_fd);
            continue;
        }
        pthread_detach(tid);
    }
    
    close(server_fd);
    return NULL;
}

/* ===================== FUNÇÃO MAIN ===================== */

int main() {
    // Setup da porta serial
    serial_fd = SetupSerial("/dev/ttyACM0");
    if(serial_fd < 0) {
        exit(EXIT_FAILURE);
    }
    
    pthread_t serial_tid, tcp_tid;
    
    // Cria thread para o loop de leitura da porta serial
    if(pthread_create(&serial_tid, NULL, SerialLoop, NULL) != 0) {
        perror("Erro ao criar thread de leitura da serial");
        exit(EXIT_FAILURE);
    }
    
    // Cria thread para o servidor TCP que aceita conexões
    if(pthread_create(&tcp_tid, NULL, TCPServerLoop, NULL) != 0) {
        perror("Erro ao criar thread do servidor TCP");
        exit(EXIT_FAILURE);
    }
    
    // Aguarda as threads (neste exemplo, elas rodam indefinidamente)
    pthread_join(serial_tid, NULL);
    pthread_join(tcp_tid, NULL);
    
    close(serial_fd);
    return 0;
}
