#include <EthernetWebServer.h>
#include <SPI.h>
//#include <SD.h>

EthernetWebServer server(80);

void handleRoot()
{
  String html = F("Hello from HelloServer running on ");
  server.send(200, F("text/html"), html);
  
//  server.send_P(200, "text/html",index);
}

void handleNotFound()
{
  String message = F("File Not Found\n\n");

  message += F("URI: ");
  message += server.uri();
  message += F("\nMethod: ");
  message += (server.method() == HTTP_GET) ? F("GET") : F("POST");
  message += F("\nArguments: ");
  message += server.args();
  message += F("\n");

  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, F("text/plain"), message);
}

void setup(void)
{

//  SD.begin();
  
  Serial.begin(115200);
  while (!Serial);

  Serial.print("\nStarting HelloServer on ");
  Serial.print(F(" with "));

  byte mac[] = { 0x18, 0xC0, 0x4D, 0xD8, 0x53, 0x18 };
  byte ip[] = { 172, 16, 9, 49};
  
  Ethernet.begin(mac,ip);

  Serial.print(F("Connected! IP address: "));
  Serial.println(Ethernet.localIP());

  server.on(F("/"), handleRoot);
  
  server.on(F("/inline"), []()
  {
    server.send(200, F("text/plain"), F("This works as well"));
  });

  server.onNotFound(handleNotFound);

  server.begin();

  Serial.print(F("HTTP EthernetWebServer is @ IP : "));
  Serial.println(Ethernet.localIP());
}

void loop(void)
{
  server.handleClient();
}
