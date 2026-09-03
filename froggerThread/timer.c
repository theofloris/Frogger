#include "timer.h"

// ---------------- TIMER ---------------- //

void *timer_(){
    int start = START; // set the start of the timer
    int end = END; 

    timer_s timer = {ID_TIMER, start};

    msg msg_timer = (msg){timer.id, 0, 0, 0, 0, 0, 0, 0, timer.timer};
    
    // send the timer to the buffer
    while (start >= end){
        msg_timer = (msg){timer.id, 0, 0, 0, 0, 0, 0, 0, timer.timer};
        write_message(msg_timer); 
        timer.timer--;
 
        sleep(1); 
    }
}