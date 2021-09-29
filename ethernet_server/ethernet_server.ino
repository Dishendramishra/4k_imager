#include <EthernetWebServer.h>
#include <SPI.h>
//#include <SD.h>

EthernetWebServer server(80);

const int led = 13;

const char index[] PROGMEM = R"=====(<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8' /><meta http-equiv='X-UA-Compatible' content='IE=edge' /><meta name='viewport' content='width=device-width, initial-scale=1.0' /><title>4K Imager</title></head><body style='text-align: center;'><div style='display: inline-block;'><fieldset><legend>Filter Wheel 1</legend><table><tr align='center'><td><button onclick='handle_click(this.id)' id='fw1_a'>A</button></td><td><button onclick='handle_click(this.id)' id='fw1_b'>B</button></td><td><button onclick='handle_click(this.id)' id='fw1_c'>C</button></td><td><button onclick='handle_click(this.id)' id='fw1_null'>BLANK</button></td></tr><tr align='center'><td><svg height='50' width='50'><circle id='fw1_a_img' cx='25' cy='25' r='20' stroke='grey' stroke-width='5' fill='#D3D3D3' /></svg></td><td><svg height='50' width='50'><circle id='fw1_b_img' cx='25' cy='25' r='20' stroke='grey' stroke-width='5' fill='#D3D3D3' /></svg></td><td><svg height='50' width='50'><circle id='fw1_c_img' cx='25' cy='25' r='20' stroke='grey' stroke-width='5' fill='#D3D3D3' /></svg></td><td><svg height='50' width='50'><circle id='fw1_null_img' cx='25' cy='25' r='20' stroke='grey' stroke-width='5' fill='#D3D3D3' /></svg></td></tr></table></fieldset><fieldset><legend>Filter Wheel 2</legend><table><tr align='center'><td><button onclick='handle_click(this.id)' id='fw2_a'>A</button></td><td><button onclick='handle_click(this.id)' id='fw2_b'>B</button></td><td><button onclick='handle_click(this.id)' id='fw2_c'>C</button></td><td><button onclick='handle_click(this.id)' id='fw2_null'>BLANK</button></td></tr><tr align='center'><td><svg height='50' width='50'><circle id='fw2_a_img' cx='25' cy='25' r='20' stroke='grey' stroke-width='5' fill='#D3D3D3' /></svg></td><td><svg height='50' width='50'><circle id='fw2_b_img' cx='25' cy='25' r='20' stroke='grey' stroke-width='5' fill='#D3D3D3' /></svg></td><td><svg height='50' width='50'><circle id='fw2_c_img' cx='25' cy='25' r='20' stroke='grey' stroke-width='5' fill='#D3D3D3' /></svg></td><td><svg height='50' width='50'><circle id='fw2_null_img' cx='25' cy='25' r='20' stroke='grey' stroke-width='5' fill='#D3D3D3' /></svg></td></tr></table></fieldset><fieldset><legend>ND Filter</legend><table><tr align='center'><td><button id='nd_set' onclick='handle_click(this.id)'>Set</button></td><td><button id='nd_unset' onclick='handle_click(this.id)'>Unset</button></td></tr><tr align='center'><td><svg height='50' width='50'><circle id='nd_set_img' cx='25' cy='25' r='20' stroke='grey' stroke-width='5' fill='#D3D3D3' /></svg></td><td><svg height='50' width='50'><circle id='nd_unset_img' cx='25' cy='25' r='20' stroke='grey' stroke-width='5' fill='#D3D3D3' /></svg></td></tr></table></fieldset></div></body> <script>function handle_click(id){element=document.getElementById(id+'_img');if(element.getAttribute('fill')=='#D3D3D3'){element.setAttribute('fill','#A6E22E');} else{element.setAttribute('fill','#D3D3D3');}}</script> </html>)=====";

void handleRoot()
{
//  String html = F("Hello from HelloServer running on ");
//  server.send(200, F("text/html"), index);
  
  server.send_P(200, "text/html",index);
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

  digitalWrite(led, 0);
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
