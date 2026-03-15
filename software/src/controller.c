#include "pico/stdlib.h"
#include <stdio.h> 
#include "controller.h"
#include "line_detectors.h"
#include "motors.h"
#include "serwo.h"
#include "distance_sensor.h"
#include "starter.h"
#include "bluetooth_connection.h"

bool beginning = true;
bool ending = false;
side_t lastSeen = left;

void anti_out_system(line_detector_t status){
    if(status == Both){
        motor_set(Left_Motor, Backward, 100);
        motor_set(Right_Motor, Backward, 100);
        sleep_ms(1500);
        if(lastSeen == left){
            motor_set(Left_Motor, Backward, 80);       
            motor_set(Right_Motor, Forward, 80); 
        }else{
            motor_set(Left_Motor, Forward, 80);       
            motor_set(Right_Motor, Backward, 80); 
        }
    }else if(status == Left){
        motor_set(Left_Motor, Backward, 100);
        motor_set(Right_Motor, Backward, 70);
        sleep_ms(1000);
        motor_set(Left_Motor, Forward, 80);
        motor_set(Right_Motor, Backward, 80);
    }else{
        motor_set(Left_Motor, Backward, 70);
        motor_set(Right_Motor, Backward, 100);
        sleep_ms(1000);
        motor_set(Left_Motor, Backward, 80);
        motor_set(Right_Motor, Forward, 80);
    }
}

void fighting_stage(){
    int irData[4];
    uint8_t addr[4]={0x30, 0x31, 0x32, 0x33};           // 0 - lewy, 1 - lewy przód, 2 - prawy przód, 3 - prawy
    for (int i=0; i<4; i++){
        tofSetCurrentAddress(addr[i]);
        irData[i]=tofReadDistance(i);
    }

    if(irData[0]>TOF_OUT_OF_RANGE && irData[1]>TOF_OUT_OF_RANGE && irData[2]>TOF_OUT_OF_RANGE && irData[3]>TOF_OUT_OF_RANGE){       // nie widać przeciwnika
        
        if(lastSeen == left){
            motor_set(Left_Motor, Backward, 80);       
            motor_set(Right_Motor, Forward, 80); 
        }else{
            motor_set(Left_Motor, Forward, 80);       
            motor_set(Right_Motor, Backward, 80); 
        }
    }else if(irData[1]<ENEMY_CLOSE && irData[2]<ENEMY_CLOSE){               // przeciwnik bardzo blisko        
        motor_set(Left_Motor, Forward, 100);       
        motor_set(Right_Motor, Forward, 100);     
    }else if(irData[3]-irData[0]>SIDE_TRESHOLD){                            // przeciwnik bardziej na lewo
        motor_set(Left_Motor, Forward, 40);       
        motor_set(Right_Motor, Forward, 100); 
        lastSeen = left;
    }else if(irData[0]-irData[3]>SIDE_TRESHOLD){                            // przeciwnik bardziej na prawo
        motor_set(Left_Motor, Forward, 100);       
        motor_set(Right_Motor, Forward, 40); 
        lastSeen = right;
    }else{                                                                  // prosto
        motor_set(Left_Motor, Forward, 80);       
        motor_set(Right_Motor, Forward, 80); 
    }
}


void exec_program(){
    State state = getState();
    
    if(state == Fighting){
        if(beginning){
            char msg[64];
            snprintf(msg, sizeof(msg), "Zaczynam walke\n");
            bluetooth_send_telemetry(msg);

            beginning = false;
            ending = false;
            sleep_ms(3000);
            motor_set(Left_Motor, Backward, 80); 
            motor_set(Right_Motor, Forward, 80);       
            sleep_ms(800);                             
            serwo_set_posiotion(LEFT_SERWO, 170);
            serwo_set_posiotion(RIGHT_SERWO, 10);
        }

        line_detector_t lineDetectorStatus = check_line_detectors();
        if(lineDetectorStatus != None){
            int status_right = read_detector_value(Right);
            char msg[64];
            snprintf(msg, sizeof(msg), "Wykryto linie, %d\n", status_right);
            bluetooth_send_telemetry(msg);

            anti_out_system(lineDetectorStatus);
        }

        fighting_stage();

    }else if(state == End){
        if(!ending){
            char msg[64];
            snprintf(msg, sizeof(msg), "Koniec walki\n");
            bluetooth_send_telemetry(msg);

            ending = true;
            beginning = true;
            all_motor_stop();
            serwo_set_posiotion(LEFT_SERWO, 90);
            serwo_set_posiotion(RIGHT_SERWO, 90);
        }
    }
}