#include <Servo.h>
#include <Nunchuk.h>
#include <Wire.h>
#include <Arduino_LPS22HB.h>
#include <Arduino_HTS221.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int servoPin = 9;

const int nunchukMax = 1023;
const int valveMin = 0;
const int valveMax = 90;
const int sparkPin = 2;

int throttlePos;

float pressure;
float altitude;
float temperature;
float RPM;

unsigned long time;

Servo valveServo;

const int startPin = 2;
const int resetPin = 3;


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin();

  Wire.setClock(400000);

  nunchuk_init();

  pinMode(sparkPin, INPUT);

  if (!BARO.begin()) {
    Serial.println("No pressure sensor detected");
    while (1);
  }

  if (!HTS.begin()) {
    Serial.println("No temp sensor detected");
    while (1);
  }

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c)) {
      Serial.println(F("SSD1306 allocation failed"));
      for(;;);
  }

    display.display();

    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.display();
}

void loop() {
  if (nunchuk_read()) {
    throttlePos = nunchuk_joystickY_raw();
    throttlePos = map(throttlePos, 0, nunchukMax, valveMin, valveMax);
    valveServo.write(throttlePos);
  }

  // stores pressure in pascals
  pressure = BARO.readPressure(MILLIBAR);
  temperature = HTS.readTemperature() + 273.15;

  // calculate altitude in meters from pressure
  altitude = ((pow(pressure / 1013.25, 1 / 5.257) - 1) * temperature) / .0065;

  if (digitalRead(sparkPin) == HIGH) {
    // finds RPM of 2 stroke engine based on spark plug actuation
    RPM = getRPM();
  }

  writeDisplay(altitude, RPM);
}

float getRPM() {
  float ret = 1.0 / (millis() - time) * 60000;
  time = millis();
  return ret;
}

void writeDisplay(float alt, float rpm) {
  
  /**
  display.drawLine(SCREEN_WIDTH/2, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT, WHITE);  // Vertical Line
  display.drawLine(0, SCREEN_HEIGHT/2, SCREEN_WIDTH, SCREEN_HEIGHT/2, WHITE); // Horizontal Line
  display.drawLine(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE); // Primary Diagonal
  display.drawLine(SCREEN_WIDTH, 0, 0, SCREEN_HEIGHT, WHITE); // Secondary Diagonal
  **/

  display.setCursor(0, 10);
  display.println("RPM : " + String(rpm, 0));
  display.println("Altitude : " + String(alt, 2));
  display.display();
}
