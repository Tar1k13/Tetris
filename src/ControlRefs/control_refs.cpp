#include <ControlRefs/control_refs.h>

TaskHandle_t start_handle=NULL;
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
      vTaskDelete(start_handle);
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

void setup_select(){
    xTaskCreate(start_control, "start_control", 2000, NULL, 2, &start_handle);
    select_game();
  // vTaskDelete(start_handle);

    if(game=='T'){
        xTaskCreate(control_listener, "calculate_water_temp", 8000, &xHandle, 2, NULL);
        xTaskCreate(mainMoving, "main_moving", 16000, NULL, 1, &xHandle);
    } else{
        xTaskCreate(test_control, "calculate_water_temp", 8000, NULL, 2, NULL);
        xTaskCreate(snake, "snake", 18000, (void *) &xSnakeHandle, 1, &xSnakeHandle);
    }
}