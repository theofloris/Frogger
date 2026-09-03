#include "render.h"
#include "collisions.h"
#include "crocodile.h"
#include "plants.h"
#include "bullet.h"
#include "timer.h"
#include "frog.h"

// ---------------- RENDER ---------------- //
 
game_status render(int pipe_fd[], WINDOW *win, game_status gs, int client_socket){
    msg mex;

    lair_s lair[5]; // initialize the lairs
    for (int i = 0; i < 5; i++){
        lair[i].id = ID_LAIR + i;
        lair[i].x = 9 + i * 12;
        lair[i].y = 4;
        lair[i].open = gs.lair_status[i];
    }

    timer_s timer = {ID_TIMER}; 

    int is_on_top;

    int n_collision = 0; 

    pid_t list_pid[150]; // list of pids of all the processes
    int count_pid = 0;

    frog_s player = {0};  // initialize the frog
    player.id = ID_FROG;
    player.y = HEIGHT_MAP - HEIGHT_FROG - 3; 
    player.x = WIDTH_MAP / 2 - WIDTH_FROG / 2;
    player.on_croc = 0;
    player.has_shot = false;

    crocodile croc[18];
    for (int i = 0; i < 18; i++){ // initialize the crocodiles
        croc[i].y = 999;
        croc[i].x = 999;
        croc[i].type = GOOD;
        croc[i].status = SAFE; 
        croc[i].first_spawn = true;
    }

    plant_s plant[3] = {0};
    for(int i = 0; i < 3; i++){ // initialize the plants
        plant[i].y = 999;
        plant[i].x = 999; 
        plant[i].has_shot = false;
    }

    // initialize the bullets of the frog and the plants
    bullet_s bullet_frog, bullet_plants[3]; 

    // initialize the variables for the pids of the bullets of the frog and the plants
    pid_t pid_bullet_frog, pid_bullet_plant[3];
    
    // initialize the variables for the pids of the plants
    pid_t pid_plant[100];
    
    while(true){

        mex = (msg){0}; 
        coloring_lifes(win, gs.lifes); 

        coloring_map(win, gs.score); // create the map

        coloring_lair(win, lair); // create the lairs

        int recev = recv(client_socket, &mex, sizeof(mex), 0); // receive the message from the client
        if(recev <= 0){
            read(pipe_fd[0], &mex, sizeof(msg)); // read the message from the pipe
        }

        if(mex.id >= 0 && mex.id < 18){ // crocodile
            if (mex.first_spawn == true){
                croc[mex.id].y = mex.y;
                croc[mex.id].x = mex.x;
                croc[mex.id].id = mex.id;
                croc[mex.id].direction = mex.direction; 
                croc[mex.id].type = mex.type_croc;
                croc[mex.id].status = mex.status_croc;
                croc[mex.id].first_spawn = mex.first_spawn;
            }
            else if (mex.first_spawn == false){ 
                croc[mex.id].y = mex.y;
                croc[mex.id].x = mex.x;
                croc[mex.id].direction = mex.direction; 
                croc[mex.id].status = mex.status_croc;
                croc[mex.id].first_spawn = mex.first_spawn;
            }

            is_on_top = collision_croc_frog(croc, &player);   // check if the frog is on top of a crocodile
            if (player.on_croc == 1 && mex.id == is_on_top) { 
                if (croc[mex.id].direction == 0) player.x++;  // if the crocodile is moving to the right, the frog moves to the right 
                else player.x--;
            }
        }

        else if(mex.id == player.id){ // frog
            player.y += mex.y;
            player.x += mex.x;
             
            // check if the frog hits the borders of the map
            collisions(&player, plant);

            // check if the frog hits the lairs
            int collision_pf = 0; 
            collision_pf = collision_plant_frog(&player, plant);

            // if the frog hits a plant, we kill all the processes
            if (collision_pf == - 1){
                kill_all(list_pid, count_pid);
                gs.status = 5;
                return gs; 
            }
            
            // check if the frog hits the lairs
            int lair_result = 0;
            lair_result = collisions_lair(&player, lair);
            
            if (lair_result > -1){
                // set lair_status to false if the frog is inside the lair
                gs.lair_status[lair_result] = false;

                kill_all(list_pid, count_pid); 
                
                gs.status = 6;
                return gs; 
            }

            // if the frog hits the top of the map, we kill all the processes
            else if(lair_result == - 2){ 
                kill_all(list_pid, count_pid); 

                gs.status = 5; 
                return gs;
            }
            
            // controllo delle tane
            int gate_closed = 0;
            for (int i = 0; i < 5; i++){
                if (!lair[i].open){
                    lair[i].open = false; 
                    gate_closed++;    
                }    
            }

            int check_collision_croc_frog = 0;
            check_collision_croc_frog = collision_croc_frog(croc, &player); // check if the frog hits a crocodile
            collision_croc_frog(croc, &player); 
            if (player.on_croc == 0 && player.y > 9 && player.y < 28) {
                kill_all(list_pid, count_pid); 
                gs.status = 5; 
                return gs; 
            }
        }

        else if(mex.id == 200){ // pids
            list_pid[count_pid] = mex.pid;
            count_pid++;
        }

        // plants
        else if(mex.id >= ID_PLANT && mex.id < 704){
            plant[mex.id - ID_PLANT].y = mex.y;
            plant[mex.id - ID_PLANT].x = mex.x;
            plant[mex.id - ID_PLANT].id = mex.id;
            plant[mex.id - ID_PLANT].is_alive = true;
            plant[mex.id - ID_PLANT].pid = mex.pid;
            
            // if the plant has not shot, we create a bullet
            if (plant[mex.id - ID_PLANT].has_shot == false){
                int rand_shot = rand_(0,1);
                if (rand_shot == 1){  
                    plant[mex.id - ID_PLANT].has_shot = true; // set the flag to true

                    // create a bullet
                    pid_bullet_plant[mex.id - ID_PLANT] = fork(); 
                    if (pid_bullet_plant[mex.id - ID_PLANT] < 0){
                        perror("bullet not created");
                        _exit(1); 
                    }

                    else if (pid_bullet_plant[mex.id - ID_PLANT] == 0){
                        bullet_plants[mex.id - ID_PLANT].dir = 0;
                        bullet_plants[mex.id - ID_PLANT].y = plant[mex.id - ID_PLANT].y;
                        bullet_plants[mex.id - ID_PLANT].x = plant[mex.id - ID_PLANT].x;
                        bullet_plants[mex.id - ID_PLANT].id = ID_BULLET + (mex.id - 700);
                        bullet_plants[mex.id - ID_PLANT].pid = getpid();
                        shot_bullet(pipe_fd, bullet_plants[mex.id - ID_PLANT]);
                    }

                    else {
                        list_pid[count_pid] = pid_bullet_plant[mex.id - ID_PLANT];
                        count_pid++;
                    }
                }
            }
        }

        // timer
        else if(mex.id == ID_TIMER){ 
            timer.id = mex.id;
            gs.timer = mex.timer;
            if (gs.timer == 99 || gs.timer == 9) werase(win);

            if (gs.timer < 0) { // if the timer is less than 0, we kill all the processes
                kill_all(list_pid, count_pid);
                gs.status = 5; 
                return gs;
            }

            mvwprintw(win, HEIGHT_MAP - 2, 1, "TIMER: %d", gs.timer); refresh(); // print the timer on the screen
        }

        else if (mex.id == ID_BULLET){ // bullet frog
            bullet_frog.y = mex.y;
            bullet_frog.x = mex.x;
            bullet_frog.pid = mex.pid;
            bullet_frog.is_alive = true;

            // check the collision between the frog's bullet and plants' bullets
            int bullet_bullet = bullet_v_bullet(&bullet_frog, bullet_plants);

            // if the frog's bullet hits a plant's bullet they both die
            if (bullet_bullet != -1){
                bullet_frog.is_alive = false;
                player.has_shot = false;

                plant[bullet_bullet].has_shot = false; 
                bullet_plants[bullet_bullet].is_alive = false;

                kill_process(bullet_frog.pid);
                kill_process(bullet_plants[bullet_bullet].pid);
            }

            // check the collision between the frog's bullet and the plants
            int result_collision = collision_bullet(&player, &bullet_frog, plant, bullet_plants);

            // if the frog's bullet hits a plant, we kill the bullet process and the plant 
            if (result_collision < 3){
                
                switch (result_collision){
                    case 0: // first plant
                        mex.x = rand_(1, 20);
                        break;
                    
                    case 1: // second plant
                        mex.x = rand_(23, 43);
                        break;

                    case 2: // third plant
                        mex.x = rand_(46, 66);
                        break;
                }
                    
                mex.y = rand_(6,8);

                // create a new plant
                pid_plant[n_collision] = fork();
                if (pid_plant[n_collision] < 0){
                    perror("new plant not created");
                    _exit(1);
                }

                else if (pid_plant[n_collision] == 0){ // in child process create plant
                    plant[result_collision] = (plant_s){ID_PLANT + result_collision, mex.y, mex.x, true, getpid(), false};
                    plant_init(pipe_fd, plant[result_collision]);
                }

                else {
                    list_pid[count_pid] = pid_plant[n_collision];
                    count_pid++;
                }

                n_collision++;
                gs.score += 7;
            }

            // check the collision between the frog's bullet and the crocodiles
            int collision_bullet_f_to_c = 0; 

            collision_bullet_f_to_c = collision_bullet_croc(&player, &bullet_frog, croc); 

            // if the frog's bullet hits a crocodile, we kill the bullet process
            if(collision_bullet_f_to_c >= 0 && collision_bullet_f_to_c < 18){
                
                // and if the crocodile was bad we set it to good
                if(bullet_frog.y == croc[collision_bullet_f_to_c].y){
                    if(croc[collision_bullet_f_to_c].type == BAD){
                        croc[collision_bullet_f_to_c].type = GOOD;

                        gs.score += 30; 
                    }
                }
            }
        }

        // plants' bullet
        else if (mex.id > ID_BULLET && mex.id < ID_BULLET + 4){
            bullet_plants[mex.id % 10 - 1].y = mex.y;
            bullet_plants[mex.id % 10 - 1].x = mex.x;
            bullet_plants[mex.id % 10 - 1].pid = mex.pid;
            bullet_plants[mex.id % 10 - 1].is_alive = true;

            // check the collision between the plant's bullet and the frog
            int result_collision_plant = collision_bullet_plants(&player, &bullet_plants[mex.id % 10 - 1], plant);

            // if the plant's bullet hits the bottom of the screen, we kill the bullet process
            if (result_collision_plant == 1) plant[mex.id % 10 - 1].has_shot = false;

            // if the plant's bullet hits the frog, we kill all the processes and the frog
            else if(result_collision_plant == -2){
                kill_all(list_pid, count_pid);
                
                gs.status = 5; 
                return gs; 
            } 
        }

        // frog's bullet
        else if (mex.id == 900){

            // if the frog has not shot, we create a new bullet
            if (player.has_shot == false){
                player.has_shot = true;

                // creiamo il nuovo processo del proiettile
                pid_bullet_frog = fork();
                if (pid_bullet_frog < 0){
                    perror("frogger's bullet not created");
                    _exit(1);
                }

                else if (pid_bullet_frog == 0){
                    bullet_frog.dir = 1;
                    bullet_frog.y = player.y; 
                    bullet_frog.x = player.x + 1;
                    bullet_frog.id = ID_BULLET;
                    bullet_frog.pid = getpid();

                    shot_bullet(pipe_fd, bullet_frog);
                }

                else {
                    list_pid[count_pid] = pid_bullet_frog;
                    count_pid++;
                }
            }    
        }

        // 'q' key pressed
        else if (mex.id == 999){ // kill all process
            kill_all(list_pid, count_pid);
            gs.status = - 1; 
            return gs;
        }
        
        coloring_croc(win, croc);
        
        coloring_frog(win, player);
        
        coloring_plant(plant, win);

        coloring_bullet_frog(win, bullet_frog);

        coloring_bullet_plants(win, bullet_plants);

        wrefresh(win);
    }

    gs.status = 0; 
    return gs;
}


