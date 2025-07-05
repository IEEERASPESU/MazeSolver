#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "API.h"

struct cell_status {
    bool top_wall;
    bool bottom_wall;
    bool left_wall;
    bool right_wall;
    int score;
    bool explored;
};

struct position {
    int x;
    int y;
    int rot;
    int rows;
    int cols;
};

void log_msg(char* text) {
    fprintf(stderr, "%s", text);
    fflush(stderr);
}

void init_cell(struct cell_status **cell_stat, struct position *pos) {
    for(int i = 0; i < pos->rows; i++)
        for(int j = 0; j < pos->cols; j++) {
            cell_stat[i][j].explored = false;
            cell_stat[i][j].top_wall = false;
            cell_stat[i][j].bottom_wall = false;
            cell_stat[i][j].left_wall = false;
            cell_stat[i][j].right_wall = false;
            cell_stat[i][j].score = 0;
        }
}

void init_cell_score(struct cell_status **cell_stat, struct position *pos) {
    int r2 = pos->rows / 2, c2 = pos->cols / 2;
    for(int i = 0; i < r2; i++)
        for(int j = 0; j < c2; j++) {
            cell_stat[i + r2][j + c2].score = i + j;
            cell_stat[r2 - 1 - i][c2 - 1 - j].score = i + j;
            cell_stat[r2 + i][c2 - 1 - j].score = i + j;
            cell_stat[r2 - 1 - i][j + c2].score = i + j;
        }
}

void init_tester(struct cell_status **cell_stat, struct position *pos) {
    char score[4];
    log_msg("PES University IEEE RAS Maze Solving Algorithm\n");
    log_msg("FOR TESTING PURPOSES ONLY!\n");
    log_msg("Version: 0.3\n");
    log_msg("Type: unknown\n");
    log_msg("Date: 05-07-2025\n\n");

    for(int i = 0; i < pos->rows; i++) {
        for(int j = 0; j < pos->cols; j++) {
            sprintf(score, "%d", cell_stat[i][j].score);
            API_setText(i, j, score);
        }
    }
}

void set_wall(struct cell_status **cell_stat, struct position *pos) {
    if(cell_stat[pos->x][pos->y].top_wall)
        API_setWall(pos->x, pos->y, 'n');
    if(cell_stat[pos->x][pos->y].bottom_wall)
        API_setWall(pos->x, pos->y, 's');
    if(cell_stat[pos->x][pos->y].left_wall)
        API_setWall(pos->x, pos->y, 'w');
    if(cell_stat[pos->x][pos->y].right_wall)
        API_setWall(pos->x, pos->y, 'e');
}

void check_wall(struct cell_status **cell_stat, struct position *pos) {
    API_setColor(pos->x, pos->y, 'y');

    if(cell_stat[pos->x][pos->y].explored == false){
        switch(pos->rot) {
            case 0:
                cell_stat[pos->x][pos->y].top_wall = API_wallFront();
                cell_stat[pos->x][pos->y].left_wall = API_wallLeft();
                cell_stat[pos->x][pos->y].right_wall = API_wallRight();
                break;
            case 1:
                cell_stat[pos->x][pos->y].top_wall = API_wallRight();
                cell_stat[pos->x][pos->y].bottom_wall = API_wallLeft();
                cell_stat[pos->x][pos->y].left_wall = API_wallFront();
                break;
            case 2:
                cell_stat[pos->x][pos->y].bottom_wall = API_wallFront();
                cell_stat[pos->x][pos->y].left_wall = API_wallRight();
                cell_stat[pos->x][pos->y].right_wall = API_wallLeft();
                break;
            case 3:
                cell_stat[pos->x][pos->y].top_wall = API_wallLeft();
                cell_stat[pos->x][pos->y].bottom_wall = API_wallRight();
                cell_stat[pos->x][pos->y].right_wall = API_wallFront();
                break;
            default:
                log_msg("Error checking walls");
                break;
        }
        set_wall(cell_stat, pos);
    }
    cell_stat[pos->x][pos->y].explored = true;
}

