#include "thread_control.h"

// ---------------- THREAD CONTROL ---------------- //

extern msg buffer[BUFFER_DIM];  
extern int buff_counter; 

// function to read the message from the buffer
msg read_message(){
    while(buff_counter == 0); // if the buffer is empty wait
    
    pthread_mutex_lock(&mtx); 

    buff_counter -= 1; 
    msg m = buffer[buff_counter]; 
    buffer[buff_counter] = (msg){0};  
    pthread_mutex_unlock(&mtx); 

    return m; 
}

// function to write the message in the buffer
void write_message(msg m){
    while(buff_counter == BUFFER_DIM); // if the buffer is full wait

    pthread_mutex_lock(&mtx); 

    buffer[buff_counter] = m; 
    buff_counter += 1;

    pthread_mutex_unlock(&mtx); 
}