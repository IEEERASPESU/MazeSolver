#include "solver.h"
//#include "API.h"
#include "queues.h"
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>

/* Initialize starting values */

// define starting position
unsigned char target = STARTING_TARGET; 
coord currentXY = {STARTING_X, STARTING_Y};
Heading currentHeading = STARTING_HEADING;

/* Arrays and Array Helper Functions */

// keeps track of the known vertical walls in the maze
int verticalWalls[MAZE_WIDTH+1][MAZE_HEIGHT] = {{0}};
// keeps track of horizontal walls in the maze
int horizontalWalls[MAZE_WIDTH][MAZE_HEIGHT+1] = {{0}};
// keeps track of current floodfill values
int floodArray[MAZE_WIDTH][MAZE_HEIGHT];
// keeps track of the path the car should take after a floodfill iteration for each cell of the maze
Heading pathArray[MAZE_WIDTH][MAZE_HEIGHT] = {{NORTH}};
// keeps track of all of the cells that the mouse has visited
int travelArray[MAZE_WIDTH][MAZE_HEIGHT] = {{0}};

// given a coord, checks to see if the mouse has visited a certain cell before
int checkTravelArray(coord c) {return travelArray[c.x][c.y];}
// given a coord, updates the travel array to mark that the mouse has visited that cell before
void updateTravelArray(coord c) {travelArray[c.x][c.y] = 1;}
// given coordinate, updates the respective cell's floodfill value
void updateFloodArray(coord c, int val) {floodArray[c.x][c.y] = val;}
// given coordinate, gets the respective cell's floodfill value
int getFloodArray(coord c) {return floodArray[c.x][c.y];}
// given coordinate, updates the respective cell's path heading
void updatePathArray(coord c, Heading h) {pathArray[c.x][c.y] = h;}
// given cordinate, gets the respective cell's path heading
Heading getPathArray(coord c) {return pathArray[c.x][c.y];}

/* Floodfill Functions */

// resets the floodfill array to target the center as destination
void resetFloodArray()
{
    // set the entire flood array to blank values (-1)
    for (int x = 0; x < MAZE_WIDTH; x++)
        for (int y = 0; y < MAZE_HEIGHT; y++)
            floodArray[x][y] = -1;
    // set desired goal values 
    if (target) // target is goal (center)
        for (int x = LOWER_X_GOAL; x <= UPPER_X_GOAL; x++)
            for (int y = LOWER_Y_GOAL; y <= UPPER_Y_GOAL; y++)
                floodArray[x][y] = 0;
    else // target is starting cell
        floodArray[STARTING_X][STARTING_Y] = 0;
}

// given heading and coordinate, check if there is a wall on that side of the cell
int checkWall(Heading heading, coord c) {
    switch (heading) {
        case NORTH: return horizontalWalls[c.x][c.y+1];
        case WEST: return verticalWalls[c.x][c.y];
        case SOUTH: return horizontalWalls[c.x][c.y];
        case EAST: return verticalWalls[c.x+1][c.y];
    }
    return 1;
}

// Increments coord in the direction of the heading by input integer, then returns updated coord
coord incrementCoord(Heading heading, coord c, int numCells) {
    switch (heading) {
        case NORTH: return (coord){c.x, c.y += numCells};
        case WEST: return (coord){c.x -= numCells, c.y};
        case SOUTH: return (coord){c.x, c.y -= numCells};
        case EAST: return (coord){c.x += numCells, c.y};
    }
    return c;
}

// turns currentHeading global variable to the left based on the mouse's current heading,
// then returns LEFT action
Action turnLeft() {
    //API_turnLeft();
    currentHeading = (currentHeading+1)%4;
    return LEFT;
}

// turns currentHeading global variable to the right based on the mouse's current heading,
// then returns RIGHT action
Action turnRight() {
    //API_turnRight();
    currentHeading = (currentHeading-1)%4;
    return RIGHT;
}

// returns whether the mouse is in the target
unsigned char mouseInGoal() {
    return (target == 1 && (currentXY.x >= LOWER_X_GOAL && currentXY.x <= UPPER_X_GOAL && currentXY.y >= LOWER_Y_GOAL && currentXY.y <= UPPER_Y_GOAL));
}

