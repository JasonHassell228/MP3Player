// #include "BluetoothSerial.h"

// #if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
// #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
// #endif

// BluetoothSerial SerialBT;

const int playPauseButtonPin = 19;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE Work!");
  pinMode(playPauseButtonPin, INPUT_PULLUP);
}

void loop() {
  // if (bleKeyboard.isConnected())
  // {
    Serial.println("Connected to BT!!");
    delay(1000);

    Serial.println("Sending enter..");
    delay(1000);

    Serial.println("Sending play/pause..");

    delay(1000);
  // }

  delay(200);
}
