/// Simple RGB LED recieve node for HouseMon
/// 2013-08-25 Olly Levett olevett@gmail.com 
/// http://opensource.org/licenses/mit-license.php

#include <JeeLib.h>

// #define DEBUG

#define NODE_ID 2
#define NODE_BAND RF12_868MHZ
#define NODE_GROUP 100

#define LED_R 6   // the PWM pin which drives the red LED
#define LED_G 5   // the PWM pin which drives the green LED
#define LED_B 9   // the PWM pin which drives the blue LED

void setup() {
  bitSet(TCCR1B, WGM12);
 
  // intialize wireless
  rf12_initialize(NODE_ID, NODE_BAND, NODE_GROUP);
  #ifdef DEBUG
  Serial.begin(57600);
  #endif
}

void loop() {  
  if (rf12_recvDone() && rf12_crc == 0) {
    Serial.println("Packet recieved");
    const byte* p = (const byte*) rf12_data;
    if (rf12_len == 3) {
      analogWrite(LED_R, (byte)rf12_data[0]);
      analogWrite(LED_G, (byte)rf12_data[1]);
      analogWrite(LED_B, (byte)rf12_data[2]);
      #ifdef DEBUG
      Serial.print("R: ");
      Serial.print(rf12_data[0]);
      Serial.print(", G: ");
      Serial.print(rf12_data[1]);
      Serial.print(", B: ");
      Serial.println(rf12_data[2]);
      #endif
    }
  }
  delay(100);
}
