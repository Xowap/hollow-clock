#include <Arduino.h>
#include <MotorController.h>

MotorController motor;

void setup() {
    Serial.begin(9600);
    delay(5000);
    Serial.println("--- setup start ---");
    motor.setDirection(Direction::FORWARD);
    motor.observeTime(0, 0);
    Serial.println("--- setup done ---");
}

void loop() {
    const long REFRESH_PERIOD_MS = 10000;

    while (true) {
        motor.refresh();
        delay(REFRESH_PERIOD_MS);
    }
}