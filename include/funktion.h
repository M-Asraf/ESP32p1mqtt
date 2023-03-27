#ifndef D9F6C42B_4BB9_4233_856F_827EB894A056
#define D9F6C42B_4BB9_4233_856F_827EB894A056

void handleRefresh(); // Funktionsdeklaration für das Aktualisieren von WLAN-Netzwerken
void handleConnect(); // Funktionsdeklaration für die WiFi-Verbindung
void handleHTML();    // Funktionsdeklaration für die Root-Webseite
void setupWiFiAP();
void setupWebServer();
void mqttConnect(const char *mqtt_server, int mqtt_port, const char *mqtt_user, const char *mqtt_password);
void handle_mqtt();

#endif /* D9F6C42B_4BB9_4233_856F_827EB894A056 */

// Globale Variablen
WiFiClient wifiClient;
PubSubClient mqttclient(wifiClient);
