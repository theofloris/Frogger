#ifndef PLANTS_H
#define PLANTS_H
#include "main.h"

#define HEIGHT_PLANT 2
#define WIDTH_PLANT 2
#define ID_PLANT 701

typedef struct{
    int id;
    int y;
    int x;
    bool is_alive;
    pid_t pid; 
    bool has_shot;
} plant_s;

void plant_init(int pipe_fd[], plant_s plant);
void coloring_plant(plant_s plant[], WINDOW *win);
void land_plants(int pipe_fd[], WINDOW *win);
#endif