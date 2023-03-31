#include <HardwareSerial.h>
#include <PubSubClient.h>
#include <WebServer.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <funktion.h>
#include <webpages.h>

// Globale Variablen
const char *ssid = "MY_ESP32_AP";
const char *password = "my_password";

WebServer server(80); // Erstellt eine Instanz der WebServer-Klasse, die auf Port 80 lauscht.

WiFiClient wifiClient;               // Erstellt eine Instanz der WiFiClient-Klasse für die WLAN-Kommunikation.
PubSubClient mqttclient(wifiClient); // Erstellt eine Instanz der PubSubClient-Klasse für die MQTT-Kommunikation, die die wifiClient-Instanz verwendet.

HardwareSerial P1_Serial(2); // Erstellt eine serielle Schnittstelle an Pin 2
const int BUFFER_SIZE = 128; // Größe des seriellen Puffers
char p1_buffer[BUFFER_SIZE]; // Erstellt einen Puffer für serielle Daten

const char *mqtt_server = "";   // Die Adresse des MQTT-Brokers, zu dem eine Verbindung hergestellt werden soll.
const int mqtt_port = 0;        // Der Port, über den die Verbindung zum MQTT-Broker hergestellt wird.
const char *mqtt_user = "";     // Der Benutzername, der zur Authentifizierung bei Verbindung zum MQTT-Broker verwendet wird.
const char *mqtt_password = ""; // Das Passwort, das zur Authentifizierung bei Verbindung zum MQTT-Broker verwendet wird.

const char *mqttStatus = ""; // Eine Zeichenfolge zur Speicherung des Verbindungszustands zum MQTT-Broker.

void setup()
{
  Serial.begin(115200);                        // Initialisiert die serielle Kommunikation mit einer Baudrate von 115200.
  setupWiFiAP();                               // Initialisiert den WLAN-Access-Point und stellt eine Verbindung zum WLAN-Netzwerk her.
  setupWebServer();                            // Initialisiert den Webserver und die Routen, die vom Server unterstützt werden.
  P1_Serial.begin(115200, SERIAL_8N1, 16, 17); // Initialisiert die zweite serielle Schnittstelle an Pin 16 (Rx) und Pin 17 (Tx)
}

void setupWiFiAP()
{
  Serial.println();
  WiFi.softAP(ssid, password);                    // Initialisiert den WLAN-Access-Point mit dem angegebenen SSID und Passwort.
  Serial.println("SSID:" + String(ssid));         // Gibt die SSID des WLAN-Access-Points über die serielle Schnittstelle aus.
  Serial.println("Passwort:" + String(password)); // Gibt das Passwort des WLAN-Access-Points über die serielle Schnittstelle aus.
  Serial.println();
  Serial.println("IP-Adresse des Access Points: " + WiFi.softAPIP().toString()); // Gibt die IP-Adresse des Access-Points über die serielle Schnittstelle aus.
  Serial.println("MAC-Adresse: " + WiFi.softAPmacAddress());                     // Gibt die MAC-Adresse des WLAN-Access-Points über die serielle Schnittstelle aus.
  Serial.println("Aktueller Kanal: " + String(WiFi.channel()));                  // Gibt den aktuellen Kanal des WLAN-Access-Points über die serielle Schnittstelle aus.
}

void setupWebServer()
{
  server.begin(); // Startet den Webserver
  Serial.println();
  Serial.println("Webserver gestartet"); // Gibt eine Bestätigungsmeldung über die serielle Schnittstelle aus.
  Serial.println();
  server.on("/", handleHTML);                        // Weist die Root-Webseite der Handler-Funktion zu
  server.on("/connect", handleConnect);              // Weist die WiFi-Verbindungs-Webseite der Handler-Funktion zu
  server.on("/refresh", handleRefresh);              // Weist die WLAN-Netzwerk-Aktualisierungs-Webseite der Handler-Funktion zu
  server.on("/handle_mqtt", HTTP_POST, handle_mqtt); // Weist die MQTT-Handler-Funktion zu, die beim Empfangen eines POST-Requests ausgeführt wird
}

