/*
  Voltmeter with INA219 and 128x32 OLED
  Displays bus voltage (0-5V) and current (0-500mA)
  I2C pins: SDA=GP4, SCL=GP5
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_INA219.h>

// OLED settings (128x32)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// INA219 sensor
Adafruit_INA219 ina219;
// For voltage, the INA219 is factory calibrated; only adjust if readings are consistently off
const float VOLTAGE_CAL_FACTOR = 1.0;
// Optional current calibration (if you want to show current)
const float CURRENT_CAL_FACTOR = 1.0;

// Variables
float voltage_V = 0;
float current_mA = 0;
float power_mW = 0;

// ----------------------------------------------------------------------
// Initialize INA219
// ----------------------------------------------------------------------
void initINA219() {
  if (!ina219.begin()) {
    Serial.println("INA219 not found!");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("INA219 ERROR!");
    display.display();
    while (1);
  }
  // Use the 32V/2A calibration (covers your voltage and current ranges)
  ina219.setCalibration_32V_2A();
  Serial.println("INA219 ready");
}

// ----------------------------------------------------------------------
// Read measurements
// ----------------------------------------------------------------------
void readMeasurements() {
  voltage_V = ina219.getBusVoltage_V() * VOLTAGE_CAL_FACTOR;
  current_mA = ina219.getCurrent_mA() * CURRENT_CAL_FACTOR;
  power_mW = ina219.getPower_mW() * CURRENT_CAL_FACTOR;  // power uses current calibration
}

// ----------------------------------------------------------------------
// Update OLED display (128x32 layout)
// ----------------------------------------------------------------------
void updateDisplay() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // Title
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Voltmeter");

  // Voltage (large font)
  display.setTextSize(1);
  display.setCursor(0, 12);
  display.print(voltage_V, 3);
  display.print(" V");

  // Current (small font, bottom line)
  display.setTextSize(1);
  display.setCursor(0, 23);
  display.print(current_mA, 1);
  display.print(" mA");

  // Optional: show calibration factor if not 1.0
  if (abs(VOLTAGE_CAL_FACTOR - 1.0) > 0.001) {
    display.setCursor(80, 26);
    display.print("Vcal:");
    display.print(VOLTAGE_CAL_FACTOR, 2);
  }

  display.display();
}

// ----------------------------------------------------------------------
// Setup
// ----------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  Serial.println("Starting Voltmeter...");

  // I2C pins: GP4 = SDA, GP5 = SCL
  Wire.setSDA(4);
  Wire.setSCL(5);
  Wire.begin();

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED init failed");
    while (1);
  }
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Voltmeter");
  display.println("Init...");
  display.display();
  delay(1000);

  // Initialize INA219
  initINA219();

  // Show ready message
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Ready");
  display.display();
  delay(1000);
}

// ----------------------------------------------------------------------
// Main loop
// ----------------------------------------------------------------------
void loop() {
  readMeasurements();
  updateDisplay();

  // Print to serial for debugging
  Serial.print("V: "); Serial.print(voltage_V, 3);
  Serial.print(" V, I: "); Serial.print(current_mA, 2);
  Serial.print(" mA, P: "); Serial.print(power_mW, 2);
  Serial.println(" mW");

  delay(500); // update twice per second
}