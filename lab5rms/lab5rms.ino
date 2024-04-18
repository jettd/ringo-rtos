#include "RingoHardware.h"
#include "Navigation.h"

volatile char edge;

void backupTurn() {
    Motors(-30, -65);
    delay(300);
    Motors(0, 0);
}

void seekWallStraight() {
    Motors(60, 60);
    edge = LookForEdge();
    while (!LeftDetected(edge) && !RightDetected(edge)) {
        edge = LookForEdge();
    }
    Motors(0, 0);
}

void TaskSense() {
    static bool toggle = true;
    if (toggle) {
        toggle = false;
        SetAllPixelsRGB(20, 0, 0);
    } else {
        toggle = true;
        SetAllPixelsRGB(0, 0, 20);
    }
    edge = LookForEdge();
    SetAllPixelsRGB(0, 0, 0);
}

void TaskMotorControl() {
    if (LeftDetected(edge) || RightDetected(edge)) {
        backupTurn();
    } else {
        Motors(60, 90);
    }
    SetAllPixelsRGB(0, 0, 0);
}

void setup() {
    HardwareBegin();
    SwitchMotorsToSerial();
    RestartTimer();
    RxIRRestart(4);
    // ResetLookAtEdge();
    PlayStartChirp();
    NavigationBegin();
    SwitchButtonToPixels();
    SetAllPixelsRGB(80, 80, 80);
    SwitchAmbientToEdge();
    ResetLookAtEdge();
    // Motors(50, 65);
    delay(1000);
}

void loop() {
    static unsigned long lastTime = 0;
    unsigned long currentTime = millis();

    if (currentTime - lastTime >= 10) {
        TaskSense();
    }

    if (currentTime - lastTime >= 50) {
        lastTime = currentTime;
        TaskMotorControl();
    }
}





// #include <avr/io.h>
// #include <avr/interrupt.h>

// // Periods for the tasks in number of timer interrupts
// #define PERIOD1 500  // Adjust this to set the period for Task1
// #define PERIOD2 1000 // Adjust this to set the period for Task2

// volatile uint16_t timerInterruptCounter = 0;
// volatile bool flagTask1 = false;
// volatile bool flagTask2 = false;

// void Task1() {
//     // Task 1 code here
// }

// void Task2() {
//     // Task 2 code here
// }

// ISR(TIMER2_COMPA_vect) {
//     timerInterruptCounter++;

//     if (timerInterruptCounter % PERIOD1 == 0) {
//         flagTask1 = true; // Set flag for Task1
//     }
//     if (timerInterruptCounter % PERIOD2 == 0) {
//         flagTask2 = true; // Set flag for Task2
//     }
// }

// void setupTimer2() {
//     cli(); // stop interrupts

//     // Set up timer with prescaler = 1024
//     TCCR2A = 0; // set entire TCCR2A register to 0
//     TCCR2A |= (1 << WGM21); // turn on CTC mode
//     TCCR2B = 0; // same for TCCR2B
//     TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); // set prescaler to 1024
//     OCR2A = 155; // = (16*10^6) / (1024*100) - 1 (approx. 100 Hz)

//     // Enable timer compare interrupt
//     TIMSK2 |= (1 << OCIE2A);

//     sei(); // allow interrupts
// }

// void setup() {
//     setupTimer2();
// }

// void loop() {
//     if (flagTask1) {
//         Task1();           // Execute Task1
//         flagTask1 = false; // Reset flag
//     }
//     if (flagTask2) {
//         Task2();           // Execute Task2
//         flagTask2 = false; // Reset flag
//     }
// }
