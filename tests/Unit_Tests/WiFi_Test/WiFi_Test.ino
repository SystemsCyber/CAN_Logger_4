#include <SPI.h>
#include <WiFi101.h>

char ssid[] = "CanLogger4";
char pass[] = "CanLogger4";

int status = WL_IDLE_STATUS;

WiFiServer server(80);


void setup() {
  Serial.begin(9600);
  while (!Serial);

  SPI.setMISO(12);
  SPI.setMOSI(11);
  SPI.setCS(10);
  SPI.setSCK(13);

  SPI.begin();

  WiFi.setPins(10, 33, 34, 35);


  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to WiFi network:
  status = WiFi.beginAP(ssid, pass);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    // don't continue
    while (true);
  }

  delay(10000);
  
  server.begin();

}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("new client");
  }

}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
