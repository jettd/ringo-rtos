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
// TaskHandle_t xNavHandler;
TaskHandle_t xMotorHandler;
TaskHandle_t xSenseHandler;

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

  // xTaskCreate(TaskNav, "test", 128, NULL, 2, &xNavHandler);
  xTaskCreate(TaskMotorControl, "motors", 128, NULL, 10, NULL);//&xMotorHandler);
  xTaskCreate(TaskSense, "sense", 128, NULL, 30, NULL);//&xSenseHandler);
  // vTaskSuspend(xNavHandler);
}

void TaskSense(void* pvParameters) {
  int loop_delay = 10; //5-15 seems to work good with the current setup
  SwitchAmbientToEdge();
  ResetLookAtEdge();

  for (;;) {
    edge = LookForEdge();
    vTaskDelay(loop_delay/portTICK_PERIOD_MS);
  }
}

void TaskMotorControl(void * pvParameters) {
  Motors(30, 30); //start straight, need to find a wall
  vTaskDelay(1000);
  OnEyes(50, 0, 0);
  
  for (;;) {
    if (LeftDetected(edge) || RightDetected(edge)) {
      backupTurn();
    } else {
      Motors(40, 110);
    }
    vTaskDelay(5/portTICK_PERIOD_MS);
  }
}

void TaskNav(void* pvParameters) {
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
  Motors(100, 100);
  edge = LookForEdge();
  while (!LeftDetected(edge) && !RightDetected(edge)) {
    edge = LookForEdge();
  }
  Motors(0, 0);
}


void loop(){ 
} // end of loop() function




