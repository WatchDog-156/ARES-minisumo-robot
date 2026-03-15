#ifndef STARTER_H
#define STARTER_H

#include <stdbool.h>

#define Start_pin 14
#define Kill_pin 15

typedef enum{
    Start, 
    Fighting,
    End
} State; 

void starter_init();
State getState();
void changeState(State newState);


#endif