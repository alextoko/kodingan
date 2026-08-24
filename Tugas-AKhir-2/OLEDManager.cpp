#include "OLEDManager.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_SDA 21
#define OLED_SCL 22
#define OLED_ADDR 0x3C
static Adafruit_SSD1306 display(SCREEN_WIDTH,SCREEN_HEIGHT,&Wire,-1);
void initOLED(){ Wire.begin(OLED_SDA,OLED_SCL); if(!display.begin(SSD1306_SWITCHCAPVCC,OLED_ADDR)){Serial.println("OLED gagal ditemukan!");return;} Serial.println("OLED OK"); display.clearDisplay(); display.setTextColor(SSD1306_WHITE); display.setTextSize(1); display.setCursor(20,0); display.println("MONITORING OLI"); display.display(); }
void updateOLED(float temperature,float pwmPercent,float rpm,int servoAngle){ display.clearDisplay(); display.setTextColor(SSD1306_WHITE); display.setTextSize(1); display.setCursor(20,0); display.println("LOKAL MONITORING"); display.drawLine(0,10,127,10,SSD1306_WHITE); display.setCursor(0,16); display.print("Suhu   : "); if(isnan(temperature)) display.println("0"); else {display.print(temperature,1);display.println(" C");} display.setCursor(0,29);display.print("PWM    : ");display.print(pwmPercent,1);display.println(" %"); display.setCursor(0,42);display.print("Speed  : ");display.print(rpm,0);display.println(" RPM"); display.setCursor(0,55);display.print("Servo  : ");display.print(servoAngle);display.println((char)247);display.display(); }
