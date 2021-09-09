#include<ESP8266WiFi.h>
#include "RF24Network.h"
#include "RF24.h"
#include "RF24Mesh.h"
#include <SPI.h>
#include<string.h>


const char* ssid = "HARSH";
const char* password = "DOORASTHA";

WiFiServer server(80);
//WiFiClient client = server.available();

IPAddress ip(192, 168, 1, 4);
uint8_t pld[255];
uint8_t buf[255];

RF24 radio(2, 4);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

void mesh_config();
static void send_cmd_to_node(uint8_t node_id, uint8_t cmd_code);
static void display_incoming_date(uint32_t m_sec);


void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(ip, ip, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, password);
  server.begin();
  mesh.setNodeID(0);
  mesh.begin();
}

void loop() {
 String str="";
 String str1="";
 String str2="";
 mesh_config();
   WiFiClient client = server.available();
   if (client) {
    if(client.connected())
    {
      Serial.println("Client Connected");
      client.write("<node_id>#<Data> Enter in this format");
    }
    
    while(client.connected()){      
      while(client.available()>0){
        int h=client.available();
      for (int i = 0; i < h; i++)
    {
      str+= (char)client.read();
    }
    int len= str.length();
    str1=str.substring(0,1);
    str2=str.substring(3,len);
    Serial.println(str1);
    Serial.println(str2);
    uint8_t id=str1.toInt();
    //str2.toCharArray((char *)pld,255);
    strcpy((char*)pld,str2.c_str());
    mesh_config();
    send_cmd_to_node(id,*pld);
    display_incoming_date(2000);
    
    str="";
    
      }
    }
    client.stop();
    Serial.println("Client disconnected");   
   }
}

void mesh_config()
{
  mesh.update();
   mesh.DHCP();
}

static void send_cmd_to_node(uint8_t node_id, uint8_t cmd_code) {
  mesh.begin();
  mesh_config();
 // uint8_t node_address = mesh.getAddress(node_id) ;
  if (mesh.write(mesh.getAddress(node_id),&cmd_code,'C',sizeof(cmd_code)) == true) {
    Serial.println("Command sent.");
    //client.write("Data sent to node");
  } else {
    Serial.println("Command couldn't be send.");
    //client.write("Data could not be sent to node");
  }
}

static void display_incoming_date(uint32_t m_sec)
{
  static uint32_t prev_time = 0;
  static uint16_t buf_size = 0;
  String strdata="";
  if (millis() - prev_time >= m_sec)
  {
     prev_time = millis();
      while (network.available())
      {
        RF24NetworkHeader header;
        buf_size = network.peek(header);
        switch (header.type)
        {
          case 'S':
              network.read(header, buf, buf_size);
              //client.write(buf,buf_size);
              Serial.println(*buf);
              break;
          default:
            network.read(header, 0, 0);
            break;
        }
      }
  }
  display_incoming_date(2000);
}