// given heading and coordinates, returns the floodfill value of the corresponding neighbor cell.
// if the neighbor is off of the maze (argument cell is on the boundary of the maze), return -2
int getNeighbor(Heading heading, coord c)
{
    switch (heading) {
        case NORTH:
            if (c.y >= 15) return OUT_OF_BOUNDS;
            else return floodArray[c.x][c.y+1];
        case WEST:
            if (c.x <= 0) return OUT_OF_BOUNDS;
            else return floodArray[c.x-1][c.y];
        case SOUTH:
            if (c.y <= 0) return OUT_OF_BOUNDS;
            else return floodArray[c.x][c.y-1];
        case EAST:
            if (c.x >= 15) return OUT_OF_BOUNDS;
            else return floodArray[c.x+1][c.y];
    }
    return 1;
}

neighbor generateNeighbor(queue q, Heading heading, neighbor current, int currentVal) {
    if (!checkWall(heading,current.coord)) {
        int nextVal = currentVal + TILE_SCORE;
        neighbor next;
        // checks if the mouse would have to turn to go north from current cell
        if (current.heading != heading) {
            nextVal += TURN_SCORE;
            next.streak = 0;
        } else { // if the mouse doesn't need to turn, records that is is on a straight streak
            nextVal += (STREAK_MULTIPLIER * (current.streak-1)) + STREAK_SCORE;
            next.streak = current.streak + 1;
        }

        // prepare neighbor to add to the floodfill queue
        next.coord = incrementCoord(heading, current.coord, 1);
        next.heading = heading;

        int neighborVal = getNeighbor(heading,current.coord);
        if (neighborVal == NOT_YET_SET || nextVal < neighborVal) {
            queue_push(q,next);
            updateFloodArray(next.coord,nextVal);
            updatePathArray(next.coord,(heading+2)%4);
        }
    }
    return current;
}

// updates the floodfill array based on known walls
void floodFill() {
    // set non-goal values to blank so that the floodfill array can be recalculated
    resetFloodArray();

    // declare/initialize relevant variables for queue for floodfill algorithm
    queue q = queue_create();

    // iterate through the 2D array, find goal values and add them to the queue
    for (int x = 0; x < MAZE_WIDTH; x++) {
        for (int y = 0; y < MAZE_HEIGHT; y++) {
            if (floodArray[x][y] == 0) {
                // for the starting goal values, it doesn't matter which direction you approach them from.
                // as such, they should be oriented from all directions
                queue_push(q,(neighbor){(coord){x,y},NORTH,0});
                queue_push(q,(neighbor){(coord){x,y},WEST,0});
                queue_push(q,(neighbor){(coord){x,y},SOUTH,0});
                queue_push(q,(neighbor){(coord){x,y},EAST,0});
            }
        }
    }

    // adds available neighbors to queue and updates their floodfill values
    while (!queue_is_empty(q)) {
        // initializes values for calculating floodfills for neighbors
        neighbor current = queue_pop(q);
        int currentVal = getFloodArray(current.coord);

        // prints the current cell's floodfill number to the simulation screen
        //char forSetText[6] = ""; sprintf(forSetText, "%d", getFloodArray(current.coord)); API_setText(current.coord.x,current.coord.y,forSetText);
        
        // pushes neighbors if available
        generateNeighbor(q,NORTH,current,currentVal);
        generateNeighbor(q,WEST,current,currentVal);
        generateNeighbor(q,SOUTH,current,currentVal);
        generateNeighbor(q,EAST,current,currentVal);        
    }
}

// places a wall in respective arrays and API at the given heading and coordinate
void placeWall(Heading heading, coord c) {
    // sets a wall in the wall arrays
    switch (heading) {
        case NORTH:
            horizontalWalls[c.x][c.y+1] = 1;
            //API_setWall(c.x,c.y,'n');
            return;
        case WEST:
            verticalWalls[c.x][c.y] = 1;
            //API_setWall(c.x,c.y,'w');
            return;
        case SOUTH:
            horizontalWalls[c.x][c.y] = 1;
            //API_setWall(c.x,c.y,'s');
            return;
        case EAST:
            verticalWalls[c.x+1][c.y] = 1;
            //API_setWall(c.x,c.y,'e');
            return;
    }
}

