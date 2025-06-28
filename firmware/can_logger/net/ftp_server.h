#ifndef FTP_SERVER_H
#define FTP_SERVER_H

#include <Arduino.h>

class FTPServer {
public:
    void begin();    
    void poll();      
    bool isActive();    

private:
    void serveClient();
    void sendFile(WiFiClient& client, const char* path);
    void listDirectory(WiFiClient& client, const char* path);
};

extern FTPServer ftpServer;

#endif
