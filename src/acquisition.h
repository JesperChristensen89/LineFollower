/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   acquisition.h
 * Author: jesper
 *
 * Created on February 22, 2016, 4:08 PM
 */

#ifndef ACQUISITION_H
#define ACQUISITION_H

#include <raspicam/raspicam_cv.h>
#include "opencv2/core.hpp"

class acquisition
{
public:
    void initCam();
    cv::Mat acquireImage();
    void stopCam();
};


#endif /* ACQUISITION_H */

