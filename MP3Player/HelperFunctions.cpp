#include "HelperFunctions.h"

// ----- HARDWARE HELPER FUNCTIONS -----

// TODO Cleanup, test, and remove unneeded/test code \/\/\\/\/

void HelperFunctions::setPullupPins(const int pins[])
{
    for (int i = 0; i < numBtnPins; i++)
    {
        pinMode(pins[i], INPUT_PULLUP);
    }
}

// Checks all physical button presses and runs the corresponding action
void HelperFunctions::checkBtnPresses()
{
    pressBluetoothBtn(VOL_UP_PIN, MEDIA_VOLUME_UP);
    pressBluetoothBtn(VOL_DOWN_PIN, MEDIA_VOLUME_DOWN);
    pressBluetoothBtn(PLAY_PAUSE_PIN, MEDIA_PLAY_PAUSE);
}

// Check if physical btn pressed, if yes press corresponding bluetooth btn
void HelperFunctions::pressBluetoothBtn(const int pin, uint16_t bluetoothBtn)
{
    if (digitalRead(pin) == LOW) {
        keyboard.tap(bluetoothBtn, 25, 50);
        keyboard.releaseAll();
    }
}

// ----- SPOTIFY API HELPER FUNCTIONS -----

// ----- wip spotify test -----
String HelperFunctions::generateCodeVerifier(int length) {
    String text = "";
    String possible = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    for (int i = 0; i < length; i++) {
        text += possible[random(0, possible.length())];
    }
    return text;
}

std::vector<uint8_t> HelperFunctions::generateSHA256(std::string& text)
{
	std::vector<uint8_t> hash(32);

	mbedtls_md_context_t ctx;
	mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;

	mbedtls_md_init(&ctx);
	mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
	mbedtls_md_starts(&ctx);
	mbedtls_md_update(&ctx, (const unsigned char*)text.c_str(), text.length());
	mbedtls_md_finish(&ctx, hash.data());
	mbedtls_md_free(&ctx);

	return hash;
}

void HelperFunctions::printBlockAsUtf8(std::vector<uint8_t> data)
{
	for(int i = 0; i < data.size(); i++)
	{
		Serial.print((char)data[i]);
	}
}

std::string replaceBase64UrlChars(std::vector<unsigned char>& replaceString, int encodedLen)
{
    size_t actualLen = 0;
    for (size_t i = 0; i < encodedLen; i++) {
        if (replaceString[i] == '+') {
            replaceString[actualLen++] = '-';
        } else if (replaceString[i] == '/') {
            replaceString[actualLen++] = '_';
        } else if (replaceString[i] == '=') { // '=' is a padding char in base64 encoding
            // Stop at padding characters
            break;
        } else {
            replaceString[actualLen++] = replaceString[i];
        }
    }
    replaceString.resize(actualLen);
    return std::string(replaceString.begin(), replaceString.end());
}

std::string HelperFunctions::encode_base64(std::vector<uint8_t>& data)
{
	size_t outputLength;
	mbedtls_base64_encode(nullptr, 0, &outputLength, (unsigned char*)data.data(), data.size());

	std::vector<uint8_t> encoded(outputLength);
	mbedtls_base64_encode(encoded.data(), outputLength, &outputLength, (unsigned char*)data.data(), data.size());

    return replaceBase64UrlChars(encoded, outputLength);
}

// ----- wip spotify test -----