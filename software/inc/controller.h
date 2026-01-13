#ifndef CONTROLLER_H
#define CONTROLLER_H

typedef enum{
    Right, 
    Left,
} side_t;

void exec_program();
programState_t get_progrogram_state();


#endif