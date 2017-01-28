#!/usr/bin/python
import os
import glob
import time
import platform


class Thermometer:
    def __init__(self, base_dir='/sys/bus/w1/devices/'):
        try:
            device_folder = glob.glob(base_dir + '28*')[0]
        except Exception, e:
            os.system('modprobe w1-gpio')
            os.system('modprobe w1-therm')
            time.sleep(1)
            device_folder = glob.glob(base_dir + '28*')[0]
        self.device_file = device_folder + '/w1_slave'      

    def read(self):
        lines = self.__read_temp_raw()
        while lines[0].strip()[-3:] != 'YES':
            time.sleep(0.2)
            lines = self.__read_temp_raw()
        equals_pos = lines[1].find('t=')
        if equals_pos != -1:
            temp_string = lines[1][equals_pos+2:]
            temp_c = float(temp_string) / 1000.0
            return temp_c
            
    def __read_temp_raw(self):
        f = open(self.device_file, 'r')
        lines = f.readlines()
        f.close()
        return lines


if __name__ == '__main__':
    if platform.system() == "Linux":
        thermometer = Thermometer()
        temp = thermometer.read()
    else:
        temp = 25.0
    print("%.2f" % temp)
