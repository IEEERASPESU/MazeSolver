#ifndef CONSTANTS_H
#define CONSTANTS_H

// floodfill weighting scores
const int TURN_SCORE = 0;
const int TILE_SCORE = 1;
const int STREAK_SCORE = 0;
const int STREAK_MULTIPLIER = 0;

// if you want the mouse to reset to the start once it reaches the middle, set RESET_AT_CENTER to 1
// otherwise, set it to 0 if you want it to make its way back to the start
const unsigned char RESET_AT_CENTER = 0;
const unsigned char STAY_AT_CENTER = 0;

// starting position. keep it the same for all regular micromouse rules
#define STARTING_TARGET 1; // 0 if going to start, 1 if going to center
#define STARTING_HEADING NORTH;
#define STARTING_X 0
#define STARTING_Y 0

// goal position, assuming the goal is a square
// when setting this, remember that the maze is zero-indexed!
#define LOWER_X_GOAL 7
#define LOWER_Y_GOAL 7
#define UPPER_X_GOAL 8
#define UPPER_Y_GOAL 8

// maze size
#define MAZE_WIDTH 16
#define MAZE_HEIGHT 16

// highlight path
const unsigned char HIGHLIGHT_PATH = 1;

// DO NOT TOUCH UNLESS YOU KNOW WHAT YOU ARE DOING
#define OUT_OF_BOUNDS -2
#define NOT_YET_SET -1

#endif