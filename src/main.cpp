#include <Arduino.h>

// WiFi
#include <ESP8266WiFi.h>

// Firebase
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// Network Credentials
#define WIFI_SSID "pldtwifi"
#define WIFI_PASSWORD "angelonikhailaguinaldo06"

// Firebase Credentials
#define API_KEY "AIzaSyAkQoMFrEyq77PH7hJ-SUFTm7FgJ7Pnj_g"
#define DATABASE_URL "https://jeeps-bdf97-default-rtdb.asia-southeast1.firebasedatabase.app/"

// Firebase Data
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
// int intValue;
// float floatValue;
bool signupOK = false;

const int IR1 = D8;
const int IR2 = D7;

int passenger = 0;
boolean ACTIVE = LOW;
boolean INACTIVE = HIGH;

void sendToFirebase(int count);

void setup()
{
  // Initialize Serial Monitor
  Serial.begin(9600);

  // ------ WiFi ----- //

  // Connecting to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }

  // Printing for WiFi Connection
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // ------ Firebase ----- //

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", ""))
  {
    Serial.println("ok");
    signupOK = true;
  }
  else
  {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  // Firebase Connection
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // ------ Hardware ----- //

  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
}

void loop()
{
  int state1 = digitalRead(IR1);
  int state2 = digitalRead(IR2);

  int delay_loop = 10;

  if ((state1 == ACTIVE) && (state2 == INACTIVE))
  {
    while (delay_loop-- && state2 == INACTIVE)
    {
      state2 = digitalRead(IR2);
      delay(50);
    }
    if (state2 == ACTIVE)
    {
      passenger += 1;
      Serial.println(passenger);
      sendToFirebase(passenger);
    }
  }
  else if ((state1 == INACTIVE) && (state2 == ACTIVE))
  {
    while (delay_loop-- && state1 == INACTIVE)
    {
      state1 = digitalRead(IR1);
      delay(50);
    }
    if (state1 == ACTIVE)
    {
      passenger -= 1;
      Serial.println(passenger);
      sendToFirebase(passenger);
    }
  }
}

void sendToFirebase(int count)
{
  if (Firebase.RTDB.setInt(&fbdo, "/passenger_count", count))
  {
    Serial.println("Passenger count updated successfully.");
  }
  else
  {
    Serial.print("Error updating passenger count: ");
    Serial.println(fbdo.errorReason());
  }
}