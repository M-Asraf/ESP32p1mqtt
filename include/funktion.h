#pragma once

void handleRefresh(); // Funktionsdeklaration für das Aktualisieren von WLAN-Netzwerken
void handleConnect(); // Funktionsdeklaration für die WiFi-Verbindung
void handleHTML();    // Funktionsdeklaration für die Root-Webseite
void setupWiFiAP();
void setupWebServer();
void mqttConnect(const char *mqtt_server, int mqtt_port, const char *mqtt_user, const char *mqtt_password);
void handle_mqtt();
