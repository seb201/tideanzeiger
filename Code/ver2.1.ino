/*

  https://github.com/seb201/tideanzeiger


  Changelog:
  ver0.6 = Andere Einteilung der LEDs pro cm Wasser, Update von /library/FastLed/platforms/esp/32/clockless_32.h
  ver0.7 = Einbindung des Temperatursensors
  ver0.8 = Entfernung Temperatursensor, Z 294 delay von 1000 ms statt 500 ms, Z227 disconnect() könnte man auskommentiert
  ver0.9
  ver1.0 = Auf https:// umgestellt
  ver2.0 = alles neuprogrammiert
  ver2.1 = Ebbe und Flut wird wieder angezeigt

  Zukunft:
  Link zum Update auf local_IP umgestellt (für DHCP wichtig), HTML angepasst, Timestamp keine komischen Werte mehr (wahrscheinlich falsches Datenformat)

  Benötigte Bibliotheken mit der getesteten Version (evtl. funktionieren auch neuere Versionen)

    WiFi in Version 2.0.0
    WiFiClientSecure in Version 2.0.0
    ArduinoJson in Version 6.14.0
    HTTPClient in Version 2.0.0
    MAXMATRIX
    SPI in Version 2.0.0
    FastLED in Version 3.5.0
    AsyncTCP in Version 1.1.1
    ESP Async WebServer in Version 1.2.3
    AsyncElegantOTA in Version 2.2.7



  Verkabelung:

  Matrix Anzeige:
    data muss an Pin 18 des ESPs
    load muss an Pin 19 des ESPs
    clk muss an Pin 21 des ESPs

  LEDs:
    DATA_PIN muss an Pin 4 des ESPs

*/
float ver = 2.1;

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>  //JSON
#include <HTTPClient.h>   //API-Abfrage
#include <MaxMatrix.h>
#define FASTLED_ALLOW_INTERRUPTS 0
#include <Arduino.h>
#include <SPI.h>
#include "FastLED.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

// WIFI-Zugangsdaten
const char* ssid = "SSID";
const char* password = "Passwort";

AsyncWebServer server(80);

#include "Numbers.h"
#include "html.h"
#include <pgmspace.h>

//Diese Zeilen Definieren, wann bei dem Matrix LED Modul welche LEDs angehen bei einem bestimmten Buchstaben oder bei einer bestimmten Zahl
int data = 18;                           // DIN pin von dem MAX7219 Modul
int load = 19;                           // CS pin von dem MAX7219 Modul
int clk = 21;                            // CLK pin von dem MAX7219 Modul
int maxInUse = 2;                        // definiert, wie viele Matrix LED Module angeschlossen sind
MaxMatrix m(data, load, clk, maxInUse);  // zusammenfassung der einzelnen Defintionen
byte buffer[100];

// LED Einstellungen
#define NUM_LEDS 54    // definiert wie viele LEDs der LED Stripe hat
#define DATA_PIN 4     // definiert den Daten Pin des LED Stripes
CRGB leds[NUM_LEDS];   // Array von LEDs
#define BRIGHTNESS 15  // definiert die Helligkeit der LEDs
int fadeAmount = 5;    // Set the amount to fade I usually do 5, 10, 15, 20, 25 etc even up to 255.
int brightness = 0;

// IP Einstellungen
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  // definition der MAC Adresse
IPAddress local_IP(192, 168, 0, 15);
IPAddress gateway(192, 168, 0, 2);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(192, 168, 0, 2);
IPAddress secondaryDNS(8, 8, 4, 4);    // Zweiter DNS Server optional
String hostname = "Tidenanzeiger";

//Meldungen auf dem Matrixdisplay
char string1[] = " Tidenanzeiger        ";
char string2[] = "Keine Verbindung zum WSA Server         ";
char string3[] = "Keine Verbindung zum WLAN         ";

int value = 0;
int trend = 0;
const char* timestamp = 0;


// Variable zur Erkenneung, ob Ebbe oder Flut
int value_alt = 0;

int versuch = 1;
int neustart = 0;

