#ifndef BULLET_H
#define BULLET_H

#include "main.h"
extern pthread_mutex_t mtx;

void *shot_bullet(void *bullet);
void coloring_bullet_frog(WINDOW *win, bullet_s bullet);
void coloring_bullet_plants(WINDOW *win, bullet_s bullet[]); 

#endif