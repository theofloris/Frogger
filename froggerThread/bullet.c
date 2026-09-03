#include "bullet.h"

// ---------------- BULLETS ---------------- //

// function to shoot bullets
void *shot_bullet(void *bullet){
    msg msg_b; 
    bullet_s *Bullet = (bullet_s *)bullet;

    while(true){
        // check if the direction is 0, then it was shot by the plant
        if(Bullet->dir == 0) Bullet->y++;
        // check if the direction is not 0, then it was shot by the frog

        else Bullet->y--;
        
        // write a message that render will receive with bullet information
        msg_b = (msg){Bullet->id, Bullet->y, Bullet->x, Bullet->tid};
        write_message(msg_b);

        usleep(50000);
    }
}

void coloring_bullet_frog(WINDOW *win, bullet_s bullet){
    if(bullet.is_alive){
        if(bullet.y > 5){
            wattron(win, COLOR_PAIR(11));
            mvwaddch(win, bullet.y, bullet.x, '0');
            wattroff(win, COLOR_PAIR(11));
        }
    }
}

void coloring_bullet_plants(WINDOW *win, bullet_s bullet[]){
    for (int i = 0; i < 3; i++){
        if (bullet[i].is_alive){
            if (bullet[i].y > 5 && bullet[i].y <= 31){
                wattron(win, COLOR_PAIR(1));
                mvwaddch(win, bullet[i].y, bullet[i].x, '0');
                wattroff(win, COLOR_PAIR(1));
            }
        }
    }
}