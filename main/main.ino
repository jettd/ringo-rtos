#include <Arduino_FreeRTOS.h>
#include "RingoHardware.h"

//int i;    //declaire any global variables here
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

void TaskFancyLights(void *pvParameters) {
  (void) pvParameters;
  int note = 2105/portTICK_PERIOD_MS; // Base note duration for timing
  int h_note = note/2;
  int q_note = note/4;
  int e_note = note/8;
  int s_note = note/16;
  double dot = 1.5;

  for (;;) {
    // Reset lights at the beginning
    SetAllPixelsRGB(0, 0, 0);
    vTaskDelay(100/portTICK_PERIOD_MS);

    // Firework Effect: From tail to head, then eyes as finale
    int fireworkColors[][3] = {{255, 69, 0}, {255, 215, 0}, {65, 105, 225}, {34, 139, 34}, {148, 0, 211}, {255, 20, 147}};
    for (int i = 5; i >= 2; i--) { // Start from TAIL_BOTTOM to BODY_TOP
      SetPixelRGB(i, fireworkColors[5-i][0], fireworkColors[5-i][1], fireworkColors[5-i][2]);
      vTaskDelay(q_note);
    }
    // Eyes finale
    SetPixelRGB(0, fireworkColors[4][0], fireworkColors[4][1], fireworkColors[4][2]); // EYE_LEFT
    SetPixelRGB(1, fireworkColors[5][0], fireworkColors[5][1], fireworkColors[5][2]); // EYE_RIGHT
    vTaskDelay(h_note);
    SetAllPixelsRGB(0, 0, 0); // Reset after explosion
    vTaskDelay(s_note);

    // Random flashes with a focus on eyes for "never gonna give you up"
    for (int beat = 0; beat < 16; beat++) {
      SetPixelRGB(0, rand() % 256, rand() % 256, rand() % 256); // EYE_LEFT
      SetPixelRGB(1, rand() % 256, rand() % 256, rand() % 256); // EYE_RIGHT
      if (beat % 4 == 0) { // Every fourth beat, light up the body and tail
        SetPixelRGB(2, 255, 255, 255); // BODY_TOP
        SetPixelRGB(5, 255, 255, 255); // TAIL_BOTTOM
      }
      vTaskDelay(s_note);
      SetAllPixelsRGB(0, 0, 0); // Reset quickly for a strobe effect
      vTaskDelay(s_note);
    }

    // Chase effect: Simulate a "running" pattern from head to tail
    for (int chase = 0; chase < 6; chase++) {
      SetAllPixelsRGB(0, 0, 0); // Reset before starting chase
      for (int i = 0; i < 6; i++) {
        SetPixelRGB((chase + i) % 6, 0, (255 - (42 * i)), (42 * i)); // Create a gradient effect
        vTaskDelay(s_note / 2);
      }
    }

    // Strobe effect with eyes and body for the chorus, tail for rhythm
    for (int strobe = 0; strobe < 20; strobe++) {
      SetPixelRGB(0, 255, 255, 255); // EYE_LEFT
      SetPixelRGB(1, 255, 255, 255); // EYE_RIGHT
      if (strobe % 2 == 0) {
        SetPixelRGB(3, 255, 0, 0); // BODY_BOTTOM
        SetPixelRGB(4, 255, 0, 0); // TAIL_TOP
      } else {
        SetPixelRGB(2, 0, 255, 0); // BODY_TOP
        SetPixelRGB(5, 0, 255, 0); // TAIL_BOTTOM
      }
      vTaskDelay(s_note / 4);
      SetAllPixelsRGB(0, 0, 0); // Off
      vTaskDelay(s_note / 4);
    }

    // Breathing effect: Slow glow in the body, with eyes and tail providing accents
    for (int breath = 0; breath < 255; breath += 15) {
      SetPixelRGB(2, breath, 0, breath); // BODY_TOP
      SetPixelRGB(3, 0, breath, breath); // BODY_BOTTOM
      if (breath % 30 == 0) { // Accent with eyes and tail
        SetPixelRGB(0, 255 - breath, breath, 255); // EYE_LEFT
        SetPixelRGB(1, breath, 255 - breath, 255); // EYE_RIGHT
        SetPixelRGB(4, 255, 255 - breath, breath); // TAIL_TOP
        SetPixelRGB(5, breath, 255, 255 - breath); // TAIL_BOTTOM
      }
      vTaskDelay(s_note / 4);
    }
    for (int breath = 255; breath > 0; breath -= 15) {
      SetPixelRGB(2, breath, 0, breath); // BODY_TOP
      SetPixelRGB(3, 0, breath, breath); // BODY_BOTTOM
      if (breath % 30 == 0) {
        SetPixelRGB(0, 255 - breath, breath, 255); // EYE_LEFT
        SetPixelRGB(1, breath, 255 - breath, 255); // EYE_RIGHT
        SetPixelRGB(4, 255, 255 - breath, breath); // TAIL_TOP
        SetPixelRGB(5, breath, 255, 255 - breath); // TAIL_BOTTOM
      }
      vTaskDelay(s_note / 4);
    }

    // Reset LEDs before the loop repeats
    SetAllPixelsRGB(0, 0, 0); // Off
    vTaskDelay(4000/portTICK_PERIOD_MS); // Pause between loops
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




