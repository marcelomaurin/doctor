#!/bin/bash

xinput set-prop 'eGalax Inc. USB TouchController' 'Coordinate Transformation Matrix' 0 1 0 1 0 0 0 0 1

#start dos servidores
/usr/bin/srvfalar &
/usr/bin/srvouve.py &
/usr/bin/srvcmptexto2.py &
/usr/bin/srvprinter.py &
/usr/bin/srvport.py &



usleep 2000
#start das aplicacoes
#/usr/bin/gerenciador &



echo "O script foi executado!" > ~/log.txt
