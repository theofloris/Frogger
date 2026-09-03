#include "crocodile.h"

// ---------------- CROCODILES ---------------- //

// sprite 
const char *sprite_croc_l[HEIGHT_CROC] = {
    "_______0",
    "_______0"
};

const char *sprite_croc_r[HEIGHT_CROC] = {
    "0_______",
    "0_______"
};

// function to create the river
void *river() {
    msg msg_c;
    int croc_speed[9] = {0}; // array of speeds for each crocodile
    int croc_direction[9] = {0}; // array of directions for each crocodile
    pthread_t tid_crc;

    int direction = rand_(0,1); // set random direction
    int inverse_direction = !direction; // set inverse direction

    for (int i = 0; i < 9; i++) { 
        croc_speed[i] = rand_(100000, 300000);
        croc_direction[i] = (i % 2 == 0) ? direction :  inverse_direction; 
    }

    int id_croc = 0; 
    for (int n_croc = 1; n_croc < 3; n_croc++) { // for each wave of crocodiles 
        for (int j = 0; j < 9; j++) { // for each crocodile
            
            // create crocodile 
            crocodile croc_data = {id_croc, j * 2 + 10, (croc_direction[j] == 0) ? - WIDTH_CROC : WIDTH_MAP, croc_speed[j], croc_direction[j], 0, 0 , true};
            pthread_create(&tid_crc, NULL, crocodile_init, &croc_data); 

            msg_c = (msg){200, 0, 0, tid_crc};
            write_message(msg_c);
            usleep(50000);
            id_croc++;
        }

        sleep(rand_(1, 4));
    }
    pthread_exit(NULL);
}

// function to create crocodile
void *crocodile_init(void *croc) {
    crocodile cr = *((crocodile *)croc); 

    srand(cr.id * time(NULL));
    if (cr.id < 9) usleep(rand_(0, 3500000));
    else usleep(rand_(4500000, 6500000));

    msg msg_c;
    int start_timer;
    int timer;

    cr.type = rand() % 2; // 0 = bad, 1 = good
    cr.status = SAFE;

    if (cr.type == BAD) {
        timer = rand_(7, 20);
        start_timer = time(NULL);
    }

    while (true) {
        if (cr.direction == 0) { // if the crocodile is going to the right
            if (cr.x == WIDTH_MAP) { 
                cr.x = -1;

                // reset del coccodrillo
                cr.type = rand() % 2;
                cr.status = SAFE;
                cr.first_spawn = true;

                sleep(2);

                
                if (cr.type == BAD) { // if the crocodile is bad, set a random timer
                    timer = rand_(7, 20);
                    start_timer = time(NULL);
                }
            }
            cr.x++;

        } else { // if the crocodile is going to the left
            if (cr.x == - WIDTH_CROC) {
                cr.x = WIDTH_MAP + 1;
                // reset del coccodrillo
                cr.type = rand() % 2;
                cr.status = SAFE;
                cr.first_spawn = true;

                sleep(2);

                if (cr.type == BAD) { 
                    timer = rand_(7, 20);
                    start_timer = time(NULL);
                }
            }
            cr.x--;
        }

        if (cr.type == BAD) {
            if (time(NULL) - start_timer >= timer) { // if the timer is over, the crocodile will go underwater
                cr.status = UNDERWATER; 
            } else if (time(NULL) - start_timer >= timer / 2) { // if the timer is half over, the crocodile will turn red
                cr.status = WARNING;
            }
        }

        usleep(cr.speed);
        
        msg_c = (msg){cr.id, cr.y, cr.x, 0, cr.type, cr.direction, cr.status, cr.first_spawn};
        write_message(msg_c);
        cr.first_spawn = false;
    }
    pthread_exit(NULL);
}

// function to color the crocodiles
void coloring_croc(WINDOW *win, crocodile croc[]){ 
    for (int k = 0; k < 18; k++){ 

        if(croc[k].type == GOOD) coloring_good_croc(k, croc, win); 
        else coloring_bad_croc(k, croc, win);    
    }   
}

// function to color the good crocodiles 
void coloring_good_croc(int k, crocodile croc[], WINDOW *win){
    if (croc[k].direction == 0){
        for (int i = 0 ; i < HEIGHT_CROC; i++){
            for (int j = 0; j < WIDTH_CROC; j++){

                if (sprite_croc_l[i][j] == '0'){
                    wattron(win, COLOR_PAIR(5));
                    mvwaddch(win, i + croc[k].y , j + croc[k].x, sprite_croc_l[i][j]);
                    wattroff(win, COLOR_PAIR(5));
                }

                else{
                    wattron(win, COLOR_PAIR(2));
                    mvwaddch(win, i + croc[k].y , j + croc[k].x, sprite_croc_l[i][j]);
                    wattroff(win, COLOR_PAIR(2));
                }
            }
        }
    }
    else {
        for (int i = 0 ; i < HEIGHT_CROC; i++){
            for (int j = 0; j < WIDTH_CROC; j++){

                if (sprite_croc_r[i][j] == '0'){
                    wattron(win, COLOR_PAIR(5));
                    mvwaddch(win, i + croc[k].y , j + croc[k].x, sprite_croc_r[i][j]);
                    wattroff(win, COLOR_PAIR(5));
                }

                else{
                    wattron(win, COLOR_PAIR(2));
                    mvwaddch(win, i + croc[k].y , j + croc[k].x, sprite_croc_r[i][j]);
                    wattroff(win, COLOR_PAIR(2));
                }
            }
        }
    }
}

// function to color the bad crocodiles 
void coloring_bad_croc(int k, crocodile croc[], WINDOW *win){
    int skin; 
    int eyes;

    switch (croc[k].status){
        case SAFE:
            skin = 8;
            eyes = 6;
            break;
        case WARNING:
            skin = 1;
            eyes = 6;
            break;
        
        case UNDERWATER:
            skin = 9;
            eyes = 10; 
            break;
    }

    if (croc[k].direction == 0){        
        for (int i = 0; i < HEIGHT_CROC; i++){
            for (int j = 0; j < WIDTH_CROC; j++){
                    
                if (sprite_croc_l[i][j] == '0'){
                    wattron(win, COLOR_PAIR(eyes));
                    mvwaddch(win, i + croc[k].y , j + croc[k].x, sprite_croc_l[i][j]);
                    wattroff(win, COLOR_PAIR(eyes));
                }

                else{
                    wattron(win, COLOR_PAIR(skin));
                    mvwaddch(win, i + croc[k].y , j + croc[k].x, sprite_croc_l[i][j]);
                    wattroff(win, COLOR_PAIR(skin));
                }
            }
        }
    }

    else {
        for (int i = 0 ; i < HEIGHT_CROC; i++){
            for (int j = 0; j < WIDTH_CROC; j++){
                    
                if (sprite_croc_r[i][j] == '0'){
                    wattron(win, COLOR_PAIR(eyes));
                    mvwaddch(win, i + croc[k].y , j + croc[k].x, sprite_croc_r[i][j]);
                    wattroff(win, COLOR_PAIR(eyes));
                }
                else{
                    wattron(win, COLOR_PAIR(skin));
                    mvwaddch(win, i + croc[k].y , j + croc[k].x, sprite_croc_r[i][j]);
                    wattroff(win, COLOR_PAIR(skin));
                }
            }
        }
    }
}