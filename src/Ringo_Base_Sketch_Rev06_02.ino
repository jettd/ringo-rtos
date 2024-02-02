#include <Arduino_FreeRTOS.h>
#include "RingoHardware.h"

//int i;    //declaire any global variables here

void taskLights( void *pvParameters);
void taskMove( void *pvParameters);
void taskStop (void *pvParameters);
void taskChirp(void *pvParameters);
void taskRickRoll(void *pvParameters);
void taskTest( void *pvParameters);

void setup(){
  HardwareBegin();        //initialize Ringo's brain to work with his circuitry
  PlayStartChirp();       //Play startup chirp and blink eyes
  SwitchMotorsToSerial(); //Call "SwitchMotorsToSerial()" before using Serial.print functions as motors & serial share a line
  RestartTimer();

  //SwitchButtonToPixels();

  // xTaskCreate(
  //   TaskStop
  //   , (const portCHAR *)"stop"
  //   , 128
  //   , NULL
  //   , 1
  //   , NULL
  // );

  // xTaskCreate(
  //   TaskMove
  //   , "move"
  //   , 128
  //   , NULL 
  //   , 2
  //   , NULL 
  // );

  // xTaskCreate(
  //   TaskLights
  //   , "lights"
  //   , 128
  //   , NULL
  //   , 3
  //   , NULL
  // );

  xTaskCreate(TaskRickRoll, "rick", 128, NULL, 4, NULL);
}

void TaskLights(void *pvParameters) {
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
    vTaskDelay(500/portTICK_PERIOD_MS);
    SetAllPixelsRGB(0,0,0);
    vTaskDelay(3000/portTICK_PERIOD_MS);
  }
}

void TaskMove(void *pvParameters) {
  (void) pvParameters;
  for (;;) {
    Motors(30, 30);
    SetPixelRGB(TAIL_TOP, 0, 255, 0);
    vTaskDelay(2000/portTICK_PERIOD_MS);
    // Motors(0, 0);
    // vTaskDelay(1000/portTICK_PERIOD_MS);
  }
}

void TaskStop(void *pvParameters) {
  for(;;) {
    Motors(0, 0);
    SetPixelRGB(TAIL_TOP, 255, 0, 0);
    vTaskDelay(2500/portTICK_PERIOD_MS);
  }
}

void TaskRickRoll(void *pvParameters) {
  (void) pvParameters;
  for(;;) {
    int bpm = 114; //reference
    int note = 2105/portTICK_PERIOD_MS;
    int h_note = note/2;
    int q_note = note/4;
    int e_note = note/8;
    int s_note = note/16;
    double dot = 1.5;
    int amp = 50;

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


void TaskTest(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  // initialize digital LED_BUILTIN on pin 13 as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  for (;;) // A Task shall never return or exit.
  {
    PlayChirp(1000, 50);

    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    vTaskDelay( 500 / portTICK_PERIOD_MS ); // wait for one second

    PlayChirp(2000, 50);
    digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
    vTaskDelay( 500 / portTICK_PERIOD_MS ); // wait for one second

    PlayChirp(1000, 25);
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    vTaskDelay( 500 / portTICK_PERIOD_MS ); // wait for one second

    digitalWrite(LED_BUILTIN, LOW);
    OffChirp();

    vTaskDelay( 10000 / portTICK_PERIOD_MS );
  }
}
    
void loop(){ 
} // end of loop() function