// Umwandlung der Werte zur Anzeige auf Weboberfläche
String processor(const String& var) {
  //Serial.println(var);
  if (var == "VALUE") {
    return String(value);
  } else if (var == "TREND") {
    return String(trend);
  } else if (var == "TIMESTAMP") {
    return String(timestamp);
    //return String(string_timestamp);
  } else if (var == "VER") {
    return String(ver, 1);
  } else if (var == "IP") {
    return String(WiFi.localIP());
  } else if (var == "RSSI") {
    return String(WiFi.RSSI());
  }
  return String();
}


void setup() {

  Serial.begin(115200);

  Serial.print("Tidenanzeiger gestartet  VERSION: ");
  Serial.println(ver, 1);

  m.init();           // initialisierung des Matrix Modus
  m.setIntensity(6);  // Helligkeit des Matrix Moduls (von 0 bis 15)

  // Gibt auf der Matrixanzeige string1 aus
  printStringWithShift(string1, 100);

  // Initialisierung der LEDs
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);  //LED Helligkeit
  delay(500);
  //FastLED.clear();
  fill_solid(leds, NUM_LEDS, CRGB::Black);

  WLANverbinden();

  // Initialisierung des Webservers
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", index_html, processor);
  });

  // Initialisierung der OTA Servers
  AsyncElegantOTA.begin(&server, "admin", "1234");  // Start ElegantOTA
  server.begin();
  Serial.println("AsyncElegantOTA started");
  Serial.println("");

  delay(500);
}



void loop() {

  // Wird benötigt, damit nach einem Update der ESP automatisch neustartet - Wird wohl in der neusten Version nicht mehr benötigt
  //AsyncElegantOTA.loop();

  runterladen();

  String value_string = String(value);
  matrix(value_string);
  ebbeflut();

  // Prüft auf Sturmflut
  if (value > 800) {  //wenn höher als 800 dann Sturmflutmodus
    sturm();
  } else {
    anzeigeRGB();
  }

  neustart = neustart + 1;

  // Täglicher Neustart
  if (neustart == 720) {
    Serial.println("Täglicher Neustart");
    delay(1000);
    ESP.restart();
  }


  delay(300000);  //loop wird alle 5 Minuten gestartet, D.h. alle 5 Minuten wird die Anzeige aktualisiert
}


/**********************************************************************************************************/
void WLANverbinden() {

  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }

  WiFi.setHostname(hostname.c_str());  //definiert den Hostnamen

  WiFi.begin(ssid, password);  //Internet-Verbindung starten

  delay(3000);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Keine Verbindung zum WLAN");
    fill_solid(leds, NUM_LEDS, CRGB::Yellow); //wenn keine Verbindung zum Wlan, dann alle LEDs gelb
    FastLED.show();
    delay(500);
    Serial.print("Verbindungsversuch:");
    Serial.print(versuch);
    Serial.println("/4");
    Serial.println("20 Sekunden warten");
    printStringWithShift(string3, 100);
    versuch = versuch + 1;
    delay(20000);
    if (versuch == 5) {
      ESP.restart();
    }
  }

  Serial.println("");
  Serial.print("Verbunden mit: ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("ESP Mac Address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Subnet Mask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway IP: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("DNS: ");
  Serial.println(WiFi.dnsIP());
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());
  Serial.println("");

  delay(500);

  // Wird für verschlüsselte Verbindungen mit dem WSA Server benötigt
  WiFiClientSecure client;
}

/**********************************************************************************************************/
void runterladen() {
  if ((WiFi.status() == WL_CONNECTED)) {

    HTTPClient http;  //Instanz von HTTPClient starten


    //Messstation WHV ALTER VORHAFEN
    http.begin("https://www.pegelonline.wsv.de/webservices/rest-api/v2/stations/f85bd17b-06c7-49bd-8bfc-ee2bf3ffea99/W/currentmeasurement.json");  //Abfrage-URL

    //Alternative Messstation WHV NEUER VORHAFEN
    //http.begin("https://www.pegelonline.wsv.de/webservices/rest-api/v2/stations/f77317d9-654f-4f51-925e-004c592049da/W/currentmeasurement.json"); //Abfrage-URL


    int httpCode = http.GET();  //Antwort des Servers abrufen

    // Verarbeitet die über JSON abgerufenen Daten
    if (httpCode == 200) {
      String payload = http.getString();  //Daten in eine Variable speichern
      const size_t capacity = JSON_ARRAY_SIZE(5) + 5 * JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + 200;
      DynamicJsonDocument doc(capacity);
      deserializeJson(doc, payload);

      value = doc["value"];
      trend = doc["trend"];
      timestamp = doc["timestamp"];
      //string_timestamp = doc["string_timestamp"];

      Serial.println(value);
      Serial.println(trend);
      Serial.println(timestamp);
      Serial.println("");

    } else {
      // Gibt einen Fehler aus, wenn keine Verbindung zum WSA Server besteht
      Serial.println("Keine Verbindung zum WSA Server. ESP startet in 2 Minuten neu");
      fill_solid(leds, NUM_LEDS, CRGB::Yellow); //wenn keine Verbindung zum Wlan, dann alle LEDs gelb
      FastLED.show();
      delay(1000);
      printStringWithShift(string2, 100);
      delay(120000);  //wartet 2 Minuten
      ESP.restart();  //und startet dann neu
    }
  }
}

