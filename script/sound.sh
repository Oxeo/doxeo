#!/bin/bash

# cf https://jeffskinnerbox.wordpress.com/2012/11/15/getting-audio-out-working-on-the-raspberry-pi/
# exemple: ./email.sh toyou@oxeo.fr subject message

if [ $1 = "stop" ]
then
	sudo killall mpg321 2> sound_error.log
else
    sudo mpg321 -qg $2 /home/pi/music/$1 2> sound_error.log > sound.log &
fi

exit 0
