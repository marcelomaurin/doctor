#!/bin/bash

xinput set-prop 'eGalax Inc. USB TouchController' 'Coordinate Transformation Matrix' 0 1 0 1 0 0 0 0 1

#start dos servidores
/usr/bin/srvfalar &
/usr/bin/srvouve.py &
/usr/bin/srvcmptexto2.py &

usleep 1000
#start das aplicacoes
/usr/bin/gerenciador &



echo "O script foi executado!" > ~/log.txt
