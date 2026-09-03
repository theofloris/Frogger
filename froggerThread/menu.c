#include "menu.h"

// ---------------- MENU ---------------- //

// main menu
const char *MENU_TEXT[WIDTH_MAIN_MENU] = {
    "      PLAY      ",
    "      QUIT      ",
    "     CREDIT     "
};

// sprite 
const char *WLCM_TEXT[WIDTH_WLCM_TEXT] = {
".___                  ",                  
"[__ ._. _  _  _  _ ._.",
"|   [  (_)(_](_](/,[  ",
"          ._|._|      "
};

// credits menu
const char *CREDITS_TEXT[WIDTH_CRDT_TEXT] = {
"Created by : Andrea A. & Theo F.",
"Music : CALMAN 160-GTA IV SONG  ",
"To go back press left arrow     ",
};

// reset menu
const char *RESTART_MENU[WIDTH_RSTRT_TEXT] = {
    "            ",
    "   RESTART  ", 
    "    QUIT    ", 
    "            ",
};

// function main manu
int menu(WINDOW *win){
    nodelay(stdscr, false);
    keypad(stdscr, true);
    coloring_starter(win);
    
    int ch; 
    do{
        ch = getch();
    }
    while(ch != 10);
    
    werase(win);

    int row = HEIGHT_MAP / 2;
    ch = 0; 
    
    while(true){
        coloring_main_menu(win);
        coloring_line_menu(win, row - HEIGHT_MAP / 2);

        ch = getch(); 
        switch(ch){
            case KEY_DOWN:
                row++; 
                if(row > HEIGHT_MAP / 2 + 2) row = HEIGHT_MAP / 2;
                break; 
            case KEY_UP:
                row--;
                if(row < HEIGHT_MAP / 2) row = HEIGHT_MAP / 2 + 2;
                break;
            case 10:
                if(row - HEIGHT_MAP / 2 == 2){
                    werase(win); 
                    credits_menu(win);
                    do{
                        ch = getch();
                    }
                    while(ch != KEY_LEFT);
                    erase(); 
                    break;  
                } 
                erase();
                nodelay(stdscr, true);
                keypad(stdscr, false); 
                return row - HEIGHT_MAP / 2; //0, 1
            default: 
                break;            
        }
        refresh();
        usleep(50000);    
    }     
}

void coloring_starter(WINDOW *win){
    mvwaddstr(win, HEIGHT_MAP / 2 + 10, WIDTH_MAP / 2 - 12, "Press Enter to open the menu");
    refresh();  
    for(int j = 0; j < HEIGHT_WLCM_TEXT; j++){
            for(int i = 0; i < WIDTH_WLCM_TEXT; i++){
            wattron(win, COLOR_PAIR(12));
            mvwaddch(win, HEIGHT_MAP / 2 + j - 5, WIDTH_MAP/ 2 + i - 9, WLCM_TEXT[j][i]);
            wattroff(win, COLOR_PAIR(12));
        }
    }

    wrefresh(win);    
}   

void coloring_main_menu(WINDOW *win){
    for(int j = 0; j < HEIGHT_MAIN_MENU; j++){
        for(int i = 0; i < WIDTH_MAIN_MENU; i++){
            wattron(win, COLOR_PAIR(4));
            mvwaddch(win, HEIGHT_MAP / 2 + j - 3, WIDTH_MAP / 2 + i - 7, MENU_TEXT[j][i]);
            wattroff(win, COLOR_PAIR(4));
        } 

        wrefresh(win);
    }
}

void coloring_line_menu(WINDOW *win, int row){
    for(int i = 0; i < WIDTH_MAIN_MENU; i++){
        wattron(win, COLOR_PAIR(11));
        mvwaddch(win, HEIGHT_MAP / 2 + row - 3, WIDTH_MAP / 2 + i - 7, MENU_TEXT[row][i]);
        wattroff(win, COLOR_PAIR(11)); 
    }

    wrefresh(win);
}

void credits_menu(WINDOW *win){
    for(int j = 0; j < HEIGHT_CRDT_TEXT; j++){
        for(int i = 0; i < WIDTH_CRDT_TEXT; i++){
            wattron(win, COLOR_PAIR(4));
            mvwaddch(win, HEIGHT_MAP / 2 + j - 5, WIDTH_MAP / 2 + i - 14, CREDITS_TEXT[j][i]);
            wattroff(win, COLOR_PAIR(4));
        } 
        wrefresh(win);
    }

}

int reset_menu(WINDOW *win){ 

    keypad(stdscr, true);
    nodelay(stdscr, false); 
    int row = HEIGHT_MAP / 2 + 1;
    int ch = 0; 
    
    while(true){
        print_restart_menu(win);
        coloring_line_r_menu(win, row - HEIGHT_MAP / 2);

        ch = getch(); 
        switch(ch){
            case KEY_DOWN:
                row++; 
                if(row > HEIGHT_MAP / 2 + 2) row = HEIGHT_MAP / 2 + 1;
                break; 
            case KEY_UP:
                row--;
                if(row < HEIGHT_MAP / 2 + 1) row = HEIGHT_MAP / 2 + 2;
                break;
            case 10:
                nodelay(stdscr, true);
                keypad(stdscr, false); 
                return row - HEIGHT_MAP / 2 + 1; 
            default: 
                break;               
        }
        refresh();
        usleep(50000);      
    }
}

void print_restart_menu(WINDOW *win){
    for(int j = 0; j < HEIGHT_RSTRT_TEXT; j++){
        for(int i = 0; i < WIDTH_RSTRT_TEXT; i++){
            wattron(win, COLOR_PAIR(4));
            mvwaddch(win, HEIGHT_MAP / 2 + j, WIDTH_MAP / 2 + i - 5, RESTART_MENU[j][i]);
            wattroff(win, COLOR_PAIR(4));
        } 
        wrefresh(win);
    }
}

void coloring_line_r_menu(WINDOW *win, int row){
    for(int i = 0; i < WIDTH_RSTRT_TEXT; i++){
        wattron(win, COLOR_PAIR(11));
        mvwaddch(win, HEIGHT_MAP / 2 + row, WIDTH_MAP / 2 + i - 5, RESTART_MENU[row][i]);
        wattroff(win, COLOR_PAIR(11)); 
    }
    wrefresh(win);
}