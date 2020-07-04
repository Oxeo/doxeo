# Web connected thermostat with Raspberry Pi

![Home](/pictures/web_presentation.png)
![Home](/pictures/android_presentation2.png)
![Home](/pictures/android_presentation.png)

## Requirements

* Qt 5 (Qtbase, QtSerialPort and Qtscript)
* Mysql

## Installation

* Generate a MakeFile: qmake
* Compile the application: make -j4
* Configure the application: sudo ./doxeo-monitor --configure
* Create a new user: ./doxeo-monitor --user
* Run : ./doxeo-monitor
* Go the web page: http://localhost:8080
* To get somme help : ./doxeo-monitor --help

## Extras

### Install Qt 5.11
 1. do: sudo apt-get update
 2. do: sudo rpi-update
 3. Install required development packages
```
 sudo apt-get install build-essential libfontconfig1-dev libdbus-1-dev libfreetype6-dev libicu-dev libinput-dev libxkbcommon-dev libsqlite3-dev libssl1.0-dev libpng-dev libjpeg-dev libglib2.0-dev libraspberrypi-dev
```
 4. Support for various databases (PostgreSQL, MariaDB/MySQL)
```
sudo apt-get install libpq-dev libmariadbclient-dev
```
 5. Install SSL driver on raspbian: sudo apt-get install openssl
 6. Download [Qt](http://download.qt.io/archive/qt/5.11/5.11.2/single/qt-everywhere-src-5.11.2.tar.xz) with wget command.
 7. Uncompress the file : tar xf qt-everywhere-src-5.11.2.tar.xz
 8. create a folder qtbase-build next to qtbase folder
 9. go to qtbase-build folder
 10. configure the build
```
PKG_CONFIG_LIBDIR=/usr/lib/arm-linux-gnueabihf/pkgconfig:/usr/share/pkgconfig \
PKG_CONFIG_SYSROOT_DIR=/ \
../qtbase/configure -v -no-opengl -no-gtk \
-device linux-rasp-pi2-g++ -device-option CROSS_COMPILE=/usr/bin/ \
-opensource -confirm-license -release -reduce-exports \
-force-pkg-config -no-kms -nomake examples -no-compile-examples -no-pch \
-qt-pcre -ssl -evdev -system-freetype -fontconfig -glib -prefix /opt/Qt5.11
```
> we do not used linux-rasp-pi3-g++ because qtscript don't like it ("Not supported ARM architecture")
 11. do: make -j4
 12. do: sudo make install
 13. do: sudo ln -s /opt/Qt5.11/bin/qmake /usr/local/bin/qmake

You now have Qt installed in /opt/Qt5.11 ready to be used. To configure your Qt project(s) to build with this version run qmake from the installation directory:
```
/opt/Qt5.11/bin/qmake
```
More details [here](http://www.tal.org/tutorials/building-qt-510-raspberry-pi-debian-stretch).
 
### Install QtSerialPort

 1. Create a folder qtserialport-build next to the repository qtserialport
 2. Go to the folder qtserialport-build and do:
```
 qmake ../qtserialport/qtserialport.pro
 make -j4
 sudo make install
```
 
### Install QtDeclarative for QJSEngine

 1. Create a folder qtdeclarative-build next to the repository qtdeclarative
 2. Go to the folder qtdeclarative-build and do:
```
 qmake ../qtdeclarative/qtdeclarative.pro
 make -j4
 sudo make install
```

### Install phpmyadmin

 1. do: sudo apt-get install phpmyadmin
 2. let dbconfig-common configuring the database of phpmyadmin
 3. enter the root password of mysql
 4. do: sudo ln -s /usr/share/phpmyadmin/ /var/www/html/
 5. autorize www-data for /var/www/html/phpmyadmin
 6. at the command line, log in to MySQL as the root user:
```
mysql -u root -p
```
 7. type the MySQL root password, and then press Enter.
 8. create database with user:
```
CREATE DATABASE doxeo;
CREATE USER 'doxeo'@'localhost' IDENTIFIED BY 'password';
GRANT ALL PRIVILEGES ON doxeo . * TO 'doxeo'@'localhost';
FLUSH PRIVILEGES;
exit
```
> If you don't known the mysql root password you can do like [here](https://www.domo-blog.fr/comment-modifier-le-mot-de-passe-mysql-root-de-jeedom/) without omitting sudo before commands.
 
### Allowing your linux userid permission to use your usb device

 1. ls -la /dev/ttyUSB0
 2. sudo usermod -a -G dialout myUserName
 
### Permission denied /dev/ttyAMA0

Disable the login shell on the serial port in the interfacing options of "sudo raspi-config", and reboot.
 
### Run Doxeo at startup

 1. do: sudo vim /etc/rc.local
 2. add: su pi -c '/home/pi/apps/doxeo-monitor/doxeo-monitor --path /home/pi/apps/doxeo-monitor'
 
### Keep internal clock synchronized

* https://formation-debian.viarezo.fr/ntp.html
* https://serverfault.com/questions/559666/why-doesnt-ntp-recover-after-an-internet-less-startup
 
### Send mail with raspberry

* http://www.pihomeserver.fr/2015/08/13/envoyer-un-email-depuis-votre-raspberry-pi/
* sudo ln -s /home/pi/doxeo/script/email.sh /usr/local/bin/email

### Increase Apache2 uri length limit

1. open /etc/apache2/apache2.conf
2. insert under AccessFileName .htaccess:
```
LimitRequestLine 1000000
LimitRequestFieldSize 1000000
```
3. sudo service apache2 restart

### Enable Let’s Encrypt (Certboot)
```
sudo apt-get install certbot
sudo certbot certonly --webroot -w /home/pi/doxeo --agree-tos --no-eff-email -d doxeo.oxeo.fr --rsa-key-size 4096
```

```
chown -R pi /etc/letsencrypt/live/doxeo.oxeo.fr
chown -R pi /etc/letsencrypt/archive/doxeo.oxeo.fr
ln -s /etc/letsencrypt/live/doxeo.oxeo.fr/cert.pem /home/pi/doxeo/cert.pem
ln -s /etc/letsencrypt/live/doxeo.oxeo.fr/privkey.pem /home/pi/doxeo/privkey.pem
ln -s /etc/letsencrypt/live/doxeo.oxeo.fr/privkey.pem /home/pi/doxeo/chain.pem
```
More details [here](https://certbot.eff.org/lets-encrypt/debianstretch-other) and [here](https://howto.wared.fr/ubuntu-certificats-ssl-tls-certbot/).

### Install ddclient for DynHost OVH

1. sudo apt-get install ddclient
2. sudo vim /etc/ddclient.conf
```
protocol=dyndns2
use=web, web=checkip.dyndns.com
server=www.ovh.com
login=nomdedomaine.net-login
password='mypassword'
dyn.nomdedomaine.net
```
3. sudo service ddclient restart

### Install Firebase Admin
1. Enable python 3.5 (details [here](https://learn.sparkfun.com/tutorials/python-programming-tutorial-getting-started-with-the-raspberry-pi/configure-your-pi))
2. Add the Firebase Admin SDK to Your Server (more details [here](https://firebase.google.com/docs/admin/setup/))
```
sudo pip3 install firebase-admin
```
3. Update urllib3 version
```
pip3 install urllib3
pip3 install --upgrade urllib3
```

### Enable ServerName for Apache2

1. do: sudo apt-get install mod_proxy
2. do: sudo a2enmod proxy_http
3. edit the file /etc/apache2/sites-enabled/000-default.conf like
```
NameVirtualHost *:80

<VirtualHost *:80>
    ServerAdmin webmaster@localhost
    ServerName mysite1.fr
    DocumentRoot /var/www/html
    ErrorLog /var/www/html/log/http.error
</VirtualHost>

<VirtualHost *:80>
    ServerAdmin webmaster@localhost
    ServerName mysitedoxeo.fr
    ProxyPass / http://localhost:8080/
    ProxyPassReverse / http://localhost:8080/
    ProxyPreserveHost On
</VirtualHost>
```
4. do: sudo /etc/init.d/apache2 restart

### Disable GSM PIN code
 
```
gsm.sendAtCmd("AT+CPIN?")
gsm.sendAtCmd('AT+CPIN="1234"')
gsm.sendAtCmd('AT+CLCK="SC",0,"1234"')
gsm.sendAtCmd("AT+CPIN?")
```
 
More details [here](http://dostmuhammad.com/blog/disable-pin-code-using-gsm-modem-at-commands/).

### Fix UART on Raspberry Pi 3 (Pin 8 and 10) by disable bluetooth

1. Add to /boot/config.txt
```
dtoverlay = pi3-disable-bt
```

2. Remove to /boot/cmdline.txt
```
console=serial0,115200
```

### Raspberry Pi - Auto WiFi Hotspot Switch

See details [here](http://www.raspberryconnect.com/network/item/331-raspberry-pi-auto-wifi-hotspot-switch-no-internet-routing).

If your SSID is not found then add your ssid in /usr/bin/autohotspot like: 
```
ssids=('mySSID1' 'mySSID2')
```

if when starting hostapd this error happen
```
Failed to start hostapd.service: Unit hostapd.service is masked
```
then do
```
sudo systemctl unmask hostapd
```

### Install Mysql driver on windows

 1. Download the driver [here](https://dev.mysql.com/downloads/connector/c/).
 2. Copy libmysql.dll into C:\Program Files\Qt\5.5\mingw492_32\bin
 
### Build QMYSQL driver on windows

Open cmd line and run the code:
```
cd %QTDIR%\qtbase\src\plugins\sqldrivers
qmake -- MYSQL_INCDIR="%Mysql%/MySQL Server 8.0/include"  MYSQL_LIBDIR="%Mysql%/MySQL Server 8.0/lib"
mingw32-make
mingw32-make install
```
%QTDIR% is the root direction for qt,
%Mysql% is the root direction for mysql,
See details [here](https://forum.qt.io/topic/110176/build-qmysql-driver-under-windows-10-a-sample-solution-for-qsqldatabase-qmysql-driver-not-loaded).
 
### Install SSL driver on windows

 1. Download Win32 OpenSSL v1.0.2n [here](http://slproweb.com/products/Win32OpenSSL.html).
 2. Install openSLL
 2. Copy libeay32.dll and ssleay32.dll into your executable folder
