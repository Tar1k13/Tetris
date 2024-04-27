#include <Arduino.h>
#include "Constants.h"
#include <music/music.h>
#include <snake/snake.h>
#include <Tetris/tetris.h>


TaskHandle_t xHandle = NULL;
TaskHandle_t xSnakeHandle = NULL;


const uint8_t IMAGES[][8] = { 
{                  //S
  0b00000000,
  0b00111100,
  0b01100110,
  0b00000110,
  0b00111100,
  0b01100000,
  0b01100110,
  0b00111100
},
{                  //Filled S
  0b11111111,
  0b11000011,
  0b10011001,
  0b11111001,
  0b11000011,
  0b10011111,
  0b10011001,
  0b11000011
},
{                 //T                 
  0b00000000,
  0b01111110,
  0b01011010,
  0b00011000,
  0b00011000,
  0b00011000,
  0b00011000,
  0b00011000
},
{                   //FILLED T
  0b11111111,
  0b10000001,
  0b10100101,
  0b11100111,
  0b11100111,
  0b11100111,
  0b11100111,
  0b11100111
}};
const int IMAGES_LEN = sizeof(IMAGES)/8;

char game='T';
bool sel;
void start_control(void *pvParameters){
  while(1){
    if(digitalRead(RIGHT_BUTTON)==LOW){
      game='T';
    }

    if(digitalRead(LEFT_BUTTON)==LOW){
      game='S';
    }

    if(digitalRead(ROTATE_BUTTON)==LOW){
      sel=true;
    }
    vTaskDelay(30/portTICK_RATE_MS);
  }
}
void select_game(){
  
  while(!sel){
    if(game=='S'){
      mx.clear();
      for (size_t i = 8; i < 16; i++){
        mx.setColumn(i,IMAGES[0][i-8]);
      }
      delay(500);
      mx.clear();
      for (size_t i = 8; i < 16; i++){
        mx.setColumn(i,IMAGES[1][i-8]);
      }
      delay(500);
    } 
    else{
      mx.clear();
      for (size_t i = 0; i < 8; i++){
        mx.setColumn(i,IMAGES[2][i]);
      }
      delay(500);
      mx.clear();
      for (size_t i = 0; i < 8; i++){
        mx.setColumn(i,IMAGES[3][i]);
      }
      delay(500);
    }
  }
  
  
  mx.clear();
}

void setup() {
  Serial.begin(9600);
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
  TaskHandle_t start_handle=NULL;
  xTaskCreate(start_control, "start_control", 2000, NULL, 2, &start_handle);
  select_game();
  vTaskDelete(start_handle);
  if(game=='T'){
    xTaskCreate(control_listener, "calculate_water_temp", 8000, &xHandle, 2, NULL);
    xTaskCreate(mainMoving, "main_moving", 16000, NULL, 1, &xHandle);
  } else{
    xTaskCreate(test_control, "calculate_water_temp", 8000, NULL, 2, NULL);
    xTaskCreate(snake, "snake", 18000, (void *) &xSnakeHandle, 1, &xSnakeHandle);
  }

  play_intro();



}

void loop() {
  delay(100000);
}
