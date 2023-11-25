/*
  ESP-NOW Demo - Transmit
  esp-now-demo-xmit.ino
  Sends data to Responder
  
  DroneBot Workshop 2022
  https://dronebotworkshop.com
*/

// Include Libraries
#include <esp_now.h>
#include <WiFi.h>

// read from joystick
int y_value = 0;
int x_value = 0;

// Variables for test data
int int_value;
float float_value;
bool bool_value = true;

int joyValue = 0;

// MAC Address of responder - edit as required
uint8_t broadcastAddress[] = {0x24, 0x6F, 0x28, 0xB2, 0x56, 0x7C};

// Define a data structure
typedef struct struct_message {
  char a[32];
  int b;
  float c;
  bool d;
} struct_message;

// Create a structured object
struct_message myData;

// Peer info
esp_now_peer_info_t peerInfo;

// Callback function called when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  
  // Set up Serial Monitor
  Serial.begin(115200);
 
  // Set ESP32 as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Initilize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the send callback
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {

  // read joystick input
  x_value = analogRead(36);
  y_value = analogRead(39);

  // Invert the boolean value
  bool_value = !bool_value;
  
  // Format structured data
  strcpy(myData.a, "Welcome to the Workshop!");
  myData.b = x_value;
  myData.c = (float) y_value / 4095.0f;
  myData.d = bool_value;
  
  // Send message via ESP-NOW
  if(x_value > 3000 || x_value < 1000) {
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
    if (result == ESP_OK) {
      Serial.println("Sending confirmed");
    }
    else {
      Serial.println("Sending error");
    }
  }
  
  delay(200);
}
