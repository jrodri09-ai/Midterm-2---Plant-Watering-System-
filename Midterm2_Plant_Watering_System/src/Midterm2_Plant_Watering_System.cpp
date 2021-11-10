/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "/Users/jessicamacbookpro/Documents/IoT/Midterm-2---Plant-Watering-System-/Midterm2_Plant_Watering_System/src/Midterm2_Plant_Watering_System.ino"
/*
 * Project Midterm2_Plant_Watering_System
 * Description: Smart watering system
 * Author:Jessica Rodriquez
 * Date:9-NOV-2021
 */


void setup();
void loop();
void MQTT_connect();
#line 9 "/Users/jessicamacbookpro/Documents/IoT/Midterm-2---Plant-Watering-System-/Midterm2_Plant_Watering_System/src/Midterm2_Plant_Watering_System.ino"
SYSTEM_MODE (SEMI_AUTOMATIC);

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BME280.h>
#include <Adafruit_MQTT.h>
#include "Adafruit_MQTT/Adafruit_MQTT.h" 
#include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"

#include "credentials.h"

#define OLED_RESET D4
#define BME_ADDRESS 0x76

Adafruit_SSD1306 display(OLED_RESET);
Adafruit_BME280 bme;

TCPClient TheClient; 

Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY);

Adafruit_MQTT_Publish mqttObjSoil = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/SoilMoisture");
Adafruit_MQTT_Publish mqttObjTempF = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/RoomTemp");
Adafruit_MQTT_Publish mqttObjPressure = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Pressure");
Adafruit_MQTT_Publish mqttObjHumidity = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Humidity");


/************Declare Variables*************/

bool status;
float tempC;
float tempF;
float pressPA;
float pressInHg;
float humidRH;

unsigned long last, lastTime;


void setup() {
  Serial.begin(9600);
  status = bme.begin(BME_ADDRESS);
  if (status == false) {
    Serial.printf ("BME at address 0x%02X failed to start", BME_ADDRESS);
  }
  pinMode(A1,INPUT);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000); // Pause for 2 seconds
  display.clearDisplay();
  display.drawPixel(10, 10, WHITE);
  display.display();
  delay(2000);
  display.clearDisplay();

  WiFi.connect();
  while(WiFi.connecting()) {
    Serial.printf(".");

}
}                                                         

void loop() {
  // Validate connected to MQTT Broker
  MQTT_connect();

  // Ping MQTT Broker every 2 minutes to keep connection alive
  if ((millis()-last)>120000) {
      Serial.printf("Pinging MQTT \n");
      if(! mqtt.ping()) {
        Serial.printf("Disconnecting \n");
        mqtt.disconnect();
      }
      last = millis();
  }
if((millis()-lastTime > 10000)) {
  int moistureReadings=analogRead(A1);
  Serial.printf("Moisture readings%i\n",moistureReadings);
  display.printf("Moisture readings%i\n",moistureReadings);
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.setTextColor(BLACK,WHITE); // Draw 'inverse' text
  display.display();
    if(mqtt.Update()) {
      mqttObjSoil .publish(moistureReadings);
      Serial.printf("Publishing %0.2f \n",moistureReadings); 
      } 
    lastTime = millis();
  }

  tempC = bme.readTemperature();
  tempF = (tempC * 9 / 5) + 32;
  Serial.printf (" temp %f \n", tempF);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.setTextColor(BLACK,WHITE); // Draw 'inverse' text
  display.printf ("temp \n %f \n", tempF);
  display.display();

if((millis()-lastTime > 10000)) {
if(mqtt.Update()) {
      mqttObjTempF .publish(tempF);
      Serial.printf("Publishing %0.2f \n",tempF); 
      } 
    lastTime = millis();
  }

  pressPA = bme.readPressure();
  pressInHg = (pressPA / 3386);
  Serial.printf(" pressure %f \n", pressInHg);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  //display.setCursor(0, 0);
  display.setTextColor(BLACK,WHITE); // Draw 'inverse' text
  display.printf ("pressure \n %f \n", pressInHg);
  display.display();

  if((millis()-lastTime>10000)) {
    if(mqtt.Update()) {
      mqttObjPressure .publish(pressInHg);
      Serial.printf("Publishing %0.2f \n",pressInHg); 
      } 
    lastTime = millis();
  }
 
humidRH = bme.readHumidity();
  Serial.printf("humidity %f \n", humidRH);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  //display.setCursor(0, 0);
  display.setTextColor(BLACK,WHITE); // Draw 'inverse' text
  display.printf("humidity \n %f \n", humidRH);
  display.display();

  if((millis()-lastTime>10000)) {
    if(mqtt.Update()) {
      mqttObjHumidity .publish(humidRH);
      Serial.printf("Publishing %0.2f \n",humidRH); 
      } 
    lastTime = millis();
  }
}

// Function to connect and reconnect as necessary to the MQTT server.
void MQTT_connect() {
  int8_t ret;
 
  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }
 
  Serial.print("Connecting to MQTT... ");
 
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.printf("%s\n",(char *)mqtt.connectErrorString(ret));
       Serial.printf("Retrying MQTT connection in 5 seconds..\n");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
  }
  Serial.printf("MQTT Connected!\n");
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  