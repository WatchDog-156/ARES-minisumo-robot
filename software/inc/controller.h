#ifndef CONTROLLER_H
#define CONTROLLER_H

#define TOF_OUT_OF_RANGE 800
#define ENEMY_CLOSE 200
#define SIDE_TRESHOLD 100

typedef enum{
    right, 
    left,
} side_t;

void exec_program();

#endif