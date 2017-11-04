#!/bin/bash

# cf http://www.framboise314.fr/donnez-la-parole-a-votre-raspberry-pi/
# cf https://www.domotique-fibaro.fr/topic/5424-tts-avec-raspberry-pi-et-pico-tts/
# run shell script without typing full path: sudo ln -s /home/pi/apps/doxeo-monitor/script/voice.sh /usr/local/bin/voice
# exemple: voice "bonjour monsieur" 10 q

if [ $# = 2 ]
then
	pico2wave -l fr-FR -w /home/pi/music/voice.wav "$1"
    rm /home/pi/music/voice.mp3
    lame /home/pi/music/voice.wav /home/pi/music/voice.mp3
    sound voice.mp3 $2
else
    sudo voice "$1" $2 2> /home/pi/music/voice.err > /home/pi/music/voice.log &
fi

exit 0
