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

static void handleClient(WiFiClient &client);

static void send404(WiFiClient &client);
static void send200(WiFiClient &client);

static void processGET(WiFiClient &client, const String &path);
static void processPOST(WiFiClient &client,
                        const String &path,
                        const String &body);

static bool processLogin(const String &body);
static bool processLogout(void);
static bool processDuty(const String &body);
static bool processDriver(const String &body);
static bool processShipping(const String &body);
static bool processTimeZone(const String &body);

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

static void send200(WiFiClient &client)
{
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println("Connection: close");
    client.println();

    client.println("OK");
}

static void send404(WiFiClient &client)
{
    client.println("HTTP/1.1 404 Not Found");
    client.println("Connection: close");
    client.println();
}

void wifiUpdate(void)
{
    WiFiClient client = server.available();

    if (!client)
        return;

    handleClient(client);

    client.stop();
}

static void handleClient(WiFiClient &client)
{
    String requestLine = client.readStringUntil('\n');
    requestLine.trim();

    String method;
    String path;

    int first = requestLine.indexOf(' ');
    int second = requestLine.indexOf(' ', first + 1);

    if(first < 0 || second < 0)
    {
        send404(client);
        return;
    }

    method = requestLine.substring(0, first);
    path   = requestLine.substring(first + 1, second);

    // Skip headers
    while(client.connected())
    {
        String line = client.readStringUntil('\n');

        if(line == "\r")
            break;
    }

    if(method == "GET")
    {
        processGET(client, path);
    }
    else if(method == "POST")
    {
        String body;

        while(client.available())
            body += (char)client.read();

        processPOST(client, path, body);
    }
    else
    {
        send404(client);
    }
}

static void processGET(WiFiClient &client,
                       const String &path)
{
    if(path == "/status")
    {
        // TODO
        send200(client);
    }
    else if(path == "/rods")
    {
        send200(client);
    }
    else if(path == "/events")
    {
        send200(client);
    }
    else if(path == "/diagnostics")
    {
        send200(client);
    }
    else
    {
        send404(client);
    }
}

static void processPOST(WiFiClient &client,
                        const String &path,
                        const String &body)
{
    bool success = false;

    if(path == "/login")
        success = processLogin(body);

    else if(path == "/logout")
        success = processLogout();

    else if(path == "/driver")
        success = processDriver(body);

    else if(path == "/duty")
        success = processDuty(body);

    else if(path == "/shipping")
        success = processShipping(body);

    else if(path == "/timezone")
        success = processTimeZone(body);

    if(success)
        send200(client);
    else
        send404(client);
}

static bool processLogin(const String &body)
{
    // TODO
    return true;
}

static bool processLogout(void)
{
    return true;
}

static bool processDriver(const String &body)
{
    return true;
}

static bool processDuty(const String &body)
{
    return true;
}

static bool processShipping(const String &body)
{
    return true;
}

static bool processTimeZone(const String &body)
{
    return true;
}