/**********************************************************************************************************/
void matrix(String aktWert) {
  char Buff[80];
  aktWert.toCharArray(Buff, 80);
  printString(Buff);
}

/**********************************************************************************************************/
void printString(char* s) {
  int col = 0;
  while (*s != 0) {
    if (*s < 32) continue;
    char c = *s - 32;
    memcpy_P(buffer, CH + 7 * c, 7);
    m.writeSprite(col, 0, buffer);
    m.setColumn(col + buffer[0], 0);
    col += buffer[0] + 1;
    s++;
  }
}

/**********************************************************************************************************/
void printCharWithShift(char c, int shift_speed) {
  if (c < 32) return;
  c -= 32;
  memcpy_P(buffer, CH + 7 * c, 7);
  m.writeSprite(32, 0, buffer);
  m.setColumn(32 + buffer[0], 0);
  for (int i = 0; i < buffer[0] + 1; i++) {
    delay(shift_speed);
    m.shiftLeft(false, false);
  }
}

/**********************************************************************************************************/
void printStringWithShift(char* s, int shift_speed) {
  while (*s != 0) {
    printCharWithShift(*s, shift_speed);
    s++;
  }
}

/**********************************************************************************************************/
void ebbeflut() {

  // Prüft mittels des vorherigen value Wert, ob der Wasserstand steigt oder sinkt
  if (value - value_alt >= 1) {
    trend = 1;
  }

  if (value - value_alt <= -1) {
    trend = -1;
  }

  if (value == value_alt) {
    trend = 0;
  }

  // Der Wert des Trends wird in die LED Anzeige umgewandelt. 0 keine LED Wasserstand gleichbleibend zur vorherigen Messung, 1 oberste LED grün Flut, -1 unterste LED grün Ebbe
  if (trend == 1) {
    leds[NUM_LEDS - 1] = CRGB::Green;
    leds[0] = CRGB::Black;
  }

  else if (trend == -1) {
    leds[0] = CRGB::Green;
    leds[NUM_LEDS - 1] = CRGB::Black;
  } else {
    leds[NUM_LEDS - 1] = CRGB::Black;
    leds[0] = CRGB::Black;
  }
  FastLED.show();

  value_alt = value;
}


/**********************************************************************************************************/
void anzeigeRGB() {

  // Lässt die LEDs leuchten, je nach Wasserstand
  int anz = value / 16;
  if (anz > 60)
    anz = 60;

  for (int whiteLed = 1; whiteLed < NUM_LEDS - 1; whiteLed = whiteLed + 1) {
    // Turn our current led on to white, then show the leds
    if (whiteLed < anz + 1) {
      leds[whiteLed] = CRGB::Blue;
    }

    else {
      leds[whiteLed] = CRGB::Black;
      // Wait a little bit
      //delay(100);
    }
    // Turn our current led back to black for the next loop around
    //leds[whiteLed] = CRGB::Black;
  }
  FastLED.show();
}

/**********************************************************************************************************/
void sturm() {
  for (int whiteLed = 1; whiteLed < NUM_LEDS - 1; whiteLed = whiteLed + 1) {
    // Turn our current led on to white, then show the leds

    leds[whiteLed] = CRGB::Red; //bei Sturmflut werden allr LEDs auf Rot geschaltet
  }
  // Show the leds (only one of which is set to white, from above)
  FastLED.show();
  // Wait a little bit
  delay(1000);
}

/**********************************************************************************************************/
