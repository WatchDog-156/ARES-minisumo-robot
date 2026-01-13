#include "controller.h"
#include "line_detectors.h"
#include "motors.h"
#include "serwo.h"
//#include "sensors_ir.h"
//#include "starter.h"

bool beginning = true;
bool ending = false;
side_t lastSeen = Left;


programState_t get_progrogram_state(){
    if(start_switch==false && kill_switch==true)
        return Start;
    else if(start_switch==true && kill_switch==true)
        return Fighting;
    else
        return End ;
}


void anti_out_system(line_detector_t status){
    if(status == Both){
        motor_set(Left_Motor, Backward, 100);
        motor_set(Right_Motor, Backward, 100);
        usleep(1500);
        if(lastSeen == Left){
            motor_set(Left_Motor, Backward, 80);       
            motor_set(Right_Motor, Forward, 80); 
        }else{
            motor_set(Left_Motor, Forward, 80);       
            motor_set(Right_Motor, Backward, 80); 
        }
    }else if(status == Left){
        motor_set(Left_Motor, Backward, 100);
        motor_set(Right_Motor, Backward, 70);
        usleep(1000);
        motor_set(Left_Motor, Forward, 80);
        motor_set(Right_Motor, Backward, 80);
    }else{
        motor_set(Left_Motor, Backward, 70);
        motor_set(Right_Motor, Backward, 100);
        usleep(1000);
        motor_set(Left_Motor, Backward, 80);
        motor_set(Right_Motor, Forward, 80);
    }
}


void fighting_stage(){
    int irData[4] = measure_all_ir_sensors();
    if(irData[0]==8191 && irData[1]==8191 && irData[2]==8191 && irData[3]==8191){       // nie widać przeciwnika
        if(lastSeen == Left){
            motor_set(Left_Motor, Backward, 80);       
            motor_set(Right_Motor, Forward, 80); 
        }else{
            motor_set(Left_Motor, Forward, 80);       
            motor_set(Right_Motor, Backward, 80); 
        }
    }else if(irData[1]<20 && irData[2]<40){                                             // przeciwnik bardzo blisko
        motor_set(Left_Motor, Forward, 100);       
        motor_set(Right_Motor, Forward, 100);     
    }else if(irData[0]-irData[3]>20){                                                   // przeciwnik bardziej na lewo
        motor_set(Left_Motor, Forward, 80);       
        motor_set(Right_Motor, Forward, 100); 
    }else if(irData[3]-irData[0]>20){                                                   // przeciwnik bardziej na prawo
        motor_set(Left_Motor, Forward, 100);       
        motor_set(Right_Motor, Forward, 80); 
    }else{                                                                              // prosto
        motor_set(Left_Motor, Forward, 80);       
        motor_set(Right_Motor, Forward, 80); 
    }
}


void exec_program(){
    programState_t state = get_progrogram_state();
    
    if(state == Fighting){
        if(beginning){
            beginning = false;
            usleep(3000);
            motor_set(Left_Motor, Backward, 80);        //może do wywalenia
            motor_set(Right_Motor, Forward, 80);        //może do wywalenia
            usleep(500);                                //może do wywalenia
            serwo_set_posiotion(LEFT_SERWO, 175);
            serwo_set_posiotion(RIGHT_SERWO, 5);
        }

        line_detector_t lineDetectorStatus = check_line_detectors();
        if(lineDetectorStatus != None)
            anti_out_system(lineDetectorStatus);

        fighting_stage();
    }else if(state == End){
        if(!ending){
            ending = true;
            all_motor_stop();
            serwo_set_posiotion(LEFT_SERWO, 90);
            serwo_set_posiotion(RIGHT_SERWO, 90);
        }
    }
}