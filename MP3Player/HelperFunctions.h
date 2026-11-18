#ifndef HelperFunctions_h
#define HelperFunctions_h

#include <ArduinoJson.h>
#include <HijelHID_BLEKeyboard.h>
#include <mbedtls/base64.h> // Base64 encoding and decoding
#include <mbedtls/md.h>     // SHA-256 / message digest
#include "SSD1306Wire.h"
#include <vector>
#include <WebServer.h>
#include <WiFiClientSecure.h>
#include <WiFi.h>
#include <Wire.h>

class HelperFunctions
{
  public:
    // Button pins
    const int PLAY_PAUSE_PIN = 18;
    const int VOL_DOWN_PIN = 5;
    const int VOL_UP_PIN = 19;
    static const int numBtnPins = 3;
    const int ALL_BTN_PINS[numBtnPins] = {
      PLAY_PAUSE_PIN,
      VOL_DOWN_PIN,
      VOL_UP_PIN
    };

    // Screen pins
    const int SCREEN_SDA_PIN = 23;
    const int SCREEN_SCK_PIN = 22;

    const char* ssid     =      "";     // your network SSID (name of wifi network)
    const char* password =      ""; // your network password
    const char* client_id =     "";
    const char* spotify_redirect_uri = "http://127.0.0.1:8080";
    String authorizationCode = "";
    String codeChallenge = "";
    String codeVerifier = "";

    HijelHID_BLEKeyboard keyboard;  // Bluetooth keyboard
    WiFiClientSecure client;        // Wifi client
    WebServer server;               // Create web server

    HelperFunctions();

    // Hardware functions
    void setPullupPins();
    void checkBtnPresses();
    void pressBluetoothBtn(const int pin, uint16_t bluetoothBtn);

    // Spotify functions
    std::vector<uint8_t> generateSHA256(String& text);
    String generateSpotifyAuthURL(const String& codeChallenge);
    String encode_base64(std::vector<uint8_t>& data);
    void handleRoot();
    void generateCodeVerifier(int length);
    void generateCodeChallenge();

    // General helper functions
    void setupWifi();
};

#endif