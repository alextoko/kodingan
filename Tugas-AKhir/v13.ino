#include <WiFi.h>
#include <HTTPClient.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>
#include <time.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <math.h>
#include <Preferences.h>
#include "Servo.h"
#include "WiFiManagerCustom.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_SDA 21
#define OLED_SCL 22
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

Preferences prefs;
float tempMax = -1000;
float tempMin = 1000;

WiFiManagerCustom wifiManager;

String getDateString()
{
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)) return "unknown";
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", &timeinfo);
    return String(buffer);
}

void saveMinMax()
{
    prefs.begin("tempdata", false);
    prefs.putFloat("max", tempMax);
    prefs.putFloat("min", tempMin);
    prefs.putString("date", getDateString());
    prefs.end();
    Serial.println("[EEPROM] MinMax Saved");
}

void loadMinMaxEEPROM()
{
    prefs.begin("tempdata", true);
    String savedDate = prefs.getString("date", "");
    String today = getDateString();

    if(savedDate == today)
    {
        tempMax = prefs.getFloat("max", -1000);
        tempMin = prefs.getFloat("min", 1000);
        Serial.println("[EEPROM] MinMax Restored");
    }
    else
    {
        tempMax = -1000;
        tempMin = 1000;
        Serial.println("[EEPROM] New Day Reset");
    }
    prefs.end();
}

#define API_KEY       "AIzaSyAtebbp9XVTRpJROF4mo8FDM50l7vSRJQo"
#define DATABASE_URL  "https://oil-plta-bta-default-rtdb.firebaseio.com/"

static FirebaseData fbdo;
static FirebaseAuth auth;
static FirebaseConfig config;
static bool firebaseReady = false;
static bool firebaseStarted = false;
static float pwmPercentActual = 60.0;
int lastDay = -1;

