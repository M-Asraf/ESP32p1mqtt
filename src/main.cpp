#include <HardwareSerial.h>
#include <PubSubClient.h>
#include <WebServer.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <funktion.h>
#include <webpages.h>

const char *ssid = "MY_ESP32_AP";
const char *password = "my_password";

WebServer server(80);

PubSubClient mqttClient(wifiClient);

HardwareSerial P1_Serial(2); // Erstellt eine serielle Schnittstelle an Pin 2
const int BUFFER_SIZE = 128; // Größe des seriellen Puffers
char p1_buffer[BUFFER_SIZE]; // Erstellt einen Puffer für serielle Daten

const char *mqtt_server = "";
const int mqtt_port = 0;
const char *mqtt_user = "";
const char *mqtt_password = "";
const char *mqttStatus = "";

void setup()
{
  Serial.begin(115200);
  setupWiFiAP();
  setupWebServer();
  P1_Serial.begin(115200, SERIAL_8N1, 16, 17); // Initialisiert die zweite serielle Schnittstelle an Pin 16 (Rx) und Pin 17 (Tx)
}

void setupWiFiAP()
{
  WiFi.softAP(ssid, password);
  Serial.println("SSID:" + String(ssid));
  Serial.println("Passwort:" + String(password));
  Serial.println("IP-Adresse des Access Points: " + WiFi.softAPIP().toString());
  Serial.println("MAC-Adresse: " + WiFi.softAPmacAddress());
  Serial.println("Aktueller Kanal: " + String(WiFi.channel()));
}

void setupWebServer()
{
  server.on("/", handleHTML);           // Weist die Root-Webseite der Handler-Funktion zu
  server.on("/connect", handleConnect); // Weist die WiFi-Verbindungs-Webseite der Handler-Funktion zu
  server.on("/refresh", handleRefresh); // Weist die WLAN-Netzwerk-Aktualisierungs-Webseite der Handler-Funktion zu
  server.on("/handle_mqtt", HTTP_POST, handle_mqtt);
  server.begin(); // Startet den Webserver
  Serial.println("Webserver gestartet");
}

void handleConnect()
{
  String ssid = server.arg("ssid");
  String password = server.arg("password");
  server.sendHeader("Location", "/");
  server.send(302);

  if (ssid != "" && password != "")
  {
    WiFi.softAPdisconnect(true);
    Serial.println("Access Point wird getrennt");
    WiFi.begin(ssid.c_str(), password.c_str());
    Serial.println("Verbindung mit: " + String(ssid) + " wird hergestellt");

    int counter = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(1000);
      Serial.print(".");
      counter++;
      if (counter >= 20)
      {
        Serial.println("");
        Serial.println("Fehler bei der Verbindung zum WLAN-Netzwerk.");
        WiFi.disconnect();
        WiFi.softAP(ssid.c_str(), password.c_str()); // Stellt den ESP32 wieder als Access Point ein
        Serial.println("Access Point wurde erneut konfiguriert.");
        break;
      }
    }

    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("");
      Serial.println("Verbunden mit: " + String(ssid));
      Serial.println("IP-Adresse: " + WiFi.localIP().toString());
      Serial.println("MAC-Adresse: " + WiFi.macAddress());
      Serial.println("Aktueller Kanal: " + String(WiFi.channel()));
    }
  }
}

void handleHTML()
{
  String html = getHtmlContent(); // Ruft den HTML-Inhalt der Webseite ab
  html.replace("[[MQTT_STATUS]]", String(mqttStatus));
  server.send(200, "text/html; charset=utf-8", html); // Sendet die HTTP-Antwort mit dem HTML-Inhalt zurück
}

void handleRefresh()
{
  server.sendHeader("Location", "/"); // Lädt die Root-Webseite nach dem Aktualisieren neu
  server.send(302);
}

void handle_mqtt()
{
  if (server.method() == HTTP_POST)
  {
    if (server.hasArg("mqttServer") && server.hasArg("mqttPort") && server.hasArg("mqttUsername") && server.hasArg("mqttPassword"))
    {
      // Ausgefüllte Formularfelder auslesen
      String mqttServer = server.arg("mqttServer");
      int mqttPort = server.arg("mqttPort").toInt();
      String mqttUsername = server.arg("mqttUsername");
      String mqttPassword = server.arg("mqttPassword");

      // Verbindung mit MQTT-Broker herstellen
      mqttConnect(mqttServer.c_str(), mqttPort, mqttUsername.c_str(), mqttPassword.c_str());
    }
    else
    {
      server.send(400, "text/html", "Fehlende Formulardaten. Stellen Sie sicher, dass alle erforderlichen Felder ausgefüllt sind.");
    }
  }
  else
  {
    server.send(405, "text/html", "Methode nicht erlaubt.");
  }
}

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Nachricht empfangen auf Topic [");
  Serial.print(topic);
  Serial.print("]: ");

  String message = "";
  for (int i = 0; i < length; i++)
  {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Analysieren Sie die Nachricht und suchen Sie nach Fehlercodes oder Statusmeldungen
  if (message.indexOf("ERROR") != -1 || message.indexOf("FEHLER") != -1)
  {
    Serial.println("Fehlermeldung empfangen:");
    Serial.println(message);
  }
  else if (message.indexOf("WARN") != -1 || message.indexOf("WARNUNG") != -1)
  {
    Serial.println("Warnmeldung empfangen:");
    Serial.println(message);
  }
  else
  {
    // Hier können Sie weitere Bedingungen hinzufügen, um auf bestimmte Nachrichteninhalte zu reagieren
  }
}

void mqttConnect(const char *mqtt_server, int mqtt_port, const char *mqtt_user, const char *mqtt_password)
{
  Serial.println("Verbindung mit MQTT-Broker wird hergestellt...");

  mqttclient.setServer(mqtt_server, mqtt_port); // Setzt den MQTT-Server und -Port

  int retries = 0;
  while (!mqttclient.connected() && retries < 3)
  {
    if (mqttclient.connect("ESP32Client", mqtt_user, mqtt_password))
    {
      Serial.println("Verbindung mit MQTT-Broker hergestellt.");
      mqttclient.setCallback(mqttCallback); // Hier z.B. mqttclient.subscribe("smartmeter/p1/data"); // Abonniert das MQTT-Topic
      mqttStatus = "MQTT-Verbindung wurde erfolgreich hergestellt.";
      server.sendHeader("Location", "/");
      server.send(302);
    }
    else
    {
      retries++;
      Serial.print("Verbindung fehlgeschlagen. RC=");
      Serial.print(mqttclient.state());
      Serial.print(" Neuer Versuch in 3 Sekunden...");
      delay(3000);
    }
  }

  if (!mqttclient.connected())
  {
    Serial.println("Verbindung mit MQTT-Broker konnte nicht hergestellt werden.");
    mqttStatus = "Verbindung mit MQTT-Broker konnte nicht hergestellt werden.";
    server.sendHeader("Location", "/");
    server.send(302);
  }
}

void P1Data()
{
  char receivedChar = 0;
  char c = 0;
  int i = 0;

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
        p1_buffer[i] = 0; // Setzt das Ende des Puffers
        if (!mqttclient.connected())
        {
          mqttConnect(mqtt_server, mqtt_port, mqtt_user, mqtt_password);
        }
        if (mqttclient.connected())
        {
          mqttclient.publish("smartmeter/p1/data", p1_buffer);
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
  P1Data();
}
