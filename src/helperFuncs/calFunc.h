#ifndef calFunc_h
#define calFunc_h

#include <Arduino.h>
#include <MD_MAX72xx.h>
#include <TM1637Display.h>
#include <music/music.h>

int calculateHeight(int fig[]);
int countOnes(int num, int numBits);
boolean checkObjection(int col,int currentfigure[],int matrix[],int fig);
boolean checkFigureObjection(int* mvdFig, int col,int matrix[],int fig);
boolean checkBoundings(boolean right,int currentfigure[],int fig);
boolean isRotateAble(int col, int rotated[], int matrix[],int fig);
boolean isGameOver(int currentfigure[],int fig,int matrix[],int col);

void deleteLine(int* matr,
                void (*deleteAnim)(int* elemToDelete, int col, int heigh,
                                   boolean isRts,MD_MAX72XX &mx),
                int heigh, int col, boolean isRts,int &mainPointCounter,int currentColumn,TM1637Display &display,MD_MAX72XX &mx);
void deleteAnimation(int *elemsToDel, int col, int heigh, boolean isRts,MD_MAX72XX &mx);

#endif