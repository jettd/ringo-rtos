#include <Arduino_FreeRTOS.h>
#include "RingoHardware.h"
#include "Navigation.h"

TaskHandle_t xEdgeHandler;
TaskHandle_t xCornerHandler;
volatile int target;

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
  // xTaskCreate(TaskTest, "test", 128, NULL, 10, NULL);

  
  vTaskSuspend(xCornerHandler);
}

void TaskEdge(void* pvParameters) {
  NavigationBegin();

  for (;;) {
    Motors(0, 0);
    SimpleGyroNavigation();
    SetPixelRGB(TAIL_TOP, 50, 0, 0);
    float duration = 5;
    float interval = 0.1;
    //pid loop
    float Kp = 5.0;
    float Ki = 0.0;
    float Kd = 0.0;

    int error = 0;
    int error_last = 0;
    int error_total = 0;
    ZeroNavigation();
    int target = GetDegrees(); //target is same as now

    int left = 30;
    int right = 30;
    int delta;

    //pid loop
    for (float time = duration; time > 0.0; time -= interval) {
      SimpleGyroNavigation();
      error = target - GetDegrees();
      error_total += error;

      delta = Kp*error + Ki*error_total + Kd*error_last;

      int out_left = max(0, min(left + delta, 60));
      int out_right = max(0, min(right - delta, 60));

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
    SetPixelRGB(TAIL_TOP, 0, 50, 0);
    //replace this 2 second delay with the correct use of navigation library to turn 90 degrees right.
    vTaskDelay(3000/portTICK_PERIOD_MS);


    ZeroNavigation();
    vTaskResume(xEdgeHandler);
    vTaskSuspend(NULL);
    // vTaskDelay(2000/port_TICK_PERIOD_MS);
  }
}

// void TaskEdge(void *pvParameters) {
//   for(;;) {}
//     //use PID control to go straight for x amount of time, then suspend self, start with simple P or PD control (by setting constants to 0)
//     //turn all lights white if near no error, if veering right (above threshold) turn red, if left turn them a light blue.

//     //probably use this while loop for pid?
//     while(/*maybe time based?*/) {
//       //Use "Motors(left, right", has range [-200,+200] and you should cap any PID commands to this for sure, if not [0,200]
//       //There is a ringo function PresentHeading but it does not loop at 360degrees and it requires lots of other ringo functions which are confusing. 
//     }
//   }

// }

// void TaskCorner(void *pvParameters) {
//   for(;;){
//   //use functions from navigation.ino, navigation.h to turn 90 degrees right
//   // ask about these if you are not sure, Ringo functions are super weird and do not always fit perfectly with an RTOS
//   }
// }




  // xTaskCreate(TaskTurn, "turn", 128, NULL, 2, &xTurnHandler);
  // xTaskCreate(TaskLight, "light", 128, NULL, 1, NULL);
  // xTaskCreate(TaskForward, "forward", 128, NULL, 1, &xForwardHandler);
  // xTaskCreate(TaskTest, "test", 128, NULL, 5, NULL);

  // vTaskSuspend(xTurnHandler);



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




