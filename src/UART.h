/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   UART.h
 * Author: jesper
 *
 * Created on February 22, 2016, 5:33 PM
 */

#ifndef UART_H
#define UART_H

class UART
{
public:
    void init();
    void send(char*);
    void receive();

    bool getMissionStart();
    void setMissionStart(bool);
    
};


#endif /* UART_H */

