Über diese README Datei
=======================

Diese Datei beschreibt die Funktionen sowie die Installation der Applikation "EmbeddedLinuxProj" für das BFH-FireFly-Cape. Diese Applikation ist eine Arbeit im Zusammenhang mit dem Embeddedd Linux Modul im CAS ITX.

Über die Applikation
====================

Dateiname: EmbeddedLinuxProj

IDE: Qt Creator 4.7.1 mit Qt 5.11.2 (Qt-Quick Application mit Slide-View)

Übersicht
==========

Verwendete Peripherie
---------------------
* Display
* Taster
* LEDs
* Umgebungslicht-Sensor (MAX44009EDT+T), I2C
* Beschleunigungs-Sensor (MPU-9250), I2C
* Farb-Sensor (TCS34725FN), I2C
* Uart (UEXT-Anschluss)

Der Touch-Sensor wird derzeit nicht verwendet, da die Kommunikation über denselben I2C-Bustreiber sporadisch zu Problemen führt. Der genaue Grund ist nicht bekannt.

Liste der Funktionen
--------------------
* Bewegen einer auf dem Display angezeigten Kugel mit dem Beschleunigungssensor
* Messen und anzeigen des Luxwertes mit Chart
* Messen und Anzeigen einer Farbe
* "Loggen" der Beschleunigungswerte (Senden der Messwerte über Serielle Schnittstelle)

Struktur / Darstellung
----------------------
Die Funktionen sind auf 3 verschiedene Slides aufgeteilt, welche über die Taster ausgewählt werden können:
* Acceleration (Bewegte Kugel)
* Light (Anzeigen des Luxwertes)
* Color (Anzeigen des Farbwertes)

Bedienung
---------
### Taster
- **T1**: Slide nach links
- **T2**: Slide nach rechts
- **T3**: Start/Stop Logging
- **T4**: Applikation beenden
### LEDs
- **L1**, L2, L3, L4: Kurzes Aufleuchten (500ms) beim Start der Applikation
- **L4**: Anzeige des "Logger" Status:

  LED leuchtet nicht: Es werden keine Daten gesendet

  LED Leuchtet: Daten werden über die Serielle Schnittstelle gesendet

Beschreibung der Funktionen
===========================

Bewegen einer Kugel mit dem Beschleunigungssensor
-------------------------------------------------
Mit dem Beschleunigungssensor wird periodisch die Beschleunigung am BFH-FireFly-Cape gemessen. Anhand dieser Messwerte (lineare Beschleunigung, Bestimmung Winkel über Erdbeschleunigung) wird eine Kugel auf dem Display bewegt. Zusätzlich wird die Farbe der Kugel vom gemessenen Farbwert des Farbsensors bestimmt und laufend aktualisiert. Die Messwerte werden ausserdem als Zahlenwerte auf dem Display angezeigt. Die Einheit ist "g" (1 Einheit = 1g = 9.81m/s²). Auf der Z-Achse wird 1g abgezogen, damit auf dem Display alle Komponenten (x,y,z) mit "0" anzeigt werden, wenn das BFH-FireFly-Cape gerade auf dem Tisch liegt.

Messen des Luxwertes
--------------------
Mit dem Luxsensor wird periodisch der aktuelle Luxwert unter dem BFH-FireFly-Cape gemessen. Der aktuelle Messwert wird als Zahlenwert auf dem Display ausgegeben, ausserdem wird der zeitliche Verlauf zusätzlich mit einem Chart dargestellt.

Messen einer Farbe
------------------
Mit dem Farbsensor wird periodisch der aktuelle Farbwert unter dem BFH-FireFly-Cape gemessen. Die auf dem Display angezeigten Farbwerte "Clear: xxx Red: xxx, .." sind ADC-Werte relativ zum maximal möglichen ADC-Wert (Anzeige = ADC_meas/ADC_max). Die klein geschriebenen Farbwerte (red, green, ..) sind normiert zum Farbwert mit der höchsten Intensität (ein RGB-Farbwert ist immer = 1).
Um die Dynamik zu vergrössern wird die Integrationszeit ("Belichtungszeit") des Sensors verändert, wenn der ADC-Messwert des Kanals "Clear" (über alle Farben) über 90% bzw. unter 10% gegenüber dem maximal möglichen ADC-Wert liegt.

Installation und Konfiguration
==============================

