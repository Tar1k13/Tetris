#include <Arduino.h>

// #include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <TM1637.h>

using namespace std;

TM1637 tm(25, 26);

// Uncomment according to your hardware type
// #define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW

// Defining size, and output pins
#define MAX_DEVICES 2
#define CS_PIN 5

// MD_Parola Display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

int mapping[15];
void mapn(){
  int counter=8;
  for (size_t i = 0; i < 8; i++){
    mapping[i]=counter++;
  }
  counter=1;
  for (size_t i = 9; i < 16; i++){
    mapping[i]=counter++;
  }
  
  // for (size_t i = 0; i < 16; i++)
  // {
  //   Serial.println(mapping[i]);
  // }
  
}

void setup()
{
  Serial.begin(9600);
  mx.begin();
  tm.begin();
  mapn();
}

char mPieces [7 /*kind */ ][4 /* rotation */ ][5 /* horizontal blocks */ ][5 /* vertical blocks */ ] =
{
// Square
{
{
{'0', '0', '0', '0', '0'},
{'0', '0', '0', '0', '0'},
{'0', '0', '1', '1', '0'},
{'0', '0', '1', '1', '0'},
{'0', '0', '0', '0', '0'}
},
{
{0, 0, 0, 0, 0},
{0, 0, 0, 0, 0},
{0, 0, 2, 1, 0},
{0, 0, 1, 1, 0},
{0, 0, 0, 0, 0}
},
{
{0, 0, 0, 0, 0},
{0, 0, 0, 0, 0},
{0, 0, 2, 1, 0},
{0, 0, 1, 1, 0},
{0, 0, 0, 0, 0}
},
{
{0, 0, 0, 0, 0},
{0, 0, 0, 0, 0},
{0, 0, 2, 1, 0},
{0, 0, 1, 1, 0},
{0, 0, 0, 0, 0}
}
},

// I
{
{
{0, 0, 0, 0, 0},
{0, 0, 0, 0, 0},
{0, 1, 2, 1, 1},
{0, 0, 0, 0, 0},
{0, 0, 0, 0, 0}
},
{
{0, 0, 0, 0, 0},
{0, 0, 1, 0, 0},
{0, 0, 2, 0, 0},
{0, 0, 1, 0, 0},
{0, 0, 1, 0, 0}
},
{
{0, 0, 0, 0, 0},
{0, 0, 0, 0, 0},
{1, 1, 2, 1, 0},
{0, 0, 0, 0, 0},
{0, 0, 0, 0, 0}
},
{
{0, 0, 1, 0, 0},
{0, 0, 1, 0, 0},
{0, 0, 2, 0, 0},
{0, 0, 1, 0, 0},
{0, 0, 0, 0, 0}
}
}
,
// L
{
{
{0, 0, 0, 0, 0},
{0, 0, 1, 0, 0},
{0, 0, 2, 0, 0},
{0, 0, 1, 1, 0},
{0, 0, 0, 0, 0}
},
{
{0, 0, 0, 0, 0},
{0, 0, 0, 0, 0},
{0, 1, 2, 1, 0},
{0, 1, 0, 0, 0},
{0, 0, 0, 0, 0}
},
{
{0, 0, 0, 0, 0},
{0, 1, 1, 0, 0},
{0, 0, 2, 0, 0},
{0, 0, 1, 0, 0},
{0, 0, 0, 0, 0}
},
{
{0, 0, 0, 0, 0},
{0, 0, 0, 1, 0},
{0, 1, 2, 1, 0},
{0, 0, 0, 0, 0},
{0, 0, 0, 0, 0}
}
},
// L mirrored
{
{
{0, 0, 0, 0, 0},
{0, 0, 1, 0, 0},
{0, 0, 2, 0, 0},
{0, 1, 1, 0, 0},
{0, 0, 0, 0, 0}
},
{
{0, 0, 0, 0, 0},
{0, 1, 0, 0, 0},
{0, 1, 2, 1, 0},
{0, 0, 0, 0, 0},
{0, 0, 0, 0, 0}
},
{
{0, 0, 0, 0, 0},
{0, 0, 1, 1, 0},
{0, 0, 2, 0, 0},
{0, 0, 1, 0, 0},
{0, 0, 0, 0, 0}
},
{
{0, 0, 0, 0, 0},
{0, 0, 0, 0, 0},
{0, 1, 2, 1, 0},
{0, 0, 0, 1, 0},
{0, 0, 0, 0, 0}
}
},
// N
{
{
{0, 0, 0, 0, 0},
{0, 0, 0, 1, 0},
{0, 0, 2, 1, 0},
{0, 0, 1, 0, 0},
{0, 0, 0, 0, 0}
},
{
{0, 0, 0, 0, 0},
{0, 0, 0, 0, 0},
{0, 1, 2, 0, 0},
{0, 0, 1, 1, 0},
{0, 0, 0, 0, 0}
},
{
{0, 0, 0, 0, 0},
{0, 0, 1, 0, 0},
{0, 1, 2, 0, 0},
{0, 1, 0, 0, 0},
{0, 0, 0, 0, 0}
},

{
{0, 0, 0, 0, 0},
{0, 1, 1, 0, 0},
{0, 0, 2, 1, 0},
{0, 0, 0, 0, 0},
{0, 0, 0, 0, 0}
}
},
// N mirrored
{
{
{0, 0, 0, 0, 0},
{0, 0, 1, 0, 0},
{0, 0, 2, 1, 0},
{0, 0, 0, 1, 0},
{0, 0, 0, 0, 0}
},
{
{0, 0, 0, 0, 0},
{0, 0, 0, 0, 0},
{0, 0, 2, 1, 0},
{0, 1, 1, 0, 0},
{0, 0, 0, 0, 0}
},
{
{0, 0, 0, 0, 0},
{0, 1, 0, 0, 0},
{0, 1, 2, 0, 0},
{0, 0, 1, 0, 0},
{0, 0, 0, 0, 0}
},
{
{0, 0, 0, 0, 0},
{0, 0, 1, 1, 0},
{0, 1, 2, 0, 0},
{0, 0, 0, 0, 0},
{0, 0, 0, 0, 0}
}
},
// T
{
{
{0, 0, 0, 0, 0},
{0, 0, 1, 0, 0},
{0, 0, 2, 1, 0},
{0, 0, 1, 0, 0},
{0, 0, 0, 0, 0}
},
{
{0, 0, 0, 0, 0},
{0, 0, 0, 0, 0},
{0, 1, 2, 1, 0},
{0, 0, 1, 0, 0},
{0, 0, 0, 0, 0}
},
{
{0, 0, 0, 0, 0},
{0, 0, 1, 0, 0},
{0, 1, 2, 0, 0},
{0, 0, 1, 0, 0},
{0, 0, 0, 0, 0}
},
{
{0, 0, 0, 0, 0},
{0, 0, 1, 0, 0},
{0, 1, 2, 1, 0},
{0, 0, 0, 0, 0},
{0, 0, 0, 0, 0}
}
}
};

