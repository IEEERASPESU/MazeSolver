#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
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
    int rot;  // 0=N, 1=W, 2=S, 3=E
    int rows;
    int cols;
};

void log_msg(const char* text) {
    fprintf(stderr, "%s", text);
    fflush(stderr);
}

void init_cell(struct cell_status **cell_stat, struct position *pos) {
    for (int i = 0; i < pos->rows; i++)
        for (int j = 0; j < pos->cols; j++) {
            cell_stat[i][j].explored = false;
            cell_stat[i][j].top_wall = false;
            cell_stat[i][j].bottom_wall = false;
            cell_stat[i][j].left_wall = false;
            cell_stat[i][j].right_wall = false;
            cell_stat[i][j].score = INT_MAX;
        }
}

// Goal is the center 1x1 or 2x2 block, depending on maze size
bool at_goal(struct position *pos) {
    int center_x1 = (pos->rows - 1) / 2, center_x2 = pos->rows / 2;
    int center_y1 = (pos->cols - 1) / 2, center_y2 = pos->cols / 2;
    return (pos->x >= center_x1 && pos->x <= center_x2 &&
            pos->y >= center_y1 && pos->y <= center_y2);
}

void set_wall(struct cell_status **cell_stat, struct position *pos) {
    if (cell_stat[pos->x][pos->y].top_wall)
        API_setWall(pos->x, pos->y, 'n');
    if (cell_stat[pos->x][pos->y].bottom_wall)
        API_setWall(pos->x, pos->y, 's');
    if (cell_stat[pos->x][pos->y].left_wall)
        API_setWall(pos->x, pos->y, 'w');
    if (cell_stat[pos->x][pos->y].right_wall)
        API_setWall(pos->x, pos->y, 'e');
}

void check_wall(struct cell_status **cell_stat, struct position *pos) {
    API_setColor(pos->x, pos->y, 'y');
    if (!cell_stat[pos->x][pos->y].explored) {
        switch (pos->rot) {
            case 0: // North
                cell_stat[pos->x][pos->y].top_wall = API_wallFront();
                cell_stat[pos->x][pos->y].left_wall = API_wallLeft();
                cell_stat[pos->x][pos->y].right_wall = API_wallRight();
                break;
            case 1: // West
                cell_stat[pos->x][pos->y].left_wall = API_wallFront();
                cell_stat[pos->x][pos->y].bottom_wall = API_wallLeft();
                cell_stat[pos->x][pos->y].top_wall = API_wallRight();
                break;
            case 2: // South
                cell_stat[pos->x][pos->y].bottom_wall = API_wallFront();
                cell_stat[pos->x][pos->y].right_wall = API_wallLeft();
                cell_stat[pos->x][pos->y].left_wall = API_wallRight();
                break;
            case 3: // East
                cell_stat[pos->x][pos->y].right_wall = API_wallFront();
                cell_stat[pos->x][pos->y].top_wall = API_wallLeft();
                cell_stat[pos->x][pos->y].bottom_wall = API_wallRight();
                break;
            default:
                log_msg("Error checking walls\n");
                break;
        }
        set_wall(cell_stat, pos);
    }
    cell_stat[pos->x][pos->y].explored = true;
}

