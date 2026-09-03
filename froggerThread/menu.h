#ifndef MENU_H
#define MENU_H
#include "main.h"

int menu(WINDOW *win); //main function for the menu
void coloring_starter(WINDOW *win); //function for coloring the "Frogger" sprite
void coloring_main_menu(WINDOW *win); //function for coloring the menu
void coloring_line_menu(WINDOW *win, int row); //function for coloring the row which is selected 
void credits_menu(WINDOW *win);

//This 3 function are for the menu which appears when the player lose a game
int reset_menu(WINDOW *win); 
void print_restart_menu(WINDOW *win);
void coloring_line_r_menu(WINDOW *win, int row);

#endif 