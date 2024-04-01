#include <Arduino_FreeRTOS.h>
#include "RingoHardware.h"
#include "Navigation.h"

TaskHandle_t xEdgeHandler;
TaskHandle_t xCornerHandler;
volatile int target;
int GetDegreesMod(void);

void setup(){
  HardwareBegin();        //initialize Ringo's brain to work with his circuitry
  SwitchMotorsToSerial(); //Call "SwitchMotorsToSerial()" before using Serial.print functions as motors & serial share a line
  RestartTimer();
  RxIRRestart(4);
  ResetLookAtEdge();      //Zero LookAtEdge() avg     
  PlayStartChirp();
  NavigationBegin();
  // Serial.begin(9600);
  SetAllPixelsRGB(0, 0, 0);

  xTaskCreate(TaskEdge, "edge", 128, NULL, 1, &xEdgeHandler);
  xTaskCreate(TaskCorner, "corner", 128, NULL, 1, &xCornerHandler);

  
  vTaskSuspend(xCornerHandler);
}

void TaskEdge(void* pvParameters) {
  NavigationBegin();

  for (;;) {
    Motors(0, 0);
    vTaskDelay(100/portTICK_PERIOD_MS); //to give it a second to stop

    SimpleGyroNavigation();
    SetPixelRGB(TAIL_TOP, 50, 0, 0);
    float duration = 2.5;
    float interval = 0.1;
    //pid loop
    //Jetts desk
    float Kp = 2.5;
    float Ki = 0.0;
    float Kd = 0.0;

    int error = 0;
    int error_last = 0;
    int error_total = 0;
    ZeroNavigation();
    vTaskDelay(100/portTICK_PERIOD_MS); //to give it a second to calibrate

    int target = GetDegrees(); //target is same as now

    int left = 80;
    int right = 80;
    int delta;

    //pid loop
    for (float time = duration; time > 0.0; time -= interval) {
      SimpleGyroNavigation();
      error = target - GetDegrees();
      error_total += error;

      delta = Kp*error + Ki*error_total + Kd*(error-error_last);

      int out_left = max(0, min(left + delta, 120));
      int out_right = max(0, min(right - delta, 120));

      //visualize motors
      SetPixelRGB(EYE_LEFT, 0, 0, out_left);
      SetPixelRGB(EYE_RIGHT, 0, 0, out_right);
      Motors(out_left, out_right);


      vTaskDelay(interval*1000/portTICK_PERIOD_MS);
      error_last = error;
    }

    //cleanup
    Motors(0, 0);
    OffEyes();

    //task swap
    vTaskResume(xCornerHandler);
    vTaskSuspend(NULL);
  }
}

void TaskCorner(void* pvParameters) {
  NavigationBegin();

  for (;;) {
    Motors(0, 0);
    vTaskDelay(100/portTICK_PERIOD_MS); //to give it a second to calibrate

    SetPixelRGB(TAIL_TOP, 0, 50, 0);
    SimpleNavigation();

    float interval = 0.1;
    //pid loop; constants for Jett's desk.
    // float Kp = 3.5;
    // float Ki = 0.25;
    // float Kd = 1.5;
    float Kp = 1.8;
    float Ki = 0.4;
    float Kd = 0.2;

    int error = 0;
    int error_last = 0;
    int error_total = 0;
    ZeroNavigation();
    vTaskDelay(100/portTICK_PERIOD_MS); //to give it a second to calibrate
    int current = GetDegrees();
    int target = (current + 90)%360; //target is same as now + 90

    int left = 0;
    int right = 0;
    int delta;

    //pid loop while angle not at target
    while(abs(target - current) > 1) {
      SimpleGyroNavigation();

      current = GetDegrees();
      error = target - current;
      error_total += error;

      delta = Kp*error + Ki*error_total + Kd*(error-error_last);

      int out_left = max(-80, min(left + delta, 80));
      int out_right = max(-80, min(right - delta, 80));

      //visualize motors
      if (out_left >= 0) {
        SetPixelRGB(EYE_LEFT, 0, out_left, 0);
      } else {  //red for reverse
        SetPixelRGB(EYE_LEFT, out_left, 0, 0);
      }
      if (out_right >= 0) {
        SetPixelRGB(EYE_RIGHT, 0, out_right, 0);
      } else {  //red for reverse
        SetPixelRGB(EYE_RIGHT, out_right, 0, 0);
      }
      Motors(out_left, out_right);


      vTaskDelay(interval*1000/portTICK_PERIOD_MS);
      error_last = error;
    }

    //cleanup
    Motors(0, 0);
    OffEyes();


    vTaskResume(xEdgeHandler);
    vTaskSuspend(NULL);
  }
}

int GetDegreesMod() {
  return (GetDegrees() % 360);
}

// void TaskTurn(void *pvParameters) {

//   for(;;) {
//     TickType_t xLastWakeTime = xTaskGetTickCount();
//     vTaskSuspend(xForwardHandler);
//     SetPixelRGB(TAIL_BOTTOM, 50, 0, 0);
//     ZeroNavigation();
//     RotateAccurate(90,1000);
//     SimpleGyroNavigation();
//     target = PresentHeading();
//     vTaskResume(xForwardHandler);
//     vTaskDelayUntil(&xLastWakeTime, 4000/portTICK_PERIOD_MS);
//   }
// }

// void TaskForward(void *pvParameters) { 
//   // target = PresentHeading(); old
//   NavigationBegin();

//   for(;;) {
//     vTaskSuspend(xTurnHandler);
//     int l_speed = 40;
//     int r_speed = 40;
//     int error;
//     int P = 5;
//     int output;
//     TickType_t xLastWakeTime = xTaskGetTickCount();
//     for (int i = 0; i < 30; i++) {
//       if (i == 0) {
//         error = 0;
//         SetPixelRGB(TAIL_BOTTOM, 50, 0, 20);
//       } else {
//         SimpleGyroNavigation();
//         error = target - PresentHeading();
//         SetPixelRGB(TAIL_BOTTOM, 50, 20, 0);
//       }
//       if(error > 0) {
//         SetPixelRGB(TAIL_TOP, 0, 50, 0);
//       } else {
//         SetPixelRGB(TAIL_TOP, 0, 0, 50);
//       }
//       output = P*error;

//       l_speed += output;
//       r_speed -= output;
//       l_speed = max(l_speed, 0);
//       l_speed = min(l_speed, 150);
//       r_speed = max(l_speed, 0);
//       r_speed = min(l_speed, 150);
      
//       Motors(l_speed, r_speed);
//       vTaskDelayUntil(&xLastWakeTime, 100/portTICK_PERIOD_MS);
//     }
//     // Motors(0,0);
//     vTaskResume(xTurnHandler);
//     vTaskDelay(1000/portTICK_PERIOD_MS);
//   }
// }

// void TaskTest(void *pvParameters) {
  // int x;
  // SetPixelRGB(TAIL_TOP, 30, 30, 30);
  // NavigationBegin();

  // for(;;) {
  //   SimpleGyroNavigation();
  //   x = GetDegrees();
  //   SetAllPixelsRGB(x, x, x);
  //   vTaskDelay(10000/portTICK_PERIOD_MS);
  // }
// }

void loop(){ 
} // end of loop() function




