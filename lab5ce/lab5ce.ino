#include <Arduino_FreeRTOS.h>
#include "RingoHardware.h"
#include "Navigation.h"

// Task handles
TaskHandle_t TaskSenseHandle;
TaskHandle_t TaskMotorHandle;

// Task periods (in milliseconds)
const TickType_t TaskSensePeriod = pdMS_TO_TICKS(20);
const TickType_t TaskMotorPeriod = pdMS_TO_TICKS(25);

// Task prototypes
void TaskSense(void *pvParameters);
void TaskMotor(void *pvParameters);

volatile char edge;

void setup() {
  HardwareBegin();        //initialize Ringo's brain to work with his circuitry
  SwitchMotorsToSerial();
  RestartTimer();
  RxIRRestart(4);
  ResetLookAtEdge();
  PlayStartChirp();
  NavigationBegin();
  SwitchButtonToPixels();

  // Create tasks
  xTaskCreate(TaskSense, "sense", configMINIMAL_STACK_SIZE, NULL, 2, &TaskSenseHandle);
  xTaskCreate(TaskMotor, "motors", configMINIMAL_STACK_SIZE, NULL, 1, &TaskMotorHandle);
}

void loop() {
}

void TaskSense(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  ResetLookAtEdge();

  while (1) {
    edge = LookForEdge();
    vTaskDelayUntil(&xLastWakeTime, TaskSensePeriod);
  }
}

void TaskMotor(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  int continue_back_counter = 0;

  while (1) {
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
    vTaskDelayUntil(&xLastWakeTime, TaskMotorPeriod);
  }
}

void frontArc(void) {
  Motors(60, 90);
}

void backArc(void) {
  Motors(-30, -70);
}

