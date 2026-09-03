#ifndef CROCODILE_H
#define CROCODILE_H
#include "main.h"


#define HEIGHT_CROC 2
#define WIDTH_CROC 8

enum type_c {
  BAD,
  GOOD
};

enum status_c {
  UNDERWATER, // light blue croc, underwater
  WARNING, // yellow, red croc
  SAFE // green croc
};

typedef struct{
    int id;
    int y;
    int x;
    int speed;               // milliseconds
    int direction;           // 0 = left, 1 = right
    enum type_c type;        // 0 = bad, 1 = good
    enum status_c status;    // 0 = underwater, 1 = warning, 2 = safe
    bool first_spawn; 
} crocodile; 


void crocodile_init(int pipe_fd[], crocodile croc);
void coloring_good_croc(int k, crocodile croc[], WINDOW *win);
void coloring_bad_croc(int k, crocodile croc[], WINDOW *win);
void coloring_croc(WINDOW *win, crocodile croc[]);
void river(int pipe_fd[]);

#endif