void get_move(struct cell_status **cell_stat, struct position *pos) {
    char score[4];
    int score_n = pos->rows * pos->cols;
    int score_s = pos->rows * pos->cols;
    int score_e = pos->rows * pos->cols;
    int score_w = pos->rows * pos->cols;

    if(pos->y < pos->cols - 1 && !cell_stat[pos->x][pos->y].top_wall)
        score_n = cell_stat[pos->x][pos->y+1].score;
    if(pos->y > 0 && !cell_stat[pos->x][pos->y].bottom_wall)
        score_s = cell_stat[pos->x][pos->y-1].score;
    if(pos->x > 0 && !cell_stat[pos->x][pos->y].right_wall)
        score_e = cell_stat[pos->x-1][pos->y].score;
    if(pos->x < pos->rows - 1 && !cell_stat[pos->x][pos->y].left_wall)
        score_w = cell_stat[pos->x+1][pos->y].score;

    sprintf(score, "%d\t%d\t%d\t%d\n", score_e, score_n, score_s, score_w);
    log_msg(score);

    switch(pos->rot){
        case 0:
            if(score_n <= score_e && score_n <= score_s && score_n <= score_w){
                API_moveForward();
                pos->y++;
            }
            else if(score_w <= score_n && score_w <= score_s && score_w <= score_e){
                API_turnLeft();
                pos->rot = 1;
                API_moveForward();
                pos->x--;
            }
            else if(score_e <= score_n && score_e <= score_w && score_e <= score_s){
                API_turnRight();
                pos->rot = 3;
                API_moveForward();
                pos->x++;
            }
            break;
        case 1:
            if(score_w <= score_e && score_w <= score_s && score_w <= score_n){
                API_moveForward();
                pos->x--;
            }
            else if(score_n <= score_w && score_n <= score_s && score_n <= score_e){
                API_turnRight();
                pos->rot = 0;
                API_moveForward();
                pos->y++;
            }
            else if(score_s <= score_n && score_s <= score_w && score_s <= score_e){
                API_turnLeft();
                pos->rot = 2;
                API_moveForward();
                pos->y--;
            }
            break;
        case 2:
            if(score_s <= score_e && score_s <= score_n && score_s <= score_w){
                API_moveForward();
                pos->y--;
            }
            else if(score_w <= score_n && score_w <= score_s && score_w <= score_e){
                API_turnRight();
                pos->rot = 1;
                API_moveForward();
                pos->x--;
            }
            else if(score_e <= score_n && score_e <= score_w && score_e <= score_s){
                API_turnLeft();
                pos->rot = 3;
                API_moveForward();
                pos->x++;
            }
            break;
        case 3:
            if(score_e <= score_n && score_e <= score_s && score_e <= score_w){
                API_moveForward();
                pos->x++;
            }
            else if(score_s <= score_n && score_s <= score_w && score_s <= score_e){
                API_turnRight();
                pos->rot = 2;
                API_moveForward();
                pos->y--;
            }
            else if(score_n <= score_e && score_n <= score_w && score_n <= score_s){
                API_turnLeft();
                pos->rot = 0;
                API_moveForward();
                pos->y++;
            }
            break;
        default:
            log_msg("Error getting move");
            break;
    }
}

int main(int argc, char* argv[]) {
    int i, j;

    struct position pos = {0, 0, 0, API_mazeHeight(), API_mazeWidth()};

    struct cell_status **cell_stat = malloc(pos.rows * sizeof(struct cell_status *));
    if (!cell_stat) {
        log_msg("Memory allocation failed for rows");
        return 1;
    }

    for (i = 0; i < pos.rows; i++) {
        cell_stat[i] = malloc(pos.cols * sizeof(struct cell_status));
        if (!cell_stat[i]) {
            log_msg("Memory allocation failed for columns");
            for (j = 0; j < i; j++)
                free(cell_stat[j]);
            free(cell_stat);
            return 1;
        }
    }

    init_cell(cell_stat, &pos);
    init_cell_score(cell_stat, &pos);
    init_tester(cell_stat, &pos);

    for(int l = 0; l < 300; l++) {
        check_wall(cell_stat, &pos);
        get_move(cell_stat, &pos);
    }

    for (i = 0; i < pos.rows; i++)
        free(cell_stat[i]);
    free(cell_stat);

    return 0;
}
