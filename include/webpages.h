#pragma once

const char ROOT_HTML[] = R"rawliteral(
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

String getHtmlContent(PubSubClient &mqttclient)
{
    String html;
    html.reserve(2048);
    html = FPSTR(ROOT_HTML);

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
        // ESP32 ist mit einem Wlan-Netzwerk verbunden
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

            // MQTT-Verbindungsformular
            html.concat(F("<h3>MQTT-Verbindung</h3>"));
            html.concat("<form method=\"post\" action=\"/handle_mqtt\">");
            html.concat(F("Server: <br><input type=\"text\" name=\"mqttServer\" style=\"margin-bottom: 10px;\" placeholder=\"xxx.xxx.xxx.xxx\" pattern=\"^(\\d{1,3}\\.){3}\\d{1,3}$\" title=\"Bitte geben Sie eine gültige IP-Adresse ein.\"> <br>"));
            html.concat(F("Port: <br><input type=\"number\" name=\"mqttPort\" style=\"margin-bottom: 10px;\" required> <br>"));
            html.concat(F("Benutzername: <br><input type=\"text\" name=\"mqttUsername\" style=\"margin-bottom: 10px;\"> <br>"));
            html.concat(F("Passwort: <br><input type=\"password\" name=\"mqttPassword\" style=\"margin-bottom: 10px;\"> <br>"));

            // Füge den "Verbinden"-Button hinzu
            html.concat(F("<input type=\"submit\" value=\"Verbinden\" style=\"background-color: blue; border: none; color: white; padding: 10px 20px;\">"));
            html.concat("</form>");

            // MQTT-Status Meldung
            html.concat("<h3>MQTT-Status: <span style='color: ");
            if (mqttclient.connected())
            {
                html.concat("green;'>verbunden!</span></h3>");
            }
            else
            {
                html.concat("red;'>nicht verbunden!</span></h3>");
            }
        }
    }

    // WLAN-Netzwerk werden gesucht und als Dropdown angezeigt
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
        html.concat("<form action=\"/submit_password\" method=\"post\">");
        html.concat("<label for=\"password\">Passwort:</label><br>");
        html.concat(F("<input type=\"password\" id=\"password\" name=\"password\" style=\"margin-bottom: 10px;\"><br>"));
        html.concat(F("<input type=\"submit\" value=\"Verbinden\" style=\"background-color: blue; border: none; color: white; padding: 10px 20px;\">"));
        html.concat("</form>");
        html.concat("&nbsp;&nbsp;&nbsp");
        html.concat("<form action=\"/refresh\" method=\"get\">");
        html.concat(F("<input type=\"submit\" value=\"Aktualisieren\" style=\"background-color: blue; border: none; color: white; padding: 10px 20px;\"><br>"));
        html.concat("</form>");
        html.concat("<br><br>");
    }
    
    html.concat(F("<footer>"));
    html.concat(F("<h5><p>&copy; 2023 - ESP32-Asraf</p></h5>"));
    html.concat(F("</footer>"));
    html.concat(F("</body>"));
    html.concat(F("</html>"));

    return html;
}
