# Tideanzeiger


![Groß (DSC01956)](https://github.com/seb201/tideanzeiger/assets/35576062/1ee6b10e-66a8-4343-9701-92dc59f031f7)
<br>


Dieses Projekt visualiesiert mittels eines ESP32, einer LED-Matrix Anzeige und einzel ansteuerbaren LEDs den Wasserstand von Flüssen oder der Küste.
Es zeigt den Wasserpegel in Zentimetern auf einer Matix LED Anzeige an, 

Das Projekt entstand im Rahmen einer Abschlussprüfung zum IT-Systemelektroniker im Jahr 2017.<br><br>


## Herkunft der Daten

Auf der Internetseite pegelonline.de, die vom Wasser- und Schifffahrtsamt betrieben wird, lassen sich hunderte Wasserstands Messstationen von Flüssen, Seeen und Küsten in Deutschland auslesen.
Hier kann man sich alle verfügbaren Stationen anzeigen lassen: https://pegelonline.wsv.de/webservices/rest-api/v2/stations.
Möchte man sich die Stationen für z.B. Wilhelmshaven anzeigen lassen, sucht man einfach mit Strg+F nach der Wilhelmshaven. Falls nichts angezeigt wird, muss man oben im Browser auf "Rohdaten" klicken.<br>
Dank einer umfangreichen Dokumentation der API Schnittstelle, ließ sich schnell die passende URL zusammenbauen.
<br>
<img width="967" alt="Bildschirmfoto 2024-02-13 um 17 42 12" src="https://github.com/seb201/tideanzeiger/assets/35576062/7436c607-c2f8-452e-8e74-44d66a5f01f5">
<br>
Übersichtlicher lassen sich die Stationen hier https://www.pegelonline.wsv.de/gast/karte/standard anzeigen lassen, nur leider sieht man hier die benötigte UUID nicht.
<br>


Die Stations ID (Identifikations Nummer) vom Alten Vorhafen ist:
48d6b739-a79d-4f8b-b47a-50eaaa7ef1b1<br>
So ergab sich dies hier als fertige URL:<br>
https://pegelonline.wsv.de/webservices/rest-api/v2/stations/48d6b739-a79d-4f8b-b47a-50eaaa7ef1b1/W/currentmeasurement.json

Wenn man diese URL im Browser öffnete, kam folgende Ausgabe:<br>
``{
  "timestamp": "2017-03-31T08:06:00+02:00",
  "value": 384.0,
  "trend": -1,
  "stateMnwMhw": "unknown",
  "stateNswHsw": "unknown"
}``<br>

- Timestamp ist das Datum und die Uhrzeit, welches aber nur für die Weboberfläche, um zu sehen wann die Daten zuletzt aktualisiert wurden, benötigt wird.<br>
- Value ist der Wasserstandswert in Zentimetern über Pegelnull. Das Niveau von Pegelnull liegt in der Regel bei 500 cm unter NN (Normal null), diese Festlegung erfolgte, damit alle Wasserstände im Tidebereich (siehe Glossar) ein positives Vorzeichen haben.
- Trend signalisiert, ob auflaufendes Wasser ist (Wert= 1), ablaufendes Wasser ist (Wert= -1) oder sich gerade nichts verändert (Wert= 0).<br><br>
- stateMnwMhw brauchen wir nicht
- stateNswHsw brauchen wir nicht


## Installation

Benötigt wird ein:
- 1X ESP32 (früher lief das Projekt mit einem Arduino Leonardo Ethernet, der ist aber zu schwach, da mittlerweile die Daten nur noch verschlüsselt vom WSA Server abgerufen werden können)
- einzel Ansteuerbare RGB-LEDs vom Typ WS2812 (Anzahl je nachdem wie lang die Anzeige werden soll. Ich verwende 54 LEDs). Ich verwende LEDs mit 30 Stück pro Meter, natürlich können auch welche mit mehr als 30 genommen werden, dann muss aber evtl. ein stärkeres Netzteil verbaut werden
- 2X Matrix LEDs mit dem Controller MAX7219
- einen Kasten wo der ESP32 und die Marrix LEDs einkommen. Ich habe diesen hier verwendet: <br>https://www.reichelt.de/desktop-box-129-x-64-x-41-9-mm-rnd-455-00102-p193316.html<br> allerdings scheint es den zumindest bei Reichelt nicht mehr zu geben
- eine Holzlatte (1 cm dick und 4 cm breit) ca. 2 Meter oder weniger, je nach Deckenhöhe
- 2X bzw. 2 Meter, Alu Schienen für die LEDs. Ich habe diese hier genommen:<br> LED ALU PROFIL N ELOXIERT, 100CM INKL. ABDECKUNG OPAL ZUM EINSCHIEBEN U. 3 MONTAGECLIPS https://www.led-konzept.de/LED-Alu-Profil-N-eloxiert-100cm-inkl-Abdeckung-opal-zum-Einschieben-u-3-Montageclips
- 5 V Netzteil, je nach Helligkeit und Anzahl der LEDs mit entsprechender Leistung (eventuell ein einfaches USB Netzteil)

Sollte sowieso jeder Bastler zu Hause haben:
- Dremel 
- Schrauben
- Säge
- Lötkolben
- Litze

Für die ordentlichen :smile::
- eine Lochrasterplatine (hab ich mir gespart, so viel ist nicht zu verkabeln)<br><br>

 
## Programmierung

Es mussen folgende externe Arduino Bibliotheken installiert werden:

- ArduinoJson
- MaxMatrix
- FastLED
- AsyncTCP (für OTA Updates und Weboberfläche)
- ESPAsyncWebServer (für OTA Updates und Weboberfläche)
- AsyncElegantOTA (für OTA Updates und Weboberfläche)<br><br>

## Verkabelung

Matrix Anzeige:
  data muss an Pin 18 des ESPs
  load muss an Pin 19 des ESPs
  clk muss an Pin 21 des ESPs
  Stromversorgung der Matrix Anzeige über 5V also VIN am ESP

LEDs:
  DATA_PIN muss an Pin 4 des ESPs
  Stromversorgung des LEDs über 5V also VIN am ESP<br>


Die Matrix LEDs bestehen aus zwei Modulen, die miteinander verlötet werden müssen.<br>
![Mittel (DSC01915)](https://github.com/seb201/tideanzeiger/assets/35576062/f75ff86e-faf5-42a1-a0ce-b864af9f689d)<br>
Die Halterung für die Matrix LEDs kann mit einem 3D Drucker ausgedruckt werden (3D Datei als STL und SketchUp liegen bei).<br><br>


## Weboberfläche

Gibt man die IP Adresse des Tidenanzeigers in einen Webbrowser ein, wird eine Weboberfläche angezeigt:<br>
<img width="697" alt="Bildschirmfoto 2024-02-13 um 18 25 06" src="https://github.com/seb201/tideanzeiger/assets/35576062/99d51319-8ef6-4bbe-a9b1-3e74c71057fe"><br>
Auf dieser sieht man die vom Pegel übermittelten Daten und die Wlan Empfangsstärke.
Zusätzlich kann man Firmware Updates einspielen. Der Standard Benutzername lautet Admin, das Passwort 1234, dies kann im Quellcode angepasst werden.<br><br>


## Was noch verbessert werden kann

- die Weboberfläche zeigt manchmal komische Werte für den Timestamp an, wahrscheinlich ein Dateityp Problem, stört aber nicht weiter

