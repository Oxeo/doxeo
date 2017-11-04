#!/bin/bash

# cf https://jeffskinnerbox.wordpress.com/2012/11/15/getting-audio-out-working-on-the-raspberry-pi/
# run shell script without typing full path: sudo ln -s /home/pi/apps/doxeo-monitor/script/sound.sh /usr/local/bin/sound
# exemple: sound music1.mp3 100

if [ $1 = "stop" ]
then
	sudo killall mpg321 2> /home/pi/music/sound.err
else
    sudo mpg321 -qg $2 /home/pi/music/$1 2> /home/pi/music/sound.err > /home/pi/music/sound.log &
fi

exit 0
