#!/bin/bash

# cf http://www.pihomeserver.fr/2015/08/13/envoyer-un-email-depuis-votre-raspberry-pi/
# exemple: ./email.sh toyou@oxeo.fr subject message
echo "$3" | mail -s "$2" $1

exit 0 

