/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   line_detection.cpp
 * Author: jesper
 *
 * Created on February 9, 2016, 12:24 PM
 */



#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <unistd.h>
#include <algorithm> 
#include "acquisition.h"
#include "UART.h"
#include "LineFollower.h"

using namespace std;

UART uart; 

bool turnLeft = true;

int nCount = 60000;
bool black = true;


/*
 this thread will handle input from user
 * control of regbot and control of turning direction is possible
 */
void *inputThread(void *parameters){
    
    while(1)
    {
    
        string str = "";
        getline(cin, str);

        string strBackup = str;

        transform(str.begin(), str.end(), str.begin(), ::tolower);

        if (str == "left")
        {
            turnLeft = true;
            cout << "RegBot will make left turns..." << endl;
        }
        else if  (str == "right")
        {
            turnLeft = false;
            cout << "RegBot will make right turns..." << endl;
        }
        
        else if (str == "linemode=0")
        {
            nCount = 60000;
            cout << "RegBot will run till end of line" << endl;
        }
        else if (str.compare(0,9,"linemode=") == 0 and str != "linemode=0")
        {
            char numberOfFrames[4];
            size_t length = str.copy(numberOfFrames,str.size()-9,9);
            numberOfFrames[length] = '\0';
            nCount = atoi(numberOfFrames);

            cout << "RegBot will run untill " << nCount <<" frames have been captured" << endl;

        }
        else if (str == "black")
        {
            black = true;
            cout << "RegBot will run on black lines" << endl;
        }
        else if(str == "white")
        {
            black = false;
            cout << "RegBot will run on white lines" << endl;
        }
        else if (str == "pihelp")
        {
            cout << "        ### COMMANDS ###" << endl << endl 
                << "pihelp:               this text" 
                << endl << "left:                 regbot will make left turns [default]" << endl
                << "right:                regbot will make right turns" << endl
                << "linemode=0:           regbot will run till end of line [default]" <<
                endl << 
                "linemode=xxxx:        regbot will run till xxxx frames have been captured" 
                << endl << "black:                regbot will run on black lines [default]" << endl
                << "white:                regbot will run on white lines" <<endl << endl
                << "        ### ######## ###" << endl << endl;
        }
        



        // if not a direction command - pass it as a command to regbot
        else
        {
            char * input = new char[strBackup.length()+1];

            strcpy(input,strBackup.c_str());

            strcat (input,"\n");

            uart.send(input);
        }
        
    
    }
    pthread_exit(NULL);
}

/**
 * @function main
 */
int main( int, char** argv )
{

    
    time_t timer_begin,timer_end;
    
    int frames = 0;
    
    pthread_t getInput[2];
    int getInput_hand = 0;
    
    uart.init();
            
    acquisition cam;
    
    cout << "Creating input thread..." << endl;
    getInput_hand = pthread_create(&(getInput[1]), NULL, inputThread, (void *)&(getInput[1]));
    
    if (getInput_hand !=0){
        printf("Not possible to create threads:[%s]\n", strerror(getInput_hand));
        exit(EXIT_FAILURE);
    }
    else
        cout << "Input mode is active!" << endl;
    
    
    /*
     Main while loop
     * Checks UART for start signal
     * 
     */
    while (1)
    {
        uart.receive();
         
        if (uart.getMissionStart())
        {
            // init the cam - needs to warm up
            cam.initCam();
        
            cout<< "Ready to start." << endl;
            cout<<"Is set to capture "<<nCount<<" frames ...."<<endl;
                        
            LineFollower lineFollower;

            // start timer for FPS calculation
            time ( &timer_begin );
            
            // run lineFollower routine
            frames = lineFollower.start(nCount, cam, uart, turnLeft, black);
    
            // this will be stop mission signal to regbot
            uart.send((char *)"998\n");
            uart.setMissionStart(false);
            
            // close cam
            cout<<"Stopping camera..."<<endl;
            cam.stopCam();

            //show time statistics
            time ( &timer_end ); 
            double secondsElapsed = difftime ( timer_end,timer_begin );
            cout<< secondsElapsed<<" seconds for "<< 
                    frames<<"  frames : FPS = "<<  ( float ) ( ( float ) ( frames ) /secondsElapsed ) <<endl;

        }
        
    }
    

    
    
    
    
    
    
    
   return 0;
}

