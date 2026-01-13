#ifndef CONTROLLER_H
#define CONTROLLER_H

#define TOF_OUT_OF_RANGE 8191
#define ENEMY_CLOSE 40
#define SIDE_TRESHOLD 60

typedef enum{
    right, 
    left,
} side_t;

void exec_program();

#endif