int figures[2][4][5] = {
  {
    {0x0,0x0,0x1C,0x8,0x0},{0x0,0x8,0xC,0x8,0x0},{0x0,0x0,0x8,0x1C,0x0},{0x0,0x8,0x18,0x8,0x0}
  },
  {
    {0x0,0x4,0x4,0x4,0x4},{0x0,0x0,0x3C,0x0,0x0},{0x0,0x4,0x4,0x4,0x4},{0x0,0x0,0x3C,0x0,0x0}
  }
};

int startPos[2][4]={
  {
    2,1,2,1
  },
  {
    2,1,2,1
  }
};




int matrix[16];
int calculateHeight(int fig[]){
  int counter=0;
  for (size_t i = 0; i < sizeof(fig); i++){
    if(fig[i]!=0) counter++;
  }
  return counter;
}
int tst=0;
void loop(){
  int fig=0;
  int rot =0;
  boolean objectionX=false;
  int heightX=calculateHeight(figures[fig][rot]);

  // mx.setColumn(16,0x8);
  
  // for (size_t col = 8; col < 16; col++){
  //   for(int t=0;t<heightX;t++){
  //         if((figures[fig][rot][startPos[fig][rot]+t] & matrix[col+t]) != 0){
  //           Serial.println("OBJECTION");
  //           objectionX=true;
  //           break;
  //         }
  //       }

  //       //moving blocks
  //       if(!objectionX){
  //         for(int t=0;t<heightX;t++){
  //           int sum=figures[fig][rot][startPos[fig][rot]+t]+matrix[col+t]; //safe put
  //           mx.setColumn(col+t,sum); //put
  //           if(col!=8)
  //             mx.setColumn(col-heightX+1,matrix[col]);
  //            //move
  //         }
  //       } else break;
  //     currentColX=col;
  //     if(col!=15)
  //       delay(1000);
  // }

  int height=calculateHeight(figures[fig][rot]);
  int currentCol=0;
  boolean objection=false;
    for (size_t col = 0; col < 17-height; col++){
          for(int t=0;t<height;t++){
            if((figures[fig][rot][startPos[fig][rot]+t] & matrix[col+t]) != 0){
              Serial.println("OBJECTION");
              objection=true;
              break;
            }
          }
        //moving blocks
        if(!objection){
            for(int t=0;t<height;t++){
              int sum=figures[fig][rot][startPos[fig][rot]+t]+matrix[col+t]; //safe put
              mx.setColumn(mapping[col+t],sum); //put
              Serial.println(mapping[col-1]);
              if(col!=0)
                mx.setColumn(mapping[col-1],matrix[col]); //move
            }
        } else break;
      currentCol=col;
      delay(1000);
  }
  for(int t=0;t<height;t++){
    matrix[currentCol+t]=figures[fig][rot][startPos[fig][rot]+t];
    // Serial.println(matrix[currentCol-t]);
  }
  for (size_t i = 0; i < 16; i++)
  {
    Serial.println(matrix[i]);
  }
}

