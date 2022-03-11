#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include "./DNSServer.h"
#include "base.hpp"
#include "heaterTimer.hpp"
#include "Logger.hpp"
#include "Heater.hpp"

const byte        DNS_PORT = 53;          // Capture DNS requests on port 53
IPAddress         apIP(10, 10, 10, 1); 
DNSServer         dnsServer;
const char* ssid = "WMOSKITO";
const char* password = ".ubX54bVSt#vxW11m.";
const char* myhostname = "Standheizung";
const char* curVersion = "2.0.0"; 
const int relais1Pin = 5;
const int relais2Pin = 4;
const int relaisFanPin = 16;

ESP8266WebServer server(80);
base indexPage(&server);
heaterTimer hTimer;
Logger* logger = Logger::instance();
heater myHeater(relais1Pin, relais2Pin, relaisFanPin, &indexPage);

void handleRoot() 
{
  char tmpShould[5];
  char tmpIs[5];
  int fanspeed = indexPage.Get_speed().toInt();
  if (0 < fanspeed)
  {
    myHeater.SetSpeed(fanspeed);
  }
  if (String("OFF") == indexPage.Get_state())
  {
    logger->Debug("Switch Off");
    myHeater.Off();
    indexPage.Render();
    return;
  }
  if (0 < indexPage.Get_timer().toInt())
  {
    hTimer.setTimer(indexPage.Get_timer().toInt());
    myHeater.On(&hTimer);
  }
  else
  {
    indexPage.Set_timer(String(hTimer.getTimer()));
  }

  int tempSoll = (int) (10.0 * indexPage.Get_tempSoll().toFloat());
  if (0 < tempSoll)
  {
    myHeater.SetTemp(tempSoll);
  }

  if (myHeater.IsOn())
  {
    logger->Debug("Heater on");
    indexPage.Set_state("ON");
  }
  else
  {
    logger->Debug("Heater off");
    indexPage.Set_state("OFF");
  }

  if (myHeater.IsHeating())
  {
    indexPage.Set_heating("ON");
  }
  else
  {
    indexPage.Set_heating("OFF");
  }
  if (String("ON") == indexPage.Get_turbo())
  {
    logger->Debug("Turbo on");
    myHeater.TurboOn();
  }
  else
  {
    logger->Debug("Turbo off");
    myHeater.TurboOff();
  }  
  sprintf(tmpShould, "%.1f", (double) myHeater.getTenthDegreesShould()/10);
  sprintf(tmpIs, "%.1f", (double) myHeater.getTenthDegrees()/10);
  
  indexPage.Set_tempSoll(tmpShould);
  indexPage.Set_tempIst(tmpIs);
  indexPage.Set_version(curVersion);
  indexPage.Set_timer(String(hTimer.getTimer()));
  if (myHeater.TurboActive())
  {
    indexPage.Set_turbo("ON");
  }
  else 
  {
    indexPage.Set_turbo("OFF");
  }

  indexPage.Render();
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void Render()
{
  if (NULL != logger)
  {
    logger->Debug("Render");
  }
  indexPage.Render();
}

void setup(void) {
 
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("Viano-Standheizung", password);
  dnsServer.start(DNS_PORT, "*", apIP);


  server.on("/", Render);
    server.send(200, "text/plain", "this works as well");
  indexPage.SetCallback_submit(handleRoot); 
  
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  dnsServer.processNextRequest();
  server.handleClient();
}