// ---------------- MAP ---------------- //

const char *sprite_lair[HEIGHT_LAIR] = {
        "XXX",
        "XXX"
    };

// function to color the map
void coloring_map(WINDOW *win, int score){
     
    for (int i = 0; i < HEIGHT_MAP; i++){
        for (int j = 0; j < WIDTH_MAP; j++){

            if (i >= 0 && i < 3){
                wattron(win, COLOR_PAIR(6));
                mvwaddch(win, i, j, ' '); // i = y_axis, j = x_axis 
                wattroff(win, COLOR_PAIR(6));
            }

            else if (i >= 3 && i  < 6){ // lair
                wattron(win, COLOR_PAIR(2));
                mvwaddch(win, i, j, '_');
                wattroff(win, COLOR_PAIR(2));
            }

            else if (i >= 6 && i  < 10){
                wattron(win, COLOR_PAIR(5)); // land
                mvwaddch(win, i, j, '-');
                wattroff(win, COLOR_PAIR(5)); 
            }

            else if (i >= 10 && i  < 28){
                wattron(win,COLOR_PAIR(3)); // river
                mvwaddch(win, i, j, '~');
                wattroff(win, COLOR_PAIR(3)); 
            }

            else if (i >= 28 && i  < 32){
                wattron(win,COLOR_PAIR(5)); // land 
                mvwaddch(win, i, j, '\\');
                wattroff(win, COLOR_PAIR(5)); 
            }
        }
    }

    wattron(win, COLOR_PAIR(4));
    mvwprintw(win, 1, 1, "SCORE: %d", score); refresh();
    wattroff(win, COLOR_PAIR(4));
}

