#include "Ethernet.h"
#include <PubSubClient.h>
//#include <ArduinoJson.h>

// Prototypes
void printConfig();
void printEthernetData();
void printIndex();
void printHelp();
void callback(char* topic, byte* payload, unsigned int length);
void getDHTReadings();
void announceDHTReadings();


#define MAX_BUF_LEN 128
#define STATUS_LED RED_LED
#define CONTROL_PIN PM_3


char buffer[128];

String prefix   = "/PersonalProjects/";      // global prefix for all topics - must be some as mobile device
String deviceID = "fishtank01";


// config for cloud mqtt broker by DNS hostname ( for example, cloudmqtt.com use: m20.cloudmqtt.com - EU )

// MQTTServer to use
char server[] = "secure.tnhh.net";            // for cloud broker - by hostname, from CloudMQTT account data
int    mqttport = 1883;                                // default 1883, but CloudMQTT.com use other, for example: 13191, 23191 (SSL), 33191 (WebSockets) - use from CloudMQTT account data
String mqttuser =  "";                              // from CloudMQTT account data
String mqttpass =  "";                              // from CloudMQTT account data

EthernetClient ethClient;
PubSubClient client(server, mqttport, callback, ethClient);

int statusConfig = 0;

float temperature = -500.0f, humidity= -500.0f;

int pumpStatus = 0;
unsigned long lastTurnedPumpOn;
unsigned long maxPumpingTime = 60*60*1000; // 60 minutes


char topicSub[128];
char topicPub[128];
char topicPubSensors[128];
  

void setupCommunication() {
  pinMode(CONTROL_PIN, OUTPUT);
  
  Serial.println("Connecting to Ethernet....");  
  IPAddress ip = IPAddress(146,252,242,129);
  IPAddress dns = IPAddress(146,252,242,12);
  IPAddress gw = IPAddress(146,252,242,254);
  IPAddress mask = IPAddress(255,255,255,0);
  pinMode(STATUS_LED, OUTPUT);
  
  Ethernet.begin(0);
  //  Ethernet.begin(0, ip, dns, gw);

  printEthernetData();
  
  
  String(prefix + deviceID + "/control").toCharArray(topicSub, 127);
  String(prefix + deviceID + "/announce").toCharArray(topicPub, 127);
  String(prefix + deviceID + "/sensors").toCharArray(topicPubSensors, 127);

  Serial.println("Communication thread started!");
}


void loopCommunication() {
    // Reconnect if the connection was lost
  if (!client.connected()) {
    Serial.println("PubSub Client is Disconnected. Connecting....");
    digitalWrite(STATUS_LED, LOW);

    if(!client.connect("energiaClient")) {
      Serial.println("Connection failed");
    } else {
      Serial.println("Connection success");
      if(client.subscribe(topicSub)) {
        Serial.println("Subscription successful!");
        digitalWrite(STATUS_LED, HIGH);
      }
    }
  }
   
  // Check if any message were received
  // on the topic we subsrcived to
  client.loop();
  autoPumpOff();
  delay(1000);
}


void autoPumpOff() {
  if (pumpStatus == 1) {
    unsigned long currentMillis = millis();
    if (currentMillis - lastTurnedPumpOn >= maxPumpingTime) {
      // automatically turn the water pump off
      digitalWrite(CONTROL_PIN, LOW);
      pumpStatus = 0;
      Serial.println("Pump is automatically turned off!");
    }
  }
}



void callback(char* topic, byte* payload, unsigned int length) {
  
  // If length is larger than MAX_BUF_LEN,
  // then make sure we do not write more then our buffer can handle
  if (length > MAX_BUF_LEN) length=MAX_BUF_LEN;
  strncpy(buffer, (const char*)payload, length);
  
  if (strncmp(buffer, "pump_on", length) == 0) {
    digitalWrite(CONTROL_PIN, HIGH);
    lastTurnedPumpOn = millis();
    pumpStatus = 1;
    Serial.println("Turning PUMP on");
  } else if (strncmp(buffer, "pump_off", length) == 0) {
    digitalWrite(CONTROL_PIN, LOW);
    pumpStatus = 0;
    Serial.println("Turning PUMP off");
  } else {
    Serial.print("Unknown message: ");
    Serial.println(buffer);
    //Serial.println(payload);
  }
    
  //
  //client.publish("outTopic", buffer, length);
}




void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    switch(inChar) {
    case 'h':
      printHelp();
      break;
    case 'i':
      printEthernetData();
      break;
    case 't':
      getDHTReadings();
      break;
    default:
      Serial.println();
      Serial.println("Invalid menu option");
    }
  } 
}

void printHelp() {
  Serial.println();
  Serial.println("+++++++++++++++++++++++++++++++++++++");
  Serial.println("Help menu:");
  Serial.println("\th: This help menu");
  Serial.println("\ti: IP address information");
  Serial.println("\tt: Update and print temperature and humidity sensors value");
  Serial.println("+++++++++++++++++++++++++++++++++++++");
}


void announceDHTReadings() {
  char tmp[127];
  int length;
  length = sprintf(tmp, "{\"sensors\":{\"temperature\":%4.2f,\"humidity\":%4.2f}}", temperature, humidity);
  client.publish(topicPubSensors, (uint8_t*)tmp, length);
}

void printHex(int num, int precision) {
  char tmp[16];
  char format[128];

  sprintf(format, "%%.%dX", precision);

  sprintf(tmp, format, num);
  Serial.print(tmp);
}

void printEthernetData() {
  // print your IP address:
  Serial.println();
  Serial.println("IP Address Information:");  
  IPAddress ip = Ethernet.localIP();
  Serial.print("IP Address:\t");
  Serial.println(ip);

  // print your MAC address:

  IPAddress subnet = Ethernet.subnetMask();
  Serial.print("NetMask:\t");
  Serial.println(subnet);

  // print your gateway address:
  IPAddress gateway = Ethernet.gatewayIP();
  Serial.print("Gateway:\t");
  Serial.println(gateway);

  // print your gateway address:
  IPAddress dns = Ethernet.dnsServerIP();
  Serial.print("DNS:\t\t");
  Serial.println(dns);
}





