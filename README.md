# Web connected thermostat with Raspberry Pi

![Home](/pictures/web_presentation.png)

## Requirements

* Qt 5
* Mysql

## Installation

* Generate a MakeFile: qmake
* Compile the application: make
* Configure the application: ./doxeo-monitor --configure
* Create a new user: ./doxeo-monitor --user
* Run : ./doxeo-monitor
* Go the web page: http://localhost:8080
* To get somme help : ./doxeo-monitor --help

## Work also with Doxeo-Android

![Home](/pictures/android_presentation2.png)
![Home](/pictures/android_presentation.png)

## Extras

### Install Mysql driver on windows

 1. Download the driver [here](https://dev.mysql.com/downloads/connector/c/).
 2. Copy libmysql.dll into C:\Program Files\Qt\5.5\mingw492_32\bin
