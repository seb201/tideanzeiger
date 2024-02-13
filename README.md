# Tideanzeiger

Dieses Projekt visualiesiert mittels eines ESP32, einer LED-Matrix Anzeige und einzel ansteuerbaren LEDs den Wasserstand von Flüssen oder der Küste.

Das Projekt entstand im Rahmen einer Abschlussprüfung zum IT-Systemelektroniker.

# Funktion

Das Wasser und Schifffahrtsamt (WSA) bietet auf seiner Webseite Wasserstände von Flüssen, Seen und Küsten an. Diese Daten werden auch im JSON Format zur Verfügung gestellt.
Diese Daten werden von dem Microcontroller abgerufen und verarbeitet. 


# Installation

Benötigt wird ein:
- Microcontroller (dieses Projekt ist angepasst an einen ESP32, erfolgreich getestet außerdem mit einem Leonardo Ethernet (wer zwingend einen Kabelanschluss benötigt)
- einzel Ansteuerbare RGB-LEDs vom Typ WS2812
- Matrix LEDs mit dem Controller MAX7219
- einen Kasten wo der ESP32 und die Marrix LEDs einkommen
- eine Holzlatte ca. 2 Meter oder weniger, je nach Deckenhöhe
- Alu Schienen für die LEDs
- 5 V Netzteil (eventuell ein einfaches USB Netzteil)

Sollte sowieso jeder Bastler zu Hause haben:
- Dremel 
- Schrauben
- Säge
- Lötkolben
- Litze

Für die ordentlichen :smile::
- Lochrasterplatine
 
![Bild 1](/bilder/IMG_0873.JPG)
Format: ![Alt Text](url)

# Programmierung

Es werden folgende Arduino Bibliotheken benötigt:
- ArduinoJSON (unbedingt eine ältere Version als 6.0 verwenden, da sonst das compilieren fehlschlägt)
