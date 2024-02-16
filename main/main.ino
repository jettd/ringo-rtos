#include <Arduino_FreeRTOS.h>
#include "RingoHardware.h"

char edge;
TaskHandle_t xRandomWalkHandle;
TaskHandle_t xTaskRickRollHandle;

void taskLights( void *pvParameters);
void taskIRLights( void *pvParameters);
void taskRandomWalk( void*pvParameters);
void taskFancyLights(void *pvParameters);
void taskRickRoll(void *pvParameters);
void taskSense(void *pvParameters);
void taskIRLights(void *pvParameters);

void setup(){
  HardwareBegin();        //initialize Ringo's brain to work with his circuitry
  SwitchMotorsToSerial(); //Call "SwitchMotorsToSerial()" before using Serial.print functions as motors & serial share a line
  RestartTimer();
  RxIRRestart(4);
  ResetLookAtEdge();      //Zero LookAtEdge() avg     

  xTaskCreate(TaskLights, "rainbow", 128, NULL, 2, NULL);
  xTaskCreate(TaskRandomWalk, "rwalk", 128, NULL, 1, &xRandomWalkHandle);
  xTaskCreate(TaskRickRoll, "rick", 128, NULL, 3, &xTaskRickRollHandle);
  xTaskCreate(TaskSense, "sense", 128, NULL, 11, NULL);
  // xTaskCreate(TaskFancyLights, "lights", 128, NULL, 5, NULL);
  // vTaskSuspend(xTaskRickRollHandle);
  // xTaskCreate(TaskIRLights, "ir", 128, NULL, 15, NULL);
}

