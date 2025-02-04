import socket
import threading
import serial
import logging

# Configurações do arquivo de log
LOG_FILE = '/var/log/serial_tcp.log'

# Configuração do logging: registra mensagens no arquivo e na saída padrão.
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s [%(levelname)s] %(message)s',
    handlers=[
        logging.FileHandler(LOG_FILE),
        logging.StreamHandler()
    ]
)

# Configurações da porta serial
SERIAL_PORT = '/dev/ttyACM0'
BAUD_RATE = 9600

# Lista de clientes conectados e lock para acesso seguro
clients = []
clients_lock = threading.Lock()

# Inicializa a porta serial
try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0)
    logging.info(f"Porta serial {SERIAL_PORT} aberta com sucesso!")
except Exception as e:
    logging.error(f"Erro ao abrir a porta serial {SERIAL_PORT}: {e}")
    exit(1)

def client_handler(conn, addr):
    """
    Trata a comunicação com cada cliente.
    Recebe dados do cliente e os encaminha para a porta serial.
    """
    logging.info(f"Cliente conectado: {addr}")
    try:
        while True:
            data = conn.recv(1024)
            if not data:
                # Cliente desconectou
                break
            logging.info(f"Recebido {len(data)} bytes do cliente {addr}. Encaminhando para a porta serial.")
            ser.write(data)
    except Exception as e:
        logging.error(f"Erro com o cliente {addr}: {e}")
    finally:
        logging.info(f"Cliente desconectado: {addr}")
        with clients_lock:
            if conn in clients:
                clients.remove(conn)
        conn.close()

def serial_reader():
    """
    Lê continuamente os dados da porta serial e os envia para todos os clientes conectados.
    """
    while True:
        try:
            if ser.in_waiting:
                # Lê todos os bytes disponíveis
                data = ser.read(ser.in_waiting)
                if data:
                    logging.info(f"Recebidos {len(data)} bytes da porta serial. Encaminhando para clientes.")
                    with clients_lock:
                        for client in list(clients):  # Usa uma cópia da lista para evitar problemas durante remoção
                            try:
                                client.sendall(data)
                            except Exception as e:
                                logging.error(f"Erro ao enviar dados para um cliente: {e}")
                                clients.remove(client)
        except Exception as e:
            logging.error(f"Erro na leitura da porta serial: {e}")

def tcp_server():
    """
    Inicia o servidor TCP, aceita conexões e cria uma thread para cada cliente.
    """
    host = ''  # Aceita conexões em todas as interfaces
    port = 8101
    server_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # Permite reusar o endereço para evitar "address already in use"
    server_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    try:
        server_sock.bind((host, port))
    except Exception as e:
        logging.error(f"Erro ao vincular o socket na porta {port}: {e}")
        return
    server_sock.listen(5)
    logging.info(f"Servidor TCP ouvindo na porta {port}...")
    
    while True:
        conn, addr = server_sock.accept()
        with clients_lock:
            clients.append(conn)
        # Cria uma thread para tratar o cliente (thread normal, não daemon)
        t = threading.Thread(target=client_handler, args=(conn, addr))
        t.start()

if __name__ == "__main__":
    # Inicia a thread de leitura da porta serial (thread normal, não daemon)
    t_serial = threading.Thread(target=serial_reader)
    t_serial.start()
    
    # Inicia o servidor TCP (executado na thread principal)
    tcp_server()
