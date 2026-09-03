#include "main.h"

// ---------------- PLANTS ---------------- //

// sprite 
const char *sprite_plant[HEIGHT_PLANT] = {
    "xO",
    "||"
};

// function to create the plants
void *plant_init(void *plant) { 
    msg msg_plant;
    plant_s *p = (plant_s *)plant; 

    while (true) {
        msg_plant = (msg){p->id, p->y, p->x, p->tid};
        write_message(msg_plant);
        flushinp();

        usleep(1000000);
    }
}

// function to create the plants
void *land_plants(){
    msg msg_p;
    
    plant_s array_plant[3];

    for(int k = 0; k < 3; k++){ 
        switch (k){ 
            case 0: 
                msg_p.x = rand_(1, 20);
                break;
            
            case 1: 
                msg_p.x = rand_(23, 43);
                break;

            case 2: 
                msg_p.x = rand_(46, 66);
                break;
        }
        
        msg_p.y = rand_(6,8); 

        
        pthread_t tid_plant;  
        array_plant[k] = (plant_s){ID_PLANT + k, msg_p.y, msg_p.x, true, 0, false};
        pthread_create(&tid_plant, NULL, &plant_init, &array_plant[k]); 
        
       
        msg msg_tid_p = (msg){TID_MSG, k, 0, tid_plant};  
        write_message(msg_tid_p); 

        sleep(rand_(2,5));
    }

    while (true) { 
        usleep(10000000);
    }
}
// function to color the plants
void coloring_plant(plant_s plant[], WINDOW *win){
    for(int k = 0; k < NUM_PLANTS; k++){
        if(plant[k].is_alive == false) continue;
        for (int i = 0; i < HEIGHT_PLANT; i++){
            for (int j = 0; j < WIDTH_PLANT; j++){
                if (sprite_plant[i][j] == 'x'){
                    wattron(win, COLOR_PAIR(2)); 
                    mvwaddch(win, i + plant[k].y, j + plant[k].x, sprite_plant[i][j]); 
                    wattroff(win, COLOR_PAIR(2));
                }
                else if(sprite_plant[i][j] == 'O'){
                    wattron(win, COLOR_PAIR(1));
                    mvwaddch(win, i + plant[k].y, j + plant[k].x, sprite_plant[i][j]);
                    wattroff(win, COLOR_PAIR(1));
                }
                else{
                    wattron(win, COLOR_PAIR(3));
                    mvwaddch(win, i + plant[k].y , j + plant[k].x, sprite_plant[i][j]);
                    wattroff(win, COLOR_PAIR(3));
                }
            }
        }
    }
}