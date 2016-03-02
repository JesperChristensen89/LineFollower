/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   LineFollower.h
 * Author: jesper
 *
 * Created on February 22, 2016, 6:14 PM
 */

#ifndef LINEFOLLOWER_H
#define LINEFOLLOWER_H

#include "UART.h"
#include "acquisition.h"

class LineFollower{
public:
    int start(int, acquisition, UART, bool, bool);
private:
    void errorMatch(int, int);
    void errorContour(int);
};

#endif /* LINEFOLLOWER_H */

