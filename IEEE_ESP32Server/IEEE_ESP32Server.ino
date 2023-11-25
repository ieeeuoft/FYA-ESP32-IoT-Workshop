/*
  ESP-NOW Demo - Receive
  esp-now-demo-rcv.ino
  Reads data from Initiator
  
  DroneBot Workshop 2022
  https://dronebotworkshop.com
*/

// Include Libraries
#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

#ifndef _BV
  #define _BV(bit) (1<<(bit))
#endif

Adafruit_LEDBackpack matrix = Adafruit_LEDBackpack();

uint16_t delayTime = 1000;
uint64_t binaryAnd = 0b11111111;

uint64_t gameover[8] = {0x00,0x40,0x40,0x40,0x40,0x40,0x7e,0x00}; //green
uint64_t xmark[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00}; //red
uint64_t obstacles[8] = {0x83,0x19,0x00,0x60,0x26,0x06,0xc0,0xc1}; //yellow
// Define a data structure
typedef struct struct_message {
  char a[32];
  int b;
  float c;
  bool d;
} struct_message;

int rep = 0;
// Create a structured object
boolean dead = false;
struct_message myData;
void shiftArrayByOne(uint64_t arr[]) {
    // Save the last element
    uint64_t lastElement = arr[8- 1];

    // Shift elements to the right
    for (int i = 8 - 1; i > 0; --i) {
        arr[i] = arr[i - 1];
    }

    // Place the last element at the beginning
    arr[0] = lastElement;
}

void emptySpaceship(void) {
  for(int i=0; i < 8; i++){
    matrix.displaybuffer[i] = xmark[i];   //red portion
    //binaryAnd = binaryAnd << 8;
  }
}
void clearMatrix(void) {
    for(int c=0; c < 8; c++){
        matrix.displaybuffer[c] = 0;
    } 
}
// Callback function executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Data received: ");
  Serial.println(len);
  Serial.print("Character Value: ");
  Serial.println(myData.a);
  Serial.print("Integer Value: ");
  Serial.println(myData.b);
  Serial.print("Float Value: ");
  Serial.println(myData.c);
  Serial.print("Boolean Value: ");
  Serial.println(myData.d);
  Serial.println();
  if (dead) {
    return;
  }
  if (myData.b > 3000) {
    if (xmark[6] != 0x80) {
      xmark[6] <<= 1;
      matrix.displaybuffer[6] = (xmark[6] << 8) | obstacles[6]; 
      matrix.writeDisplay();
    } 
  } else if (myData.b < 1000) {
    if (xmark[6] != 0x01){
      xmark[6] >>= 1;
      matrix.displaybuffer[6] = (xmark[6] << 8) | obstacles[6]; 
      matrix.writeDisplay();
    }
  }
  
  if ((obstacles[6] & xmark[6]) > 0) {
    dead = true;
  }
}

void setup() {
  // Set up Serial Monitor
  Serial.begin(115200);
  matrix.begin(0x70);  // pass in the address
  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  emptySpaceship();
    
  binaryAnd = 0b11111111;
  matrix.writeDisplay();
  // Register callback function
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {
  shiftArrayByOne(obstacles);
  for(int i=0; i < 8; i++){
      matrix.displaybuffer[i] = obstacles[i];   //red portion
      //binaryAnd = binaryAnd << 8;   
  }
  if (dead){
    for(int i = 0; i < 8; i++) {
      matrix.displaybuffer[i] = gameover[i];
     }
     matrix.writeDisplay();
     delay(100000);
  }
  matrix.displaybuffer[6] |= (xmark[6] << 8);
  matrix.writeDisplay();
  delay(1000);
}
