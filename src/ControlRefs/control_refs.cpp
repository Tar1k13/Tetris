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





void digit_animation(void *pvParameters){
  uint8_t SEG_DONE[] = {
	SEG_D,           
	SEG_D,  
	SEG_D,           
	SEG_D          
};
uint8_t seg_pos=0;
uint8_t SEGS[]={SEG_D,SEG_E,SEG_F,SEG_A,SEG_B,SEG_C};
bool direction=true;
  while(1){
    //4 digit
      if(direction){
        for(int i=0;i<4;i++){
          SEG_DONE[i]|=SEGS[seg_pos];
        }
      } else {
        for(int i=0;i<4;i++){
          SEG_DONE[i]^=SEGS[seg_pos];
          
          
        }
        
      }
      display.setSegments(SEG_DONE);
      seg_pos++;
        if(seg_pos>5){
          seg_pos=0;
          direction=!direction;
        } 
    if(sel){
      display.showNumberDec(0,true);
      vTaskDelete(NULL);
    } 
    vTaskDelay(200/portTICK_RATE_MS);
  }
}


void select_game(){
  while(!sel){
  for (size_t i = 8; i < 16; i++){
    mx.setColumn(i,IMAGES[0][i-8]);
  };
  for (size_t i = 0; i < 8; i++){
    mx.setColumn(i,IMAGES[2][i]);
  };

    if(game=='S'){
      for (size_t i = 8; i < 16; i++){
        mx.setColumn(i,IMAGES[0][i-8]);
      }
      delay(500);
      for (size_t i = 8; i < 16; i++){
        mx.setColumn(i,IMAGES[1][i-8]);
      }
      delay(500);
    } 
    else{
      for (size_t i = 0; i < 8; i++){
        mx.setColumn(i,IMAGES[2][i]);
      }
      delay(500);
      for (size_t i = 0; i < 8; i++){
        mx.setColumn(i,IMAGES[3][i]);
      }
      delay(500);
    }
  }
  
  
  mx.clear();
  delay(50);
}

void setup_select(){
  sel=false;
    xTaskCreate(start_control, "start_control", 2000, NULL, 2, &start_handle);
    xTaskCreate(digit_animation,"digit_animation",2000,NULL,2,NULL);
    select_game();
    if(game=='T'){
        xTaskCreate(control_listener, "calculate_water_temp", 8000, &xHandle, 2, &xTetrisControlHandle);
        xTaskCreate(mainMoving, "main_moving", 16000, NULL, 1, &xHandle);
    } else{
      Serial.println("HUYUGG");
        xTaskCreate(test_control, "calculate_water_temp", 8000, NULL, 2, &xSnakeControlHandle);
        xTaskCreate(snake, "snake", 18000, (void *) &xSnakeHandle, 1, &xSnakeHandle);
    }
}