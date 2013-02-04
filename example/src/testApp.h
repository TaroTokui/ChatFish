#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"

// uncomment this to read from two kinects simultaneously
//#define USE_TWO_KINECTS

class testApp : public ofBaseApp {
public:
	
	void setup();
	void update();
	void draw();
	void exit();
	
	void keyPressed(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
    
    void createMask(CvPoint center, int range);
	
	ofxKinect kinect;
	
	ofxKinect kinect2;
	
	ofxCvColorImage colorImg;
	
	ofxCvGrayscaleImage grayImage; // grayscale depth image
	
	int angle;
	
    bool bBGFlag;
    ofxCvGrayscaleImage bgImage;    // background depth image
    ofxCvGrayscaleImage grayDiffImage;
    ofxCvGrayscaleImage maskImage;
    ofxCvGrayscaleImage maskedGrayImage;
    ofxCvColorImage maskedColorImage;
    ofxCvColorImage blackImage;
    ofxCvColorImage detectionImage;
    
    CvPoint maskPosition;
    int maskR;
    
    // ピクセルデータ保存用
    unsigned char *bgPixels;
    unsigned char *fsPixels;
    
    ofSerial	serial;
};
