#ifndef CROCODILE_H
#define CROCODILE_H
#include "main.h"

void *crocodile_init(void *croc);

void *river(); 

void coloring_croc(WINDOW *win, crocodile croc[]);

void coloring_good_croc(int k, crocodile croc[], WINDOW *win);
 
void coloring_bad_croc(int k, crocodile croc[], WINDOW *win); 

#endif