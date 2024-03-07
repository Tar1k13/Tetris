#pragma once

#include <Arduino.h>
#include <MD_MAX72xx.h>

void move(int col, int* currentfigure, boolean right,int fig,int matrix[],MD_MAX72XX &mx) ;
int rotate(int col, int* currentFig,int mvr,int fig,int &rot,int matrix[],MD_MAX72XX &mx,int &height, int &cl);
void moveDown(int col,int currentfigure[],int matrix[],int fig,MD_MAX72XX &mx);