void handleConnect()
{
  String ssid = server.arg("ssid");         // Liest den SSID-Parameter aus dem HTTP-Request
  String password = server.arg("password"); // Liest das Passwort-Parameter aus dem HTTP-Request

  if (ssid != "" && password != "") // Prüft, ob der SSID- und das Passwort-Felder ausgefüllt wurden
  {
    WiFi.softAPdisconnect(true);                  // Trennt die Verbindung zum Access Point
    Serial.println("Access Point wird getrennt"); // Gibt eine Statusmeldung über die serielle Schnittstelle aus
    Serial.println();

    WiFi.begin(ssid.c_str(), password.c_str());                              // Stellt eine Verbindung zum angegebenen WLAN-Netzwerk her
    Serial.println("Verbindung mit: " + String(ssid) + " wird hergestellt"); // Gibt eine Statusmeldung über die serielle Schnittstelle aus

    int counter = 0; // Initialisiert den Zähler, der die Anzahl der Versuche zählt, um eine Verbindung zum WLAN-Netzwerk herzustellen

    while (WiFi.status() != WL_CONNECTED) // Wartet auf eine Verbindung zum WLAN-Netzwerk
    {
      delay(1000);       // Verzögert die Ausführung des Codes für 1 Sekunde, um auf die Verbindung zum WLAN-Netzwerk zu warten
      Serial.print("."); // Gibt einen Punkt auf der seriellen Schnittstelle aus, um anzuzeigen, dass der Code noch ausgeführt wird
      Serial.println();
      counter++; // Erhöht den Zähler für die Anzahl der Versuche, eine Verbindung zum WLAN-Netzwerk herzustellen

      if (counter >= 20) // Nach 20 Sekunden wird ein Fehler ausgegeben
      {
        Serial.println("!!!");                                          // Gibt eine leere Zeile auf der seriellen Schnittstelle aus, um den Fehler deutlicher zu machen
        Serial.println("Fehler bei der Verbindung zum WLAN-Netzwerk."); // Gibt eine Fehlermeldung auf der seriellen Schnittstelle aus
        WiFi.disconnect();                                              // Trennt die Verbindung zum WLAN-Netzwerk
        setupWiFiAP();                                                  // Stellt den ESP32 wieder als Access Point ein
        Serial.println("Access Point wurde erneut konfiguriert.");      // Gibt eine Statusmeldung auf der seriellen Schnittstelle aus
        break;                                                          // Beendet die While-Schleife, da ein Fehler aufgetreten ist
      }
    }

    if (WiFi.status() == WL_CONNECTED)                          // Wenn die Verbindung erfolgreich ist, werden Informationen ausgegeben    {
      Serial.println("Verbunden mit: " + String(ssid));         // Gibt eine Statusmeldung auf der seriellen Schnittstelle aus, dass eine Verbindung zum WLAN-Netzwerk hergestellt wurde
    Serial.println("IP-Adresse: " + WiFi.localIP().toString()); // Gibt die lokale IP-Adresse des ESP32 auf der seriellen Schnittstelle aus
    Serial.println("MAC-Adresse: " + WiFi.macAddress());        // Gibt die MAC-Adresse des ESP32 auf der seriellen Schnittstelle aus
  }
}

void handleHTML()
{
  String html = getHtmlContent();                     // Ruft den HTML-Inhalt der Webseite ab
  server.send(200, "text/html; charset=utf-8", html); // Sendet die HTTP-Antwort mit dem HTML-Inhalt zurück
  mqttStatus;
}

void handleRefresh()
{
  server.sendHeader("Location", "/"); // Lädt die Root-Webseite nach dem Aktualisieren neu
  server.send(302);                   // Sendet einen HTTP-302-Redirect-Statuscode an den Client, um ihn auf eine andere Webseite weiterzuleiten
}

