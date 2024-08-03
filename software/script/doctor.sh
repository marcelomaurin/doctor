#!/bin/bash

# Inicializando servicos
/usr/bin/srvfalar &
/usr/bin/srvouve.py &


usleep 10000

#inicializando aplicações
/usr/bin/gerenciador &


# Este script cria um arquivo de log com a data e hora atuais
LOGFILE=~/login_log.txt
echo "Login em: $(date)" >> $LOGFILE
