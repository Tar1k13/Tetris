#include <music/music.h>

int melody[] = {
  NOTE_E5, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_C5, NOTE_B4,
  NOTE_A4, NOTE_A4, NOTE_C5, NOTE_E5, NOTE_D5, NOTE_C5,
  NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5,
  NOTE_C5, NOTE_A4, NOTE_A4, NOTE_A4, NOTE_B4, NOTE_C5,
  
  NOTE_D5, NOTE_F5, NOTE_A5, NOTE_G5, NOTE_F5,
  NOTE_E5, NOTE_C5, NOTE_E5, NOTE_D5, NOTE_C5,
  NOTE_B4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5,
  NOTE_C5, NOTE_A4, NOTE_A4, 0, 
  
//   NOTE_E5, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_C5, NOTE_B4,
//   NOTE_A4, NOTE_A4, NOTE_C5, NOTE_E5, NOTE_D5, NOTE_C5,
//   NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5,
//   NOTE_C5, NOTE_A4, NOTE_A4, NOTE_A4, NOTE_B4, NOTE_C5,
  
//   NOTE_D5, NOTE_F5, NOTE_A5, NOTE_G5, NOTE_F5,
//   NOTE_E5, NOTE_C5, NOTE_E5, NOTE_D5, NOTE_C5,
//   NOTE_B4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5,
//   NOTE_C5, NOTE_A4, NOTE_A4, 0, 
  
//   NOTE_E5, NOTE_C5,
//   NOTE_D5, NOTE_B4,
//   NOTE_C5, NOTE_A4,
//   NOTE_GS4, NOTE_B4, 0, 
//   NOTE_E5, NOTE_C5,
//   NOTE_D5, NOTE_B4,
//   NOTE_C5, NOTE_E5, NOTE_A5,
//   NOTE_GS5
};


int durations[] = {
  4, 8, 8, 4, 8, 8,
  4, 8, 8, 4, 8, 8,
  4, 8, 4, 4,
  4, 4, 8, 4, 8, 8,
  
  4, 8, 4, 8, 8,
  4, 8, 4, 8, 8,
  4, 8, 8, 4, 4,
  4, 4, 4, 4,
  
//   4, 8, 8, 4, 8, 8,
//   4, 8, 8, 4, 8, 8,
//   4, 8, 4, 4,
//   4, 4, 8, 4, 8, 8,
  
//   4, 8, 4, 8, 8,
//   4, 8, 4, 8, 8,
//   4, 8, 8, 4, 4,
//   4, 4, 4, 4,
  
//   2, 2,
//   2, 2,
//   2, 2,
//   2, 4, 8, 
//   2, 2,
//   2, 2,
//   4, 4, 2,
//   2
};
int game_over_melody[]={
    NOTE_E5,NOTE_A4
};
int duration1[]={
    8,4
};
// void play_music(void *pvParameters){
//   while(1){
//     int size = sizeof(durations) / sizeof(int);

//   for (int note = 0; note < size; note++) {
//     //to calculate the note duration, take one second divided by the note type.
//     //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
//     int duration = 1000 / durations[note];
//     tone(33, melody[note], duration);

//     //to distinguish the notes, set a minimum time between them.
//     //the note's duration + 30% seems to work well:
//     int pauseBetweenNotes = duration * 1.30;
//     vTaskDelay(pauseBetweenNotes/portTICK_RATE_MS);
    
//     //stop the tone playing:
//     noTone(33);
//   }
//   }
// }

void play_intro(){
    int size = sizeof(durations) / sizeof(int);

  for (int note = 0; note < size; note++) {
    //to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int duration = 1000 / durations[note];
    tone(33, melody[note], duration);

    //to distinguish the notes, set a minimum time between them.
    //the note's duration + 30% seems to work well:
    int pauseBetweenNotes = duration * 1.30;
    delay(pauseBetweenNotes);
    
    //stop the tone playing:
    noTone(33);
  }
}

void play_game_over(){
    int size = sizeof(duration1) / sizeof(int);
    for (int note = 0; note < size; note++) {
        int duration = 1000 / duration1[note];
        tone(33, game_over_melody[note],duration);
        int pauseBetweenNotes = duration * 1.30;
        vTaskDelay(pauseBetweenNotes/portTICK_RATE_MS);
    
        noTone(33);
    }
}