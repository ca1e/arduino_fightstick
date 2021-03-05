#ifndef WifiManager_h
#define WifiManager_h

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>

#define MAX_SRV_CLIENTS 2   //max client count
const unsigned int SERVER_PORT PROGMEM   = 8266; //range 0-65535
const unsigned int HTTP_TIMEOUT   = 5000; //timeout

WiFiServer server(SERVER_PORT);
WiFiClient serverClients[MAX_SRV_CLIENTS];

void smartConfig()
{
  if (WiFi.getAutoConnect())
  {
    WiFi.begin();
    for (int i = 0; i < 5; i++)
    {
      if (WiFi.status() == WL_CONNECTED)
      {
        //Serial.println("AutoConfig Success");
        //Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
        //Serial.printf("PSW:%s\r\n", WiFi.psk().c_str());
        break;
      }
      delay(100);
    }
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.mode(WIFI_STA);
    WiFi.beginSmartConfig();
    //Waiting for Smartconfig
    while (WiFi.status() != WL_CONNECTED) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(150);
      digitalWrite(LED_BUILTIN, LOW);
      delay(150);
    }
    WiFi.setAutoConnect(true);
  }
}

void wifi_loop()
{
  uint8_t i;
  if (server.hasClient()) {
    for (i = 0; i < MAX_SRV_CLIENTS; i++)
    {
      if (!serverClients[i] || !serverClients[i].connected())
      {
        if (serverClients[i]) serverClients[i].stop();
        serverClients[i] = server.available();
        continue;
      }
    }
    WiFiClient serverClient = server.available();
    if (serverClient)
    {
      serverClient.stop();
    }
  }

  for (i = 0; i < MAX_SRV_CLIENTS; i++)
  {
    if (serverClients[i] && serverClients[i].connected())
    {
      //处理客户端发过来的数据
      if (serverClients[i].available())
      {
        while (serverClients[i].available())
        {
          Serial.write(serverClients[i].read());
        }//串口打印客户端发的数据
        delay(200);
        while (Serial.available() > 0)
        {
          String comdata = "";
          comdata += char(Serial.read());
          //String comdata = Serial.readString();
          int data_len = comdata.length() + 1;
          char a[data_len] ;
          delay(2);
          comdata.toCharArray(a, data_len);
          if (comdata.length() > 0)
          {
            serverClients[i].write(a);//串口接收数据发给客户端
            comdata = "";
          }
        }
      }
    }
  }
}
#endif
