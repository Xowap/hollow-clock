#include "MotorController.h"

MotorController::MotorController() {
    for (int i = 0; i < 4; i++) {
        pinMode(port[i], OUTPUT);
    }
}

void MotorController::setDirection(Direction dir) {
    currentDirection = dir;
    Serial.print("Setting direction: ");
    Serial.println(dir == Direction::FORWARD ? "forward" : "backward");
}

void MotorController::turn(unsigned int thousandths, unsigned int durationMs) {
    int           steps      = (STEPS_PER_REVOLUTION * thousandths) / 1000;
    unsigned long startTime  = millis();
    int           stepsTaken = 0;

    Serial.print("Turning by ");
    Serial.print(thousandths);
    Serial.print(" thousandth of turn in ");
    Serial.print(durationMs);
    Serial.println("ms");

    while (stepsTaken < steps && (millis() - startTime) < durationMs) {
        step();
        stepsTaken++;

        int delayTime = durationMs / steps;
        delay(delayTime);
    }

    // Power cut
    for (int i = 0; i < 4; i++) {
        digitalWrite(port[i], LOW);
    }
}

void MotorController::turnBy(int hours, int minutes, int seconds) {
    if (hours < 0 || minutes < 0 || seconds < 0) {
        return; // Don't allow negative durations
    }

    Serial.print("Turning by ");
    Serial.print(hours);
    Serial.print(" hours, ");
    Serial.print(minutes);
    Serial.print(" minutes and ");
    Serial.print(seconds);
    Serial.println(" seconds");

    long totalSeconds = hours * 3600L + minutes * 60L + seconds;
    long totalSteps   = (totalSeconds * STEPS_PER_MINUTE) / 60;

    long revolutionsToTurn = totalSteps / STEPS_PER_REVOLUTION;
    int  remainingSteps    = totalSteps % STEPS_PER_REVOLUTION;

    // Turn full revolutions
    for (long i = 0; i < revolutionsToTurn; i++) {
        turn(1000, MAX_SPEED_MS_PER_REVOLUTION);
    }

    // Turn remaining steps
    if (remainingSteps > 0) {
        int thousandths = (remainingSteps * 1000) / STEPS_PER_REVOLUTION;
        int durationMs  = (remainingSteps * MAX_SPEED_MS_PER_REVOLUTION) / STEPS_PER_REVOLUTION;
        turn(thousandths, durationMs);
    }

    // Update currentHandsTime
    unsigned long msToAdd = (unsigned long)totalSeconds * 1000UL;
    currentHandsTime      = (currentHandsTime + msToAdd) % TWELVE_HOURS_MS;
}

void MotorController::turnTo(int hours, int minutes, int seconds) {
    if (hours < 0 || minutes < 0 || seconds < 0) {
        return; // Don't allow negative durations
    }

    Serial.print("Setting time to ");
    Serial.print(hours);
    Serial.print(" hours, ");
    Serial.print(minutes);
    Serial.print(" minutes and ");
    Serial.print(seconds);
    Serial.println(" seconds");

    auto expect = (hours % 12) * 3600000UL + minutes * 60000UL + seconds * 1000UL;
    auto delta  = (expect + currentHandsTime) % TWELVE_HOURS_MS;

    turnBy(0, 0, delta / 1000);
    observedClockTime = expect;
    observedClockTime = millis();
}

void MotorController::observeTime(int hour, int minute) {
    observedHandsTime = ((hour % 12) * 3600UL + minute * 60UL) * 1000UL;
    observedClockTime = millis();
    currentHandsTime  = observedHandsTime;

    Serial.print("Observed that it's ");
    Serial.print(hour);
    Serial.print(":");
    Serial.print(minute);
    Serial.print(" at ");
    Serial.print(observedClockTime);
    Serial.println("ms");
}

void MotorController::refresh() {
    unsigned long currentClockTime = millis();
    unsigned long elapsedTime      = currentClockTime - observedClockTime;
    unsigned long targetHandsTime  = (observedHandsTime + elapsedTime) % TWELVE_HOURS_MS;

    Serial.println("Refreshing displayed time (cf next)");

    long timeDifference = (long)targetHandsTime - (long)currentHandsTime;
    if (timeDifference < 0) {
        timeDifference += TWELVE_HOURS_MS;
    }

    int hours   = timeDifference / 3600000;
    int minutes = (timeDifference % 3600000) / 60000;
    int seconds = (timeDifference % 60000) / 1000;

    turnBy(hours, minutes, seconds);
}

void MotorController::step() {
    static int phase = 0;
    int        delta = (currentDirection == Direction::BACKWARD) ? 1 : 3;

    phase = (phase + delta) % 4;
    for (int i = 0; i < 4; i++) {
        digitalWrite(port[i], seq[phase][i]);
    }
}