#include "bullet.h"

// ---------------- BULLETS ---------------- //

// function to shoot bullets
void shoot_bullet(int pipe_fd[], bullet_s bullet){
    msg msg_b; // initialize a message

    while(1){
        // check if the direction is 0, then it was shot by the plant
        if(bullet.dir == 0) bullet.y++; 
        // check if the direction is not 0, then it was shot by the frog
        else bullet.y--; 

        // write a message that render will receive with bullet information
        msg_b = (msg){bullet.id, bullet.y, bullet.x, bullet.pid};
        write(pipe_fd[1], &msg_b, sizeof(msg));

        flushinp();
        usleep(50000);
    }

    _exit(1); 
}


// function to color frog bullets
void coloring_bullet_frog(WINDOW *win, bullet_s bullet){
    if (bullet.is_alive){
        if(bullet.y > 5){
            wattron(win, COLOR_PAIR(11));
            mvwaddch(win, bullet.y, bullet.x, '0');
            wattroff(win, COLOR_PAIR(11));
        }
    }
}

// function to color plant bullets
void coloring_bullet_plants(WINDOW *win, bullet_s bullet[]){
    for (int i = 0; i < 3; i++){
        if (bullet[i].is_alive){
            if (bullet[i].y > 5){
                wattron(win, COLOR_PAIR(1));
                mvwaddch(win, bullet[i].y, bullet[i].x, '0');
                wattroff(win, COLOR_PAIR(1));
            }
        }
    }
}