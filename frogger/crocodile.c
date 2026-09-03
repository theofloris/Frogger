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

int id_croc = 0; 

// function to initialize crocodiles
void crocodile_init(int pipe_fd[], crocodile croc){ 
    close(pipe_fd[0]);
    
    srand(croc.id * time(NULL));
    if(croc.id < 9) usleep(rand_(0, 3500000));
    else usleep(rand_(5500000, 7000000));

    msg msg_c;
    int start_timer;
    int timer; // timer that will be used to determine how long a BAD crocodile stays before going underwater

    croc.type = rand() % 2;  // 0 = Bad Crocodile, 1 = Good Crocodile;
    croc.status = SAFE; 

    if(croc.type == BAD){
        timer = rand_(7,20);
        start_timer = time(NULL);
    }

    while(true){
        if (croc.direction == 0){ // if the crocodile is moving to the right
            if (croc.x == WIDTH_MAP){ // if the crocodile goes out of the game area, reset it
                croc.x = - 1;
                croc.type = rand() % 2;
                croc.status = SAFE;
                croc.first_spawn = true;
                sleep(2);

                if(croc.type == BAD){ 
                    timer = rand_(7,20);
                    start_timer = time(NULL);
                }
            }
            croc.x++; // move to the right
        }

        else{ // if the crocodile is moving to the left
            if (croc.x == - WIDTH_CROC){ 
                croc.x = WIDTH_MAP + 1;
                croc.type = rand() % 2;
                croc.status = SAFE;
                croc.first_spawn = true;
                sleep(2);

                if(croc.type == BAD){
                    timer = rand_(7,20);
                    start_timer = time(NULL);
                }
            }
            croc.x--; // move to the left
        }
        
        if(croc.type == BAD){ // if the crocodile is bad
            if(time(NULL) - start_timer >= timer){ // and its time above the water is up
                croc.status = UNDERWATER; // the crocodile will go underwater
            }
            else if(time(NULL) - start_timer >= timer / 2){ // if its timer is halfway, the crocodile will change from yellow to red
                croc.status = WARNING;
            }
        }

        usleep(croc.speed);

        // message sent with the crocodile information to render
        msg_c = (msg){croc.id, croc.y, croc.x, 0 , croc.type, croc.direction, croc.status, croc.first_spawn}; 
        write(pipe_fd[1], &msg_c, sizeof(msg)); 
        croc.first_spawn = false;
    }

    close(pipe_fd[1]);
    _exit(0);
} 

// function to create the river's elements
void river(int pipe_fd[]){
    msg msg_c;
    int croc_speed[9] = {0}; // array with the speed of each lane
    int croc_direction[9] = {0}; // array with the direction of each lane

    for(int i = 0; i < 9; i++){
        croc_speed[i] = rand_(100000, 300000);
        croc_direction[i] = rand_(0, 1);  
    }

    for(int n_croc = 1; n_croc < 3; n_croc++){
        
        for(int i = 0; i < 9; i++){

            // create a child process for each crocodile
            pid_t pid = fork();

            if (pid < 0){  // gestisco evenutali errori
                perror("Error, child not created"); 
                _exit(1);
            }
            else if (pid == 0){
                crocodile_init(pipe_fd,(crocodile){id_croc, i * 2 + 10, (croc_direction[i] == 0) ? - WIDTH_CROC : WIDTH_MAP, croc_speed[i], croc_direction[i], 0, 0 , true});
            }

            else{ 
                close(pipe_fd[0]);
                msg_c = (msg){200, 0, 0, pid};
                write(pipe_fd[1], &msg_c, sizeof(msg_c));
            }

            id_croc++;
        }
        sleep(rand_(1, 4));
    }
}

// function to color crocodiles
void coloring_croc(WINDOW *win, crocodile croc[]){
    for (int k = 0; k < 18; k++){ 

        if(croc[k].type == GOOD) coloring_good_croc(k, croc, win);
        else coloring_bad_croc(k, croc, win);    
    }   
}

// function to color good crocodiles
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

// function to color bad crocodiles
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