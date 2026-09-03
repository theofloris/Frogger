#include "plants.h"

// ---------------- PLANTS ---------------- //

// sprite 
const char *sprite_plant[HEIGHT_PLANT] = {
        "xO",
        "||"
    };

// function to initialize plants
void plant_init(int pipe_fd[], plant_s plant){ 
    close(pipe_fd[0]); 
    sleep(rand_(2,5)); 

    msg msg_plant;

    while(1){
        msg_plant = (msg){plant.id, plant.y, plant.x, plant.pid}; 
        write(pipe_fd[1], &msg_plant, sizeof(msg)); // invia le informazioni della pianta a render

        flushinp();
        sleep(1); 
    }

    close(pipe_fd[1]);
    _exit(0); 
}

void land_plants(int pipe_fd[], WINDOW *win){
    msg msg_p;
    
    plant_s array_plant[3];

    for(int k = 0; k < 3; k++){ // 3 plants
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
        
        msg_p.y = rand_(6,8); // randomize the y position of the plant 

        // create a plant
        pid_t pid = fork(); 
        if (pid < 0){
            perror("plant not created"); 
            _exit(1);
        }
        else if (pid == 0){
            array_plant[k] = (plant_s){ID_PLANT + k, msg_p.y, msg_p.x, true, getpid(), false};
            plant_init(pipe_fd, array_plant[k]);
        }
        else{
            msg_p = (msg){200, 0, 0, pid};
            write(pipe_fd[1], &msg_p, sizeof(msg));
        }

        sleep(rand_(2,5));
    }
}

// color the plants
void coloring_plant(plant_s plant[], WINDOW *win){
    for(int k = 0; k < 3; k++){
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