#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

// MQTT library
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// DHT Setup
#include "DHTesp.h"
DHTesp dht;

// Variables
unsigned long previousMillis = 0;
//const long interval = 60000;
const long interval = 5000;

//const char* mqtt_server = "m23.cloudmqtt.com";
//const int mqtt_port = 17822;
//const char* mqtt_user = "hhwlirgn";
//const char* mqtt_pass = "FXwQWwN1fZhw";

// const char* mqtt_server = "192.168.168.120";
// const int mqtt_port = 1883;
// const char* mqtt_user = "mosquitto";
// const char* mqtt_pass = "mosquittopass";

#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883
#define AIO_USERNAME "aburnsni"
#define AIO_KEY "aad58026fc614464b68e4df7f4b2a44a"

char temp2[5];
char humidity2[5];

//  Initialise pubsubslient
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish worktemp = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/work.temp");
Adafruit_MQTT_Publish workhumidity = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/work.humidity");

void MQTT_connect();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset saved settings
  //wifiManager.resetSettings();

  //set custom ip for portal
  //wifiManager.setAPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  //fetches ssid and pass from eeprom and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("AutoConnectAP");
  //or use this for auto generated name ESP + ChipID
  //wifiManager.autoConnect();


  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  dht.setup(2); // data pin 2

}

// void reconnect() {
//   // Loop until we're reconnected
//   while (!client.connected()) {
//     Serial.print("Attempting MQTT connection...");
//     // Attempt to connect
//     if (client.connect("ESP8266Client", mqtt_user, mqtt_pass)) {
//       Serial.println("connected");
//       // Once connected, publish an announcement...
//       client.publish("connected", "hello world");
//       // ... and resubscribe
//       //      client.subscribe("inTopic");
//     } else {
//       Serial.print("failed, rc=");
//       Serial.print(client.state());
//       Serial.println(" try again in 5 seconds");
//       // Wait 5 seconds before retrying
//       delay(5000);
//     }
//   }
// }

void loop() {
MQTT_connect();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
  // if (!client.connected()) {
  //   reconnect();
  // }
  //  client.loop();
    // put your main code here, to run repeatedly:
    //delay(dht.getMinimumSamplingPeriod());
    float temperature = dht.getTemperature();
    float humidity = dht.getHumidity();
    temperature = ((int)(temperature * 100)) / 100.0;
    humidity = ((int)(humidity * 100)) / 100.0;
    Serial.print(dht.getStatusString());
    Serial.print("\t");
    Serial.print(temperature);
    Serial.print("\t");
    Serial.println(humidity);
    
  if (! worktemp.publish(temperature)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
  if (! workhumidity.publish(humidity)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }

    // dtostrf(temperature, 0, 1, temp2);
    // String payload = temp2;

    // client.publish("work/temp", (char*) payload.c_str(), true);

    // dtostrf(humidity, 0, 1, humidity2);
    // payload = humidity2;
    // client.publish("work/humidity", (char*) payload.c_str(), true);

  }
}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}