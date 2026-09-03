#ifndef MAIN_H
#define MAIN_H

#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <fcntl.h>

#define HEIGHT_MAP 35
#define WIDTH_MAP 69

typedef struct{
	int lifes; 
	int timer; 
	int score;
	int status;
	bool lair_status [5]; 
}game_status;

typedef struct{
    int id;
    int y;
    int x;
	pid_t pid;
	int type_croc; 
	int direction;
	int status_croc;
	bool first_spawn;
	int timer;
	int lifes;   
} msg;
	 
void kill_process(pid_t pid); 
void kill_all(int pid[], int len_pid);
int rand_(int min, int max);
void end_music(Mix_Music *bgm);

#include "frog.h"
#include "render.h"
#include "crocodile.h"
#include "plants.h"
#include "bullet.h"
#include "menu.h"
#include "timer.h"

#endif