void handle_mqtt()
{
  if (server.method() == HTTP_POST) // Überprüft, ob die Anfrage eine HTTP-POST-Anfrage ist
  {
    if (server.hasArg("mqttServer") && server.hasArg("mqttPort") && server.hasArg("mqttUsername") && server.hasArg("mqttPassword")) // Überprüft, ob alle Formularfelder ausgefüllt sind
    {
      // Formulardaten auslesen
      String mqttServer = server.arg("mqttServer");     // Liest den MQTT-Server-Parameter aus dem HTTP-Request
      int mqttPort = server.arg("mqttPort").toInt();    // Liest den MQTT-Port-Parameter aus dem HTTP-Request und konvertiert ihn in eine Integer-Zahl
      String mqttUsername = server.arg("mqttUsername"); // Liest den MQTT-Benutzername-Parameter aus dem HTTP-Request
      String mqttPassword = server.arg("mqttPassword"); // Liest das MQTT-Passwort-Parameter aus dem HTTP-Request

      mqttConnect(mqttServer.c_str(), mqttPort, mqttUsername.c_str(), mqttPassword.c_str()); // Verbindung mit MQTT-Broker herstellen
    }
    else // Wenn Formularfelder fehlen, wird eine Fehlermeldung an den Client gesendet
    {
      server.send(400, "text/html", "Fehlende Formulardaten."); // Sendet eine HTTP-400-Bad-Request-Antwort mit einer Fehlermeldung an den Client
    }
  }
  else // Wenn die Anfrage keine HTTP-POST-Anfrage ist, wird eine Fehlermeldung an den Client gesendet
  {
    server.send(405, "text/html", "Methode nicht erlaubt."); // Sendet eine HTTP-405-Method-Not-Allowed-Antwort mit einer Fehlermeldung an den Client
  }
}

void mqttCallback(char *topic, byte *payload, unsigned int length) // Callback-Funktion, die aufgerufen wird, wenn eine MQTT-Nachricht empfangen wird

{
  Serial.print("Nachricht empfangen auf Topic ["); // Gibt eine Nachricht auf der Konsole aus, dass eine MQTT-Nachricht empfangen wurde
  Serial.print(topic);                             // Gibt das Topic auf der Konsole aus, auf dem die Nachricht empfangen wurde
  Serial.print("]: ");                             // Schließt die Topic-Ausgabe ab

  String message = "";             // Erstellt eine leere String-Variable, in der die MQTT-Nachricht gespeichert wird
  for (int i = 0; i < length; i++) // Durchläuft das payload-Array, um die Nachricht zu extrahieren
  {
    message += (char)payload[i]; // Fügt jedes Byte der Nachricht dem String hinzu
  }
  Serial.println(message); // Gibt die empfangene MQTT-Nachricht auf der Konsole aus

  if (message.indexOf("ERROR") != -1 || message.indexOf("FEHLER") != -1) // Prüft, ob die empfangene Nachricht eine Fehlermeldung enthält
  {
    Serial.println("Fehlermeldung empfangen:"); // Gibt eine Nachricht auf der Konsole aus, dass eine Fehlermeldung empfangen wurde
    Serial.println(message);                    // Gibt die empfangene Nachricht auf der Konsole aus
  }
  else if (message.indexOf("WARN") != -1 || message.indexOf("WARNUNG") != -1) // Prüft, ob die empfangene Nachricht eine Warnmeldung enthält
  {
    Serial.println("Warnmeldung empfangen:"); // Gibt eine Nachricht auf der Konsole aus, dass eine Warnmeldung empfangen wurde
    Serial.println(message);                  // Gibt die empfangene Nachricht auf der Konsole aus
  }
  else
  {
    // Hier können Sie weitere Bedingungen hinzufügen, um auf bestimmte Nachrichteninhalte zu reagieren
  }
}

