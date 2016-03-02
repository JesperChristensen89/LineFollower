/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   control.h
 * Author: jesper
 *
 * Created on February 25, 2016, 8:07 AM
 */

#ifndef CONTROL_H
#define CONTROL_H

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

class Control
{
public:
    int regulator(int, int);
    void reset();
};

#endif /* CONTROL_H */

