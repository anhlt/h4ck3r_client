#ifndef WEBSOCKETHANDLER_H_
#define WEBSOCKETHANDLER_H_

#include <string>
#include <WebSocketsClient.h>

class WebSocketHandler
{
private:
    std::string host;
    std::string path;
    WebSocketsClient webSocket;
public:
    WebSocketHandler(std::string host, std::string path);
    void begin();
    void loop();
    ~WebSocketHandler();

};

#endif