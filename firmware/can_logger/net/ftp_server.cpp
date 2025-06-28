#include "FTPServer.h"
#include "LoggerManager.h"
#include "LEDStatus.h"

#include <WiFi101.h>
#include <SPI.h>
#include <SD.h>

const char* ssid = "CanLogger4";
const char* pass = "CanLogger4"; //Change password later

WiFiServer server(80);
WiFiClient currentClient;
bool active = false;

FTPServer ftpServer;

void FTPServer::begin() {
    SPI.setMISO(12);
    SPI.setMOSI(11);
    SPI.setSCK(13);
    SPI.setCS(10);
    SPI.begin();

    WiFi.setPins(10, 33, 34, 35);
    if (WiFi.status() == WL_NO_SHIELD) {
        Serial.println("No WiFi shield detected.");
        while (true);
    }

    int status = WiFi.beginAP(ssid, pass);
    if (status != WL_AP_LISTENING) {
        Serial.println("Failed to create AP");
        while (true);
    }

    delay(10000); 
    server.begin();

    Serial.println("AP + server ready");
    setLEDMode(3, LED_BLINK_SLOW);  
}

void FTPServer::poll() {
    WiFiClient client = server.available();
    if (client) {
        currentClient = client;
        active = true;

        Serial.println("FTP client connected");
        loggerManager.enterTransferMode();  
        setLEDMode(3, LED_ON);  

        serveClient();

        currentClient.stop();
        active = false;

        loggerManager.exitTransferMode();  
        setLEDMode(3, LED_BLINK_SLOW);   
        Serial.println("FTP client disconnected");
    }
}

bool FTPServer::isActive() {
    return active;
}

void FTPServer::serveClient() {
    if (!currentClient.connected()) return;

    while (currentClient.available() == 0) delay(1);

    String request = currentClient.readStringUntil('\r');
    currentClient.readStringUntil('\n');

    String filename = "/";
    int pathStart = request.indexOf(' ') + 1;
    int pathEnd = request.indexOf(' ', pathStart);
    if (pathStart >= 0 && pathEnd > pathStart) {
        filename = request.substring(pathStart, pathEnd);
    }

    if (filename == "/") {
        listDirectory(currentClient, "/");
    } else {
        sendFile(currentClient, filename.c_str());
    }
}

void FTPServer::listDirectory(WiFiClient& client, const char* path) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println();
    client.println("<html><body><h2>CAN Logger Files</h2><ul>");

    File dir = SD.open(path);
    if (!dir) {
        client.println("<li>Failed to open directory</li>");
        return;
    }

    File file;
    while ((file = dir.openNextFile())) {
        client.print("<li><a href=\"");
        client.print(file.name());
        client.print("\">");
        client.print(file.name());
        client.println("</a></li>");
        file.close();
    }

    client.println("</ul></body></html>");
}

void FTPServer::sendFile(WiFiClient& client, const char* path) {
    File file = SD.open(path);
    if (!file) {
        client.println("HTTP/1.1 404 Not Found\r\n\r\nFile not found.");
        return;
    }

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/octet-stream");
    client.print("Content-Disposition: attachment; filename=\"");
    client.print(file.name());
    client.println("\"\r\n");

    const size_t chunk = 512;
    uint8_t buffer[chunk];
    while (file.available()) {
        size_t len = file.read(buffer, chunk);
        client.write(buffer, len);
    }

    file.close();
}