// Flood-fill from center, score = distance to center
void flood_fill_score(struct cell_status **cell_stat, struct position *pos) {
    int rows = pos->rows, cols = pos->cols;
    int center_x1 = (rows - 1) / 2, center_x2 = rows / 2;
    int center_y1 = (cols - 1) / 2, center_y2 = cols / 2;

    // Reset all scores to max
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            cell_stat[i][j].score = INT_MAX;

    // BFS queue
    typedef struct { int x, y; } QNode;
    QNode *queue = malloc(rows * cols * sizeof(QNode));
    int front = 0, back = 0;

    // Enqueue all center cells (for both even and odd sizes)
    for (int cx = center_x1; cx <= center_x2; cx++)
        for (int cy = center_y1; cy <= center_y2; cy++) {
            cell_stat[cx][cy].score = 0;
            queue[back++] = (QNode){cx, cy};
        }

    // Directions: N, E, S, W
    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {1, 0, -1, 0};

    while (front < back) {
        QNode curr = queue[front++];
        int curr_score = cell_stat[curr.x][curr.y].score;
        for (int d = 0; d < 4; d++) {
            int nx = curr.x + dx[d];
            int ny = curr.y + dy[d];
            if (nx >= 0 && nx < rows && ny >= 0 && ny < cols) {
                // Check for walls
                bool wall = false;
                if (d == 0) wall = cell_stat[curr.x][curr.y].top_wall;
                else if (d == 1) wall = cell_stat[curr.x][curr.y].right_wall;
                else if (d == 2) wall = cell_stat[curr.x][curr.y].bottom_wall;
                else if (d == 3) wall = cell_stat[curr.x][curr.y].left_wall;
                if (wall) continue;

                if (cell_stat[nx][ny].score > curr_score + 1) {
                    cell_stat[nx][ny].score = curr_score + 1;
                    queue[back++] = (QNode){nx, ny};
                }
            }
        }
    }
    free(queue);
}

// For visualization: display score at each cell
void show_scores(struct cell_status **cell_stat, struct position *pos) {
    char score[8];
    for (int i = 0; i < pos->rows; i++)
        for (int j = 0; j < pos->cols; j++) {
            sprintf(score, "%d", cell_stat[i][j].score);
            API_setText(i, j, score);
        }
}

// Move to neighbor with lowest score (greedy)
void get_move(struct cell_status **cell_stat, struct position *pos) {
    int x = pos->x, y = pos->y;
    int best_dir = -1, best_score = INT_MAX;
    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {1, 0, -1, 0};
    bool wall[4] = {
        cell_stat[x][y].top_wall,
        cell_stat[x][y].right_wall,
        cell_stat[x][y].bottom_wall,
        cell_stat[x][y].left_wall
    };
    int nx[4] = {x, x+1, x, x-1};
    int ny[4] = {y+1, y, y-1, y};

    // Find neighbor with lowest score
    for (int d = 0; d < 4; d++) {
        if (!wall[d] && nx[d] >= 0 && nx[d] < pos->rows && ny[d] >= 0 && ny[d] < pos->cols) {
            int s = cell_stat[nx[d]][ny[d]].score;
            if (s < best_score) {
                best_score = s;
                best_dir = d;
            }
        }
    }

    // Turn and move
    int rot = pos->rot;
    int turn = (best_dir - rot + 4) % 4;
    if (turn == 1) {
        API_turnRight();
        rot = (rot + 1) % 4;
    } else if (turn == 3) {
        API_turnLeft();
        rot = (rot + 3) % 4;
    } else if (turn == 2) {
        API_turnRight();
        API_turnRight();
        rot = (rot + 2) % 4;
    }
    API_moveForward();
    pos->rot = rot;
    pos->x = nx[best_dir];
    pos->y = ny[best_dir];
}

int main(int argc, char* argv[]) {
    struct position pos = {0, 0, 0, API_mazeHeight(), API_mazeWidth()};
    struct cell_status **cell_stat = malloc(pos.rows * sizeof(struct cell_status *));
    if (!cell_stat) {
        log_msg("Memory allocation failed for rows\n");
        return 1;
    }
    for (int i = 0; i < pos.rows; i++) {
        cell_stat[i] = malloc(pos.cols * sizeof(struct cell_status));
        if (!cell_stat[i]) {
            log_msg("Memory allocation failed for columns\n");
            for (int j = 0; j < i; j++)
                free(cell_stat[j]);
            free(cell_stat);
            return 1;
        }
    }

    log_msg("Flood-fill Micromouse Maze Solver\n");

    init_cell(cell_stat, &pos);
    flood_fill_score(cell_stat, &pos);
    show_scores(cell_stat, &pos);

    while (!at_goal(&pos)) {
        check_wall(cell_stat, &pos);
        flood_fill_score(cell_stat, &pos);
        show_scores(cell_stat, &pos);
        get_move(cell_stat, &pos);
    }
    log_msg("Goal reached!\n");

    for (int i = 0; i < pos.rows; i++)
        free(cell_stat[i]);
    free(cell_stat);

    return 0;
}
