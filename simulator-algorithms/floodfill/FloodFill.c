// this shit workssss yayyy
#include <stdio.h>
#include <stdbool.h>

#define MAX_SIZE 16
int W = 16, H = 16;
bool wall_known[MAX_SIZE][MAX_SIZE][4]; // [x][y][dir]
bool wall_exists[MAX_SIZE][MAX_SIZE][4]; 
int dist_map[MAX_SIZE][MAX_SIZE];

// Movement directions: N, E, S, W
int dx[4] = {0, 1, 0, -1};
int dy[4] = {1, 0, -1, 0};
int opp_dir[4] = {2, 3, 0, 1};

// Simple string comparison for "true" responses
bool is_true(const char* str) {
    return str[0] == 't' && str[1] == 'r' && str[2] == 'u' && str[3] == 'e';
}

void set_wall(int x, int y, int dir) {
    if (x < 0 || x >= W || y < 0 || y >= H) return;  //if x and y are out of the bound then exit 
    
    wall_known[x][y][dir] = true;
    wall_exists[x][y][dir] = true; //marking wall direction from x,y as known and exists
    
    int nx = x + dx[dir];
    int ny = y + dy[dir]; //pakkad cell cordinate calculation in specific direction
    if (nx >= 0 && nx < W && ny >= 0 && ny < H) {           // makin sure nx and ny are within the bounds
        wall_known[nx][ny][opp_dir[dir]] = true; 
        wall_exists[nx][ny][opp_dir[dir]] = true;  //setting the wall in opposite diection from pakkad mane pov
    }
}

void flood_fill(int goal_x, int goal_y) {  //distance from goal to all cells
    int queue_x[MAX_SIZE*MAX_SIZE];
    int queue_y[MAX_SIZE*MAX_SIZE]; // using 2 arays as queues for BFS
    int q_start = 0, q_end = 0;
    
    // Initialize distances
    for (int x = 0; x < W; x++) {
        for (int y = 0; y < H; y++) {
            dist_map[x][y] = W*H; // initializing all distances to a Large number 
        }
    }
    
    // Seeding the quue with the goal cells in bottom left and its adjacent right side and its top cells 
    // and setting the distance to 0 
    for (int x = goal_x; x <= goal_x + 1; x++) {
        for (int y = goal_y; y <= goal_y + 1; y++) {
            if (x < W && y < H) {
                dist_map[x][y] = 0;
                queue_x[q_end] = x;
                queue_y[q_end] = y;
                q_end++;
            }
        }
    }
    
    while (q_start < q_end) {  //BFS loop starts when the queue is not empty
        int x = queue_x[q_start];
        int y = queue_y[q_start]; 
        q_start++; // dequeue-ing the next cell
        
        for (int dir = 0; dir < 4; dir++) {    //checking in all 4 directions 
            // Skip if wall exists
            if (wall_known[x][y][dir] && wall_exists[x][y][dir]) continue;  // if we already know there is a wall then skip it 
            
            int nx = x + dx[dir];
            int ny = y + dy[dir];
            
            if (nx < 0 || nx >= W || ny < 0 || ny >= H) continue;  // check the bounds of the pakkad cell
            
            // Update distance if we found a shorter path
            if (dist_map[nx][ny] > dist_map[x][y] + 1) {  //if a shorter path exists then update it 
                dist_map[nx][ny] = dist_map[x][y] + 1;
                queue_x[q_end] = nx;
                queue_y[q_end] = ny;
                q_end++;  //update pakkad mane distance and enqueueing it
            }
        }
    }
}

void turn_to(int *current_dir, int target_dir, char *buf) {  //rotate the mouse till it reached the direction needed 
    while (*current_dir != target_dir) {  //keep turning till the direction matches 
        int right_turns = (target_dir - *current_dir + 4) % 4;  //calc the no of turns needed to reach the target 
        if (right_turns <= 2) {  //if this is quicker then turn right 
            printf("turnRight\n");
            fflush(stdout);
            scanf("%s", buf);
            *current_dir = (*current_dir + 1) % 4; // send the turnright command and check the response and update direction 
        } else {
            printf("turnLeft\n");
            fflush(stdout);
            scanf("%s", buf);
            *current_dir = (*current_dir + 3) % 4;  //if turning left is shorter do that then
        }
    }
}

int main() {
    int x = 0, y = 0, dir = 0; // Start facing north
    char response[16];  //buring to store text input responses as true or false
    int goal_x = W/2 - 1, goal_y = H/2 - 1;  // goal is a 2x2 square at 7,7
    
    // Initial wall sensing
    printf("wallLeft\n"); fflush(stdout);
    scanf("%s", response);
    if (is_true(response)) set_wall(x, y, (dir+3)%4);  //ask wall if left of the current direction is correct if it is then set the wall in dir+3 which is left direction 
    
    printf("wallFront\n"); fflush(stdout);
    scanf("%s", response);
    if (is_true(response)) set_wall(x, y, dir); //ask if front wall is there and if it is then record it
    
    printf("wallRight\n"); fflush(stdout);
    scanf("%s", response);
    if (is_true(response)) set_wall(x, y, (dir+1)%4); //ask if right wall is there then record it if its there
    
    while (true) {  //looping till goal is reached 
        flood_fill(goal_x, goal_y); // update the dist_map using the current wall map
        
        // Check if reached center 2x2 block if yes goal reached and exit 
        if ((x == goal_x || x == goal_x+1) && 
            (y == goal_y || y == goal_y+1)) {
            printf("Done!\n");
            break;
        }
        
        // Find best move
        int best_dist = dist_map[x][y];
        int best_dir = -1;  // initialixe the best direction as none and current cells distance 
        
        for (int d = 0; d < 4; d++) {
            // Skip if wall exists
            if (wall_known[x][y][d] && wall_exists[x][y][d]) continue; //loop over all 4 direction if a wall exists in the direction then skip it 
            
            int nx = x + dx[d];
            int ny = y + dy[d];
            if (nx < 0 || nx >= W || ny < 0 || ny >= H) continue;  //cacl the pakkad cell nx and ny and skip it if its out of the bound
            
            if (dist_map[nx][ny] < best_dist) {
                best_dist = dist_map[nx][ny];
                best_dir = d;  //if pakkad cell has shorter distance then update best direction 
            }
        }
        
        if (best_dir == -1) {
            printf("No path found!\n");
            break;
        } // if nothing leads closer to the goal then report and exit
        
        // Turn to best direction
        turn_to(&dir, best_dir, response);  //trurn mouse to face the best direction 
        
        // Move forward
        printf("moveForward\n");
        fflush(stdout);
        scanf("%s", response);  //tell mouse to move forward and wait for response
        
        // Update position
        x += dx[dir];
        y += dy[dir]; // update position based on moment direction 
        
        // Sense walls in new cell
        printf("wallLeft\n"); fflush(stdout);
        scanf("%s", response);
        if (is_true(response)) set_wall(x, y, (dir+3)%4); //sense left wall at new postion and record if present 
        
        printf("wallFront\n"); fflush(stdout);
        scanf("%s", response);
        if (is_true(response)) set_wall(x, y, dir);  //sense front wall and do the same
        
        printf("wallRight\n"); fflush(stdout);
        scanf("%s", response);
        if (is_true(response)) set_wall(x, y, (dir+1)%4);   // sense the right wall do the same
    }
    
    return 0;
}