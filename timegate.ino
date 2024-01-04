#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display TWI address
#define OLED_ADDR   0x3C
// OLED display width, in pixels
#define SCREEN_WIDTH 128
// OLED display height, in pixels
#define SCREEN_HEIGHT 32

// Ultrasonic Sensor Pins
#define TRIG_PIN 25
#define ECHO_PIN 26

// Potentiometer Pin
#define POT_PIN 39

// Declaration for SSD1306 display connected using software I2C (default case):
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

unsigned long startTime = 0;
unsigned long currentTime = 0;
unsigned long stoppedTime = 0;
float distance = 0;
int distanceThreshold = 30;
bool isTimerStopped = false;

void setup() {
  Serial.begin(9600);
  Serial.println(F("Starting..."));
  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.display();
  delay(2000); // Pause for 2 seconds
  display.clearDisplay();

  // Initialize Ultrasonic Sensor
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  startTime = millis(); // Initialize startTime
}

void loop() {
  Serial.println(F("Loop Start."));
  // Read potentiometer value and map it to a distance range (e.g., 10 - 100 cm)
  distanceThreshold = map(analogRead(POT_PIN), 0, 512, 5, 100);

  // Measure distance
  distance = getDistance();
  Serial.println(distance);

  // Check if distance is less than the threshold
  if (distance < distanceThreshold && !isTimerStopped) {
    // Stop timer
    stoppedTime = millis();
    isTimerStopped = true;
  }
  // Check if distance is greater than threshold and timer is stopped
  else if (distance >= distanceThreshold && isTimerStopped) {
    // Reset timer
    startTime = millis();// - (stoppedTime - startTime);
    isTimerStopped = false;
  }

  // Update current time
  if (!isTimerStopped) {
    currentTime = millis() - startTime;
  }

  // Clear the display
  display.clearDisplay();

  // Display updated time since last reset
  display.setCursor(1, 1);
  display.setTextSize(3);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  //display.setCursor(0,0);
 
  display.println(currentTime/1000.0, 3);

  // Display distance and threshold
  display.setTextSize(1); 
  display.print("Dist: ");
  display.print(distance, 0);
  display.print(" Thres: ");
  display.print(distanceThreshold, 0);
  display.println("");

  // Refresh the display
  display.display();
  //Serial.println(F(".Loop End"));

  delay(100); // Update more frequently
}

float getDistance() {
  // Ensure trigger pin is low
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Trigger the sensor by setting the trigPin high for 10 microseconds
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(ECHO_PIN, HIGH);
  //Serial.println(duration);

  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  return distance;
}
