#ifndef RENDER_H
#define RENDER_H

#define COLOR_UNDERWATER 9
#define COLOR_GRAY 10
#define HEIGHT_LAIR 2
#define ID_LAIR 300


#include "main.h"

typedef struct{
    int id;
    int x;
    int y;
    bool open;
} lair_s;

void coloring_map(WINDOW *win, int score);

void game_colors();
void init_colors();
void coloring_lair(WINDOW *win, lair_s lair[]);
game_status render(int pipe_fds[], WINDOW *win, game_status gs, int client_socket);


#endif