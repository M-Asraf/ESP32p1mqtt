#ifndef F82BEDC2_2A77_4D15_8B16_3DC3BB5F5390
#define F82BEDC2_2A77_4D15_8B16_3DC3BB5F5390

const char *ROOT_HTML = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ESP32-DevKitC, Wroom-32U</title>
</head>
<body>
<table width="100%" border="0">
<h2 style="color: #555555;">ESP32-DevKitC, Wroom-32U</h2>
)rawliteral";

String getHtmlContent()
{
    String html = String(ROOT_HTML);

    if (WiFi.softAPgetStationNum() > 0)
    {
        // ESP32 ist im Access Point-Modus
        html += "<h3>Access Point-Modus</h3>";
        html += "<h5 style='margin-bottom: 0px;'><b>SSID:</b> " + String(WiFi.softAPSSID()) + "</h5>";
        html += "<h5 style='margin-bottom: 0px;'><b>IP-Adresse:</b> " + WiFi.softAPIP().toString() + "</h5>";
        html += "<h5 style='margin-bottom: 0px;'><b>MAC-Adresse:</b> " + WiFi.softAPmacAddress() + "</h5>";
        html += "<h5 style='margin-bottom: 0px;'><b>Kanal:</b> " + String(WiFi.channel()) + "</h5>";
    }
    else
    {
        // ESP32 ist mit einem Netzwerk verbunden
        if (WiFi.status() == WL_CONNECTED)
        {
            html += "<h3>Aktuelles Netzwerk</h3>";
            html += "<h5 style='margin-bottom: 0px;'><b>SSID:</b> " + String(WiFi.SSID()) + "</h5>";
            html += "<h5 style='margin-bottom: 0px;'><b>IP-Adresse:</b> " + WiFi.localIP().toString() + "</h5>";
            html += "<h5 style='margin-bottom: 0px;'><b>MAC-Adresse:</b> " + WiFi.macAddress() + "</h5>";
            html += "<h5 style='margin-bottom: 0px;'><b>Kanal:</b> " + String(WiFi.channel()) + "</h5>";

            html += "<h3>MQTT-Verbindung</h3>";
            html += "<form method=\"post\" action=\"/handle_mqtt\">";
            html += "Server: <br><input type=\"text\" name=\"mqttServer\" style=\"margin-bottom: 10px;\" placeholder=\"xxx.xxx.xxx.xxx\"> <br>";
            html += "Port: <br><input type=\"number\" name=\"mqttPort\" style=\"margin-bottom: 10px;\"> <br>";
            html += "Benutzername: <br><input type=\"text\" name=\"mqttUsername\" style=\"margin-bottom: 10px;\"> <br>";
            html += "Passwort: <br><input type=\"password\" name=\"mqttPassword\" style=\"margin-bottom: 10px;\"> <br>";
            html += "<input type=\"submit\" value=\"Verbinden\" style=\"background-color: red; border: none; color: white; padding: 10px 20px;\">";
            html += "</form>";
            html += "</td>";
            html += "</tr>";
            html += "</table>";
            html += "</body>";
            html += "</html>";

            html += "</head>";
            html += "<body>";
            html += "<h3>MQTT-Status</h3>";
            html += "<div style='background-color: black; padding: 10px; display: inline-block;'>";
            html += "<p style=' padding: 5px; margin: 0; color: red; display: inline-block;'>[[MQTT_STATUS]]</p>";
            html += "</div>";
            html += "</body>";
            html += "</html>";
        }
        else
        {
            html += "<p>Nicht mit einem Netzwerk verbunden</p>";
        }
    }

    html += "<h3>Verfügbare WLAN-Netzwerke</h3>";

    int num_networks = WiFi.scanNetworks();
    if (num_networks == 0)
    {
        html += "<p>Keine WLAN-Netzwerke gefunden.</p>";
    }
    else
    {
        html += "<form action=\"/connect\">";
        html += "<label for=\"ssid\">SSID:</label><br>";
        html += "<select name=\"ssid\" id=\"ssid\" style=\"margin-bottom: 10px;\">";
        for (int i = 0; i < num_networks; i++)
        {
            html += "<option value=\"" + String(WiFi.SSID(i)) + "\">" + String(WiFi.SSID(i)) + "</option>";
        }
        html += "</select>";
        html += "<br>";
        html += "<label for=\"password\">Passwort:</label><br>";
        html += "<input type=\"password\" id=\"password\" name=\"password\" style=\"margin-bottom: 10px;\"><br>";
        html += "<input type=\"submit\" value=\"Verbinden\" style=\"background-color: red; border: none; color: white; padding: 10px 20px;\">";
        html += "&nbsp;&nbsp;&nbsp;";
        html += "<form action=\"/refresh\">";
        html += "<input type=\"submit\" value=\"Aktualisieren\" style=\"background-color: red; border: none; color: white; padding: 10px 20px;\">";
        html += "</form>";
        html += "<br><br>";
    }
    

    

    return html;
}

#endif /* F82BEDC2_2A77_4D15_8B16_3DC3BB5F5390 */
