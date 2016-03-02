/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "acquisition.h"
#include <iostream>
#include <stdio.h>
#include <unistd.h>

raspicam::RaspiCam_Cv Camera;

// initialize cam properties
void acquisition::initCam()
{
    //set camera params
    Camera.set( CV_CAP_PROP_FORMAT, CV_8UC3 );
    Camera.set( CV_CAP_PROP_FRAME_WIDTH, 640);
    Camera.set( CV_CAP_PROP_FRAME_HEIGHT, 480);
    //Open camera
    std::cout<<"Opening Camera..."<<std::endl;
    if (!Camera.open()) std::cerr<<"Error opening the camera"<<std::endl;
    
    Camera.set( CV_CAP_PROP_EXPOSURE,100);
    //Camera.set(CV_CAP_PROP_BRIGHTNESS,50);
    //Camera.set(CV_CAP_PROP_CONTRAST,35);
    //Camera.set(CV_CAP_PROP_SATURATION,20);
    //Camera.set(CV_CAP_PROP_GAIN,50);
    
    // wait for cam to finish warmup
    usleep(2000000);
    
    
}

// grab and return an image
cv::Mat acquisition::acquireImage()
{
    cv::Mat image;
    
    Camera.grab();
    Camera.retrieve ( image);   
    
    
    return image;
}

// release the camera
void acquisition::stopCam()
{
    Camera.release();
}