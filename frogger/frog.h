#ifndef FROG_H
#define FROG_H

#define HEIGHT_FROG 2
#define WIDTH_FROG 3

#define INIT_LIFES 3
#define MIN_LIFES 0

#define ID_FROG 100
#include "main.h"

typedef struct{
	int id;
	int y;
	int x; 
	int on_croc; // 0 = not on croc, 1 = on croc
	bool has_shot;  
} frog_s;


void frog(int pipe_fds[]); 
void coloring_lifes(WINDOW *win, int lifes);
void coloring_frog(WINDOW *win, frog_s f);
  
#endif