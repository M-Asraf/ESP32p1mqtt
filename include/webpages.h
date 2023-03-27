#ifndef F82BEDC2_2A77_4D15_8B16_3DC3BB5F5390
#define F82BEDC2_2A77_4D15_8B16_3DC3BB5F5390

const char ROOT_HTML[] PROGMEM = R"rawliteral(
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
    String html;
    html.reserve(2048);
    html = String(ROOT_HTML);

    if (WiFi.softAPgetStationNum() > 0)
    {
        // ESP32 ist im Access Point-Modus
        html.concat(F("<h3>Access-Point-Modus</h3>"));
        html.concat(F("<h5 style='margin-bottom: 0px; line-height: 0;'><b>SSID:</b> "));
        html.concat((WiFi.softAPSSID()));
        html.concat("</h5>");
        html.concat(F("<h5 style='margin-bottom: 0px; line-height: 0;'><b>IP-Adresse:</b> "));
        html.concat(WiFi.softAPIP().toString());
        html.concat("</h5>");
        html.concat(F("<h5 style='margin-bottom: 0px; line-height: 0;'><b>MAC-Adresse:</b> "));
        html.concat(WiFi.softAPmacAddress());
        html.concat("</h5>");
        html.concat(F("<h5 style='margin-bottom: 0px; line-height: 0;'><b>Kanal:</b> "));
        html.concat((WiFi.channel()));
        html.concat("</h5>");
    }
    else
    {
        // ESP32 ist mit einem Netzwerk verbunden
        if (WiFi.status() == WL_CONNECTED)
        {
            html.concat(F("<h3>Aktuelles Netzwerk</h3>"));
            html.concat(F("<h5 style='margin-bottom: 0px; line-height: 0;'><b>SSID:</b> "));
            html.concat((WiFi.SSID()));
            html.concat("</h5>");
            html.concat(F("<h5 style='margin-bottom: 0px; line-height: 0;'><b>IP-Adresse:</b> "));
            html.concat(WiFi.localIP().toString());
            html.concat("</h5>");
            html.concat(F("<h5 style='margin-bottom: 0px; line-height: 0;'><b>MAC-Adresse:</b> "));
            html.concat(WiFi.macAddress());
            html.concat("</h5>");
            html.concat(F("<h5 style='margin-bottom: 0px; line-height: 0;'><b>Kanal:</b> "));
            html.concat((WiFi.channel()));
            html.concat("</h5>");

            html.concat(F("<h3>MQTT-Verbindung</h3>"));
            html.concat("<form method=\"post\" action=\"/handle_mqtt\">");
            html.concat(F("Server: <br><input type=\"text\" name=\"mqttServer\" style=\"margin-bottom: 10px;\" placeholder=\"xxx.xxx.xxx.xxx\"> <br>"));
            html.concat(F("Port: <br><input type=\"number\" name=\"mqttPort\" style=\"margin-bottom: 10px;\"> <br>"));
            html.concat(F("Benutzername: <br><input type=\"text\" name=\"mqttUsername\" style=\"margin-bottom: 10px;\"> <br>"));
            html.concat(F("Passwort: <br><input type=\"password\" name=\"mqttPassword\" style=\"margin-bottom: 10px;\"> <br>"));
            html.concat(F("<input type=\"submit\" value=\"Verbinden\" style=\"background-color: red; border: none; color: white; padding: 10px 20px;\">"));
            html.concat("</form>");
            html.concat("</td>");
            html.concat("</tr>");
            html.concat("</table>");
            html.concat("</body>");
            html.concat("</html>");

            html.concat("</head>");
            html.concat("<body>");
            html.concat(F("<h3>MQTT-Status</h3>"));
            html.concat("<div style='border: none; background-color: white; padding: 10px; display: inline-block;'>");
            html.concat(F("<p style=' padding: 5px; margin: 0; color: red; display: inline-block;'>[[MQTT_STATUS]]</p>"));
            html.concat("</div>");
            html.concat("</body>");
            html.concat("</html>");
        }
        else
        {
            html.concat(F("<p>Nicht mit einem Netzwerk verbunden</p>"));
        }
    }

    html.concat(F("<h3>Verfügbare WLAN-Netzwerke</h3>"));

    int num_networks = WiFi.scanNetworks();
    if (num_networks == 0)
    {
        html.concat(F("<p>Keine WLAN-Netzwerke gefunden.</p>"));
    }
    else
    {
        html.concat("<form action=\"/connect\">");
        html.concat("<label for=\"ssid\">SSID:</label><br>");
        html.concat(F("<select name=\"ssid\" id=\"ssid\" style=\"margin-bottom: 10px;\">"));
        for (int i = 0; i < num_networks; i++)
        {
            html.concat("<option value=\"");
            html.concat((WiFi.SSID(i)));
            html.concat("\">");
            html.concat((WiFi.SSID(i)));
            html.concat("</option>");
        }

        html.concat("</select>");
        html.concat("<br>");
        html.concat("<label for=\"password\">Passwort:</label><br>");
        html.concat(F("<input type=\"password\" id=\"password\" name=\"password\" style=\"margin-bottom: 10px;\"><br>"));

        html.concat(F("<input type=\"submit\" value=\"Verbinden\" style=\"background-color: red; border: none; color: white; padding: 10px 20px;\">"));
        html.concat("&nbsp;&nbsp;&nbsp");
        html.concat("<form action=\"/refresh\">");
        html.concat(F("<input type=\"submit\" value=\"Aktualisieren\" style =\"background-color: red; border: none; color: white; padding: 10px 20px;\"><br>"));
        html.concat("</form>");
        html.concat("<br><br>");
    }
    return html;
}

#endif /* F82BEDC2_2A77_4D15_8B16_3DC3BB5F5390 */