void coloring_lair(WINDOW *win, lair_s lair[]){
    for (int i = 0; i < 5; i++){
        if (lair[i].open == true){
            for (int j = 0; j < HEIGHT_LAIR; j++){
                for (int k = 0; k < 3; k++){
                    wattron(win, COLOR_PAIR(6));
                    mvwaddch(win, lair[i].y + j, lair[i].x + k, sprite_lair[j][k]);
                    wattroff(win, COLOR_PAIR(6));
                }
            }
        }
        else {
            for (int j = 0; j < HEIGHT_LAIR; j++){
                for (int k = 0; k < 3; k++){
                    wattron(win, COLOR_PAIR(10));
                    mvwaddch(win, lair[i].y + j, lair[i].x + k, sprite_lair[j][k]);
                    wattroff(win, COLOR_PAIR(10));
                }
            }
        }
    }
}

// ---------------- COLORS ---------------- //

void game_colors(){ // set the colors of the game
    init_colors(); 
    init_pair(1, COLOR_RED, COLOR_RED);                     // Set the color pair 1 to red on red
    init_pair(2, COLOR_GREEN, COLOR_GREEN);                 // Set the color pair 2 to green on green
    init_pair(3, COLOR_CYAN, COLOR_CYAN);                   // Set the color pair 3 to cyan on cyan
    init_pair(4, COLOR_WHITE, COLOR_BLACK);                 // Set the color pair 4 to white on black
    init_pair(5, COLOR_MAGENTA, COLOR_MAGENTA);             // Set the color pair 5 to magenta on magenta
    init_pair(6, COLOR_BLACK, COLOR_BLACK);                 // Set the color pair 6 to black on black
    init_pair(7, COLOR_BLACK, COLOR_GREEN) ;                // Set the color pair 7 to black on green
    init_pair(8, COLOR_YELLOW, COLOR_YELLOW);               // Set the color pair 8 to yellow on yellow
    init_pair(9, COLOR_UNDERWATER, COLOR_UNDERWATER);       // Set the color pair 9 to the croc underwater color  
    init_pair(10, COLOR_GRAY, COLOR_GRAY);                  // Set the color pair 10 to gray on gray  
    init_pair(11, COLOR_BLACK, COLOR_WHITE);                // Set the color pair 11 to black on white
    init_pair(12, COLOR_GREEN, COLOR_MAGENTA);              // Set the color pair 12 to green on magenta
}

void init_colors(){ // initialize new colors
    init_color(COLOR_YELLOW, 1000, 1000, 0);      // Set the color yellow to RGB
    init_color(COLOR_UNDERWATER, 670, 904, 1000);  // Set the color of the croc UNDERWATER to RGB
    init_color(COLOR_GRAY, 548, 592, 612);        // Set the color gray to RGB
}