void TaskIRLights(void *pvParameters) {
  (void) pvParameters;
  for(;;) {
    if(IsIRDone()) {
      cli();
      byte ir_button = GetIRButton();
      if(ir_button == 1) {
        SetAllPixelsRGB(100, 100, 100);
        vTaskResume(xTaskRickRollHandle);
      } else if (ir_button == 19) {
        SetPixelRGB(TAIL_BOTTOM, 0, 100, 0);
      } else if (ir_button == 20) {
        SetPixelRGB(TAIL_BOTTOM, 0, 0, 100);
      }
      sei();
    }
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
}

void TaskSense(void *pvParameters) { // Aperiodic handler task when we sense somemthing
  (void) pvParameters;
  for(;;){
    edge = LookForEdge();
    vTaskDelay(100/portTICK_PERIOD_MS);
    if FrontEdgeDetected(edge){
      SetAllPixelsRGB(0,100,100);
      vTaskSuspend(xRandomWalkHandle);         // Suspend random walk
      Motors(-50,-50);
      delay(1000);
      vTaskResume(xRandomWalkHandle);         // Restart running random walk
    } else{
      SetAllPixelsRGB(100,0,0);
    }

    vTaskDelay(100/portTICK_PERIOD_MS);    
  }
}

void TaskLights(void *pvParameters) { // Periodic Task
  (void) pvParameters;
  for (;;) {
    SetAllPixelsRGB(0,0,0);
    vTaskDelay(100/portTICK_PERIOD_MS);
    for(int i =0; i < 6; i++){
      SetPixelRGB(i, 80, 0, 0);
    }
    vTaskDelay(100/portTICK_PERIOD_MS);
    for(int i =0; i < 6; i++){
      SetPixelRGB(i, 80, 80, 0);
    }
    vTaskDelay(100/portTICK_PERIOD_MS);
    for(int i =0; i < 6; i++){
      SetPixelRGB(i, 0, 80, 0);
    }
    vTaskDelay(100/portTICK_PERIOD_MS);
    for(int i =0; i < 6; i++){
      SetPixelRGB(i, 0, 80, 80);
    }
    vTaskDelay(100/portTICK_PERIOD_MS);
    for(int i =0; i < 6; i++){
      SetPixelRGB(i, 0, 0, 80);
    }
    vTaskDelay(100/portTICK_PERIOD_MS);
    for(int i =0; i < 6; i++){
      SetPixelRGB(i, 80, 0, 80);
    }
    vTaskDelay(100/portTICK_PERIOD_MS);
    for(int i =0; i < 6; i++){
      SetPixelRGB(i, 80, 80, 80);
    }
    vTaskDelay(100/portTICK_PERIOD_MS);
    SetAllPixelsRGB(0,0,0);
    vTaskDelay(4000/portTICK_PERIOD_MS);
  }
}

void TaskRickRoll(void *pvParameters) { // Sporadic Task, don't respond to button press until song is over 
  (void) pvParameters;
  for(;;) {
    int bpm = 114; //reference
    int note = 2105/portTICK_PERIOD_MS;
    int h_note = note/2;
    int q_note = note/4;
    int e_note = note/8;
    int s_note = note/16;
    double dot = 1.5;
    int amp = 20;

    //s s s s e. e. q. 
    // never gonna give you up
    PlayChirp(NOTE_D4, amp);
    vTaskDelay(s_note);
    PlayChirp(NOTE_E4, amp);
    vTaskDelay(s_note);
    PlayChirp(NOTE_G4, amp);
    vTaskDelay(s_note);
    PlayChirp(NOTE_E4, amp);
    vTaskDelay(s_note);
    PlayChirp(NOTE_B5, amp);
    vTaskDelay(e_note*dot); 
    PlayChirp(NOTE_B5, amp);
    vTaskDelay(e_note*dot); 
    PlayChirp(NOTE_A5, amp);
    vTaskDelay(q_note*dot);

    //never gonna let you down
    // s s s s e. e. e. s e 
    PlayChirp(NOTE_D4, amp);
    vTaskDelay(s_note); 
    PlayChirp(NOTE_E4, amp);
    vTaskDelay(s_note);
    PlayChirp(NOTE_G4, amp);
    vTaskDelay(s_note);
    PlayChirp(NOTE_E4, amp);
    vTaskDelay(s_note);
    PlayChirp(NOTE_A5, amp);
    vTaskDelay(e_note*dot); 
    PlayChirp(NOTE_A5, amp);
    vTaskDelay(e_note*dot);
    PlayChirp(NOTE_G4, amp);
    vTaskDelay(e_note*dot);
    PlayChirp(NOTE_F4, amp);
    vTaskDelay(s_note);
    PlayChirp(NOTE_E4, amp);
    vTaskDelay(e_note);

    //never gonna run around and desert you    
    // s s s s q e e. s e e e q h 
    PlayChirp(NOTE_D4, amp);
    vTaskDelay(s_note); 
    PlayChirp(NOTE_E4, amp);
    vTaskDelay(s_note);
    PlayChirp(NOTE_G4, amp);
    vTaskDelay(s_note);
    PlayChirp(NOTE_E4, amp);
    vTaskDelay(s_note);
    PlayChirp(NOTE_G4, amp);
    vTaskDelay(q_note);
    PlayChirp(NOTE_A5, amp);
    vTaskDelay(e_note);
    PlayChirp(NOTE_F4, amp);
    vTaskDelay(e_note*dot);
    PlayChirp(NOTE_E4, amp);
    vTaskDelay(s_note);
    PlayChirp(NOTE_D4, amp);
    vTaskDelay(e_note);
    PlayChirp(NOTE_D4, amp);
    vTaskDelay(e_note);
    PlayChirp(NOTE_D4, amp);
    vTaskDelay(e_note);
    PlayChirp(NOTE_A5, amp);
    vTaskDelay(q_note);
    PlayChirp(NOTE_G4, amp);
    vTaskDelay(h_note);

    // never gonna make you cry
    // s s s s e. e. q. 
    PlayChirp(NOTE_D4, amp);
    vTaskDelay(s_note);
    PlayChirp(NOTE_E4, amp);
    vTaskDelay(s_note);
    PlayChirp(NOTE_G4, amp);
    vTaskDelay(s_note);
    PlayChirp(NOTE_E4, amp);
    vTaskDelay(s_note);
    PlayChirp(NOTE_B5, amp);
    vTaskDelay(e_note*dot); 
    PlayChirp(NOTE_B5, amp);
    vTaskDelay(e_note*dot); 
    PlayChirp(NOTE_A5, amp);
    vTaskDelay(q_note*dot);

    //never gonna say goodbye
    // s s s s q e e. s e 
    PlayChirp(NOTE_D4, amp);
    vTaskDelay(s_note);
    PlayChirp(NOTE_E4, amp);
    vTaskDelay(s_note);
    PlayChirp(NOTE_G4, amp);
    vTaskDelay(s_note);
    PlayChirp(NOTE_E4, amp);
    vTaskDelay(s_note);
    PlayChirp(NOTE_D5, amp);
    vTaskDelay(q_note); 
    PlayChirp(NOTE_F4, amp);
    vTaskDelay(e_note); 
    PlayChirp(NOTE_G4, amp);
    vTaskDelay(e_note*dot);
    PlayChirp(NOTE_F4, amp);
    vTaskDelay(s_note); 
    PlayChirp(NOTE_E4, amp);
    vTaskDelay(e_note);

    //never gonna tell a lie and hurt you
    // s s s s q e e. s q e q h
    PlayChirp(NOTE_D4, amp);
    vTaskDelay(s_note);
    PlayChirp(NOTE_E4, amp);
    vTaskDelay(s_note);
    PlayChirp(NOTE_G4, amp);
    vTaskDelay(s_note);
    PlayChirp(NOTE_E4, amp);
    vTaskDelay(s_note);
    PlayChirp(NOTE_G4, amp);
    vTaskDelay(q_note);
    PlayChirp(NOTE_A5, amp);
    vTaskDelay(e_note);
    PlayChirp(NOTE_F4, amp);
    vTaskDelay(e_note*dot);
    PlayChirp(NOTE_E4, amp);
    vTaskDelay(s_note);
    PlayChirp(NOTE_D4, amp);
    vTaskDelay(q_note);
    PlayChirp(NOTE_D4, amp);
    vTaskDelay(e_note);
    PlayChirp(NOTE_A5, amp);
    vTaskDelay(q_note);
    PlayChirp(NOTE_G4, amp);
    vTaskDelay(h_note);

    OffChirp();
    vTaskDelay(4000/portTICK_PERIOD_MS);
  }
}

void TaskRandomWalk(void *pvParameters) { 
  TickType_t xLastWakeTime = xTaskGetTickCount();

  for(;;) {
    int l = random(20, 50);
    int r = random(20, 50);
    Motors(l, r);
    vTaskDelayUntil(&xLastWakeTime, 500/portTICK_PERIOD_MS);
  }
}

void loop(){ 
} // end of loop() function




