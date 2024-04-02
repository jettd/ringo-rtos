#include <Arduino_FreeRTOS.h>
#include "RingoHardware.h"
#include "Navigation.h"

typedef enum
{
  START_s, 
  CLEAR_s,
  OBSTACLE_s
} state_t;

volatile state_t RINGO_STATE = START_s;
TaskHandle_t xTestHandler;

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

  xTaskCreate(TaskTest, "test", 128, NULL, 2, &xTestHandler);
  xTaskCreate(TaskStateManager, "state_machine", 128, NULL, 5, NULL);
  // xTaskCreate(TaskCorner, "corner", 128, NULL, 1, &xCornerHandler);
  vTaskSuspend(xTestHandler);
  RINGO_STATE = START_s;
}

void TaskStateManager(void* pvParameters) {
  for (;;) {
    switch (RINGO_STATE) {
      case START_s:
        SetPixelRGB(TAIL_TOP, 120, 120, 120);
        vTaskResume(xTestHandler);
        break;
      case CLEAR_s:
        SetPixelRGB(TAIL_TOP,0, 120, 0);
        break;
      case OBSTACLE_s:
        SetPixelRGB(TAIL_TOP, 120, 0, 0);
        break;
      default: 
        SetPixelRGB(TAIL_TOP, 150, 150, 150);
        break;

    }
    vTaskDelay(250/portTICK_PERIOD_MS);
  }
}

void TaskTest(void* pvParameters) {
  bool edge_reset_flag = false;
  int ramp_turn_radius = 2;
  int until_ramp_counter = 100;
  char edge;
  int LeftAverageHold, RightAverageHold;
  int loopHold = 15; //10 works good
  int left_speed = 60;
  int right_speed = 70;
  int turn_diff = 50;
  int max_allowed = 125;
  int min_allowed = 0;
  SwitchAmbientToEdge();
  seekWallStraight();
  for (;;) {
    Motors(left_speed, right_speed);
    RINGO_STATE = CLEAR_s;
    SimpleGyroNavigation();
    edge = LookForEdge();
    if (LeftDetected(edge) || RightDetected(edge)) {
      OnEyes(100, 0, 0);
      Motors(0, 0);
      vTaskDelay(100/portTICK_PERIOD_MS);
      // pidTurn(-90);
      backupTurn();
      vTaskDelay(loopHold/portTICK_PERIOD_MS);
      RINGO_STATE = OBSTACLE_s;
      edge_reset_flag = true;
    } 
    if (edge_reset_flag) {
      left_speed = 60;
      right_speed = 75;
      edge_reset_flag = false;
    } else if (until_ramp_counter < 0) {
      right_speed += ramp_turn_radius;
      left_speed -= ramp_turn_radius;
      right_speed = max(min(right_speed, max_allowed), min_allowed);
      left_speed = max(min(left_speed, max_allowed), min_allowed);
      until_ramp_counter = 100;
      LeftEye(0, left_speed/2, 0);
      RightEye(0, right_speed/2, 0);
    } else {
      until_ramp_counter--;
    }
    vTaskDelay(loopHold/portTICK_PERIOD_MS);
  }
}

void backupTurn() {
  // OnEyes(50, 50, 50)
  Motors(-30, -65);
  vTaskDelay(300/portTICK_PERIOD_MS);
  Motors(0, 0);
}

void seekWallStraight() {
  OnEyes(100, 0, 100);
  char edge;
  Motors(100, 100);
  edge = LookForEdge();
  while (!LeftDetected(edge) && !RightDetected(edge)) {
    edge = LookForEdge();
  }
  Motors(0, 0);
}

void pidTurn(int turn_deg) {
    Motors(0, 0);
    vTaskDelay(100/portTICK_PERIOD_MS); //to give it a second to calibrate

    // SetPixelRGB(TAIL_TOP, 50, 0, 50);
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
    int target = (current + turn_deg)%360; //target is same as now + 90

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
}

void loop(){ 
} // end of loop() function




