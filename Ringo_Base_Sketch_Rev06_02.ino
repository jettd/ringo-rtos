#include <Arduino_FreeRTOS.h>
#include "RingoHardware.h"

//int i;    //declaire any global variables here

void taskLights( void *pvParameters);
void taskMove( void *pvParameters);
void taskStop (void *pvParameters);
void taskChirp(void *pvParameters);
void taskTest( void *pvParameters);

void setup(){
  HardwareBegin();        //initialize Ringo's brain to work with his circuitry
  PlayStartChirp();       //Play startup chirp and blink eyes
  SwitchMotorsToSerial(); //Call "SwitchMotorsToSerial()" before using Serial.print functions as motors & serial share a line
  RestartTimer();

  //SwitchButtonToPixels();

  xTaskCreate(
    TaskStop
    , (const portCHAR *)"stop"
    , 128
    , NULL
    , 1
    , NULL
  );

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

  xTaskCreate(TaskChirp, "chirp", 128, NULL, 4, NULL);
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

void TaskChirp(void *pvParameters) {
  int t = 280;
  int q_note = t/portTICK_PERIOD_MS;
  int h_note = q_note * 2;
  int w_note = h_note * 2;
  int amp = 20;
  int high_amp = amp*1.1;
  int q_rest = q_note;
  int h_rest = h_note;
  for(;;) {
    //"... give you up..."
    PlayChirp(NOTE_D4, amp);
    vTaskDelay(q_note); // Quarter note
    PlayChirp(NOTE_E4, amp);
    vTaskDelay(q_note);
    PlayChirp(NOTE_G4, amp);
    vTaskDelay(q_note);
    PlayChirp(NOTE_E4, amp);
    vTaskDelay(q_note);
    PlayChirp(NOTE_B4, high_amp);
    vTaskDelay(h_note); // Half note
    PlayChirp(NOTE_B4, high_amp);
    vTaskDelay(q_note); // Quarter note again
    PlayChirp(NOTE_A4, high_amp);
    vTaskDelay(q_note);

    OffChirp();
    vTaskDelay(h_rest);

    //...let you down...
    PlayChirp(NOTE_D4, amp);
    vTaskDelay(q_note);
    PlayChirp(NOTE_E4, amp);
    vTaskDelay(q_note);
    PlayChirp(NOTE_G4, amp);
    vTaskDelay(q_note);
    PlayChirp(NOTE_E4, amp);
    vTaskDelay(q_note);
    PlayChirp(NOTE_A4, amp);
    vTaskDelay(h_note); // Half note
    PlayChirp(NOTE_A4, high_amp);
    vTaskDelay(q_note); // Quarter note again
    PlayChirp(NOTE_G4, high_amp);
    vTaskDelay(h_note);
    PlayChirp(NOTE_D4, high_amp);

    OffChirp();
    vTaskDelay(h_rest);

    //...run around...
    PlayChirp(NOTE_D4, amp);
    vTaskDelay(q_note); // Quarter note
    PlayChirp(NOTE_E4, amp);
    vTaskDelay(q_note);
    PlayChirp(NOTE_G4, amp);
    vTaskDelay(q_note);
    PlayChirp(NOTE_E4, amp);
    vTaskDelay(q_note);
    PlayChirp(NOTE_G4, amp);
    vTaskDelay(q_note);
    PlayChirp(NOTE_A4, amp);
    vTaskDelay(q_note);
    PlayChirp(NOTE_F4, amp); //maybe 4?
    vTaskDelay(q_note);
    PlayChirp(NOTE_D4, amp);
    vTaskDelay(h_note);
    PlayChirp(NOTE_D4, amp);
    vTaskDelay(q_note);
    PlayChirp(NOTE_A4, high_amp);
    vTaskDelay(h_note);
    PlayChirp(NOTE_G4, high_amp);
    vTaskDelay(h_note);

    OffChirp();
    vTaskDelay(2*h_rest);

    //..make you cry..
    PlayChirp(NOTE_D4, amp);
    vTaskDelay(q_note);
    PlayChirp(NOTE_E4, amp);
    vTaskDelay(q_note);
    PlayChirp(NOTE_G4, amp);
    vTaskDelay(q_note);
    PlayChirp(NOTE_E4, amp);
    vTaskDelay(q_note);
    PlayChirp(NOTE_B4, amp);
    vTaskDelay(h_note);
    PlayChirp(NOTE_B4, amp);
    vTaskDelay(q_note);
    PlayChirp(NOTE_A4, amp);
    vTaskDelay(h_note);

    OffChirp();
    vTaskDelay(q_rest);

    //...say goodbye...
    vTaskDelay(q_note); // Quarter note
    PlayChirp(NOTE_E4, amp);
    vTaskDelay(q_note);
    PlayChirp(NOTE_G4, amp);
    vTaskDelay(q_note);
    PlayChirp(NOTE_E4, amp);
    vTaskDelay(q_note);
    PlayChirp(NOTE_D5, high_amp); // lower octave D, need to adjust frequency if available
    vTaskDelay(h_note);
    PlayChirp(NOTE_FS4, high_amp);
    vTaskDelay(h_note);
    PlayChirp(NOTE_G4, high_amp);
    vTaskDelay(h_note); // Half note

    OffChirp();
    vTaskDelay(q_rest);

    //...tell a like
    PlayChirp(NOTE_D4, amp);
    vTaskDelay(q_note); // Quarter note
    PlayChirp(NOTE_E4, amp);
    vTaskDelay(q_note);
    PlayChirp(NOTE_G4, amp);
    vTaskDelay(q_note);
    PlayChirp(NOTE_E4, amp);
    vTaskDelay(q_note);
    PlayChirp(NOTE_G4, amp);
    vTaskDelay(h_note);
    PlayChirp(NOTE_A4, amp);
    vTaskDelay(q_note);
    PlayChirp(NOTE_FS4, amp);
    vTaskDelay(h_note);
    PlayChirp(NOTE_D5, amp); // lower octave D, need to adjust frequency if available
    vTaskDelay(h_note);
    PlayChirp(NOTE_A4, amp);
    vTaskDelay(h_note);
    PlayChirp(NOTE_G4, amp);
    vTaskDelay(w_note);

    OffChirp();
    vTaskDelay(3000/portTICK_PERIOD_MS);
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