void mqttConnect(const char *mqtt_server, int mqtt_port, const char *mqtt_user, const char *mqtt_password) // Funktion zum Herstellen einer Verbindung mit dem MQTT-Broker
{
  Serial.println("Verbindung mit MQTT-Broker wird hergestellt..."); // Gibt eine Nachricht auf der Konsole aus, dass eine Verbindung zum MQTT-Broker hergestellt wird

  mqttclient.setServer(mqtt_server, mqtt_port); // Setzt den MQTT-Server und -Port

  int retries = 0;
  while (!mqttclient.connected() && retries < 3) // Versucht, eine Verbindung zum MQTT-Broker herzustellen
  {
    if (mqttclient.connect("ESP32Client", mqtt_user, mqtt_password)) // Verbindet den MQTT-Client mit dem Broker
    {
      Serial.println("Verbindung mit MQTT-Broker hergestellt.");     // Gibt eine Nachricht auf der Konsole aus, dass die Verbindung zum Broker erfolgreich hergestellt wurde
      mqttclient.setCallback(mqttCallback);                          // Setzt die Callback-Funktion, die aufgerufen wird, wenn eine Nachricht empfangen wird
      mqttStatus = "MQTT-Verbindung wurde erfolgreich hergestellt."; // Setzt den MQTT-Status auf "verbunden"
      server.sendHeader("Location", "/");                            // Leitet den Benutzer zurück zur Root-Webseite
      server.send(302);                                              // Sendet einen HTTP-302-Redirect an den Client
    }
    else
    {
      retries++;                                       // Erhöht die Anzahl der Verbindungsversuche um 1
      Serial.print("Verbindung fehlgeschlagen. RC=");  // Gibt eine Nachricht auf der Konsole aus, dass die Verbindung fehlgeschlagen ist und gibt den Rückgabewert der MQTT-Verbindungsstatus-Methode aus
      Serial.print(mqttclient.state());                // Gibt den Rückgabewert der MQTT-Verbindungsstatus-Methode auf der Konsole aus
      Serial.print(" Neuer Versuch in 3 Sekunden..."); // Gibt eine Nachricht auf der Konsole aus, dass in 3 Sekunden ein neuer Verbindungsversuch unternommen wird
      delay(3000);                                     // Wartet 3 Sekunden, bevor ein neuer Verbindungsversuch unternommen wird
    }
  }

  if (!mqttclient.connected()) // Überprüft, ob eine Verbindung zum MQTT-Broker hergestellt werden konnte
  {
    Serial.println("Verbindung mit MQTT-Broker konnte nicht hergestellt werden."); // Gibt eine Nachricht auf der Konsole aus, dass die Verbindung zum MQTT-Broker fehlgeschlagen ist
    mqttStatus = "Verbindung mit MQTT-Broker konnte nicht hergestellt werden.";    // Setzt den MQTT-Status auf "nicht verbunden"
    server.sendHeader("Location", "/");                                            // Leitet den Benutzer zurück zur Root-Webseite
    server.send(302);                                                              // Sendet einen HTTP-302-Redirect an den Client
  }
}

void P1Data() // Definiert die Funktion "P1Data"
{
  char receivedChar = 0; // Initialisiert die Variable "receivedChar" mit dem Wert 0
  char c = 0;            // Initialisiert die Variable "c" mit dem Wert 0
  int i = 0;             // Initialisiert die Variable "i" mit dem Wert 0

  if (P1_Serial.available() > 0) // Wenn Daten auf der zweiten seriellen Schnittstelle verfügbar sind
  {
    receivedChar = P1_Serial.read(); // Liest ein einzelnes Zeichen ein
  }

  if (receivedChar == '!') // Wenn das Zeichen ein Ausrufezeichen ist
  {
    i = 0;
    while (P1_Serial.available() > 0 && i < BUFFER_SIZE - 1) // Solange Daten verfügbar sind und der Puffer nicht voll ist
    {
      c = P1_Serial.read(); // Liest das nächste Zeichen ein

      if (c == '\r') // Wenn das Zeichen ein Wagenrücklauf ist
      {
        p1_buffer[i] = 0;            // Setzt das Ende des Puffers
        if (!mqttclient.connected()) // Überprüft, ob eine Verbindung zum MQTT-Broker besteht
        {
          mqttConnect(mqtt_server, mqtt_port, mqtt_user, mqtt_password); // Stellt eine Verbindung zum MQTT-Broker her, falls keine Verbindung besteht
        }
        if (mqttclient.connected()) // Überprüft, ob eine Verbindung zum MQTT-Broker besteht
        {
          mqttclient.publish("smartmeter/p1/data", p1_buffer); // Veröffentlicht den Inhalt des P1-Buffers auf dem MQTT-Topic "smartmeter/p1/data"
        }
        i = 0; // Setzt den Pufferzähler zurück
      }
      else if (c != '\n') // Wenn das Zeichen kein Zeilenumbruch ist
      {
        p1_buffer[i] = c; // Fügt das Zeichen dem Puffer hinzu
        i++;              // Erhöht den Pufferzähler
      }
    }
  }
}

void loop()
{
  server.handleClient(); // Behandelt eingehende Anfragen an den Webserver

  if (wifiClient.connected()) // Wenn eine Verbindung besteht
  {
    while (wifiClient.available()) // Wenn Daten verfügbar sind
    {
      Serial.write(wifiClient.read()); // Schreibt die empfangenen Daten auf die serielle Schnittstelle
    }
  }

  P1Data(); // Aufruf der Funktion
}
