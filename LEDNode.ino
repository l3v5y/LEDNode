/// Simple RGB LED recieve node for HouseMon
/// 2013-08-25 Olly Levett olevett@gmail.com 
/// http://opensource.org/licenses/mit-license.php

#include <JeeLib.h>
#include <EEPROM.h>

// #define DEBUG

#define NODE_ID 2
#define NODE_BAND RF12_868MHZ
#define NODE_GROUP 100

#define LED_R 9   // the PWM pin which drives the red LED
#define LED_G 6   // the PWM pin which drives the green LED
#define LED_B 5   // the PWM pin which drives the blue LED
#define FADE_TIME 20

struct PinValueSet {
  byte Pin;
  byte CurrentValue;
  byte TargetValue;
};

PinValueSet rgbPins[] = {
  {
    LED_R, 0, 0  }
  ,
  {
    LED_G, 0, 0  }
  ,
  {
    LED_B, 0, 0  }
};

byte calculateNewValue(struct PinValueSet values) {
  if(values.CurrentValue == values.TargetValue) {
    return values.CurrentValue;
  }
  int stepSize = values.CurrentValue > values.TargetValue ? -1 : 1;

  if(abs(values.CurrentValue - values.TargetValue)<abs(stepSize))
  {
    return values.TargetValue;
  }
  return  values.CurrentValue + stepSize;
}

void setup() {
  bitSet(TCCR1B, WGM12);

  // intialize wireless
  rf12_initialize(NODE_ID, NODE_BAND, NODE_GROUP);
#ifdef DEBUG
  Serial.begin(57600);
#endif

  for (int i = 0; i < 3; i++) {
    rgbPins[i].TargetValue = EEPROM.read(i);
  }
}

void loop() {
  static unsigned long lastFadeTime;
  if (rf12_recvDone() && rf12_crc == 0) {
#ifdef DEBUG
    Serial.println("Packet recieved");
#endif
    if (rf12_len == 3) {
      for (int i = 0; i < 3; i++) {
        EEPROM.write(i, rf12_data[i]);
        rgbPins[i].TargetValue = rf12_data[i];
      }
#ifdef DEBUG
      Serial.print("New values R: ");
      Serial.print(rgbPins[0].TargetValue);
      Serial.print(", G: ");
      Serial.print(rgbPins[1].TargetValue);
      Serial.print(", B: ");
      Serial.println(rgbPins[2].TargetValue);
#endif
    }
  }

  bool valuesChanged = false;
  for(int i = 0; i < 3; i++) {
    valuesChanged =  valuesChanged || rgbPins[i].CurrentValue != rgbPins[i].TargetValue;
  }

  if(valuesChanged && (millis() - lastFadeTime > FADE_TIME)) {
    lastFadeTime = millis();
    for(int i = 0; i < 3; i++) {
      rgbPins[i].CurrentValue = calculateNewValue(rgbPins[i]);
#ifdef DEBUG
      Serial.print("Current Values R: ");
      Serial.print(rgbPins[0].CurrentValue);
      Serial.print(", G: ");
      Serial.print(rgbPins[1].CurrentValue);
      Serial.print(", B: ");
      Serial.println(rgbPins[2].CurrentValue);
#endif
    }
  }

  for(int i = 0; i < 3; i++) {
    analogWrite(rgbPins[i].Pin, rgbPins[i].CurrentValue);
  }
}

