// WiFi module (ATWINC1500)
#include <SPI.h>
#include <WiFi101.h>
#define WIFI_MISO              12
#define WIFI_MOSI              11 
#define WIFI_CS                10
#define WIFI_SCK               13  
#define WIFI_IRQ               33
#define WIFI_RST               34
#define WIFI_EN                35
char ssid[] = "CanLogger4ELD";
char pass[] = "deadbeef77";
int status = WL_IDLE_STATUS;
WiFiServer server(80);

bool setupWifi()
{
    SPI.setMISO(WIFI_MISO);
    SPI.setMOSI(WIFI_MOSI);
    SPI.setCS(WIFI_CS);
    SPI.setSCK(WIFI_SCK);
    SPI.begin();
    WiFi.setPins(WIFI_CS, WIFI_IRQ, WIFI_RST, WIFI_EN);
    if (WiFi.status() == WL_NO_SHIELD) {
        #ifdef DEBUG 
        Serial.println("WiFi shield not present");
        #endif
        while (true);
    }
    // attempt to create access point
    status = WiFi.beginAP(ssid, pass);
    if (status != WL_AP_LISTENING) {
        #ifdef DEBUG
        Serial.println("Creating access point failed");
        #endif
        while (true);
    }
    delay(10000);
    server.begin();
    Serial.println("Wifi Setup Successful");
    return true;
}

