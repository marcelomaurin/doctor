import socket
import serial
import threading
import logging
from datetime import datetime

# Configurações da porta serial
serial_port = '/dev/ttyUSB0'  # Substitua pelo nome da sua porta serial
baud_rate = 9600  # Taxa de transmissão da porta serial

# Configurações do servidor TCP
tcp_ip = '0.0.0.0'  # Escuta em todas as interfaces de rede
tcp_port = 8101

# Configuração do log
log_file = '/var/log/serial_tcp.log'
logging.basicConfig(
    filename=log_file,
    level=logging.INFO,
    format='%(asctime)s - %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S'
)

# Função para registrar logs
def log_message(direction, data):
    direction_str = 'TX' if direction == 'send' else 'RX'
    logging.info(f'{direction_str}: {data.strip().decode(errors="replace")}')

# Função para ler da porta serial e enviar para o cliente TCP
def serial_to_tcp(client_socket, ser):
    while True:
        data = ser.read(ser.in_waiting or 1)
        if data:
            log_message('recv', data)  # Log de RX
            client_socket.sendall(data)

# Função para ler do cliente TCP e enviar para a porta serial
def tcp_to_serial(client_socket, ser):
    while True:
        data = client_socket.recv(1024)
        if data:
            log_message('send', data)  # Log de TX
            ser.write(data)

def main():
    try:
        # Inicializa a porta serial
        ser = serial.Serial(serial_port, baud_rate, timeout=1)
        logging.info("Porta serial inicializada com sucesso.")

        # Inicializa o servidor TCP
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.bind((tcp_ip, tcp_port))
        server_socket.listen(1)
        logging.info(f'Servidor TCP aguardando conexões na porta {tcp_port}...')
        
        client_socket, addr = server_socket.accept()
        logging.info(f'Conexão estabelecida com {addr}')

        # Cria threads para comunicação bidirecional
        threading.Thread(target=serial_to_tcp, args=(client_socket, ser), daemon=True).start()
        threading.Thread(target=tcp_to_serial, args=(client_socket, ser), daemon=True).start()

        # Mantém o script em execução
        while True:
            pass
    except Exception as e:
        logging.error(f'Erro: {e}')
    finally:
        logging.info('Encerrando servidor...')
        if ser.is_open:
            ser.close()
        if client_socket:
            client_socket.close()
        server_socket.close()

if __name__ == '__main__':
    main()
