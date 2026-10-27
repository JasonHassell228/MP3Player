#ifndef HelperFunctions_h
#define HelperFunctions_h

#include <Arduino.h>
#include <vector>
#include <string>
#include <cstdint>

class HelperFunctions
{
  public:
    const int PLAY_PAUSE_PIN = 18;
    const int VOL_DOWN_PIN = 5;
    const int VOL_UP_PIN = 19;
    const int numBtnPins = 3;

    const int SCREEN_SDA_PIN = 23;
    const int SCREEN_SCK_PIN = 22;

    const int ALL_BTN_PINS[numBtnPins] = {
        PLAY_PAUSE_PIN,
        VOL_DOWN_PIN,
        VOL_UP_PIN
    };

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