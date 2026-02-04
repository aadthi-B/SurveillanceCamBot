#include <WiFi.h>
#include <WebSocketsServer.h>
#include <WebServer.h>
#include <U8g2lib.h>
#include <ESP32Servo.h>

// Wi-Fi Credentials
const char* ssid = "wifi";
const char* password = "password";

// Motor Driver Pins
#define IN1 18
#define IN2 19
#define IN3 14
#define IN4 25
#define ENA 26
#define ENB 27

// Servo Pins
#define PAN_SERVO_PIN 4
#define TILT_SERVO_PIN 5

Servo panServo;
Servo tiltServo;
int panAngle = 90;  // Default middle position
int tiltAngle = 90; // Default middle position

// OLED Display
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* SCL=*/ 22, /* SDA=*/ 21, /* reset=*/ U8X8_PIN_NONE);

// Web Server & WebSocket Server
WebServer server(80);
WebSocketsServer webSocket(81);

// Serve Web Page
const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 Control</title>
    <script>
        let ws = new WebSocket("ws://" + location.hostname + ":81");
        function sendCmd(cmd) { ws.send(cmd); }
    </script>
</head>
<body>
    <h2>Motor Control</h2>
    <button onclick="sendCmd('forward')">Forward</button>
    <button onclick="sendCmd('backward')">Backward</button>
    <button onclick="sendCmd('left')">Left</button>
    <button onclick="sendCmd('right')">Right</button>
    <button onclick="sendCmd('stop')">Stop</button>

    <h2>Pan-Tilt Control</h2>
    <button onclick="sendCmd('pan_left')">Pan Left</button>
    <button onclick="sendCmd('pan_right')">Pan Right</button>
    <br>
    <button onclick="sendCmd('tilt_up')">Tilt Up</button>
    <button onclick="sendCmd('tilt_down')">Tilt Down</button>
</body>
</html>
)rawliteral";

// Handle Web Page Request
void handleRoot() {
    server.send(200, "text/html", webpage);
}

// Function to Update OLED
void updateOLED(const String &message) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(10, 30);
    u8g2.print(message);
    u8g2.sendBuffer();
}

// Function to Stop Motors
void stopMotors() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
    updateOLED("Stopped");
}

// Function to Control Motors
void controlMotors(const String &command) {
    stopMotors(); // Ensure previous movement stops before a new command

    if (command == "forward") {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        analogWrite(ENA, 255);
        analogWrite(ENB, 255);
        updateOLED("Moving Forward");
    } else if (command == "backward") {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        analogWrite(ENA, 255);
        analogWrite(ENB, 255);
        updateOLED("Moving Backward");
    } else if (command == "left") {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        analogWrite(ENA, 255);
        analogWrite(ENB, 255);
        updateOLED("Turning Left");
    } else if (command == "right") {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        analogWrite(ENA, 255);
        analogWrite(ENB, 255);
        updateOLED("Turning Right");
    } else {
        stopMotors();
    }
}

// Function to Control Pan-Tilt Servos
void controlPanTilt(const String &command) {
    if (command == "pan_left") {
        panAngle = min(panAngle + 10, 180);
        panServo.write(panAngle);
        updateOLED("Pan Left: " + String(panAngle));
    } else if (command == "pan_right") {
        panAngle = max(panAngle - 10, 0);
        panServo.write(panAngle);
        updateOLED("Pan Right: " + String(panAngle));
    } else if (command == "tilt_up") {
        tiltAngle = max(tiltAngle - 10, 0);
        tiltServo.write(tiltAngle);
        updateOLED("Tilt Up: " + String(tiltAngle));
    } else if (command == "tilt_down") {
        tiltAngle = min(tiltAngle + 10, 180);
        tiltServo.write(tiltAngle);
        updateOLED("Tilt Down: " + String(tiltAngle));
    }
}

// WebSocket Event Handler
void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
    if (type == WStype_TEXT) {
        String command = String((char*)payload);
        Serial.print("Received: ");
        Serial.println(command);

        if (command.startsWith("pan") || command.startsWith("tilt")) {
            controlPanTilt(command);
        } else {
            controlMotors(command);
        }
    }
}

void setup() {
    Serial.begin(115200);

    // Motor Driver Pin Modes
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);
    stopMotors();

    // Initialize Servos
    panServo.attach(PAN_SERVO_PIN);
    tiltServo.attach(TILT_SERVO_PIN);
    panServo.write(panAngle);
    tiltServo.write(tiltAngle);
    Serial.println("Servos Initialized!");

    // Initialize OLED
    if (!u8g2.begin()) {
        Serial.println("OLED init failed!");
        return;
    }
    Serial.println("OLED Initialized!");

    // Start Wi-Fi as Access Point
    WiFi.softAP(ssid, password);
    delay(2000);
    Serial.println("Access Point Started!");
    Serial.print("ESP32 IP: ");
    Serial.println(WiFi.softAPIP());

    // Start Web Server
    server.on("/", handleRoot);
    server.begin();

    // Start WebSocket Server
    webSocket.begin();
    webSocket.onEvent(onWebSocketEvent);
}

void loop() {
    server.handleClient();
    webSocket.loop();
    delay(1);
}
