#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include "API.h"

struct cell_status {
    int top_wall;
    int bottom_wall;
    int left_wall;
    int right_wall;
    int score;
    int explored;
};

struct position {
    int x;
    int y;
    int rot;
};

void log_msg(char* text) {
    fprintf(stderr, "%s\n", text);
    fflush(stderr);
}

void init_cell(struct cell_status **cell_stat, int rows, int cols) {
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++) {
            cell_stat[i][j].explored = 0;
            cell_stat[i][j].top_wall = 0;
            cell_stat[i][j].bottom_wall = 0;
            cell_stat[i][j].left_wall = 0;
            cell_stat[i][j].right_wall = 0;
            cell_stat[i][j].score = 1;
        }
}

void init_cell_score(struct cell_status **cell_stat, int rows, int cols) {
    int r2 = rows / 2;
    int c2 = cols / 2;

    for (int i = 0; i < r2; i++)
        for (int j = 0; j < c2; j++)
            cell_stat[i][j].score += (i * j) + (i + j);

    for (int i = rows - 1; i >= r2; i--)
        for (int j = 0; j < c2; j++)
            cell_stat[i][j].score = cell_stat[abs(i + 1 - rows)][j].score;

    for (int i = 0; i < r2; i++)
        for (int j = cols - 1; j >= c2; j--)
            cell_stat[i][j].score = cell_stat[i][abs(j + 1 - cols)].score;

    for (int i = rows - 1; i >= r2; i--)
        for (int j = cols - 1; j >= c2; j--)
            cell_stat[i][j].score = cell_stat[abs(i + 1 - rows)][abs(j + 1 - cols)].score;
}

void display_scores(struct cell_status **cell_stat, int rows, int cols) {
    char score_str[4];
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++) {
            sprintf(score_str, "%d", cell_stat[i][j].score);
            API_setText(i, j, score_str);
        }
}

void set_wall(struct cell_status **cell_stat, struct position pos) {
    if(cell_stat[pos.x][pos.y].explored == 0)
        if(cell_stat[pos.x][pos.y].top_wall)
            API_setWall(pos.x, pos.y, 'n');
        if(cell_stat[pos.x][pos.y].bottom_wall)
            API_setWall(pos.x, pos.y, 's');
        if(cell_stat[pos.x][pos.y].left_wall)
            API_setWall(pos.x, pos.y, 'w');
        if(cell_stat[pos.x][pos.y].right_wall)
            API_setWall(pos.x, pos.y, 'e');
}

void check_wall(struct cell_status **cell_stat, struct position pos) {
    API_setColor(pos.x, pos.y, 'y');

    switch(pos.rot) {
        case 0:
            cell_stat[pos.x][pos.y].top_wall = API_wallFront();
            cell_stat[pos.x][pos.y].left_wall = API_wallLeft();
            cell_stat[pos.x][pos.y].right_wall = API_wallRight();
            break;
        case 1:
            cell_stat[pos.x][pos.y].top_wall = API_wallRight();
            cell_stat[pos.x][pos.y].bottom_wall = API_wallLeft();
            cell_stat[pos.x][pos.y].left_wall = API_wallFront();
            break;
        case 2:
            cell_stat[pos.x][pos.y].bottom_wall = API_wallFront();
            cell_stat[pos.x][pos.y].left_wall = API_wallRight();
            cell_stat[pos.x][pos.y].right_wall = API_wallLeft();
            break;
        case 3:
            cell_stat[pos.x][pos.y].top_wall = API_wallLeft();
            cell_stat[pos.x][pos.y].bottom_wall = API_wallRight();
            cell_stat[pos.x][pos.y].right_wall = API_wallFront();
            break;
        default:
            log_msg("Error checking walls");
            break;
    }
    set_wall(cell_stat, pos);
    cell_stat[pos.x][pos.y].explored = 1;
}

void rotate_to(int *rot, int target) {
    int diff = (target - *rot + 4) % 4;
    if (diff == 1) API_turnRight();
    else if (diff == 2) { API_turnRight(); API_turnRight(); }
    else if (diff == 3) API_turnLeft();
    *rot = target;
}

void get_next_move(struct cell_status **cell_stat, struct position **pos, int rows, int cols, int *rot, int *next_x, int *next_y) {
    int min_score = INT_MAX;
    int min_dir = -1;
    int dirs[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}}; // N, E, S, W

    for (int dir = 0; dir < 4; dir++) {
        int nx = pos.x + dirs[dir][0];
        int ny = pos.y + dirs[dir][1];

        if (nx < 0 || ny < 0 || nx >= cols || ny >= rows) continue;

        // Skip if wall blocks direction
        if ((dir == 0 && cell_stat[pos.x][pos.y].top_wall) ||
            (dir == 1 && cell_stat[pos.x][pos.y].right_wall) ||
            (dir == 2 && cell_stat[x][pos.y].bottom_wall) ||
            (dir == 3 && cell_stat[x][pos.y].left_wall)) continue;

        int score = cell_stat[nx][ny].score;

        // Prefer unexplored neighbors first
        if (cell_stat[nx][ny].explored == 0 && score <= min_score) {
            min_score = score;
            min_dir = dir;
            *next_x = nx;
            *next_y = ny;
        }
    }

    // If no unexplored found, fall back to lowest score overall
    if (min_dir == -1) {
        for (int dir = 0; dir < 4; dir++) {
            int nx = x + dirs[dir][0];
            int ny = pos.y + dirs[dir][1];

            if (nx < 0 || ny < 0 || nx >= cols || ny >= rows) continue;

            if ((dir == 0 && cell_stat[x][pos.y].top_wall) ||
                (dir == 1 && cell_stat[x][pos.y].right_wall) ||
                (dir == 2 && cell_stat[x][pos.y].bottom_wall) ||
                (dir == 3 && cell_stat[x][pos.y].left_wall)) continue;

            int score = cell_stat[nx][ny].score;

            if (score < min_score) {
                min_score = score;
                min_dir = dir;
                *next_x = nx;
                *next_y = ny;
            }
        }
    }

    if (min_dir != -1) {
        rotate_to(rot, min_dir);
        API_moveForward();
    }
}



int main() {
    int rows = API_mazeHeight();
    int cols = API_mazeWidth();
    struct position pos = {0, 0, 0}; // Initialize position

    int goal_x = cols / 2;
    int goal_y = rows / 2;
    if (cols % 2 == 0) goal_x--;
    if (rows % 2 == 0) goal_y--;

    struct cell_status **cell_stat = malloc(rows * sizeof(struct cell_status *));
    for (int i = 0; i < rows; i++)
        cell_stat[i] = malloc(cols * sizeof(struct cell_status));

    init_cell(cell_stat, rows, cols);
    init_cell_score(cell_stat, rows, cols);
    display_scores(cell_stat, rows, cols);

    while (1) {
        check_wall(cell_stat, pos);

        int nx = x, ny = pos.y;
        get_next_move(cell_stat, pos, rows, cols, &nx, &ny);
        x = nx;
        pos.y = ny;

        if ((x == goal_x || x == goal_x + 1) && (pos.y == goal_y || pos.y == goal_y + 1)) {
            API_setColor(x, pos.y, 'g');
            log_msg("Goal reached!");
            break;
        }
    }

    for (int i = 0; i < rows; i++) free(cell_stat[i]);
    free(cell_stat);
    return 0;
}
