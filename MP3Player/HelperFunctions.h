#ifndef HelperFunctions_h
#define HelperFunctions_h

#include <ArduinoJson.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <HijelHID_BLEKeyboard.h>
#include <mbedtls/base64.h> // Base64 encoding and decoding
#include <mbedtls/md.h>		// SHA-256 / message digest
#include <UI.h>
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
		VOL_UP_PIN};

	// Screen pins
	const int SCREEN_SDA_PIN = 23;
	const int SCREEN_SCK_PIN = 22;

  // Network credentials
  const char *ssid = "";     // your network SSID (name of wifi network)
  const char *password = ""; // your network password
  const char *client_id = "";

	// Spotify auth info
	const char *spotify_redirect_uri = "http://127.0.0.1:8080";
	String authorizationCode = "";
	String accessToken = "";
	String codeChallenge = "";
	String codeVerifier = "";

	// Playback state
	bool isPlaying = false;
	TrackInfo currentlyPlaying = {
		"Just inited",
		"Fresh loop"
	};

	HijelHID_BLEKeyboard keyboard; // Bluetooth keyboard
	WiFiClientSecure client;	   // Wifi client
	WebServer webServer;		   // Create web server
	UI ui;						   // UI object

	HelperFunctions();

	// Hardware functions
	void setPullupPins();
	void checkBtnPresses();

	// Spotify functions
	void exchangeSpotifyAuthForAccess();
	void getSpotifyCurrentPlayingSong();
	void handleRoot();
	void generateCodeChallenge();
	void playSpotify();
	// RTOS tasks stuffs
	void startSpotifyTask(); // Start Spotify RTOS task
	void startUiTask(); // Start UI RTOS task
	TrackInfo getCurrentlyPlaying(); // Currently playing thread-safe access stuff
	void setCurrentlyPlaying(const TrackInfo &trackInfo); // Currently playing thread-safe access stuff

	// General helper functions
	void uiLoop(unsigned long &currentTime);
	void setupWifi();
	void setupWebServer();
	void initializeDisplay();

private:
	// Hardware functions
	void pressBluetoothBtn(const int pin, uint16_t bluetoothBtn);

	// Spotify functions
	String encode_base64(std::vector<uint8_t> &data);
	std::vector<uint8_t> generateSHA256(String &text);
	void generateCodeVerifier(int length);
	String generateSpotifyAuthURL(const String &codeChallenge);
	String spotifyRequest(String host,
						  String method,
						  String endpoint,
						  String body = "",
						  String contentType = "application/json");
	// RTOS tasks stuffs
	static void spotifyTask(void *parameter); // Spotify RTOS task
	static void uiTask(void *parameter); // UI loop RTOS task
	SemaphoreHandle_t currentlyPlayingMutex = nullptr; // Currently playing thread-safe access stuff

};

#endif