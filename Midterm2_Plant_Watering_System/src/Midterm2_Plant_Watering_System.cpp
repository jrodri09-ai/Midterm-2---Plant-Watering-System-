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


// SYSTEM_MODE (SEMI_AUTOMATIC);

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BME280.h>
#include <Adafruit_MQTT.h>
#include "Adafruit_MQTT/Adafruit_MQTT.h" 
#include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"
#include "Grove_Air_quality_Sensor.h"


#include "credentials.h"

void setup();
void loop();
void startBME();
void startDisplay();
void startWifi();
void MQTT_connect();
void pingMQTT();
void printMoisture(int _moistureReadings);
void publishMoisture(int _moistureReadings);
void showBME(float _tempC);
void showPress(float _pressPA);
void showHum(float _humidRH);
void checkDust();
void checkAQ();
void printTime();
int getbuttonvalue();
void pumpOn(int moistureReadings, int buttonvalue);
void pumpOff ();
#line 22 "/Users/jessicamacbookpro/Documents/IoT/Midterm-2---Plant-Watering-System-/Midterm2_Plant_Watering_System/src/Midterm2_Plant_Watering_System.ino"
#define OLED_RESET D4
#define BME_ADDRESS 0x76
#define MOTORPIN 11
#define DUST_PIN A2

Adafruit_SSD1306 display(OLED_RESET);
Adafruit_BME280 bme;
AirQualitySensor airQuality(A3);

TCPClient TheClient; 

Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY);

Adafruit_MQTT_Publish mqttObjSoil = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/SoilMoisture");
Adafruit_MQTT_Publish mqttObjTempF = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/RoomTemp");
Adafruit_MQTT_Publish mqttObjPressure = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Pressure");
Adafruit_MQTT_Publish mqttObjHumidity = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Humidity");
Adafruit_MQTT_Publish mqttObjDust = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Dust");
Adafruit_MQTT_Publish mqttObjAQ = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/AQ");

Adafruit_MQTT_Subscribe mqttObjPumpOn = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/OnOff");

/************Declare Variables*************/
int airQualityValue, moistureReadings;

bool status;

float tempC;
float tempF;
float pressPA;
float pressInHg;
float humidRH;


unsigned long last, lastTime, startPumpTime, duration, starttime;
unsigned long sampletime_ms = 30000;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;

String DateTime, TimeOnly, DateOnly;

void setup() {
  Serial.begin(9600);
  waitFor(Serial.isConnected, 15000);

  startBME();
  airQuality.init();
  pinMode(A1,INPUT);
  pinMode(11,OUTPUT);

  startDisplay();

  startWifi();

// Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&mqttObjPumpOn);
  Time.zone(-7);
  Particle.syncTime();
}                                                         

void loop() {
  // Validate connected to MQTT Broker
  MQTT_connect();
  // Ping MQTT Broker every 2 minutes to keep connection alive
  pingMQTT();

  moistureReadings=analogRead(A1);
  if((millis()-lastTime>10000)) {
    if(mqtt.Update()) {
     printMoisture(moistureReadings);
     publishMoisture(moistureReadings);
     tempC = bme.readTemperature();
     showBME(tempC);
     pressPA = bme.readPressure();
     showPress(pressPA);
     humidRH = bme.readHumidity();
     showHum(humidRH);
     checkDust();
     checkAQ();
     printTime();
     } 
    lastTime = millis();
  }

  pumpOn(moistureReadings,getbuttonvalue());
  pumpOff();
}

void startBME() {
  status = bme.begin(BME_ADDRESS);
  if (status == false) {
    Serial.printf ("BME at address 0x%02X failed to start", BME_ADDRESS);
  }
}

void startDisplay() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
}

