#include <stdio.h>
#include "solver.h"
#include "API.h"


// You do not need to edit this file.
// This program just runs your solver and passes the choices
// to the simulator.
int main(int argc, char* argv[]) {
    debug_log("Running...");
    generateInitialWalls();
    while (1) {
        Action nextMove = solver();
    }
}