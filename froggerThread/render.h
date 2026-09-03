#ifndef RENDER_H
#define RENDER_H

#include "main.h"

game_status render(WINDOW *win, game_status gs, int client_socket);
void coloring_map(WINDOW *win, int score);
void coloring_lair(WINDOW *win, lair_s lair[]);
void game_colors();
void init_colors();

#endif