Kopieren der Applikation in ein lokales Verzeichnis
---------------------------------------------------
Empfohlener Pfad: ~/qt5/
Dateiname: EmbeddedLinuxProj

Devices
-------
- Der Name der i2c-Schnittstelle (Sensoren) muss "i2c-4" lauten
- Der Name des Terminals mit der Seriellen Schnittstelle (UEXT) muss "ttyS1" lauten

Benötigte udev-Rules
--------------------
**i2c**, /etc/udev/rules.d/i2c.rules:
```
    KERNEL=="i2c-[0-9]*", NAME="i2c/%n", SYMLINK+="%k"
    KERNEL=="i2c-0" , GROUP="dialout", MODE="0660"
    KERNEL=="i2c-[1-9]*", GROUP="dialout", MODE="0666"
```
**gpio**, /etc/udev/rules.d/gpio.rules:
```
    SUBSYSTEM=="gpio", KERNEL=="gpiochip*", ACTION=="add", PROGRAM="/bin/sh -c 'chown root:dialout /sys/class/gpio/export /sys/class/gpio/unexport ; chmod 220 /sys/class/gpio/export /sys/class/gpio/unexport'"
    SUBSYSTEM=="gpio", KERNEL=="gpio*", ACTION=="add", PROGRAM="/bin/sh -c 'chown root:dialout /sys%p/active_low /sys%p/direction /sys%p/edge /sys%p/value ; chmod 660 /sys%p/active_low /sys%p/direction /sys%p/edge /sys%p/value'"
```
**uart bzw. ttyS1**, /etc/udev/rules.d/tty.rules:
```
    KERNEL=="ttyS[0-9]*",MODE="0666"
```
Konfiguration für die Serielle Schnittstelle auf dem Host
---------------------------------------------------------
- baudrate:        115200
- bits:            8
- parity:          N
- stopbits:        1
- rtscts:          No 

Neuen Device Tree auf dem Target installieren
---------------------------------------------
Damit die Serielle Schnittstelle am UEXT-Anschluss unterstützt wird muss ein neuer Device Tree Blob auf dem BFH-FireFly-Cape installiert werden:

1. Neuen DeviceTree auf dem Target (FireFly) in ein lokales Verzeichnis kopieren (neuer devicetree z.B. unter [Link Github](https://github.com/akasilov/EmbeddedLinuxProj/tree/master/devicetree), rk3399-firefly-linux.dtb)
```
    $ cp rk3399-firefly-linux.dtb /usr/local/bin
    $ cd /usr/local/bin
```
2. Prüfen, ob der Device Tree Blob ausführbar ist
```
    $ ls -lh rk3399-firefly-linux.dtb
```
3. Falls nicht, müssen zusätzlich Ausführungsrechte vergeben werden
```
    $ sudo chmod +x rk3399-firefly-linux.dtb
```
4. U-boot Partition mounten:
```
    $ sudo mount /dev/mmcblk1p4 /mnt/
```
5. Device Tree Blob in U-boot Partition kopieren (gegebenenfalls aktuellen Device Tree Blob zuvor sichern !)
```
    $ sudo cp /usr/local/bin/rk3399-firefly-linux.dtb /mnt/
```
6. U-boot Partition unmounten:
```
    $ sudo umount /mnt
```
7. BFH-FireFly-Cape neu starten
```
    $ sudo reboot
```

Applikation starten
===================

Vor dem Starten der Applikation
-------------------------------
### Trennen des Touch-Screen Sensors vom I2C-4-Treiber (edt_ft5x06):
Touchscreen deaktivieren (vor Start der Applikation):
```
    $ cd /sys/bus/i2c/drivers/edt_ft5x06
    $ sudo su
    $ echo -n "4-0038" > unbind
    $ exit
```
Touchscreen reaktivieren (nach Beenden der Applikation):
```
    $ cd /sys/bus/i2c/drivers/edt_ft5x06
    $ sudo su
    $ echo -n "4-0038" > bind 
    $ exit
```
### Herunterfahren des X-Servers
X-Server herunterfahren (vor Start der Applikation):
```
    $ sudo /usr/local/bin/xdown.sh
```
X-Server starten (nach Beenden der Applikation):
```
    $ sudo /usr/local/bin/xup.sh
```
Starten
-------
```
    $ ~/qt5/EmbeddedLinuxProj
```

