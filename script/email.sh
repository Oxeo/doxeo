#!/bin/bash

# cf http://www.pihomeserver.fr/2015/08/13/envoyer-un-email-depuis-votre-raspberry-pi/
# run shell script without typing full path: sudo ln -s /home/pi/apps/doxeo-monitor/script/email.sh /usr/local/bin/email
# exemple: email toyou@oxeo.fr subject message
echo "$3" | mail -s "$2" $1

exit 0 

