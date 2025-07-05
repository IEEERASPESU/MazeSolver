#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
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
    fprintf(stderr, "%s", text);
    fflush(stderr);
}

void init_cell(struct cell_status **cell_stat, int rows, int cols) {
    for(int i = 0; i < rows; i++)
        for(int j = 0; j < cols; j++) {
            cell_stat[i][j].explored = 0;
            cell_stat[i][j].top_wall = 0;
            cell_stat[i][j].bottom_wall = 0;
            cell_stat[i][j].left_wall = 0;
            cell_stat[i][j].right_wall = 0;
            cell_stat[i][j].score = 0;
        }
}

void init_cell_score(struct cell_status **cell_stat, int rows, int cols) {
    int r2 = rows / 2, c2 = cols / 2;
    for(int i = 0; i < r2; i++)
        for(int j = 0; j < c2; j++) {
            cell_stat[i + r2][j + c2].score = i + j;
            cell_stat[r2 - 1 - i][c2 - 1 - j].score = i + j;
            cell_stat[r2 + i][c2 - 1 - j].score = i + j;
            cell_stat[r2 - 1 - i][j + c2].score = i + j;
        }
}

void init_tester(struct cell_status **cell_stat, int rows, int cols) {
    char score[4];
    log_msg("PES University IEEE RAS Maze Solving Algorithm\n");
    log_msg("FOR TESTING PURPOSES ONLY!\n");
    log_msg("Version: 0.2\n");
    log_msg("Type: unknown\n");
    log_msg("Date: 25-06-2025\n");
    log_msg("\n");

    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            sprintf(score, "%d", cell_stat[i][j].score);
            API_setText(i, j, score);
            log_msg(score);
            log_msg("\t");
        }
        log_msg("\n");
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

void get_move(struct cell_status **cell_stat, struct position pos) 
{
    int score_n = 0, score_s = 0, score_e = 0, score_w = 0;
    if(pos.y < 15 && !cell_stat[pos.x][pos.y].top_wall)
        score_n = cell_stat[pos.x][pos.y+1].score ;
    if(pos.y > 0 && !cell_stat[pos.x][pos.y].bottom_wall)
        score_s = cell_stat[pos.x][pos.y-1].score;
    if(pos.x > 0 && !cell_stat[pos.x][pos.y].right_wall)
        score_e = cell_stat[pos.x-1][pos.y].score;
    if(pos.x < 15 && !cell_stat[pos.x][pos.y].left_wall)
        score_w = cell_stat[pos.x+1][pos.y].score;  
}

int main(int argc, char* argv[]) {
    int i, j;

    int rows = API_mazeHeight();
    int cols = API_mazeWidth();

    struct position pos = {0, 0, 0}; // Initialize position

    struct cell_status **cell_stat = malloc(rows * sizeof(struct cell_status *));
    if (!cell_stat) {
        log_msg("Memory allocation failed for rows");
        return 1;
    }

    for (i = 0; i < rows; i++) {
        cell_stat[i] = malloc(cols * sizeof(struct cell_status));
        if (!cell_stat[i]) {
            log_msg("Memory allocation failed for columns");
            for (j = 0; j < i; j++)
                free(cell_stat[j]);
            free(cell_stat);
            return 1;
        }
    }

    init_cell(cell_stat, rows, cols);
    init_cell_score(cell_stat, rows, cols);
    init_tester(cell_stat, rows, cols);

    check_wall(cell_stat, pos);
    get_move(cell_stat, pos);

    for (i = 0; i < rows; i++)
        free(cell_stat[i]);
    free(cell_stat);

    return 0;
}
