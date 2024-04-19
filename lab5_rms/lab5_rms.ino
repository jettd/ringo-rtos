#include <Arduino_FreeRTOS.h>
#include "RingoHardware.h"
#include "Navigation.h"
#include <Arduino.h>


TaskHandle_t TaskSenseHandle;
TaskHandle_t TaskMotorHandle;

// Task periods (in milliseconds)
const TickType_t TaskSensePeriod = pdMS_TO_TICKS(20);
const TickType_t TaskMotorPeriod = pdMS_TO_TICKS(25);

void TaskSense(void *pvParameters);
void TaskMotor(void *pvParameters);

volatile char edge;

unsigned long total = 0;  //used to time the execution time
volatile int x = 0;

void setup() {
  HardwareBegin(); 
  SwitchMotorsToSerial();
  RestartTimer();
  RxIRRestart(4);
  ResetLookAtEdge();
  PlayStartChirp();
  NavigationBegin();
  SwitchButtonToPixels();
  xTaskCreate(TaskSense, "sense", configMINIMAL_STACK_SIZE, NULL, 2, &TaskSenseHandle);
  xTaskCreate(TaskMotor, "motors", configMINIMAL_STACK_SIZE, NULL, 1, &TaskMotorHandle);
}

void loop() {
}

void TaskSense(void *pvParameters) { //1.4 ms average execution time; 8 runs 800 samples
  TickType_t xLastWakeTime = xTaskGetTickCount();
  ResetLookAtEdge();

  while (1) {
    edge = LookForEdge();
    vTaskDelayUntil(&xLastWakeTime, TaskSensePeriod);
  }
}

void TaskMotor(void *pvParameters) { //average ms = 1.8; ran 8x, 800 samples
  TickType_t xLastWakeTime = xTaskGetTickCount();
  int continue_back_counter = 0;
  int a;
  int b;

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



/********************************************************************************
TIMING ESTIMATION IMPLEMENTATION
*********************************************************************************/


// void TaskSense(void *pvParameters) { //1.5
//   TickType_t xLastWakeTime = xTaskGetTickCount();
//   ResetLookAtEdge();
//   unsigned long a, b;

//   while (1) {
//     b = micros();
//     edge = LookForEdge();
//     a = micros();
//     total += a-b;
//     vTaskDelayUntil(&xLastWakeTime, TaskSensePeriod);
//     if (x < 100) {
//       x += 1;
//     } else {
//       // PlayStartChirp();
//       while (1) {
//         unsigned long average = total/10000;  //this factor (100 samples) will make the light blink for x times where: x = avg_time_in_microseconds/100 (== time_in_ms * 10)
//         OffEyes();
//         // PlayStartChirp();
//         SwitchSerialToMotors();
//         vTaskSuspend(TaskMotorHandle);
//         for (int i = 0; i < average; i++) {
//           OnEyes(100, 100, 100);
//           vTaskDelay(250/portTICK_PERIOD_MS);
//           OffEyes();
//           vTaskDelay(250/portTICK_PERIOD_MS);
//         }
//         while(1) {
//           Serial.print(average);
//           vTaskSuspend(TaskSenseHandle);
//           vTaskDelay(100000);
//         }
//       }
//     }
//   }
// }

// void TaskMotor(void *pvParameters) { //average ms = 1.8 200 samples
//   TickType_t xLastWakeTime = xTaskGetTickCount();
//   int continue_back_counter = 0;
//   unsigned long a, b;

//   while (1) {
//     b = micros();
//     SetAllPixelsRGB(0, 0, 50);
//     if (continue_back_counter > 0) {
//       OnEyes(70, 70, 0);
//       backArc();
//       continue_back_counter -= 1;
//     } else if (LeftDetected(edge) || RightDetected(edge)) {
//       OnEyes(50, 0, 0);
//       backArc();
//       continue_back_counter = 25;
//     } else {
//       OnEyes(0, 50, 0);
//       frontArc();
//     }
//     a = micros();
//     total += a-b;
//     vTaskDelayUntil(&xLastWakeTime, TaskMotorPeriod);
//     if (x < 100) {
//       x += 1;
//     } else {
//       // PlayStartChirp();
//       while (1) {
//         unsigned long average = total/10000; 
//         OffEyes();
//         // PlayStartChirp();
//         SwitchSerialToMotors();
//         vTaskSuspend(TaskSenseHandle);
//         for (int i = 0; i < average; i++) {
//           OnEyes(100, 100, 100);
//           vTaskDelay(100/portTICK_PERIOD_MS);
//           OffEyes();
//           vTaskDelay(200/portTICK_PERIOD_MS);
//         }
//         while(1) {
//           Serial.print(average);
//           vTaskSuspend(TaskMotorHandle);
//           vTaskDelay(100000);
//         }
//       }
//     }
//   }
// }

