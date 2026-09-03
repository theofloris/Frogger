#include "render.h"

// ---------------- RENDER ---------------- //

game_status render(WINDOW *win, game_status gs, int client_socket){
    msg m; 

    // initialize the lair
    lair_s lair[5]; 
    for(int i = 0; i < 5; i++){
        lair[i].id = ID_LAIR + i;
        lair[i].x = 9 + i * 12;
        lair[i].y = 4;
        lair[i].open = gs.lair_status[i];
    }

    // initialize the timer
    timer_s timer = {ID_TIMER};
    int is_on_top;
    int n_collision = 0;
    int first_render = 0;

    pthread_t list_tid[100]; // lista of tids 
    int count_tid = 0;

    // initialize the frog
    frog_s player = {0};
    player.id = ID_FROG;
    player.y = HEIGHT_MAP - HEIGHT_FROG - 3;
    player.x = WIDTH_MAP / 2 - WIDTH_FROG / 2;
    player.has_shot = false;

    // initialize the crocodile
    crocodile croc[18]; 
    for (int i = 0; i < 18; i++) {
        croc[i].y = 999;
        croc[i].x = 999;
        croc[i].type = GOOD;
        croc[i].status = SAFE;
        croc[i].first_spawn = true;
    }

    // initialize the plant
    plant_s plant[3] = {0};
    for(int i = 0; i < 3; i++){
        plant[i].y = 999;
        plant[i].x = 999;
        plant[i].is_alive = false;
        plant[i].has_shot = false;
    }

    // initialize the bullet
    bullet_s bullet_frog, bullet_plants[3];

    // initialize the threads'ids
    pthread_t tid_bullet_frog, tid_bullet_plant[3], tid_plant[150];

    while(true){

        coloring_lifes(win, gs.lifes);

        coloring_map(win, gs.score); 

        coloring_lair(win, lair); 
        int recev = recv(client_socket, &m, sizeof(m), 0); // accept the message from the client
        if(recev <= 0){
            m = read_message();
        }
        
        if(m.id >= 0 && m.id < 18){ // crocodile
            if(m.first_spawn == true){
                croc[m.id].y = m.y;
                croc[m.id].x = m.x;
                croc[m.id].id = m.id;
                croc[m.id].direction = m.direction;
                croc[m.id].type = m.type_croc;
                croc[m.id].status = m.status_croc;
                croc[m.id].first_spawn = m.first_spawn;

            }
            else if(m.first_spawn == false){
                croc[m.id].y = m.y;
                croc[m.id].x = m.x;
                croc[m.id].direction = m.direction;
                croc[m.id].status = m.status_croc;
                croc[m.id].first_spawn = m.first_spawn;
            }

            is_on_top = collision_croc_frog(croc, &player); // check if frog is on top of a croc
            if (player.on_croc == 1 && m.id == is_on_top) {
                if (croc[m.id].direction == 0) player.x++;
                else player.x--;
            }
        }

        else if(m.id == player.id){ // frog
            
            player.y += m.y;
            player.x += m.x;

            // check collision with the screen
            collision(&player); 

            // check collision with the plants
            int collisionPlantFrog = 0;
            collisionPlantFrog = collision_plant_frog(&player, plant);
            
            if(collisionPlantFrog == -1){
                kill_all_threads(list_tid, count_tid);            
                gs.status = 5; 
                return gs; 
            }

            // check collision with the lair
            int lair_result = 0;
            lair_result = collisions_lair(&player, lair);
            
            if (lair_result > -1){
                gs.lair_status[lair_result] = false;

                kill_all_threads(list_tid, count_tid); 

                gs.status = 6;
                return gs; 
            }

            else if(lair_result == - 2){ 
                kill_all_threads(list_tid, count_tid);

                gs.status = 5; 
                return gs;
            }

            int gate_closed = 0;
            for (int i = 0; i < 5; i++){
                if (!lair[i].open){
                    lair[i].open = false; 
                    gate_closed++;    
                }    
            }

            int check_collision_croc_frog = 0;
            check_collision_croc_frog = collision_croc_frog(croc, &player); // check if frog is on top of a croc
            collision_croc_frog(croc, &player); // check if frog is on top of a croc
            if (player.on_croc == 0 && player.y > 9 && player.y < 28) {// if frog is in the river but not on a croc reset frog position
                kill_all_threads(list_tid, count_tid); 
                gs.status = 5; 
                return gs; 
            }
        }

        else if (m.id == 200){ // tid
            list_tid[count_tid] = m.tid;
            count_tid++;
        }

        else if (m.id == TID_MSG){
            plant[m.y].tid = m.tid;
            list_tid[count_tid] = m.tid;
            count_tid++;
        }

        // plants
        else if (m.id >= ID_PLANT && m.id < 704){
            plant[m.id - ID_PLANT].id = m.id; 
            plant[m.id - ID_PLANT].y = m.y;
            plant[m.id - ID_PLANT].x = m.x; 
            plant[m.id - ID_PLANT].is_alive = true; 

            // if the plant has not shot yet
            if (plant[m.id - ID_PLANT].has_shot == false){             
                int rand_shot = rand_(0, 1);
                if (rand_shot == 1){
                    plant[m.id - ID_PLANT].has_shot = true; 

                    // create a new thread for the bullet
                    bullet_plants[m.id - ID_PLANT].dir = 0;
                    bullet_plants[m.id - ID_PLANT].y = plant[m.id - ID_PLANT].y;
                    bullet_plants[m.id - ID_PLANT].x = plant[m.id - ID_PLANT].x;
                    bullet_plants[m.id - ID_PLANT].id = ID_BULLET + (m.id - 700);

                    pthread_create(&tid_bullet_plant[m.id - ID_PLANT], NULL, &shot_bullet, &bullet_plants[m.id - ID_PLANT]); 
                    bullet_plants[m.id - ID_PLANT].tid = tid_bullet_plant [m.id - ID_PLANT]; 
                    
                    list_tid[count_tid] = tid_bullet_plant[m.id - ID_PLANT];
                    count_tid++;
                }
            }
        }
        else if(m.id == ID_TIMER){ // timer
            timer.id = m.id; 
            gs.timer = m.timer; 

            if(gs.timer == 99 || gs.timer == 9) werase(win);
            if(gs.timer < 0){

                kill_all_threads(list_tid, count_tid); 
                gs.status = 5; 
                return gs; 
            }
            mvwprintw(win, HEIGHT_MAP - 2, 1, "TIMER: %d", gs.timer); refresh(); 
        }

        else if(m.id == ID_BULLET){ // bullet frog
            bullet_frog.y = m.y; 
            bullet_frog.x = m.x;
            bullet_frog.tid = m.tid; 
            bullet_frog.is_alive = true;

            // check collision between the bullet of the frog and the plants
            int bullet_bullet = bullet_v_bullet(&bullet_frog, bullet_plants);

            if (bullet_bullet != -1){
                bullet_frog.is_alive = false;
                player.has_shot = false;
                plant[bullet_bullet].has_shot = false; 
                bullet_plants[bullet_bullet].is_alive = false;

                pthread_cancel(bullet_frog.tid);
                pthread_cancel(bullet_plants[bullet_bullet].tid);
            }


            // check collision between the bullet of the frog and the plants
            int result_collision = collision_bullet(&player, &bullet_frog, plant, bullet_plants);
 
            if(result_collision < 3){
                pthread_cancel(tid_bullet_frog);
                pthread_cancel(plant[result_collision].tid); 
                
                switch(result_collision){
                    case 0:
                        m.x = rand_(1, 20); 
                        break;
                    case 1:
                        m.x = rand_(23, 43); 
                        break; 
                    case 2:
                        m.x = rand_(46, 66); 
                        break; 
                }
                m.y = rand_(6, 8);
                
                // create a new thread for the plant
                plant[result_collision] = (plant_s){ID_PLANT + result_collision, m.y, m.x, true, m.tid, false}; 
                pthread_create(&tid_plant[n_collision], NULL, &plant_init, &plant[result_collision]);

                plant[result_collision].tid = tid_plant[n_collision];

                list_tid[count_tid] = tid_plant[n_collision]; 
                count_tid++; 

                n_collision++;
                gs.score += 7;  
            }
            
            // check collision between the bullet of the frog and the crocodiles
            int collision_bullet_f_to_c = 0; 

            collision_bullet_f_to_c = collision_bullet_croc(&player, &bullet_frog, croc); 

            if(collision_bullet_f_to_c >= 0 && collision_bullet_f_to_c < 18){
                
                if(bullet_frog.y == croc[collision_bullet_f_to_c].y){
                    if(croc[collision_bullet_f_to_c].type == BAD){
                        croc[collision_bullet_f_to_c].type = GOOD;

                        gs.score += 30; 
                    }
                }     
            }
        }
        
        // plant's bullets
        else if(m.id > ID_BULLET && m.id < ID_BULLET + 4){
            bullet_plants[m.id - ID_BULLET - 1].y = m.y;
            bullet_plants[m.id - ID_BULLET - 1].x = m.x;
            bullet_plants[m.id - ID_BULLET - 1].is_alive = true;

            // check collision between the bullet of the plant and the frog
            int result_collision_bullet_plants = collision_bullet_plants(&player, &bullet_plants[m.id - ID_BULLET - 1]); 
            
            if(result_collision_bullet_plants == 1){
                plant[m.id - ID_BULLET - 1].has_shot = false;
                pthread_cancel(tid_bullet_plant[m.id - ID_BULLET - 1]);
            }
            
            else if(result_collision_bullet_plants == -2){
                kill_all_threads(list_tid, count_tid); 

                gs.status = 5; 
                return gs; 
            }
        }

        // bullet has been shot by the frog
        else if(m.id == 900){

            if(player.has_shot == false){ 
                player.has_shot = true;

                // create a new thread for the bullet
                bullet_frog.dir = 1; 
                bullet_frog.id = ID_BULLET;
                bullet_frog.y = player.y; 
                bullet_frog.x = player.x + 1;
                
                if(pthread_create(&tid_bullet_frog, NULL, &shot_bullet, &bullet_frog)!= 0){
                    perror("Bullet wrong!");
                    pthread_exit(NULL);  
                }
                else{
                    bullet_frog.tid = tid_bullet_frog;
                    list_tid[count_tid] = tid_bullet_frog; 
                    count_tid++;
                }
            }
        }

        // 'q' pressed by the user to quit the game
        else if (m.id == 999){
            kill_all_threads(list_tid, count_tid); 

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
    
    if(first_render < 0){
        wclear(win);
        first_render++; 
    }
    
    gs.status = 0; 
    return gs; 
}

// ---------------- MAP ---------------- //

const char *sprite_lair[HEIGHT_LAIR] = {
    "XXX",
    "XXX"
};

// coloring the map
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
    mvwprintw(win, 1, 1, "SCORE: %d", score);  
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

