#include <Arduino.h>
#include <HardwareSerial.h>
#include <DFRobotDFPlayerMini.h>

HardwareSerial mySerial(1);  // Use Serial1 for DFPlayer communication
DFRobotDFPlayerMini mp3;

const int buttonPins[] = {5, 6, 7, 9, 10, 20, 21};  // Button GPIOs
const int totalButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);

int currentSong = 0;  // Track currently playing song (0 = none)

void setup() {
    Serial.begin(115200);
    mySerial.begin(9600, SERIAL_8N1, 1, 0);  // RX=16, TX=17 (ESP32 UART2)
    
    if (!mp3.begin(mySerial)) { 
        Serial.println("MP3 module not detected!");
        while (true);
    }

    mp3.volume(20);  // Set volume (0-30)
    
    for (int i = 0; i < totalButtons; i++) {
        pinMode(buttonPins[i], INPUT_PULLUP);
    }
}

void loop() {
    for (int i = 0; i < totalButtons; i++) {
        if (digitalRead(buttonPins[i]) == LOW) {  // Button pressed
            int songNumber = i + 1;  // Map button index to song (001-007)

            if (currentSong == songNumber) {
                mp3.pause();  // Pause if the same button is pressed again
                currentSong = 0;
                Serial.println("Paused");
            } else {
                mp3.stop();  // Stop current song
                delay(100);  // Small delay for stability
                mp3.play(songNumber);  // Play new song
                currentSong = songNumber;
                Serial.print("Playing song: ");
                Serial.println(songNumber);
            }

            delay(300);  // Debounce delay
            while (digitalRead(buttonPins[i]) == LOW);  // Wait for button release
        }
    }
}
