#include "RingoHardware.h"
#include "Navigation.h"

volatile char edge;
volatile uint16_t timerInterruptCounter = 0;
volatile int control_counter  = 0;
volatile int senseFlag = false;
volatile int motorFlag = false;
volatile int continue_back_counter = 0;



ISR(TIMER2_COMPA_vect) {
    timerInterruptCounter++;
    if (timerInterruptCounter % 25 == 0) { //10hz
      control_counter += 1;
    }
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
    setupTimer2();
    delay(1000);
}

void loop() {
    if (control_counter % 2) {
        TaskSense(); 
    }
    if (control_counter % 3) {
        TaskMotorControl(); 
    }
}

void seekWallStraight() {
    Motors(60, 60);
    edge = LookForEdge();
    while (!LeftDetected(edge) && !RightDetected(edge)) {
        edge = LookForEdge();
    }
    Motors(0, 0);
}


void TaskSense(void) {
  cli();
  edge = LookForEdge();
  sei();
}

void TaskMotorControl(void) {

  SetAllPixelsRGB(0, 0, 50);
  if (continue_back_counter > 0) {
    OnEyes(70, 70, 0);
    backArc();
    continue_back_counter -= 1;
  } else if (LeftDetected(edge) || RightDetected(edge)) {
    OnEyes(50, 0, 0);
    backArc();
    continue_back_counter = 25;
  } else {
    OnEyes(0, 50, 0);
    frontArc();
  }
}


void frontArc(void) {
  Motors(60, 90);
}

void backArc(void) {
  Motors(-30, -70);
}

void setupTimer2() {
    cli(); // stop interrupts

    // Set up timer with prescaler = 1024
    TCCR2A = 0; // set entire TCCR2A register to 0
    TCCR2A |= (1 << WGM21); // turn on CTC mode
    TCCR2B = 0; // same for TCCR2B
    TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); // set prescaler to 1024
    // OCR2A = 155; // = (16*10^6) / (1024*100) - 1 (approx. 50 Hz)
    OCR2A = 31; //10 hz

    // Enable timer compare interrupt
    TIMSK2 |= (1 << OCIE2A);

    sei(); // allow interrupts
}

