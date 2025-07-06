#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAZE_SIZE 16
#define INF 255

typedef struct {
    unsigned char walls; // Bitmask: N=1, E=2, S=4, W=8
    unsigned char value; // Distance to goal
} Cell;

Cell maze[MAZE_SIZE][MAZE_SIZE];

// Helper to send a command and read a boolean response
bool query_wall(const char *cmd) {
    char response[16];
    printf("%s\n", cmd);
    fflush(stdout);
    fgets(response, sizeof(response), stdin);
    return (strncmp(response, "true", 4) == 0);
}

// Example: check for walls and update internal map
void sense_walls(int x, int y, int dir) {
    // dir: 0=N, 1=E, 2=S, 3=W
    // For each direction, send wallFront, wallLeft, wallRight as needed
    // Update maze[y][x].walls accordingly
    // Example for front:
    if (query_wall("wallFront")) maze[y][x].walls |= (1 << dir);
    // Repeat for left/right/back as needed
}

// Example: move forward and wait for ack
void move_forward() {
    char response[16];
    printf("moveForward\n");
    fflush(stdout);
    fgets(response, sizeof(response), stdin);
    // Check for "ack" or "crash"
}

// Main loop
int main() {
    // Initialize maze, set goal, etc.
    // Use API to get maze size if needed:
    printf("mazeWidth\n"); fflush(stdout);
    int width; scanf("%d", &width); getchar();
    printf("mazeHeight\n"); fflush(stdout);
    int height; scanf("%d", &height); getchar();

    // Main exploration loop
    int x = 0, y = 0, dir = 0; // Start position and direction
    while (1) {
        sense_walls(x, y, dir);
        // Run floodfill, decide next move
        // Use move_forward(), turnLeft/turnRight as needed
        // Update x, y, dir accordingly
        // Break when goal is reached
    }
    return 0;
}