// char matrix[8][8]={{'0'}};
// void loop(){
//   int current=0;
  
//   for (size_t i = 0; i < 5; i++){
//     int ct=0;
//     Serial.println(current);
//     for (size_t j = 0; j < 5; j++){
//       if(mPieces[0][0][i][j]=='1'){
//         Serial.println(ct);
//         matrix[current][ct++]='1';
//       };
//     }
//     current++;
//   }
//   // matrix[0][0]='1';
//   // matrix[0][1]='2';
//   // for (size_t i = 0; i < 8; i++){
//   //   int val=atoi(matrix[i]);
//   //   Serial.println(val);
//   //   Serial.println();
//   //   Serial.println("------");
//   // }
//   for (size_t i = 0; i <  8; i++){
//     mx.setRow(i,atoi(matrix[i]));
//   }
  
//   delay(10000);
  
// }

// String toHex(int nms[]){
//   String number="";
//   for (size_t i = 0; i < 5; i++){
//     if(nms[i]==2)
//       number+=1;
//     else
//       number+=nms[i];
//   }
//   return number;
// }

// void loop(){
//   for (size_t i = 0; i < 5; i++){
//     Serial.println(toHex(mPieces[0][0][i]).toInt());
    
//   }
//   // Serial.println(mPieces[1][1][2][2]);
//   delay(5000);
// }

// int kvp[2][4][5] = {
//   {
//     {0x0,0x0,0x1C,0x8,0x0},{0x0,0x8,0xC,0x8,0x0},{0x0,0x0,0x8,0x1C,0x0},{0x0,0x8,0x18,0x8,0x0}
//   },
//   {
//     {0x0,0x4,0x4,0x4,0x4},{0x0,0x0,0x3C,0x0,0x0},{0x0,0x4,0x4,0x4,0x4},{0x0,0x0,0x3C,0x0,0x0}
//   }
// };
// int mtrx[8];

// boolean checkIfEmpty(int fig, int pos,int height)
// {
//   Serial.print(fig);
//   Serial.print(" ");
//   Serial.print(pos);
//   Serial.println();
//   if((mtrx[(pos*height)] & fig)==0) return true;
//   // for (size_t i = pos; i <= 8; i++){
//   //   Serial.print(mtrx[i]);
//   //   Serial.print(" ");
//   //   if((mtrx[i] & fig)==0) return true;
//   // }
//   return false;
  
//   // if ((mtrx[pos] & vl.val) == 0)
//   // {
//   //   Serial.println("--------");
//   //   return true;
//   // }
//   // Serial.println("false");
//   // return false;
// }

// // int tstr = 0;
// int calculateStartPosition(int fig[]){
//   for (size_t i = 0; i < sizeof(fig); i++){
//     if(fig[i]!=0) return i;
//   }
//   return 0;
// }
// int calculateHeight(int fig[]){
//   int counter=0;
//   for (size_t i = 0; i < sizeof(fig); i++){
//     if(fig[i]!=0) counter++;
//   }
//   return counter;
// }

// int tstr=0;
// int elm=0;
// int clmn=0;
// void loop()
// {
//   int currentFig=1;
//   int currentRot=0;
//   int startMove=calculateStartPosition(kvp[currentFig][currentRot]);
//   int height=calculateHeight(kvp[currentFig][currentRot]);
//   // Serial.println(height);
//   // Serial.println(height);
//   tm.display(1234);
//   for (uint8_t col = 0; col<COL_SIZE-(height-1); col++){
//     //put
//     for (size_t i = 0; i < height; i++){
//       int element=kvp[currentFig][currentRot][startMove+i] << tstr;
       
//       if(mtrx[col+i]==0){
//         mx.setColumn(col+i,element);
//         elm=element;
//       } else {
//         int sum=mtrx[col+i]+element;
//         // mtrx[col+i]=sum;
//         elm=sum;
//         clmn=col+i;
//         // for(int m:mtrx){
//         //   Serial.println(m);
//         // }
//         // Serial.println(mtrx[col+i]);
        
