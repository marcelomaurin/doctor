# Nome do compilador
CC = gcc

# Flags de compilação: -Wall e -Wextra para avisos, -pthread para threads
CFLAGS = -Wall -Wextra -pthread -std=c99

# Nome do executável
TARGET = srvserial

# Arquivo(s) fonte(s)
SRCS = main.c

# Regras padrão
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

# Regra para limpar os arquivos gerados
clean:
	rm -f $(TARGET)

.PHONY: all clean