void generateInitialWalls() {
    for (int x = 0; x < MAZE_WIDTH; x++) {
        placeWall(SOUTH,(coord){x,0});
        placeWall(NORTH,(coord){x,MAZE_HEIGHT-1});
    }
    for (int y = 0; y < MAZE_HEIGHT; y++) {
        placeWall(WEST,(coord){0,y});
        placeWall(EAST,(coord){MAZE_WIDTH-1,y});
    }
}

// checks for and then updates the walls for the current cell
void updateWalls()
{
    // based on the current heading, places walls at the respective locations
    //if (API_wallFront()) placeWall(currentHeading,currentXY);
    //if (API_wallLeft()) placeWall((currentHeading+1)%4,currentXY);
    //if (API_wallRight()) placeWall((currentHeading-1)%4,currentXY);
}

// based on updated wall and floodfill information, return the next action that the mouse should do
Action nextAction()
{
    // stay at center if already in center
    if (target && mouseInGoal() && STAY_AT_CENTER)
        return IDLE;

    Heading newHeading = getPathArray(currentXY);
    updateTravelArray(currentXY);
    //coord originalCoord = currentXY;

    // moves forward if the mouse is already facing the correct heading
    if (newHeading == currentHeading) {
        int moveNumber = 0;
        while (checkTravelArray(currentXY) == 1 && (!checkWall(newHeading,currentXY))
        && getPathArray(currentXY) == currentHeading) {
            moveNumber++;
            updateTravelArray(currentXY);
            currentXY = incrementCoord(newHeading,currentXY,1);
        } 

        //char forSetText[4] = "";
        //sprintf(forSetText, "%d", moveNumber);
        //debug_log(forSetText);

        for (int i = 0; i < moveNumber; i++)
            //API_moveForward();
        return FORWARD;
    }

    // determines which way to turn based on current direction and desired direction
    if (currentHeading == (newHeading+1)%4)
        return turnRight();
    else if (currentHeading == (newHeading-1)%4)
        return turnLeft();
    else {
        //debug_log("turned 180");
        turnLeft();
        return turnLeft();
    }
}

// checks if the mouse has reached its target
void checkDestination()
{
    if (target) {
        if (mouseInGoal()) {
            if (RESET_AT_CENTER) {
                //API_ackReset();
                currentXY = (coord){0,0};
                currentHeading = NORTH;
            }
            else if (!STAY_AT_CENTER)
                target = 0;       
        }
    } else if (currentXY.x == STARTING_X && currentXY.y == STARTING_Y)
        target = 1;
}

// highlights the optimal path for the mouse
void highlightPath()
{
    //API_clearAllColor();
    if (target) {
        int x = STARTING_X;
        int y = STARTING_Y;
        while (!(x >= LOWER_X_GOAL && x <= UPPER_X_GOAL && y >= LOWER_Y_GOAL && y <= UPPER_Y_GOAL)) {
            //API_setColor(x,y,'w');
            switch (pathArray[x][y]) {
                case NORTH: y++; break;
                case WEST: x--; break;
                case SOUTH: y--; break;
                case EAST: x++; break;
            }
        }
    } else {
        int x = LOWER_X_GOAL;
        int y = LOWER_Y_GOAL;
        while (!(x == STARTING_X && y == STARTING_Y)) {
            //API_setColor(x,y,'w');
            switch (pathArray[x][y]) {
                case NORTH: y++; break;
                case WEST: x--; break;
                case SOUTH: y--; break;
                case EAST: x++; break;
            }
        }
    }  
    // highlight start and goal values
    for (int x = LOWER_X_GOAL; x <= UPPER_X_GOAL; x++)
        for (int y = LOWER_Y_GOAL; y <= UPPER_Y_GOAL; y++)  
        	break;
            //API_setColor(x,y,'w');
    //API_setColor(STARTING_X,STARTING_Y,'w');
}

// sends the mouse's recommended next action back to main
Action solver() {
    checkDestination();
    updateWalls();    
    floodFill();
    if (HIGHLIGHT_PATH) highlightPath();
    return nextAction();
}
