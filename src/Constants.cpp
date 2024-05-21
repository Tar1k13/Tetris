#include <Constants.h>

const int figures[7][4][5]={{{0x0, 0x10, 0x18, 0x10, 0x0},  // T
                         {0x0, 0x0, 0x38, 0x10, 0x0},
                         {0x0, 0x10, 0x30, 0x10, 0x0},
                         {0x0, 0x10, 0x38, 0x0, 0x0}},
                        {{0x0, 0x8, 0x8, 0x8, 0x8},  // I
                         {0x0, 0x0, 0x3C, 0x0, 0x0},
                         {0x0, 0x8, 0x8, 0x8, 0x8},
                         {0x0, 0x0, 0x3C, 0x0, 0x0}},
                        {{0x0, 0x0, 0x18, 0x18, 0x0},  // O
                         {0x0, 0x0, 0x18, 0x18, 0x0},
                         {0x0, 0x0, 0x18, 0x18, 0x0},
                         {0x0, 0x0, 0x18, 0x18, 0x0}},
                        {{0x0, 0xC, 0x8, 0x8, 0x0},  // L
                         {0x0, 0x0, 0x1C, 0x4, 0x0},
                         {0x0, 0x8, 0x8, 0x18, 0x0},
                         {0x0, 0x10, 0x1C, 0x0, 0x0}},
                        {{0x0, 0xC, 0x8, 0x8, 0x0},  // J
                         {0x0, 0x0, 0x1C, 0x4, 0x0},
                         {0x0, 0x8, 0x8, 0x18, 0x0},
                         {0x0, 0x10, 0x1C, 0x0, 0x0}},
                        {{0x0, 0x8, 0xC, 0x4, 0x0},  // S
                         {0x0, 0x0, 0xC, 0x18, 0x0},
                         {0x0, 0x10, 0x18, 0x8, 0x0},
                         {0x0, 0xC, 0x18, 0x0, 0x0}},
                        {{0x0, 0x4, 0xC, 0x8, 0x0},  // Z
                         {0x0, 0x0, 0x18, 0xC, 0x0},
                         {0x0, 0x8, 0x18, 0x10, 0x0},
                         {0x0, 0x18, 0xC, 0x0, 0x0}}};
const int scope[7][2]={{1,3},{1,4},{2,3},{1,3},{1,3},{1,3},{1,3}};
const int startPos[7][4] = {
                      {1, 2, 1, 1},   // T
                      {1, 2, 1, 2},   // I
                      {2, 2, 2, 2},   // O
                      {1, 2, 1, 1},   // L
                      {1, 2, 1, 1},   // J
                      {1, 2, 1, 1},   // S
                      {1, 2, 1, 1}};  // Z
int mapping[16] = {15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0};

// const uint8_t IMAGES[][8]={
//   {
//   0b00000000,
//   0b01111110,
//   0b01011010,
//   0b00011000,
//   0b00011000,
//   0b00011000,
//   0b00011000,
//   0b00011000
//   }
// };

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
  0b00011000,
  0b00011000,
  0b00011000,
  0b00011000,
  0b00011000,
  0b01011010,
  0b01111110,
  0b00000000
},
{
      0b11100111,
  0b11100111,
  0b11100111,
  0b11100111,
  0b11100111,
  0b10100101,
  0b10000001,
  0b11111111
                     //FILLED T

}};

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

TM1637Display display = TM1637Display(25, 26);

TaskHandle_t xHandle = NULL;
TaskHandle_t xSnakeHandle = NULL;
TaskHandle_t xSnakeControlHandle=NULL;
TaskHandle_t xTetrisControlHandle=NULL;

bool run_flag=false;
