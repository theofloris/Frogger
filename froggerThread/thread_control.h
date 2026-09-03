#ifndef THREAD_CONTROL_H
#define THREAD_CONTROL_H

#include "main.h"
extern pthread_mutex_t mtx;

msg read_message(); 
void write_message(msg m);

#endif