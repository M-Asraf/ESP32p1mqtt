/*#pragma once

#include <HardwareSerial.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Arduino.h>
#include <funktion.h>

#define P1_SERIAL Serial2 // Serial2 ist die Hardware-Schnittstelle des ESP32 für die P1-Schnittstelle
#define BAUDRATE 115200   // Baudrate der P1-Schnittstelle

float aktueller_verbrauch = 0.0;
float gesamt_verbrauch = 0.0;
float momentane_spannung = 0.0;
float momentaner_strom = 0.0;
float gesamte_einspeisung = 0.0;
float gesamte_ausspeisung = 0.0;


float extract_value(String p1_data)
{
    int start = p1_data.indexOf("(") + 1;
    int end = p1_data.indexOf("*");
    String value_str = p1_data.substring(start, end);
    float value = value_str.toFloat();
    return value;
}


int get_data_type(String p1_data)
{
    if (p1_data.startsWith("1-0:1.7.0"))
    {
        return 1; // Aktueller Verbrauch
    }
    else if (p1_data.startsWith("1-0:1.8.1"))
    {
        return 2; // Gesamtverbrauch Tarif 1
    }
    else if (p1_data.startsWith("1-0:32.7.0"))
    {
        return 3; // Momentane Spannung
    }
    else if (p1_data.startsWith("1-0:31.7.0"))
    {
        return 4; // Momentaner Strom
    }
    else if (p1_data.startsWith("1-0:2.8.1"))
    {
        return 5; // Gesamte Einspeisung
    }
    else if (p1_data.startsWith("1-0:1.8.2"))
    {
        return 6; // Gesamte Ausspeisung
    }
    else
    {
        return 0; // Keine bekannte P1-Datenart
    }
}
String mqtt_topic; // Variablendeklaration vor der switch-Anweisung
String mqtt_payload;
void switch_p1_data(String p1_data){
    switch (get_data_type(p1_data)){
    case 1: // Aktueller Verbrauch
        aktueller_verbrauch = extract_value(p1_data);
         mqtt_topic = "/stromzaehler/aktuellerverbrauch";
         mqtt_payload = String(aktueller_verbrauch);
        mqttclient.publish(mqtt_topic.c_str(), mqtt_payload.c_str());
        break;
    case 2: // Gesamtverbrauch Tarif 1
        gesamt_verbrauch = extract_value(p1_data);
         mqtt_topic = "/stromzaehler/gesamtverbrauch";
         mqtt_payload = String(gesamt_verbrauch);
        mqttclient.publish(mqtt_topic.c_str(), mqtt_payload.c_str());
        break;
    case 3: // Momentane Spannung
        momentane_spannung = extract_value(p1_data);
         mqtt_topic = "/stromzaehler/momentanespannung";
         mqtt_payload = String(momentane_spannung);
        mqttclient.publish(mqtt_topic.c_str(), mqtt_payload.c_str());
        break;
    case 4: // Momentaner Strom
        momentaner_strom = extract_value(p1_data);
         mqtt_topic = "/stromzaehler/momentanerstrom";
         mqtt_payload = String(momentaner_strom);
        mqttclient.publish(mqtt_topic.c_str(), mqtt_payload.c_str());
        break;
    case 5: // Gesamte Einspeisung
        gesamte_einspeisung = extract_value(p1_data);
         mqtt_topic = "/stromzaehler/gesamteinspeisung";
         mqtt_payload = String(gesamte_einspeisung);
        mqttclient.publish(mqtt_topic.c_str(), mqtt_payload.c_str());
        break;
    case 6: // Gesamte Ausspeisung
        gesamte_ausspeisung = extract_value(p1_data);
         mqtt_topic = "/stromzaehler/gesamtausspeisung";
         mqtt_payload = String(gesamte_ausspeisung);
        mqttclient.publish(mqtt_topic.c_str(), mqtt_payload.c_str());
        break;
    default:
        break;
    }
}


void loop()
{
    if (!mqttclient.connected())
    {
        reconnect(); // Bei Verbindungsabbruch mit MQTT-Broker erneut verbinden
    }
    mqttclient.loop();

    // Daten von P1-Schnittstelle lesen und verarbeiten
    if (P1_SERIAL.available() > 0)
    {
        String p1_data = P1_SERIAL.readStringUntil('\n'); // Daten von der P1-Schnittstelle lesen
        int data_type = get_data_type(p1_data);           // Typ der P1-Daten ermitteln
        if (data_type != 0)
        {                                       // Wenn die Daten bekannt sind, verarbeiten
            switch_p1_data(p1_data, data_type); // P1-Daten verarbeiten
        }
    }

    // Aktualisierte Daten an MQTT-Broker senden
    if (millis() - last_mqtt_send > mqtt_send_interval)
    {
        publish_data_to_mqtt();    // Daten an MQTT-Broker senden
        last_mqtt_send = millis(); // Zeitpunkt der letzten MQTT-Sendung aktualisieren
    }
}
*/