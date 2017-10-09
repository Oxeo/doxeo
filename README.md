# Web connected thermostat with Raspberry Pi

![Home](/pictures/web_presentation.png)

## Requirements

* Qt 5 (Qtbase, QtSerialPort and Qtscript)
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

### Install Qtbase

 1. Download Qt [here](http://download.qt.io/archive/qt/5.5/5.5.1/single/qt-everywhere-opensource-src-5.5.1.tar.xz).
 2. Copy the folder qtbase into your raspberry pi
 4. create a folder qtbase-build next to qtbase folder
 3. go to qtbase-build folder
 3. configure the build
```
../qtbase/configure \
 -v -opengl es2 \
 -device linux-rasp-pi-g++ \
 -device-option CROSS_COMPILE=/usr/bin/ \
 -opensource -confirm-license \
 -optimized-qmake -reduce-exports -force-pkg-config \
 -nomake examples -no-compile-examples \
 -release -qt-pcre -prefix /opt/Qt5.5.1
```
 5. do: make
 6. do: sudo make install
 
### Install QtSerialPort

 1. Download Qt [here](http://download.qt.io/archive/qt/5.5/5.5.1/single/qt-everywhere-opensource-src-5.5.1.tar.xz).
 2. Copy the folder qtserialport into your raspberry pi
 3. Create a folder qtserialport-build next to the repository qtserialport
 4. Open the folder qtserialport-build
 5. do: qmake ../qtserialport/qtserialport.pro
 6. do: make
 7. do: sudo make install
 
### Install Qtscript

 1. Download Qt [here](http://download.qt.io/archive/qt/5.5/5.5.1/single/qt-everywhere-opensource-src-5.5.1.tar.xz).
 2. Copy the folder qtscript into your raspberry pi
 3. Create a folder qtscript-build next to the repository qtscript
 4. Open the folder qtscript-build
 5. do: qmake ../qtscript/qtscript.pro
 6. do: make
 7. do: sudo make install
