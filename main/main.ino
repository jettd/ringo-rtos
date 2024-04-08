#include <Arduino_FreeRTOS.h>
#include "RingoHardware.h"
#include "Navigation.h"

volatile char edge;

void TaskSense(void *pvParameters);
void TaskMotorControl(void * pvParameters);

void setup(){
  HardwareBegin();        //initialize Ringo's brain to work with his circuitry
  SwitchMotorsToSerial(); //Call "SwitchMotorsToSerial()" before using Serial.print functions as motors & serial share a line
  RestartTimer();
  RxIRRestart(4);
  ResetLookAtEdge();      //Zero LookAtEdge() avg     
  PlayStartChirp();
  NavigationBegin();
  SwitchButtonToPixels();
  SetAllPixelsRGB(100, 0, 100);

  xTaskCreate(TaskMotorControl, "motors", 128, NULL, 10, NULL);
  xTaskCreate(TaskSense, "sense", 128, NULL, 30, NULL);
}

void backupTurn() {
  // OnEyes(50, 50, 50)
  Motors(-30, -65);
  vTaskDelay(300/portTICK_PERIOD_MS);
  Motors(0, 0);
}

void seekWallStraight() {
  // OnEyes(50, 50, 50);
  Motors(100, 100);
  edge = LookForEdge();
  while (!LeftDetected(edge) && !RightDetected(edge)) {
    edge = LookForEdge();
  }
  Motors(0, 0);
}

void TaskSense(void* pvParameters) {
  int loop_delay = 10; //5-15 seems to work good with the current setup
  bool toggle = true;
  SwitchAmbientToEdge();
  ResetLookAtEdge();

  for (;;) {
        if (toggle) {
      toggle = false;
      SetAllPixelsRGB(50, 0, 0);
    } else {
      toggle = true;
      SetAllPixelsRGB(0, 0, 50);
    }
    edge = LookForEdge();
    vTaskDelay(loop_delay/portTICK_PERIOD_MS);
    SetAllPixelsRGB(0,0,0);
  }
}

void TaskMotorControl(void *pvParameters) {
  bool toggle = true;
  Motors(50, 65);
  vTaskDelay(1000/portTICK_PERIOD_MS);
  for (;;) {
    if (LeftDetected(edge) || RightDetected(edge)) {
      backupTurn();
    } else {
      Motors(60, 90);
    }
    vTaskDelay(50/portTICK_PERIOD_MS);
    SetAllPixelsRGB(0,0,0);
  }
}

// void TaskNav(void* pvParameters) {
//   bool edge_reset_flag = false;
//   int ramp_turn_radius = 2;
//   int until_ramp_counter = 100;
//   char edge;
//   int LeftAverageHold, RightAverageHold;
//   int loopHold = 15; //10 works good
//   int left_speed = 60;
//   int right_speed = 70;
//   int turn_diff = 50;
//   int max_allowed = 125;
//   int min_allowed = 0;
//   SwitchAmbientToEdge();
//   seekWallStraight();
//   for (;;) {
//     Motors(left_speed, right_speed);
//     RINGO_STATE = CLEAR_s;
//     SimpleGyroNavigation();
//     edge = LookForEdge();
//     if (LeftDetected(edge) || RightDetected(edge)) {
//       OnEyes(100, 0, 0);
//       Motors(0, 0);
//       vTaskDelay(100/portTICK_PERIOD_MS);
//       // pidTurn(-90);
//       backupTurn();
//       vTaskDelay(loopHold/portTICK_PERIOD_MS);
//       RINGO_STATE = OBSTACLE_s;
//       edge_reset_flag = true;
//     } 
//     if (edge_reset_flag) {
//       left_speed = 60;
//       right_speed = 75;
//       edge_reset_flag = false;
//     } else if (until_ramp_counter < 0) {
//       right_speed += ramp_turn_radius;
//       left_speed -= ramp_turn_radius;
//       right_speed = max(min(right_speed, max_allowed), min_allowed);
//       left_speed = max(min(left_speed, max_allowed), min_allowed);
//       until_ramp_counter = 100;
//       LeftEye(0, left_speed/2, 0);
//       RightEye(0, right_speed/2, 0);
//     } else {
//       until_ramp_counter--;
//     }
//     vTaskDelay(loopHold/portTICK_PERIOD_MS);
//   }
// }

void loop(){ 
} // end of loop() function




