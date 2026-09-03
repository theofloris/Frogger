#include "timer.h"

// ---------------- TIMER ---------------- //

void timer_(int pipe_fd[]){
    close(pipe_fd[0]);

    int start = START; // set the start of the timer to 60
    int end = END;

    timer_s timer = {ID_TIMER, start};

    msg msg_timer = (msg){timer.id, 0, 0, 0, 0, 0, 0, 0, timer.timer};

    // send the timer to the render
    while (start >= end){
        msg_timer = (msg){timer.id, 0, 0, 0, 0, 0, 0, 0, timer.timer};
        write(pipe_fd[1], &msg_timer, sizeof(msg));
        timer.timer--;
 
        sleep(1);
    }

    close(pipe_fd[1]);
}