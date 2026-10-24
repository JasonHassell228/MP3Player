#ifndef HelperFunctions_h
#define HelperFunctions_h

#include <Arduino.h>
#include <vector>
#include <string>
#include <cstdint>

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