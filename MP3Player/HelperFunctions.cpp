#include "HelperFunctions.h"

HelperFunctions::HelperFunctions() : webServer(80) {} // Initialise web server to port 80

// ----- LOCAL FUNCTIONS -----
JsonDocument parseResponseJson(String response)
{
    JsonDocument doc;
    deserializeJson(doc, response.substring(response.indexOf("\r\n\r\n"))); // Find HTTP header separator
    return doc;                                                             // TODO error handling
}

template <typename T>
T getSingleJsonItem(JsonDocument &doc, String item)
{
    return doc[item].as<T>(); // TODO error handling for no item found
}

// ----- HARDWARE HELPER FUNCTIONS -----

void HelperFunctions::setPullupPins()
{
    for (int i = 0; i < numBtnPins; i++)
    {
        pinMode(ALL_BTN_PINS[i], INPUT_PULLUP);
    }
}

// Checks all physical button presses and runs the corresponding action
void HelperFunctions::checkBtnPresses()
{
    // TODO: Btns control spotify API by default, add BT mode later
    pressBluetoothBtn(VOL_UP_PIN, MEDIA_VOLUME_UP);
    pressBluetoothBtn(VOL_DOWN_PIN, MEDIA_VOLUME_DOWN);
    pressBluetoothBtn(PLAY_PAUSE_PIN, MEDIA_PLAY_PAUSE);
}

// Check if physical btn pressed, if yes press corresponding bluetooth btn
void HelperFunctions::pressBluetoothBtn(const int pin, uint16_t bluetoothBtn)
{
    if (digitalRead(pin) == LOW)
    {
        keyboard.tap(bluetoothBtn, 25, 50);
        keyboard.releaseAll();
    }
}

// ----- SPOTIFY API HELPER FUNCTIONS -----

void HelperFunctions::generateCodeVerifier(int length)
{
    String text = "";
    String possible = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    for (int i = 0; i < length; i++)
    {
        text += possible[random(0, possible.length())];
    }
    codeVerifier = text;
}

void HelperFunctions::generateCodeChallenge()
{
    generateCodeVerifier(64);                       // Generate code verifier
    String codeVerifierCStr = codeVerifier.c_str(); // TODO can we make this one line? It was giving me runtime issues before
    auto codeVerifierHashed = generateSHA256(codeVerifierCStr);
    // Encode code verifier as base64
    std::vector<uint8_t> data(codeVerifierHashed.begin(), codeVerifierHashed.end());
    codeChallenge = encode_base64(data);
}

std::vector<uint8_t> HelperFunctions::generateSHA256(String &text)
{
    std::vector<uint8_t> hash(32);

    mbedtls_md_context_t ctx;
    mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;

    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
    mbedtls_md_starts(&ctx);
    mbedtls_md_update(&ctx, (const unsigned char *)text.c_str(), text.length());
    mbedtls_md_finish(&ctx, hash.data());
    mbedtls_md_free(&ctx);

    return hash;
}

String HelperFunctions::encode_base64(std::vector<uint8_t> &data)
{
    size_t outputLength;
    mbedtls_base64_encode(nullptr, 0, &outputLength, (unsigned char *)data.data(), data.size());
    std::vector<uint8_t> encoded(outputLength);
    mbedtls_base64_encode(encoded.data(), outputLength, &outputLength, (unsigned char *)data.data(), data.size());

    // Replace base64 url chars
    String replaceString = String((char *)encoded.data());
    replaceString.replace("+", "-");
    replaceString.replace("/", "_");
    int padding = replaceString.indexOf('=');
    if (padding != -1)
    {
        replaceString.remove(padding);
    }
    return replaceString;
}

String HelperFunctions::generateSpotifyAuthURL(const String &codeChallenge)
{
    static const char format[] =
        "https://accounts.spotify.com/authorize?"
        "client_id=%s"
        "&response_type=code"
        "&redirect_uri=%s"
        "&code_challenge_method=S256"
        "&code_challenge=%s"
        "&scope=user-read-currently-playing user-read-playback-state user-modify-playback-state"; // TODO add more perms as needed
    size_t len = snprintf(nullptr, 0, format, client_id,
                          spotify_redirect_uri,
                          codeChallenge.c_str());
    char url[len + 1];
    snprintf(url, sizeof(url), format, client_id,
             spotify_redirect_uri,
             codeChallenge.c_str());
    return String(url);
}

