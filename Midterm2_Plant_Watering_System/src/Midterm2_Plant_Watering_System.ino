/*
 * Project Midterm2_Plant_Watering_System
 * Description: Smart watering system
 * Author:Jessica Rodriquez
 * Date:9-NOV-2021
 */


SYSTEM_MODE (SEMI_AUTOMATIC);

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BME280.h>

#define OLED_RESET D4
#define BME_ADDRESS 0x76
Adafruit_SSD1306 display(OLED_RESET);
Adafruit_BME280 bme;

bool status;
float tempC;
float tempF;
float pressPA;
float pressInHg;
float humidRH;


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

}


void loop() {
 int moistureReadings=analogRead(A1);
  Serial.printf("Moisture readings%i\n",moistureReadings);
  display.printf("Moisture readings%i\n",moistureReadings);
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.setTextColor(BLACK,WHITE); // Draw 'inverse' text
  display.display();


  tempC = bme.readTemperature();
  tempF = (tempC * 9 / 5) + 32;
  Serial.printf (" temp %f \n", tempF);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.setTextColor(BLACK,WHITE); // Draw 'inverse' text
  display.printf ("temp \n %f \n", tempF);
  display.display();

  pressPA = bme.readPressure();
  pressInHg = (pressPA / 3386);
  Serial.printf(" pressure %f \n", pressInHg);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  //display.setCursor(0, 0);
  display.setTextColor(BLACK,WHITE); // Draw 'inverse' text
  display.printf ("pressure \n %f \n", pressInHg);
  display.display();
 
humidRH = bme.readHumidity();
  Serial.printf("humidity %f \n", humidRH);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  //display.setCursor(0, 0);
  display.setTextColor(BLACK,WHITE); // Draw 'inverse' text
  display.printf("humidity \n %f \n", humidRH);
  display.display();

}