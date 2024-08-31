#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* Define the names for ESP8266 pin for HC-SR04*/
#define trigger_pin D7 // trig from the ultrasonic sensor
#define Echo_pin    D6 // Echo form the ultrosonic sensor
#define VibPin      D5 // Dout from the vibration  sensor, pin is configures as interrups as well


// screen dimesnions
#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   64
#define SOUND_VELOCITY  0.034
#define CM_TO_INCH      0.393701
#define max_tank_height 11.15 // in cm
#define threshold       5 // in cm


float CFL;        // current fuel level
float PFL;        // pervious fuel level
bool vibration;   // check if the system have any unusual vibration
bool theftStatus; // goes high if theft is detected

// wifi Icons 
const unsigned char wifiBitmap[] PROGMEM = {
  0b00011000, 
  0b00111100,
  0b01111110,
  0b11111111,
  0b00100100, 
  0b00011000,
  0b00000000,
  0b00011000
};

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire, -1);
float currentFuelLevel(int trigPin, int EchoPin,float v, float l);


void setup() {
 //pin directions
  pinMode(trigger_pin, OUTPUT);
  pinMode(Echo_pin, INPUT);    
  pinMode(VibPin,INPUT);       
  
  // initialize the uart communication and i2c LCD
  Serial.begin(9600); 
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
    delay(1000);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.drawBitmap(SCREEN_WIDTH - 16, 0, wifiBitmap, 8, 8, WHITE);

attachInterrupt(digitalPinToInterrupt(VibPin), CheckForTheft, RISING);
}

void loop() {


display.fillRect(0, 10, 128, 16, BLACK);
display.setCursor(0,0);
display.setTextSize(1);
PFL = currentFuelLevel(trigger_pin,Echo_pin,SOUND_VELOCITY,max_tank_height);
if (PFL < 0) PFL = 0;
Serial.println(PFL);
// vibration = digitalRead(VibPin);
// Serial.println(vibration);
Serial.println(theftStatus);
 //writing to the OLED
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Fuel Level: ");
  display.setTextSize(2);
  display.setCursor(0,10);
  display.print(PFL);
  display.setTextSize(2);
  display.print("cm");
  Serial.println();
  display.display();
  delay(300);
}


ICACHE_RAM_ATTR void CheckForTheft()
{
  // This ISR check for the current fuel level and matches it with the fuel threshold to 
  // detect the potential theft
  Serial.println("From ISR");
  CFL = currentFuelLevel(trigger_pin, Echo_pin, SOUND_VELOCITY, max_tank_height);
if (CFL <= 0) CFL = 0;  // Ensure CFL doesn't go below 0
if (PFL == 0 && CFL == 0) {
    theftStatus = LOW;  // Tank is empty, no theft detected
}
else if (PFL > 0 && CFL == 0 && PFL > threshold) {
    theftStatus = HIGH;  // Significant drop to zero, likely theft
}
else if (PFL - CFL > threshold) {
    theftStatus = HIGH;  // Large decrease in fuel, likely theft
}
else if (PFL <= threshold && CFL <= threshold && CFL > 0) {
    theftStatus = LOW;  // Low fuel level, but no sudden drop, no theft
}
else {
    theftStatus = LOW;  // Default case, no theft detected
}}

float currentFuelLevel(int trigPin, int EchoPin,float v, float l){
  // INPUTs:
  //   trigPin: trigger pin attached to ultrasonic 
  //   EchoPin: Echo pin attach to  ultrasonic
  //   v: velocity of sound
  //   l: maximum height of the fuel tank
  //OUTPUTS:
  //   fuelLevel: fuel level of the tank

  float fuelLevel;
  long duration;
  digitalWrite(trigPin, LOW); //set trigger signal low for 2us
  delayMicroseconds(2);
  /*send 10 microsecond pulse to trigger pin of HC-SR04 */
  digitalWrite(trigPin, HIGH);  // make trigger pin active high
  delayMicroseconds(10);            // wait for 10 microseconds
  digitalWrite(trigPin, LOW);   // make trigger pin active low
  /*Measure the Echo output signal duration or pulss width */
  duration = pulseIn(EchoPin, HIGH); // save time duration value in "duration variable
  fuelLevel = max_tank_height -  duration * v/2;
  return fuelLevel;
}
