#ifndef MAIN_H
#define MAIN_H

#include <stdlib.h>

#include <ncurses.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <signal.h>
#include <pthread.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <fcntl.h>

#define HEIGHT_MAP 35
#define WIDTH_MAP 69

#define WIDTH_MAIN_MENU 16
#define HEIGHT_MAIN_MENU 3
#define WIDTH_WLCM_TEXT 22
#define HEIGHT_WLCM_TEXT 4
#define WIDTH_CRDT_TEXT 32
#define HEIGHT_CRDT_TEXT 3

#define WIDTH_RSTRT_TEXT 12
#define HEIGHT_RSTRT_TEXT 4

#define COLOR_UNDERWATER 9
#define COLOR_GRAY 10

#define BUFFER_DIM 100
typedef struct {
    int id;
    int y;
    int x;
	pthread_t tid;
	int type_croc; 
	int direction;
	int status_croc;
	bool first_spawn;
	int timer;
    bool is_alive; 
} msg; 

#define HEIGHT_FROG 2
#define WIDTH_FROG 3
#define ID_FROG 100
#define INIT_LIFES 3
typedef struct{
	int id;
	int y;
	int x; 
	int on_croc; // 0 = not on croc, 1 = on croc
	bool has_shot;
	int lifes;   
} frog_s;

#define HEIGHT_PLANT 2
#define WIDTH_PLANT 2
#define NUM_PLANTS 3
#define TID_MSG 300
#define ID_PLANT 701
typedef struct{
    int id;
    int y;
    int x;
    pthread_t tid;
    bool is_alive;
    bool has_shot; 
} plant_s;

#define ID_BULLET 800
typedef struct bullet{
    int id; 
    int y;
    int x;
    pthread_t tid; 
    bool is_alive;
    int dir; // 0 for down, 1 for up
} bullet_s;

#define HEIGHT_CROC 2
#define WIDTH_CROC 8
enum type_c {
  BAD,
  GOOD
};

enum status_c {
  UNDERWATER, //light blue croc, underwater
  WARNING, //yellow, red croc
  SAFE //green croc
};

#define TID_MSG_C 500
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

#define ID_TIMER 400
#define START 60
#define END 0
typedef struct{
    int id;
    int timer;
} timer_s;

typedef struct{
	int lifes; 
	int timer; 
	int score;
	int status;
	bool lair_status [5]; 
}game_status;

#define COLOR_UNDERWATER 9
#define COLOR_GRAY 10
#define HEIGHT_LAIR 2
#define ID_LAIR 300
typedef struct{
    int id;
    int x;
    int y;
    bool open;
} lair_s;

void init();
void kill_all_threads(pthread_t tid[], int count_tid); 
int rand_(int min, int max);
void end_music(Mix_Music *bgm); 

#include "thread_control.h"
#include "render.h"
#include "frog.h"
#include "bullet.h"
#include "collision.h"
#include "plants.h"
#include "crocodile.h"
#include "timer.h"
#include "menu.h"

#endif