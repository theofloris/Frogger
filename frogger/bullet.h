#ifndef BULLET_H
#define BULLET_H

#define ID_BULLET 800
#include "main.h"

typedef struct bullet{
    int id; 
    int y;
    int x;
    pid_t pid;
    bool is_alive;
    int dir; // 0 for down, 1 for up
} bullet_s;

void shot_bullet(int pipe_fd[], bullet_s bullet);
void coloring_bullet_frog(WINDOW *win, bullet_s bullet);
void coloring_bullet_plants(WINDOW *win, bullet_s bullet[]);

#endif