//         Serial.println(clmn);
//         mx.setColumn(col+i,sum);
//       };

//     }
  
//     delay(1000);

//     //clear
//     boolean isEm=false;
//     for (size_t i = 0; i < height; i++){
   
//         isEm=(mtrx[col+i]<<tstr|kvp[currentFig][currentRot][startMove+i] << tstr)== (mtrx[col+i] << tstr);
//       // else
//       //   isEm=(mtrx[col+i]<<tstr|kvp[currentFig][currentRot][startMove+i] << tstr)== (elm);
//       // if(isEm) break;
//     }
//     // boolean isEmt=(mtrx[col+height]|kvp[currentFig][currentRot][startMove])==(mtrx[col+height]);
//     // Serial.println(isEm);
//     Serial.println(isEm);
//     if(col<COL_SIZE-(height-1)-1 && !isEm){
//       mx.setColumn(col,0x0);
//     } else{
//       for (size_t i = 0; i < height; i++){
//         mtrx[col+i]=kvp[currentFig][currentRot][startMove+i] << tstr;
//       }
//       if(clmn!=0)
//         mtrx[clmn]=elm;

//       // for(int m:mtrx){
//       //   Serial.println(m);
//       // }


//       tstr+=2;
//       break;      

//     }
      
  // }
  
  // mx.setColumn(8,0xFF);

  // for (uint8_t row = 8; row < 16; row++)
  // {
  //   // Serial.println(analogRead(34));
  //   mx.setColumn(row, kvp[tstr].key);
  //   mx.setColumn(row + 1, kvp[tstr].val);
  //   delay(1000);

  // //   if (row + 1 != 16)
  // //   {
  // //     mx.setColumn(row, 0x00);
  // //   }
  // //   else
  // //   {
  // //     if (mtrx[row] == 0 && mtrx[row - 1] == 0)
  // //     {
  // //       mtrx[row] = kvp[tstr].val;
  // //       mtrx[row - 1] = kvp[tstr].key;
  // //     }
  // //     mx.setColumn(15, kvp[tstr].val);
  // //     mx.setColumn(row, 0x00);
  // //     mx.setColumn(row+1, 0x00);
      
  // //     break;
  // //   }
  // // }
  
  // for (uint8_t row = 0; row < 8; row++)
  // {
  //   // Serial.println(4<<2);
  //   // Serial.println(analogRead(34));
  //   mx.setColumn(row, kvp[tstr].key);
  //   mx.setColumn(row + 1, kvp[tstr].val);
  //   delay(1000);

  //   if (row + 2 != COL_SIZE && row != COL_SIZE - 1 && checkIfEmpty(kvp[tstr], row + 1))
  //   {
  //     // // int rw=kvp[tstr].val^(mtrx[row]+kvp[tstr].val);
  //     // for(int val:mtrx){
  //     //   Serial.println(val);
  //     // }


  //     // int vl=cur^(mtrxEl+cur);
  //     // int sp=(mtrx[rw]);
  //     // Serial.println(mtrx[row]);
  //     int res=kvp[tstr].key^(mtrx[row+1]+kvp[tstr].key);
  //     // Serial.println(mtrx[row+1]+kvp[tstr].val);
  //     Serial.println(row);
      
      
  //     mx.setColumn(row,0x00);
  //     mx.setColumn(1, res);
  //     // Serial.println(mtrx[9]);
  //     // mx.setColumn(row+1, kvp[tstr].val^(mtrx[row]+kvp[tstr].val));
  //     // mx.setColumn(row, kvp[tstr].key^(mtrx[row-1]+kvp[tstr].key));
  //   }
  //   else
  //   {
  //     if (mtrx[row] == 0 && mtrx[row - 1] == 0)
  //     {
        
  //       mtrx[row] = kvp[tstr].val;
  //       mtrx[row - 1] = kvp[tstr].key;
  //     }
  //   tstr++;
  //     return;
  //   }
  // }
  // // Display.setTextAlignment(PA_LEFT);
  // // Display.print("E");
  // // delay(2000);

  // // Display.setTextAlignment(PA_CENTER);
  // // Display.print("ESP32");
  // // delay(2000);

  // // Display.setTextAlignment(PA_RIGHT);
  // // Display.print("ESP32");
  // // delay(2000);

  // // Display.setTextAlignment(PA_CENTER);
  // // Display.setInvert(true);
  // // Display.print("ESP32");
  // // delay(2000);

  // // Display.setInvert(false);
  // // delay(2000);
// }