#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

// MQTT library
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// DHT Setup
#include "DHTesp.h"
DHTesp dht;

// WiFi settings
const char* ssid = "ffwifi";
const char* password = "UpperMalon35";

// Variables
const int sleepTimeS = 10;
float temperature;
float oldtemperature = 0;
float humidity;
float oldhumidity = 0;

#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883
#define AIO_USERNAME "aburnsni"
#define AIO_KEY "aad58026fc614464b68e4df7f4b2a44a"

// Use WiFiClient class to create TCP connections
WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish worktemp = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/work.temp");
Adafruit_MQTT_Publish workhumidity = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/work.humidity");

void setup() {
  // Serial
  Serial.begin(115200);
  Serial.println("ESP8266 in normal mode");
  
  Serial.println("DHT setup");
  dht.setup(2); // data pin 2
  Serial.println("DHT setup complete");
  delay(5000);
  temperature = dht.getTemperature();
  humidity = dht.getHumidity();
  temperature = ((int)(temperature * 100)) / 100.0;
  humidity = ((int)(humidity * 100)) / 100.0;
  Serial.print(dht.getStatusString());
  Serial.print("\t");
  Serial.print(temperature);
  Serial.print("\t");
  Serial.println(humidity);
    

//READ DHT CHECK FOR CHANGE
  if (temperature != oldtemperature || humidity != oldhumidity) {
    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    
    // Print the IP address
    Serial.println(WiFi.localIP());

    // Logging data to cloud
    Serial.print("Connecting to ");
    Serial.println(AIO_SERVER);

//MQTT_connect();
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

//Publish readings
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

    oldtemperature = temperature;
    oldhumidity = humidity;
    
    Serial.println();
    Serial.println("closing connection");
  }

  // Sleep
  Serial.println("ESP8266 in sleep mode");
  ESP.deepSleep(sleepTimeS * 1000000);
  delay(1000);
}

void loop() {
}