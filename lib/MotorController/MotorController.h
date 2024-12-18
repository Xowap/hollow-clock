#pragma once

#include <Arduino.h>

enum class Direction { BACKWARD, FORWARD };

class MotorController {
private:
    static const int           STEPS_PER_REVOLUTION        = 2048;
    static const int           STEPS_PER_MINUTE            = 256;
    static const int           MAX_SPEED_MS_PER_REVOLUTION = 5000;
    static const unsigned long TWELVE_HOURS_MS             = 12 * 60 * 60 * 1000UL;
    int                        port[4]                     = { 5, 4, 3, 2 };
    int                        seq[4][4]                   = {
        {  LOW,  LOW, HIGH,  LOW },
        {  LOW,  LOW,  LOW, HIGH },
        { HIGH,  LOW,  LOW,  LOW },
        {  LOW, HIGH,  LOW,  LOW }
    };
    Direction     currentDirection  = Direction::FORWARD;
    unsigned long observedHandsTime = 0;
    unsigned long observedClockTime = 0;
    unsigned long currentHandsTime  = 0;

public:
    MotorController();
    void setDirection(Direction dir);
    void turn(unsigned int thousandths, unsigned int durationMs);
    void turnBy(int hours, int minutes, int seconds);
    void turnTo(int hours, int minutes, int seconds);
    void observeTime(int hour, int minute);
    void refresh();

private:
    void step();
};