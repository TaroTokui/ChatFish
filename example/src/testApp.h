#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"

// uncomment this to read from two kinects simultaneously
//#define USE_TWO_KINECTS

const int DEFAULT_NEAR = 255;
const int DEFAULT_FAR = 200;

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
    
	ofxKinect kinect;
	
	int angle;
    
    // 有効深度の閾値
    int nearThreshold, farThreshold;
	
    // ピクセルデータ保存用
    unsigned char *fsPixels;
    
    // 有効深度内にある画像
	ofxCvGrayscaleImage grayImage; // grayscale depth image
	
};
