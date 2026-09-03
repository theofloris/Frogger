#ifndef TIMER_H
#define TIMER_H
#include "main.h"

#define ID_TIMER 400
#define START 60
#define END 0

typedef struct{
    int id;
    int timer;
} timer_s;

void timer_(int pipe_fd[]);

#endif