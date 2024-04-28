#include <Arduino.h>
#include "Constants.h"
#include <music/music.h>
#include <snake/snake.h>
#include <Tetris/tetris.h>
#include <ControlRefs/control_refs.h>


// TaskHandle_t xHandle = NULL;
// TaskHandle_t xSnakeHandle = NULL;





// char game='T';
// bool sel;
// void start_control(void *pvParameters){
//   while(1){
//     if(digitalRead(RIGHT_BUTTON)==LOW){
//       game='T';
//     }

//     if(digitalRead(LEFT_BUTTON)==LOW){
//       game='S';
//     }

//     if(digitalRead(ROTATE_BUTTON)==LOW){
//       sel=true;

//     }
//     vTaskDelay(30/portTICK_RATE_MS);
//   }
// }
// void select_game(){
  
//   while(!sel){
//     if(game=='S'){
//       mx.clear();
//       for (size_t i = 8; i < 16; i++){
//         mx.setColumn(i,IMAGES[0][i-8]);
//       }
//       delay(500);
//       mx.clear();
//       for (size_t i = 8; i < 16; i++){
//         mx.setColumn(i,IMAGES[1][i-8]);
//       }
//       delay(500);
//     } 
//     else{
//       mx.clear();
//       for (size_t i = 0; i < 8; i++){
//         mx.setColumn(i,IMAGES[2][i]);
//       }
//       delay(500);
//       mx.clear();
//       for (size_t i = 0; i < 8; i++){
//         mx.setColumn(i,IMAGES[3][i]);
//       }
//       delay(500);
//     }
//   }
  
  
//   mx.clear();
// }

void power(void *pvParameters){
  while(true){
    if(digitalRead(13)==LOW){
    long pressed=millis();
    while(digitalRead(13)==LOW){
      if(millis()-pressed > 2000){
        // digitalWrite(0,HIGH);
        digitalWrite(2,LOW);
        continue;
      }
    }
  }
  vTaskDelay(50/portTICK_RATE_MS);
  }
}



void setup() {
  Serial.begin(115200);
  pinMode(2,OUTPUT);
  pinMode(13,INPUT_PULLUP);
  digitalWrite(2,HIGH);
  randomSeed(0);
  xTaskCreate(power,"power",4000,NULL,3,NULL);
  
  mx.begin();                 // led matrix initialization
  display.setBrightness(7);
  display.showNumberDec(0000);

  randomSeed(analogRead(0));  // randomizer
  pinMode(LEFT_BUTTON, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);
  pinMode(ROTATE_BUTTON, INPUT_PULLUP);
  pinMode(SPEED_BUTTON, INPUT);
  pinMode(SKIP_BUTTON, INPUT_PULLUP);
  // pinMode(33,OUTPUT);
  // digitalWrite(LEFT_BUTTON,HIGH);

  // TaskHandle_t start_handle=NULL;
  // xTaskCreate(start_control, "start_control", 2000, NULL, 2, &start_handle);
  
  // select_game();
  // // vTaskDelete(start_handle);

  // if(game=='T'){
  //   xTaskCreate(control_listener, "calculate_water_temp", 8000, &xHandle, 2, NULL);
  //   xTaskCreate(mainMoving, "main_moving", 16000, NULL, 1, &xHandle);
  // } else{
  //   xTaskCreate(test_control, "calculate_water_temp", 8000, NULL, 2, NULL);
  //   xTaskCreate(snake, "snake", 18000, (void *) &xSnakeHandle, 1, &xSnakeHandle);
  // }
  setup_select();
  // play_intro();



}

void loop() {
  delay(100000);
}