// Main webpage
void HelperFunctions::handleRoot()
{
    String authLink = generateSpotifyAuthURL(codeChallenge);

    String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <title>ESP32 Test Web Server</title>
      <meta name="viewport" content="width=device-width, initial-scale=1">
    </head>
    <body>
      <h1>Spotify authentication</h1>
      <p>
        Spotify auth link:
        <a href=")rawliteral" +
                  authLink + R"rawliteral(" target="_blank">Auth link</a>
      </p>
      <p>Here's the authorisation link. It will open a login page for Spotify in a new tab.</p>
      <p>Once you login, just copy the code parameter from the URL. It should look something like this:</p>
      <p>http://127.0.0.1:8080/?code=AQCYMmZY1PagU3rt4ji_9h8yGREfBHOZEpqEH1W204caWfeyLA70...</p>
      <p>You want the part after 'code='!</p>
      <form action="/auth-code" method="GET">
        <input name="code" placeholder="Paste Spotify auth code">
        <button type="submit">Submit</button>
      </form>
    </body>
    </html>
  )rawliteral";

    webServer.send(200, "text/html", html);
}

void HelperFunctions::exchangeSpotifyAuthForAccess()
{
    String body =
        "grant_type=authorization_code"
        "&code=" +
        authorizationCode +
        "&redirect_uri=" + String(spotify_redirect_uri) +
        "&client_id=" + client_id +
        "&code_verifier=" + codeVerifier;
    String response = spotifyRequest("accounts.spotify.com",
                                     "POST",
                                     "/api/token",
                                     body,
                                     "application/x-www-form-urlencoded");

    // Debug logging (remove)
    Serial.println("Auth POST body: ");
    Serial.println(body);
    Serial.print("finished printing body: ");
    Serial.println("Auth exchange response: ");
    Serial.println(response);
    // Debug logging (remove)

    // reading response json section
    auto doc = parseResponseJson(response);
    accessToken = getSingleJsonItem<String>(doc, "access_token");
    // Debug logging (remove)
    Serial.println("FINAL ACCESS TOKEN: ");
    Serial.println(accessToken);
    // Debug logging (remove)
    // reading response json section
}

TrackInfo HelperFunctions::getSpotifyCurrentPlayingSong()
{
    String response = spotifyRequest("api.spotify.com",
                                     "GET",
                                     "/v1/me/player/currently-playing");

    // Debug logging (remove)
    Serial.println("GET current playing response: ");
    Serial.println(response);
    // Debug logging (remove)

    // Parse json section
    auto doc = parseResponseJson(response);
    isPlaying = getSingleJsonItem<bool>(doc, "is_playing"); // Set isPlaying for future use
    return {
        doc["item"]["name"].as<String>(),
        doc["item"]["artists"][0]["name"].as<String>()};
    // Parse json section
}

void HelperFunctions::playSpotify()
{
    String response = spotifyRequest("api.spotify.com",
                                     "PUT",
                                     "/v1/me/player/play");

    // Debug logging (remove)
    Serial.println("PUT resume play response: ");
    Serial.println(response);
    // Debug logging (remove)
}

String HelperFunctions::spotifyRequest(String host,
                                       String method,
                                       String endpoint,
                                       String body,
                                       String contentType)
{
    if (!client.connect("api.spotify.com", 443))
    { // Check connected to spotify
        Serial.println("Spotify connection failed 443");
    }
    // Make request
    client.println(method + " " + endpoint + " HTTP/1.1");
    client.println("Host: " + host);
    client.println("Content-Type: " + contentType);
    if (host == "api.spotify.com")
    {
        client.println("Authorization: Bearer " + accessToken);
    }
    client.println("Content-Length: " + String(body.length()));
    client.println("Connection: close");
    client.println();
    client.print(body);
    // Read & return response
    String response = "";
    while (client.connected() || client.available())
    { // TODO is this blocking main loop?
        if (client.available())
        {
            response += client.readString();
        }
    }
    client.stop();
    return response;
}

// ----- GENERAL HELPER FUNCTIONS -----
void HelperFunctions::setupWifi()
{
    WiFi.begin(ssid, password);

    // attempt to connect to Wifi network (blocking loop, as nothing works without wifi active)
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        // wait 1 second for re-trying
        delay(1000);
    }

    Serial.print("Connected to ");
    Serial.println(ssid);

    client.setInsecure(); // skip verification
}

// Run a local webserver for the user to login to spotify easily
void HelperFunctions::setupWebServer()
{
    // TODO: Need to display local IP on screen @ startup instead of serial print
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Configure local webserver routes
    webServer.on("/", HTTP_GET, [&]() {handleRoot();});
    webServer.on("/auth-code", HTTP_GET, [&]() {
        authorizationCode = webServer.arg("code"); // Get auth code pasted by user (after successful login)
        int pos = authorizationCode.indexOf("&ubi"); // Strip out other arg to get auth code
        if (pos != -1) {
            authorizationCode.remove(pos);
        }

        exchangeSpotifyAuthForAccess(); // Use auth code to get access code

        // TODO remove get song from web server setup, move to main loop
        auto trackInfo = getSpotifyCurrentPlayingSong();
        // Debug logging (remove)
        Serial.print("Song: ");
        Serial.println(trackInfo.songName);
        Serial.print("Artist: ");
        Serial.println(trackInfo.artist);
        // Debug logging (remove)
    });

    // Start server
    webServer.begin();

    Serial.println("Web server started");
}