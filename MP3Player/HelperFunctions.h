#ifndef HelperFunctions_h
#define HelperFunctions_h

#include <Arduino.h>
#include <cstdint>
#include <HijelHID_BLEKeyboard.h>
#include <mbedtls/md.h>     // SHA-256 / message digest
#include <mbedtls/base64.h> // Base64 encoding and decoding
#include <string>
#include <vector>
#include <WiFiClientSecure.h>

// Button pins
const int PLAY_PAUSE_PIN = 18;
const int VOL_DOWN_PIN = 5;
const int VOL_UP_PIN = 19;
static const int numBtnPins = 3;
// Screen pins
const int SCREEN_SDA_PIN = 23;
const int SCREEN_SCK_PIN = 22;

// Consts for spotify api
// ----- wip spotify test -----
const char* host = "api.spotify.com";
const char* token_host = "accounts.spotify.com";
const int httpsPort = 443;
// ----- wip spotify test -----

// ----- wip wifi test -----
const char* ssid     = "";     // your network SSID (name of wifi network)
const char* password = ""; // your network password
const char* client_id = "";
const char* client_secret = "";
const char*  server = "www.howsmyssl.com";  // Server URL
// ----- wip wifi test -----

HijelHID_BLEKeyboard keyboard;
WiFiClientSecure client;

class HelperFunctions
{
  public:
    HelperFunctions();
    void setPullupPins(const int pins[]);
    void checkBtnPresses();
    void pressBluetoothBtn(const int pin, uint16_t bluetoothBtn);

    String generateCodeVerifier(int length);
    std::vector<uint8_t> generateSHA256(std::string& text);
    void printBlockAsUtf8(std::vector<uint8_t> data);
    std::string encode_base64(std::vector<uint8_t>& data);
    std::vector<uint8_t> decode_base64(std::string& text);
    String getAccessToken();
};

#endif