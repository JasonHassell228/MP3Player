#include "HelperFunctions.h"

// ----- HARDWARE HELPER FUNCTIONS -----

// TODO DEFINE ALL VARS IN CPP FILE \/\/\\/\/
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

std::string HelperFunctions::encode_base64(std::vector<uint8_t>& data)
{
	size_t outputLength;
	mbedtls_base64_encode(nullptr, 0, &outputLength, (unsigned char*)data.data(), data.size());

	std::vector<uint8_t> encoded(outputLength);
	mbedtls_base64_encode(encoded.data(), outputLength, &outputLength, (unsigned char*)data.data(), data.size());

	std::string rst((char*)encoded.data(), outputLength);
	return rst;
}

std::vector<uint8_t> HelperFunctions::decode_base64(std::string& text)
{
	size_t outputLength;
	mbedtls_base64_decode(nullptr, 0, &outputLength, (unsigned char*)text.c_str(), text.length());

	std::vector<uint8_t> decoded(outputLength);
	mbedtls_base64_decode(decoded.data(), outputLength, &outputLength, (unsigned char*)text.c_str(), text.length());
	return decoded;
}

String HelperFunctions::getAccessToken() {
    client.setTimeout(8000); // Reduced timeout
    
    if (!client.connect(token_host, httpsPort)) {
        return "";
    }

    String body = String("grant_type=client_credentials") +
                  "&client_id=" + String(client_id) +
                  "&client_secret=" + String(client_secret);

    client.println("POST /api/token HTTP/1.1");
    client.println("Host: accounts.spotify.com");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(body.length());
    client.println("Connection: close");
    client.println();
    client.print(body);

    unsigned long timeout = millis() + 8000;
    while (client.connected() && millis() < timeout) {
        String line = client.readStringUntil('\n');
        if (line == "\r") break;
    }

    if (millis() >= timeout) {
        client.stop();
        return "";
    }

    String response = client.readString();
    client.stop();
    
    int startIndex = response.indexOf("\"access_token\":\"") + 16;
    int endIndex = response.indexOf("\"", startIndex);
    if (startIndex > 15 && endIndex > startIndex) {
        return response.substring(startIndex, endIndex);
    }

    return "";
}
// ----- wip spotify test -----