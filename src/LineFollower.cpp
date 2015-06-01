/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "LineFollower.h"
#include "control.h"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

#define PI 3.14159265

using namespace cv;
using namespace std;

// for error detection
int k = 0;
int endOfLine = 0;
vector<vector<Point> > contours;
vector<vector<Point> > contour_prev;
int imgError = 0;
bool err = false;

void LineFollower::errorMatch(int im, int i)
{
    
    if (im!=0)
    {
        // match prev contour with current
        double match = matchShapes(contours[i],contour_prev[0],1,0.0);

        if (match > 10)
        {
            endOfLine = endOfLine + 1;
            
            err = true;

            printf("** Match error **");

            /*
            char errStr[50];
            sprintf(errStr,"img/imgERR%d.jpg",im);
            imwrite(errStr,img);
            */

            // if not coherent - reset
            if (im - imgError >= 4)
            {
                endOfLine = 0;
            }
            
            imgError = im;
        }
    }
    else
    {
        contour_prev.push_back(contours[i]);
    }
}

void LineFollower::errorContour(int im)
{
    if (k == 0)
        {
            endOfLine = endOfLine + 1;

            err = true;
            
            printf("** Contour error **");
            
            
             
            if (im - imgError >= 4)
            {
                endOfLine = 0;
            }
            imgError = im;
        }
}


int LineFollower::start(int nCount,acquisition cam, UART uart, bool turnLeft, bool black, int errThresh)
{
    Control control;
    
    int angle = 0;
    int anglePrev = 0;

        
    int frames = 0;
                
    // capture nCount images    

    
    for ( int im=0; im<nCount; im++ ) 
    {
        anglePrev = angle;

        cout << "\nImage: "<<im << "/" << nCount<< " ";
        
        // read image
        Mat img = cam.acquireImage();
         
        // create ROI
        Rect roi = Rect(10, img.rows-60, img.cols-20, img.rows/12);
        
        Mat img_roi = img(roi);

        // convert to gray
        Mat img_gray;
        cvtColor(img_roi,img_gray,COLOR_BGR2GRAY);
        
        // Blur
        Mat img_blur;
        GaussianBlur(img_gray, img_blur, Size(9, 9), 2);
            

        // Threshold
        Mat img_thresh;

        int threshType;
        threshType = black ? THRESH_BINARY_INV | THRESH_OTSU : THRESH_BINARY | THRESH_OTSU;
        
        threshold(img_blur, img_thresh, 0, 255, threshType);
            
        
        // Opening (reduces noise)
        Mat kernel(5,5, CV_8UC1,1);
        Mat img_open;

        morphologyEx(img_thresh,img_open,2,kernel);

        // find contours
        
        vector<Vec4i> hierarchy;

        findContours(img_open,contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);

        
        vector<Rect> r;
        vector<Point> leftEdge;
        
        Point midRect;
        
        
        k = 0;
    
        for( int i = 0; i< contours.size(); i++ ) // iterate through each contour. 
        {
            double a = contourArea( contours[i],false);  //  Find the area of contour
            
            // use only contours with area above 3500
            if(a > 3500)
            {
                // get bounding rec of contour
                r.push_back(boundingRect(contours[i]));
                r[k].x = r[k].x + 10;
                r[k].y = r[k].y + img.rows-60;
                
                rectangle(img,r[k],Scalar(0,255,0));
                 
                // get mid-point of rect
                midRect = Point(r[k].x+r[k].width/2, 
                        r[k].y+r[k].height/2);

                circle(img,midRect
                        ,5,Scalar(0,0,255),-1,1,8);

                // get the left edge of rect
                // used as offset as raspicam is not
                // mounted on mid of regbot
                leftEdge.push_back( Point(r[k].x,r[k].y+r[k].height/2));

                circle(img,leftEdge[k],5,Scalar(255,0,0),-1,1,8);
                
                
                k = k+1;
                
                drawContours(img,contours, i,Scalar(0,255,255),1,8,hierarchy,0,Point(10,img.rows-60));
                
                // used for false-positive checking
                errorMatch(im, i);
                    
                
            }
        }
        
        
        // if k = 0 no contour was found -> error
        errorContour(im);

        /*
        if (err)
        {
            char errStr2[50];
            sprintf(errStr2,"../img/imgERR%d.jpg",im);
            imwrite(errStr2,img);
            printf("saved");
            
        }
        */  
        
        // error routine
        if (endOfLine == errThresh)
        {
            cout << "\n### REACHED END OF LINE ### ( or made an error :-) )" << endl;
            endOfLine = 0;
            break;
        }
            
        
        

        // get the edge points
        int leftmostEdge = img.cols;
        
        for (int i = 0; i < leftEdge.size(); i++)
        {
            int edge = leftEdge[i].x;
                                    
            if (edge < leftmostEdge)
            {
                leftmostEdge = edge;
            }
        }
        
        int rightmostEdge = 0;
        
        for (int i = 0; i < leftEdge.size(); i++)
        {
            int edge = leftEdge[i].x;
            
            if (edge > rightmostEdge)
            {
                rightmostEdge = edge;
            }
        }
        
                
        // calculate angle
        double deltaX = 0;
        if (turnLeft)
        {
            deltaX = leftmostEdge - img.cols/2;
        }
        else
        {
            deltaX = rightmostEdge - img.cols/2;
        }
                
        double deltaY = midRect.y - img.rows;


        int refAngle = atan(deltaY/deltaX) * 180/PI;

        // convert from img coordinates to regbot coordinates
        refAngle = -(refAngle-90);

        if (refAngle > 90)
            refAngle = refAngle - 180;

        
             

        // call regulator with directions       
        if (turnLeft)
        {
            angle = control.regulator(leftmostEdge, img.cols/2) ;
        }
        else
        {
            angle = control.regulator(rightmostEdge, img.cols/2) ;
        }

       
        // angle = err ? anglePrev : angle;


        
        
        // if you uncomment this save section the FPS
        // of the application will drop from 30 to 10
        // can be used as debugging
        /*
        char str2[50];
        sprintf(str2,"../img/imgRes%d.jpg",im);
        imwrite(str2,img);
        */
          
        //cout << refAngle << endl;

        // send calculated angle to regbot
        char angleStr[10];
        snprintf (angleStr,10,"%d\n",angle);
        uart.send(angleStr);

        
        

        //cout<<"Reg angle: " << angleStr;

        frames = im;
        err = false;

        
    }
    
    control.reset();
    endOfLine = 0;
    
    return frames;
}