void initFirebase(float alarmDefault)
{
  if(!wifiManager.connected())
  {
    return;
  }

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (!Firebase.signUp(&config, &auth, "", ""))
  {
    Serial.printf("[Firebase] Signup failed: %s\n", config.signer.signupError.message.c_str());
    firebaseReady = false;
    return;
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  firebaseReady = true;
  firebaseStarted = true;
  Serial.println("[Firebase] Connected");

  if (!Firebase.RTDB.get(&fbdo, "/Set/alarm_on"))
  {
    Firebase.RTDB.setFloat(&fbdo, "/Set/alarm_on", alarmDefault);
  }
}

void readAlarmSetpoint(float &alarmSet)
{
  static float lastSet = -999;

  if (!firebaseReady || !Firebase.ready()) return;
  if (Firebase.RTDB.get(&fbdo, "/Set/alarm_on")) {

    float v;

    if (fbdo.dataType() == "string") {
      v = fbdo.stringData().toFloat();
    } else {
      v = fbdo.to<float>();
    }

    if (!isnan(v) && v > 0 && v < 200) {

      if (v != lastSet) {
        alarmSet = v;
        lastSet = v;
        Serial.print("🔥 SETPOINT UPDATE: ");
        Serial.println(alarmSet);
      }
    }
  }
}

void writeFloat(const String &path, float value)
{
  if (!firebaseReady || !Firebase.ready()) return;
  Firebase.RTDB.setFloat(&fbdo, path.c_str(), value);
}

void writeAlarmStatus(bool value)
{
  static bool last = false;
  if (!firebaseReady || !Firebase.ready()) return;
  if (last != value) {
    Firebase.RTDB.setBool(&fbdo, "/Status/alarm", value);
    last = value;
  }
}

void writePWM(int pwmValue)
{
  static int last = -1;
  if (!firebaseReady || !Firebase.ready()) return;
  if (last != pwmValue)
  {
    Firebase.RTDB.setInt(&fbdo, "/Status/pwm_motor", pwmValue);
    float percent = round(pwmPercentActual * 10.0) / 10.0;
    Firebase.RTDB.setFloat(&fbdo, "/Status/pwm_percent", percent);
    last = pwmValue;
  }
}

void updateDeviceIP()
{
  static String lastIP = "";
  if (!firebaseReady || !Firebase.ready()) return;
  if (!wifiManager.connected()) return;
  String ip = wifiManager.getIP();
  if (ip != lastIP)
  {
    Firebase.RTDB.setString(&fbdo, "/Device/ip", ip);
    lastIP = ip;
  }
}

void updateHeartbeat()
{
  static unsigned long lastSend = 0;
  if (!firebaseReady || !Firebase.ready()) return;
  if (millis() - lastSend > 5000) {
    time_t now = time(nullptr);
    Firebase.RTDB.setInt(&fbdo, "/Status/last_update", now * 1000);
    lastSend = millis();
  }
}

class PT100_420 {
public:

  PT100_420(float resistor,float minTemp,float maxTemp)
    : _resistor(resistor),
      _minTemp(minTemp),
      _maxTemp(maxTemp){}

  void begin(){
    ads.setGain(GAIN_ONE);
    ads.begin();
  }

  float readAll(int16_t &adc, float &voltage, float &current){
    long total = 0;

    for(int i = 0; i < 5; i++){
      total += ads.readADC_SingleEnded(0);
      delay(1);
    }

    int16_t raw = total / 5;
    float adcCal = raw * 1.0180f;
    adc = adcCal;
    voltage = adcCal * 0.125f / 1000.0f;
    current = (voltage / _resistor) * 1000.0f;

    float temp = ((current - 4.0f) / 16.0f) * (_maxTemp - _minTemp) + _minTemp;

    if(current < 4 || current > 20.5)
    {
      return NAN;
    }

    return temp;
  }

private:

  Adafruit_ADS1115 ads;

  float _resistor;
  float _minTemp;
  float _maxTemp;
};

#define MOTOR_PWM_PIN  23
#define MOTOR_PWM_FREQ 1000
#define MOTOR_PWM_RES  8
#define PWM_MAX        255
#define TEMP_MAX_VALID 100

// ============================================================
// SERVO CONTROL
// Suhu <= 30 C  -> 50 derajat
// Suhu 30-65 C  -> interpolasi linear 50-90 derajat
// Suhu >= 65 C  -> 90 derajat
// ============================================================
#define SERVO_PIN 18
#define SERVO_MIN_ANGLE 50
#define SERVO_MAX_ANGLE 90
#define SERVO_TEMP_MIN 30.0
#define SERVO_TEMP_MAX 65.0

Servo myServo;
static int servoAngle = SERVO_MIN_ANGLE;

void initServo()
{
  myServo.attach(SERVO_PIN);
  servoAngle = SERVO_MIN_ANGLE;
  myServo.write(servoAngle);

  Serial.println("[SERVO] Initialized");
  Serial.print("[SERVO] Initial angle: ");
  Serial.print(servoAngle);
  Serial.println(" degree");
}

void updateServo(float temperature)
{
    // ============================================================
    // PROTEKSI SUHU TINGGI
    // Jika suhu > 75°C, servo ke posisi 57°
    // ============================================================
    if (!isnan(temperature) && temperature > 75.0)
    {
      servoAngle = 57;

      myServo.write(servoAngle);

      Serial.println("[SERVO] Temperature > 75 C");
      Serial.println("[SERVO] Position = 57 degree");

      return;
    }

    // ============================================================
    // SENSOR INVALID
    // ============================================================
    if (isnan(temperature))
    {
      servoAngle = SERVO_MIN_ANGLE;
    }
    else if (temperature <= SERVO_TEMP_MIN)
    {
      servoAngle = SERVO_MIN_ANGLE;
    }
    else if (temperature >= SERVO_TEMP_MAX)
    {
      servoAngle = SERVO_MAX_ANGLE;
    }
    else
    {
      float angle =
        SERVO_MIN_ANGLE +
        ((temperature - SERVO_TEMP_MIN) *
        (SERVO_MAX_ANGLE - SERVO_MIN_ANGLE) /
        (SERVO_TEMP_MAX - SERVO_TEMP_MIN));

      servoAngle = round(angle);
    }

    servoAngle = constrain(
      servoAngle,
      SERVO_MIN_ANGLE,
      SERVO_MAX_ANGLE
    );

    myServo.write(servoAngle);
  }

static int currentPWM = 0;
static int targetPWM  = 0;
static float currentRPM = 0;

void initMotor()
{
  pinMode(MOTOR_PWM_PIN, OUTPUT);
  digitalWrite(MOTOR_PWM_PIN, HIGH);
  delay(50);
  ledcAttach(MOTOR_PWM_PIN, MOTOR_PWM_FREQ, MOTOR_PWM_RES);
  currentPWM = 0;
  targetPWM  = 0;
  ledcWrite(MOTOR_PWM_PIN, 255);
}

void updateMotorTarget(float tempRaw)
{
    // ============================================================
    // PROTEKSI SUHU TINGGI
    // Jika suhu > 75°C, motor DC OFF
    // ============================================================
    if (!isnan(tempRaw) && tempRaw > 75.0)
    {
      pwmPercentActual = 0.0;
      targetPWM = 0;

      Serial.println("WARNING: Temperature > 75 C");
      Serial.println("[MOTOR] OFF - Over Temperature");

      return;
    }

    // ============================================================
    // SENSOR INVALID
    // ============================================================
    if(isnan(tempRaw) || tempRaw <= 0 || tempRaw > TEMP_MAX_VALID)
    {
      Serial.println("WARNING: Sensor invalid!");

      pwmPercentActual = 60.0;
      targetPWM = round((60.0 / 100.0) * PWM_MAX);
      targetPWM = constrain(targetPWM, 0, PWM_MAX);

      return;
    }

    float t = tempRaw;

    float tempMin = 30.0;
    float tempMax = 65.0;

    float pwmMin = 60.0;
    float pwmMax = 100.0;

    float pwmPercent;

    if(t <= tempMin)
    {
      pwmPercent = pwmMin;
    }
    else if(t >= tempMax)
    {
      pwmPercent = pwmMax;
    }
    else
    {
      pwmPercent =
        pwmMin +
        ((t - tempMin) *
        (pwmMax - pwmMin) /
        (tempMax - tempMin));
    }

    pwmPercentActual = pwmPercent;

    targetPWM = round((pwmPercent / 100.0) * PWM_MAX);
    targetPWM = constrain(targetPWM, 0, PWM_MAX);
  }

void motorPWMUpdate()
{
  currentPWM = targetPWM;
  currentPWM = constrain(currentPWM, 0, PWM_MAX);
  uint8_t pwmOut = PWM_MAX - currentPWM;
  ledcWrite(MOTOR_PWM_PIN, pwmOut);
  currentRPM = (pwmPercentActual / 100.0) * 1430.0;
}

void writeRPM(float rpmValue)
{
  static int lastRPM = -1;
  int rpmRounded = round(rpmValue);

  if (!firebaseReady || !Firebase.ready()) return;
  if (rpmRounded != lastRPM)
  {
    Firebase.RTDB.setInt( &fbdo, "/Status/rpm_motor", rpmRounded);
    lastRPM = rpmRounded;
  }
}

int getMotorPWM()
{
  return currentPWM;
}

#define ALARM_PIN 19
static bool alarmState = false;

void initAlarm()
{
  pinMode(ALARM_PIN, OUTPUT);
  digitalWrite(ALARM_PIN, LOW);
}

void updateAlarm(float temp, float alarmSet)
{
  static unsigned long lastTime = 0;
  static uint8_t step = 0;
  const uint16_t BEEP_ON  = 100;
  const uint16_t BEEP_OFF = 300;
  const uint16_t PAUSE    = 200;

  if(!isnan(temp) && (temp < 0 || temp > 150))
  {
    alarmState = false;
    digitalWrite(ALARM_PIN, LOW);
    lastTime = millis();
    step = 0;
    return;
  }

  alarmState = (isnan(temp) || temp <= 0.5 || temp >= alarmSet);

  if(!alarmState)
  {
    digitalWrite(ALARM_PIN, LOW);
    step = 0;
    return;
  }

  unsigned long now = millis();
  switch(step)
  {
    case 0:
      digitalWrite(ALARM_PIN, HIGH);
      if(now - lastTime >= BEEP_ON){ lastTime = now; step = 1; }
      break;

    case 1:
      digitalWrite(ALARM_PIN, LOW);
      if(now - lastTime >= BEEP_OFF){ lastTime = now; step = 2; }
      break;

    case 2:
      digitalWrite(ALARM_PIN, HIGH);
      if(now - lastTime >= BEEP_ON){ lastTime = now; step = 3; }
      break;

    case 3:
      digitalWrite(ALARM_PIN, LOW);
      if(now - lastTime >= BEEP_OFF){ lastTime = now; step = 4; }
      break;

    case 4:
      digitalWrite(ALARM_PIN, HIGH);
      if(now - lastTime >= BEEP_ON){ lastTime = now; step = 5; }
      break;

    case 5:
      digitalWrite(ALARM_PIN, LOW);
      if(now - lastTime >= PAUSE){ lastTime = now; step = 0; }
      break;
  }
}

#define FB_INTERVAL     100
#define SENSOR_INTERVAL 100
#define PRINT_INTERVAL  100
#define SEND_DELTA_TEMP 0.1

PT100_420 sensor(165.0, 0.0, 100.0);
float tempCurrent = 0;
float tempSum = 0;
float tempAvg = 0;
float alarmSet = 65.0;
unsigned long sampleCount = 0;
unsigned long lastSensor = 0;
unsigned long lastFirebase = 0;
unsigned long lastPrint = 0;
const char* SCRIPT_URL =
"https://script.google.com/macros/s/AKfycbz4entNHRnsf8rgvCbZUx00GVo2W-5X7HmNqUvF8AMXOmT1z25z1Omj_R9UMgmB2OO3vQ/exec";

float round1(float v){
  return round(v * 10.0) / 10.0;
}

float round3(float v){
  return round(v * 1000.0) / 1000.0;
}

int16_t adcVal = 0;
float voltageVal = 0;
float currentVal = 0;

void sendToSheetIfChanged(float temperature)
{
  if(!wifiManager.connected()) return;
  static float lastTemp = -9999;
  static unsigned long lastSend = 0;
  unsigned long now = millis();
  bool waktu = (now - lastSend >= 60000);
  bool perubahan = abs(temperature - lastTemp) >= SEND_DELTA_TEMP;
  if (!(waktu || perubahan)) return;
  HTTPClient http;
  http.begin(SCRIPT_URL);
  http.addHeader("Content-Type","application/json");
  String payload = "{\"temperature\":" + String(temperature,1) + ",\"alarm\":" + String(alarmSet,1) + "}";
  http.POST(payload);
  http.end();
  lastTemp = temperature;
  lastSend = now;
}

void setup(){
  Serial.begin(115200);
  initMotor();
  initServo();
  initAlarm();

  // WiFi dikelola sepenuhnya oleh WiFiManagerCustom.
  // OTA sengaja tidak digunakan di v13.ino.
  wifiManager.begin("ESP32-IoT");

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  Serial.println("NTP started...");
  delay(2000);

  tempMax = -1000;
  tempMin = 1000;

  initFirebase(alarmSet);
  sensor.begin();

  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR))
  {
      Serial.println("OLED gagal ditemukan!");
  }
  else
  {
      Serial.println("OLED OK");

      display.clearDisplay();
      display.setTextColor(SSD1306_WHITE);
      display.setTextSize(1);
      display.setCursor(20, 0);
      display.println("MONITORING OLI");
      display.display();
  }

}

