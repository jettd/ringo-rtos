#include <Arduino_FreeRTOS.h>
#include "RingoHardware.h"
#include "Navigation.h"

volatile char edge;
int max_time = 0;
int accumulated_time = 0;
int num_samples = 0;

void setup(){
  HardwareBegin();        //initialize Ringo's brain to work with his circuitry
  SwitchMotorsToSerial(); //Call "SwitchMotorsToSerial()" before using Serial.print functions as motors & serial share a line
  RestartTimer();
  RxIRRestart(4);
  ResetLookAtEdge();      //Zero LookAtEdge() avg     
  PlayStartChirp();
  NavigationBegin();
  SwitchButtonToPixels();
  Serial.begin(9600);
}

void loop(){ 
  int start;
  int end;
  int t;
  SwitchAmbientToEdge();
  // seekWallStraight();
  for (int i=0; i < 500; i++) {
    SetPixelRGB(TAIL_BOTTOM, 50, 50, 50);
    edge = LookForEdge();
    delay(10);
    SetPixelRGB(TAIL_BOTTOM, 0, 0, 0);
    if (LeftDetected(edge) || RightDetected(edge)) {
      // start = millis();
      backwards();
      // end = millis();
      // t = end-start;
      // accumulated_time += t;
      // if (max_time < t) {
      //   max_time = t;
      // }
    } else {
      start = millis();
      forward();
      end = millis();
      t = end-start;
      accumulated_time += t;
      if (max_time < t) {
        max_time = t;
      }
      num_samples += 1;
    }
  }
  Motors(0, 0);
  SetPixelRGB(TAIL_TOP, 50, 0, 50);
  // Serial.println(accumulated_time);
  // Serial.println(max_time);
  for (int j = 0; j < (accumulated_time/num_samples); j++) {
    SetPixelRGB(TAIL_TOP, 0, 0, 60);
    delay(200);
    SetPixelRGB(TAIL_TOP, 0, 0, 0);
    delay(200);
  }
  delay(100000);
}// end of loop() function

void forward() {\
  int left_speed = 90;
  int right_speed = 120;
  SetPixelRGB(TAIL_TOP, 0, 50, 0);
  Motors(left_speed, right_speed);
  delay(5);
}

void backwards() {
  SetPixelRGB(TAIL_TOP, 50, 0, 0);
  Motors(0, 0);
  delay(10);
  backupTurn();
  delay(10);
}

void backupTurn() {
  Motors(-30, -65);
  delay(300);
  Motors(0, 0);
}

//ramp up speed over time code
    // if (edge_reset_flag) {
    //   left_speed = 60;
    //   right_speed = 75;
    //   edge_reset_flag = false;
    // } else if (until_ramp_counter < 0) {
    //   right_speed += ramp_turn_radius;
    //   left_speed -= ramp_turn_radius;
    //   right_speed = max(min(right_speed, max_allowed), min_allowed);
    //   left_speed = max(min(left_speed, max_allowed), min_allowed);
    //   until_ramp_counter = 100;
    //   LeftEye(0, left_speed/2, 0);
    //   RightEye(0, right_speed/2, 0);
    // } else {
    //   until_ramp_counter--;
    //   Motors(60, 90);
    // }
    // vTaskDelay(loopHold/portTICK_PERIOD_MS);
    // vTaskDelay(50/portTICK_PERIOD_MS);
    // SetAllPixelsRGB(0,0,0);

