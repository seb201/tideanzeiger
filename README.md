# Tideanzeiger


![Groß (DSC01956)](https://github.com/seb201/tideanzeiger/assets/35576062/1ee6b10e-66a8-4343-9701-92dc59f031f7)



Dieses Projekt visualiesiert mittels eines ESP32, einer LED-Matrix Anzeige und einzel ansteuerbaren LEDs den Wasserstand von Flüssen oder der Küste.

Das Projekt entstand im Rahmen einer Abschlussprüfung zum IT-Systemelektroniker.


# Herkunft der Daten

Auf der Internetseite pegelonline.de, die vom Wasser- und Schifffahrtsamt betrieben wird, lassen sich hunderte Wasserstands Messstationen von Flüssen, Seeen und Küsten in Deutschland auslesen.
Hier kann man sich alle verfügbaren Stationen anzeigen lassen: https://pegelonline.wsv.de/webservices/rest-api/v2/stations.
Möchte man sich die Stationen für z.B. Wilhelmshaven anzeigen lassen, sucht man einfach mit Strg+F nach der Wilhelmshaven. Falls nichts angezeigt wird, muss man oben im Browser auf "Rohdaten" klicken
Dank einer umfangreichen Dokumentation der API Schnittstelle, ließ sich schnell die passende URL zusammenbauen. Als Beispiel wurde hier:
https://pegelonline.wsv.de/webservices/rest-api/v2/stations/Stations ID hier eingeben/W/currentmeasurement.json
genommen.

Übersichtlicher lassen sich die Stationen hier https://www.pegelonline.wsv.de/gast/karte/standard anzeigen lassen, nur leider sieht man hier die benötigte UUID nicht.

<img width="967" alt="Bildschirmfoto 2024-02-13 um 17 42 12" src="https://github.com/seb201/tideanzeiger/assets/35576062/7436c607-c2f8-452e-8e74-44d66a5f01f5">



Die Stations ID (Identifikations Nummer) vom Alten Vorhafen ist:
48d6b739-a79d-4f8b-b47a-50eaaa7ef1b1
So ergab sich dies hier als fertige URL:
https://pegelonline.wsv.de/webservices/rest-api/v2/stations/48d6b739-a79d-4f8b-b47a-50eaaa7ef1b1/W/currentmeasurement.json

Wenn man diese URL im Browser öffnete, kam folgende Ausgabe:
``{
  "timestamp": "2017-03-31T08:06:00+02:00",
  "value": 384.0,
  "trend": -1,
  "stateMnwMhw": "unknown",
  "stateNswHsw": "unknown"
}``

Timestamp ist das Datum und die Uhrzeit, welches aber nicht benötigt wird.
Value ist der Wasserstandswert in Zentimetern über Pegelnull. Das Niveau von Pegelnull liegt in der Regel bei 500 cm unter NN (Normal null), diese Festlegung erfolgte, damit alle Wasserstände im Tidebereich (siehe Glossar) ein positives Vorzeichen haben.
Trend signalisiert, ob auflaufendes Wasser ist (Wert= 1), ablaufendes Wasser ist (Wert= -1) oder sich gerade nichts verändert (Wert= 0).


# Installation

Benötigt wird ein:
- 1X ESP32 (früher lief das Projekt mit einem Arduino Leonardo Ethernet, der ist aber zu schwach, da mittlerweile die Daten nur noch verschlüsselt vom WSA Server abgerufen werden können)
- einzel Ansteuerbare RGB-LEDs vom Typ WS2812 (Anzahl je nachdem wie lang die Anzeige werden soll. Ich verwende 54 LEDs)
- 2X Matrix LEDs mit dem Controller MAX7219
- einen Kasten wo der ESP32 und die Marrix LEDs einkommen. Ich habe diesen hier verwendet https://www.reichelt.de/desktop-box-129-x-64-x-41-9-mm-rnd-455-00102-p193316.html allerdings scheint es den zumindest bei Reichelt nicht mehr zu geben
- eine Holzlatte (1 cm dick und 4 cm breit) ca. 2 Meter oder weniger, je nach Deckenhöhe
- 2X bzw. 2 Meter, Alu Schienen für die LEDs. Ich habe diese hier genommen LED ALU PROFIL N ELOXIERT, 100CM INKL. ABDECKUNG OPAL ZUM EINSCHIEBEN U. 3 MONTAGECLIPS https://www.led-konzept.de/LED-Alu-Profil-N-eloxiert-100cm-inkl-Abdeckung-opal-zum-Einschieben-u-3-Montageclips
- 5 V Netzteil, je nach Helligkeit und Anzahl der LEDs mit entsprechender Leistung (eventuell ein einfaches USB Netzteil)

Sollte sowieso jeder Bastler zu Hause haben:
- Dremel 
- Schrauben
- Säge
- Lötkolben
- Litze

Für die ordentlichen :smile::
- evtl. eine Lochrasterplatine


![Mittel (DSC01915)](https://github.com/seb201/tideanzeiger/assets/35576062/f75ff86e-faf5-42a1-a0ce-b864af9f689d)


 
# Programmierung

Es mussen folgende externe Arduino Bibliotheken installiert werden:

- ArduinoJson
- MaxMatrix
- FastLED
- AsyncTCP (für OTA Updates und Weboberfläche)
- ESPAsyncWebServer (für OTA Updates und Weboberfläche)
- AsyncElegantOTA (für OTA Updates und Weboberfläche)


# Weboberfläche

Gibt man die IP Adresse des Tidenanzeigers in einen Webbrowser ein, wird eine 

<img width="697" alt="Bildschirmfoto 2024-02-13 um 18 25 06" src="https://github.com/seb201/tideanzeiger/assets/35576062/99d51319-8ef6-4bbe-a9b1-3e74c71057fe">



# Was noch verbessert werden kann

- die Weboberfläche zeigt manchmal komische Werte für den Timestamp an, wahrscheinlich ein Dateiformat Problem, stört aber nicht weiter