void updateOLED()
{
    display.clearDisplay();

    display.setTextColor(SSD1306_WHITE);

    // Judul
    display.setTextSize(1);
    display.setCursor(20, 0);
    display.println("LOKAL MONITORING");

    display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

    // Suhu
    display.setCursor(0, 16);
    display.print("Suhu   : ");

    if (isnan(tempCurrent))
    {
        display.println("0");
    }
    else
    {
        display.print(tempCurrent, 1);
        display.println(" C");
    }

    // PWM
    display.setCursor(0, 29);
    display.print("PWM    : ");
    display.print(pwmPercentActual, 1);
    display.println(" %");

    // Speed
    display.setCursor(0, 42);
    display.print("Speed  : ");
    display.print(currentRPM, 0);
    display.println(" RPM");

    // Servo
    display.setCursor(0, 55);
    display.print("Servo  : ");
    display.print(servoAngle);
    display.println((char)247);

    display.display();
}

void loop(){
  wifiManager.loop();

  if(wifiManager.connected() && !firebaseStarted)
  {
    initFirebase(alarmSet);
  }

  updateHeartbeat();
  struct tm timeinfo;

  if(getLocalTime(&timeinfo))
  {
    if(timeinfo.tm_mday != lastDay)
    {
        lastDay = timeinfo.tm_mday;
        tempMax = -1000;
        tempMin = 1000;
        saveMinMax();
        Serial.println("[SYSTEM] Daily MinMax Reset");
    }
  }

  updateDeviceIP();
  readAlarmSetpoint(alarmSet);

  if(millis() - lastSensor >= SENSOR_INTERVAL){
    lastSensor = millis();
    tempCurrent = sensor.readAll( adcVal, voltageVal, currentVal);

    if(tempCurrent > 0 && tempCurrent < 100)
    {
      if(tempMax == -1000)
        tempMax = tempCurrent;

      if(tempMin == 1000)
        tempMin = tempCurrent;

      if(tempCurrent > tempMax)
        tempMax = tempCurrent;

      if(tempCurrent < tempMin)
        tempMin = tempCurrent;
    }
  }

  tempSum += tempCurrent;
  sampleCount++;
  tempAvg = round1(tempSum / sampleCount);

  updateAlarm(tempCurrent, alarmSet);
  updateMotorTarget(tempCurrent);
  motorPWMUpdate();
  updateServo(tempCurrent);
  int pwmNow = getMotorPWM();
  updateOLED();

  if(millis() - lastFirebase >= FB_INTERVAL){
    lastFirebase = millis();

    if (isnan(tempCurrent))
    {
        writeFloat("/Temperature/current", 0.0);
    }
    else
    {
        writeFloat("/Temperature/current", round1(tempCurrent));
    }

    writeFloat("/Temperature/maximal", round1(tempMax));
    if(tempMin > 0)
    {
      writeFloat("/Temperature/minimal", round1(tempMin));
    }

    writeAlarmStatus(alarmState);
    writePWM(pwmNow);
    writeRPM(currentRPM);
    sendToSheetIfChanged(tempCurrent);
  }

  static float lastTempPrint = -999;
  static int   lastPWMPrint  = -999;
  static int16_t lastADCPrint = -999;

  if(millis() - lastPrint >= PRINT_INTERVAL)
  {
    lastPrint = millis();
    bool tempChanged = abs(tempCurrent - lastTempPrint) >= 0.1;
    bool pwmChanged  = pwmNow != lastPWMPrint;
    bool adcChanged  = abs(adcVal - lastADCPrint) >= 5;

    if(tempChanged || pwmChanged || adcChanged)
    {
      Serial.println();
      Serial.println("==========================================");
      Serial.println("        MONITORING SUHU OLI PLTA");
      Serial.println("==========================================");

      // Arus
      Serial.print("Arus        : ");
      Serial.print(currentVal, 2);
      Serial.println(" mA");

      // Tegangan
      Serial.print("Tegangan    : ");
      Serial.print(round3(voltageVal), 3);
      Serial.println(" V");

      // ADC
      Serial.print("ADC         : ");
      Serial.println(adcVal);

      // Temperatur
      Serial.print("Temperatur  : ");

      if (isnan(tempCurrent))
      {
          Serial.println("0.0 °C");
      }
      else
      {
          Serial.print(round1(tempCurrent), 1);
          Serial.println(" °C");
      }

      // Suhu Minimum
      Serial.print("Suhu Min    : ");

      if (tempMin > 0 && tempMin < 100)
      {
          Serial.print(round1(tempMin), 1);
          Serial.println(" °C");
      }
      else
      {
          Serial.println("-");
      }

      // Suhu Maksimum
      Serial.print("Suhu Max    : ");

      if (tempMax > 0 && tempMax < 100)
      {
          Serial.print(round1(tempMax), 1);
          Serial.println(" °C");
      }
      else
      {
          Serial.println("-");
      }

      // Setpoint
      Serial.print("Setpoint    : ");
      Serial.print(round1(alarmSet), 1);
      Serial.println(" °C");

      // PWM
      Serial.print("PWM Motor   : ");
      Serial.print(pwmNow);
      Serial.print(" (");
      Serial.print(round(pwmPercentActual * 10.0) / 10.0, 1);
      Serial.println("%)");

      // Speed
      Serial.print("Speed       : ");
      Serial.print(round(currentRPM));
      Serial.println(" RPM");

      // Servo
      Serial.print("Servo       : ");
      Serial.print(servoAngle);
      Serial.println(" degree");

      // Alarm
      Serial.print("Alarm       : ");

      if (alarmState)
      {
          Serial.println("ON");
      }
      else
      {
          Serial.println("OFF");
      }

      // WiFi
      Serial.print("WiFi        : ");

      if (wifiManager.connected())
      {
          Serial.println("CONNECTED");
      }
      else
      {
          Serial.println("DISCONNECTED");
      }

      // IP
      Serial.print("IP          : ");

      if (wifiManager.connected())
      {
          Serial.println(wifiManager.getIP());
      }
      else
      {
          Serial.println("-");
      }

      Serial.println("==========================================");

      lastTempPrint = tempCurrent;
      lastPWMPrint  = pwmNow;
      lastADCPrint  = adcVal;
    }
  }
}
