#include <Arduino.h>
// #include <helperFuncs/calFunc.h>
// #include <movementFuncs/MovementFuncs.h>
// #include <MD_Parola.h>
#include "Constants.h"
#include <music/music.h>
#include <snake/snake.h>
#include <Tetris/tetris.h>


TaskHandle_t xHandle = NULL;
TaskHandle_t xSnakeHandle = NULL;
void setup() {
  Serial.begin(9600);
  mx.begin();                 // led matrix initialization
  display.setBrightness(7);
  display.showNumberDec(0000);

  randomSeed(analogRead(0));  // randomizer
  pinMode(LEFT_BUTTON, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON, INPUT_PULLUP);
  pinMode(ROTATE_BUTTON, INPUT_PULLUP);
  pinMode(SPEED_BUTTON, INPUT_PULLUP);
  pinMode(SKIP_BUTTON, INPUT_PULLUP);
  pinMode(33,OUTPUT);
 
  
  xTaskCreate(control_listener, "calculate_water_temp", 8000, &xHandle, 2, NULL);
  xTaskCreate(mainMoving, "main_moving", 16000, NULL, 1, &xHandle);
  play_intro();

  // xTaskCreate(test_control, "calculate_water_temp", 8000, NULL, 2, NULL);
  // xTaskCreate(snake, "snake", 18000, (void *) &xSnakeHandle, 1, &xSnakeHandle);

}

void loop() {
  delay(100000);
}
