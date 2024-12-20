#include <Arduino.h>
#include <MotorController.h>
#include <SimpleWebSerial.h>

MotorController motor;
SimpleWebSerial webSerial;

void setup() {
    Serial.begin(9600);
    delay(5000);
    Serial.println("--- setup start ---");
    motor.setDirection(Direction::FORWARD);
    motor.observeTime(0, 0);
    Serial.println("--- setup done ---");
}

void loop() {
    const long REFRESH_PERIOD_MS = 1 * 1000;

    while (true) {
        motor.refresh();
        delay(REFRESH_PERIOD_MS);
        webSerial.send("youpi", "updating the time yolo");
    }
}