#ifndef MENU_H
#define MENU_H
#include "main.h"

#define WIDTH_MAIN_MENU 16
#define HEIGHT_MAIN_MENU 3
#define WIDTH_WLCM_TEXT 22
#define HEIGHT_WLCM_TEXT 4
#define WIDTH_CRDT_TEXT 32
#define HEIGHT_CRDT_TEXT 3


int menu(WINDOW *win); 
void coloring_starter(WINDOW *win); 
void coloring_main_menu(WINDOW *win); 
void coloring_line_menu(WINDOW *win, int row);
void credits_menu(WINDOW *win);

#define WIDTH_RSTRT_TEXT 12
#define HEIGHT_RSTRT_TEXT 4

int reset_menu(WINDOW *win); 
void print_restart_menu(WINDOW *win);
void coloring_line_r_menu(WINDOW *win, int row);

#endif 