void startWifi() {
  WiFi.connect();
  if(WiFi.connecting()) {
    Serial.printf("Trying to connect to Wifi...\nOne moment please...\n");
  }
  while (WiFi.connecting()) {
    //waiting for wifi to connect
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

void pingMQTT() {
    // Ping MQTT Broker every 2 minutes to keep connection alive
  if ((millis()-last)>120000) {
      Serial.printf("Pinging MQTT \n");
      if(! mqtt.ping()) {
        Serial.printf("Disconnecting \n");
        mqtt.disconnect();
      }
      last = millis();
  }
}

void printMoisture(int _moistureReadings) {
  Serial.printf("Soil Moisture %i\n",_moistureReadings);
  display.setCursor(0,0);             // Start at top-left corner
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(BLACK,WHITE); // Draw 'inverse' text
  display.printf("Soil Moisture %i\n",_moistureReadings);
  display.display();
}

void publishMoisture(int _moistureReadings) {
      mqttObjSoil.publish(_moistureReadings);
      Serial.printf("Publishing Soil %i \n",_moistureReadings); 
}

void showBME(float _tempC) {
  tempF = (_tempC * 9 / 5) + 32;
  Serial.printf ("temp %f \n", tempF);
  display.printf ("temp %0.1f \n", tempF);
  display.display();

  mqttObjTempF.publish(tempF);
  Serial.printf("Publishing Temp %0.2f \n",tempF); 
}

void showPress(float _pressPA) {
  pressInHg = (_pressPA / 3386);
  Serial.printf("pressure %f \n", pressInHg);
  display.printf ("pressure %0.2f \n", pressInHg);
  display.display();
  
  mqttObjPressure.publish(pressInHg);
  Serial.printf("Publishing Press %0.2f \n",pressInHg); 
}

void showHum(float _humidRH) {
  Serial.printf("humidity %f \n", _humidRH);
  display.printf("humidity %0.1f \n", _humidRH);
  display.display();

  mqttObjHumidity.publish(_humidRH);
  Serial.printf("Publishing Hum %0.2f \n",_humidRH); 
}

void checkDust() {
  duration = pulseIn(DUST_PIN, LOW);
  lowpulseoccupancy = lowpulseoccupancy + duration;
    if ((millis() - starttime) > sampletime_ms) {
      ratio = lowpulseoccupancy / (sampletime_ms * 10.0);  // Integer percentage 0=>100
      concentration = 1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62; // using spec sheet curve
      lowpulseoccupancy = 0;
      starttime = millis();
    }
    Serial.printf("Dust Concentration: %f\n", concentration);
    mqttObjDust.publish(concentration);
    Serial.printf("Publishing Dust %0.2f \n",concentration);
    display.printf("Dust %0.3f \n", concentration);
    display.display();
}

void checkAQ() {
  airQualityValue = airQuality.getValue();
  display.printf("Air Qual %i \n", airQualityValue);
  display.display();
  mqttObjAQ.publish(airQualityValue);
  Serial.printf("Publishing Air Qual %i\n",airQualityValue);
}

void printTime() {
   TimeOnly = Time.timeStr().substring(11,16);
   DateOnly = Time.timeStr().substring(4,10);
   display.printf("Time:%s\n",TimeOnly.c_str());
   display.printf("Date:%s\n",DateOnly.c_str());
   DateTime = Time.timeStr();
   Serial.printf("Date String %s\n", DateTime.c_str());
}

int getbuttonvalue(){
  int buttonvalue = 0;
Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(1000))) {
    if (subscription == &mqttObjPumpOn) {
      buttonvalue = atoi((char *)mqttObjPumpOn.lastread);
      Serial.printf("Received %i from Adafruit.io feed OnOff \n", buttonvalue);
    }
  }
  return buttonvalue;
}


//Function to get motor turning on every half second
void pumpOn(int moistureReadings, int buttonvalue){
  Serial.printf("pump on\n");
  if(moistureReadings > 3446||buttonvalue) {
    digitalWrite(MOTORPIN,HIGH);
    startPumpTime=(millis());
  }
}

void pumpOff () { 
  Serial.printf("pump off\n");
  if (millis()-startPumpTime>2000) {
    digitalWrite(MOTORPIN, LOW);
  }
}
 