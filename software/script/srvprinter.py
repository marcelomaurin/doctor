import socket
import threading
from escpos.printer import Serial
from PIL import Image
import logging

# Configurações da porta serial para a impressora térmica
serial_port = '/dev/ttyACM0  # Substitua pela porta serial da impressora
baud_rate = 9600  # Taxa de transmissão da porta serial

# Configurações do servidor TCP
tcp_ip = '0.0.0.0'
tcp_port = 8102

# Configuração do log
log_file = '/var/log/thermal_printer.log'
logging.basicConfig(
    filename=log_file,
    level=logging.INFO,
    format='%(asctime)s - %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S'
)

# Função para ajustar texto ao número de colunas
def format_text(text, columns, align='left'):
    if align == 'left':
        return text.ljust(columns)[:columns]
    elif align == 'center':
        return text.center(columns)[:columns]
    elif align == 'right':
        return text.rjust(columns)[:columns]
    else:
        return text.ljust(columns)[:columns]

# Função para processar os comandos ESC/POS
def process_command(command, printer):
    try:
        if command.startswith("TEXT:"):
            parts = command[len("TEXT:"):].strip().split("|")
            text = parts[0].strip()
            columns = int(parts[1]) if len(parts) > 1 else 32
            align = parts[2].strip() if len(parts) > 2 else 'left'
            formatted_text = format_text(text, columns, align)
            printer.set(align='left')
            printer.text(formatted_text + "\n")
        elif command.startswith("BIGTEXT:"):
            text = command[len("BIGTEXT:"):].strip()
            printer.set(text_type='B', align='center', width=2, height=2)
            printer.text(text + "\n")
        elif command.startswith("MICROTEXT:"):
            text = command[len("MICROTEXT:"):].strip()
            printer.set(text_type='NORMAL', align='left', width=1, height=1)
            printer.text(text + "\n")
        elif command.startswith("BARCODE:"):
            barcode = command[len("BARCODE:"):].strip()
            printer.barcode(barcode, 'EAN13', function_type='B')
        elif command.startswith("QRCODE:"):
            qrcode = command[len("QRCODE:"):].strip()
            printer.qr(qrcode)
        elif command.startswith("IMAGE:"):
            image_path = command[len("IMAGE:"):].strip()
            img = Image.open(image_path)
            printer.image(img)
        elif command.startswith("PICCUT:"):
            cut_type = command[len("PICCUT:"):].strip().upper()
            if cut_type == "PARTIAL":
                printer.cut(mode='PART')
            elif cut_type == "FULL":
                printer.cut(mode='FULL')
            else:
                logging.warning(f"Tipo de corte inválido: {cut_type}")
        else:
            logging.warning(f"Comando não reconhecido: {command}")
        
        printer.set()  # Reseta configurações após cada comando
        logging.info(f'Comando processado: {command}')
    except Exception as e:
        logging.error(f'Erro ao processar comando: {command} - {e}')

# Função para gerenciar conexões TCP
def handle_client(client_socket, printer):
    try:
        while True:
            data = client_socket.recv(1024).decode().strip()
            if not data:
                break
            commands = data.split("/n")
            for command in commands:
                process_command(command, printer)
    except Exception as e:
        logging.error(f'Erro na conexão com cliente: {e}')
    finally:
        client_socket.close()

# Função principal
def main():
    try:
        # Inicializa a impressora térmica via porta serial
        printer = Serial(serial_port, baudrate=baud_rate, timeout=1)
        logging.info("Impressora inicializada com sucesso.")

        # Inicializa o servidor TCP
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.bind((tcp_ip, tcp_port))
        server_socket.listen(1)
        logging.info(f'Servidor TCP aguardando conexões na porta {tcp_port}...')
        
        while True:
            client_socket, addr = server_socket.accept()
            logging.info(f'Conexão estabelecida com {addr}')
            threading.Thread(target=handle_client, args=(client_socket, printer), daemon=True).start()
    except Exception as e:
        logging.error(f'Erro no servidor principal: {e}')
    finally:
        logging.info('Encerrando servidor...')
        server_socket.close()

if __name__ == '__main__':
    main()
