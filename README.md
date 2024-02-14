# Tideanzeiger


![Groß (DSC01956)](https://github.com/seb201/tideanzeiger/assets/35576062/1ee6b10e-66a8-4343-9701-92dc59f031f7)
<br>


Dieses Projekt visualisiert den Wasserstand von Flüssen oder der Küste mit Hilfe eines ESP32, einer LED-Matrix-Anzeige und einzeln ansteuerbaren LEDs.<br>
Es zeigt den Wasserstand in Zentimetern auf einem LED-Matrix-Display an.<br>
Bei Sturmflut leuchten alle LEDs rot.<br>
Wenn die oberste LED grün leuchtet, steigt das Wasser (Flut).<br>
Leuchtet die unterste LED grün, läuft das Wasser ab (Ebbe).<br>
50 blaue LEDs signalisieren weithin sichtbar den Wasserstand.<br>

Das Projekt entstand im Rahmen einer Abschlussprüfung zum IT-Systemelektroniker im Jahr 2017.<br>


## Herkunft der Daten
Die benötigten Daten zur Anzeige lassen sich auf der Internetseite pegelonline.de auslesen.
Diese Seite wird vom Wasser- und Schifffahrtsamt betrieben und bietet einen Datenabruf im JSON-Format an.
Über folgenden Link können alle verfügbaren Stationen angezeigt werden: https://pegelonline.wsv.de/webservices/rest-api/v2/stations.<br>
Wenn Sie beispielsweise die Stationen für Wilhelmshaven anzeigen möchten, suchen Sie einfach mit Strg+F nach Wilhelmshaven. Wenn keine Daten angezeigt werden, klicken Sie oben im Browser auf 'Rohdaten' (z.B. bei Firefox, siehe Bild).
Dank der umfangreichen Dokumentation der API-Schnittstelle lässt sich schnell die passende URL zusammenstellen.
<br>
<img width="967" alt="Bildschirmfoto 2024-02-13 um 17 42 12" src="https://github.com/seb201/tideanzeiger/assets/35576062/7436c607-c2f8-452e-8e74-44d66a5f01f5">
<br>
Die Stationen können unter https://www.pegelonline.wsv.de/gast/karte/standard übersichtlicher dargestellt werden, leider ist hier die erforderliche UUID nicht ersichtlich.
<br>


Die Stations-ID (Identifikationsnummer) des Alten Vorhafens lautet: 48d6b739-a79d-4f8b-b47a-50eaaa7ef1b1.
Die fertige URL lautet: https://pegelonline.wsv.de/webservices/rest-api/v2/stations/48d6b739-a79d-4f8b-b47a-50eaaa7ef1b1/W/currentmeasurement.json.<br>

Beim Öffnen dieser URL im Browser erscheint folgende Ausgabe:<br>
``{
  "timestamp": "2017-03-31T08:06:00+02:00",
  "value": 384.0,
  "trend": -1,
  "stateMnwMhw": "unknown",
  "stateNswHsw": "unknown"
}``<br>

- **Timestamp** Das Datum und die Uhrzeit werden lediglich für die Weboberfläche benötigt, um anzuzeigen, wann die Daten zuletzt aktualisiert wurden.<br>
- **Value** Der Wasserstand wird in Zentimetern über Pegelnull angegeben. Pegelnull liegt normalerweise 500 cm unter NN (Normal null). Diese Festlegung sorgt dafür, dass alle Wasserstände im Tidebereich (siehe Glossar) ein positives Vorzeichen haben.<br>
- **Trend** Der Wert signalisiert, ob sich Wasser steigt (Wert = 1), abfließt (Wert = -1) oder sich der Wasserstand nicht verändert (Wert = 0).<br>
- **stateMnwMhw** brauchen wir nicht<br>
- **stateNswHsw** brauchen wir nicht<br>


## Installation

Benötigt wird ein:
- 1X ESP32 (früher lief das Projekt mit einem Arduino Leonardo Ethernet, der ist aber zu schwach, da mittlerweile die Daten nur noch verschlüsselt vom WSA Server abgerufen werden können)
- einzel Ansteuerbare RGB-LEDs vom Typ WS2812 (Anzahl je nachdem wie lang die Anzeige werden soll. Ich verwende 54 LEDs). Ich verwende LEDs mit 30 Stück pro Meter, natürlich können auch welche mit mehr als 30 genommen werden, dann muss aber evtl. ein stärkeres Netzteil verbaut werden
- 2X Matrix LEDs mit dem Controller MAX7219
- einen Kasten wo der ESP32 und die Marrix LEDs einkommen. Ich habe diesen hier verwendet: <br>https://www.reichelt.de/desktop-box-129-x-64-x-41-9-mm-rnd-455-00102-p193316.html<br> allerdings scheint es den zumindest bei Reichelt nicht mehr zu geben
- eine Holzlatte (1 cm dick und 4 cm breit, länge je nach Deckenhöhe, ich habe 2 Meter genommen)
- 2X bzw. 2 Meter, Alu Schienen für die LEDs. Ich habe diese hier genommen:<br> LED ALU PROFIL N ELOXIERT, 100CM INKL. ABDECKUNG OPAL ZUM EINSCHIEBEN U. 3 MONTAGECLIPS https://www.led-konzept.de/LED-Alu-Profil-N-eloxiert-100cm-inkl-Abdeckung-opal-zum-Einschieben-u-3-Montageclips
- 5 V Netzteil, je nach Helligkeit und Anzahl der LEDs mit entsprechender Leistung (eventuell ein einfaches USB Netzteil)

Sollte sowieso jeder Bastler zu Hause haben:
- Dremel 
- Schrauben
- Säge (zum sägen der Holzlatte und der Alu Schienen)
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

Diese Dinge müssen individuell im Quellcode angepasst werden:<br>
- ``const char* ssid = "SSID";``<br>
- ``const char* password = "Passwort";``<br>
- ``#define NUM_LEDS 54`` je nachdem wie viele LEDs man nutzen möchte<br>
- ``IPAddress local_IP(192, 168, 0, 15);`` IP Adresse eintragen<br>
- ``IPAddress gateway(192, 168, 0, 2);`` Gateway eintragen<br>
- ``IPAddress subnet(255, 255, 255, 0);`` Subnetz eintragen<br>
- ``IPAddress primaryDNS(192, 168, 0, 2);`` DNS eintragen<br>
- ``if (value > 800) {`` bei über 800 cm Wasserpegel gehen alle LEDs auf rot, dieser Wert kann natürlich angepasst werden<br>
- ``delay(300000);`` hier kann man eintragen, wie oft die Daten aktualisiert werden sollen (in Millisekunden. 300000, also 300 Sekunden, also alle 5 Minuten eine Aktualisierung<br>
- ``http.begin("https://www.pegelonline.wsv.de/webservices/rest-api/v2/stations/f85bd17b-06c7-49bd-8bfc-ee2bf3ffea99/W/currentmeasurement.json");``<br>
  Statt ``f85bd17b-06c7-49bd-8bfc-ee2bf3ffea99`` muss man seine Station ID eintragen<br>
  

## Verkabelung

Matrix Anzeige:
- data muss an Pin 18 des ESPs
-  load muss an Pin 19 des ESPs
- clk muss an Pin 21 des ESPs
- Stromversorgung der Matrix Anzeige über 5V also VIN am ESP<br>


LEDs:
- DATA_PIN muss an Pin 4 des ESPs
- Stromversorgung des LEDs über 5V also VIN am ESP<br>


Die Matrix LEDs bestehen aus zwei Modulen, die miteinander verlötet werden müssen.<br>
![Mittel (DSC01915)](https://github.com/seb201/tideanzeiger/assets/35576062/f75ff86e-faf5-42a1-a0ce-b864af9f689d)<br>
Die Halterung für die Matrix LEDs kann mit einem 3D Drucker ausgedruckt werden (3D Datei als STL und SketchUp liegen bei).<br><br>


## Weboberfläche

Gibt man die IP Adresse des Tidenanzeigers in einen Webbrowser ein, wird eine Weboberfläche angezeigt:<br>
<img width="697" alt="Bildschirmfoto 2024-02-13 um 18 25 06" src="https://github.com/seb201/tideanzeiger/assets/35576062/99d51319-8ef6-4bbe-a9b1-3e74c71057fe"><br>
Auf dieser Seite werden die Daten vom Pegel und die WLAN-Empfangsstärke angezeigt.<br>
Außerdem können Firmware-Updates eingespielt werden. Der Standard-Benutzername lautet 'Admin' und das Passwort '1234'. Diese Zugangsdaten können im Quellcode angepasst werden.<br><br>


## Was noch verbessert werden kann

- die Weboberfläche zeigt manchmal komische Werte für den Timestamp an, wahrscheinlich ein Dateityp Problem, stört aber nicht weiter

