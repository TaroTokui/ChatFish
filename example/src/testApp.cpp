#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	// enable depth->video image calibration
	kinect.setRegistration(true);
    
	kinect.init();
    
	kinect.open();		// opens first available kinect
	
	ofSetFrameRate(60);
	
	// zero the tilt on startup
	angle = 0;
	kinect.setCameraTiltAngle(angle);
    
    // 有効深度の初期化
    nearThreshold = DEFAULT_NEAR;
    farThreshold = DEFAULT_FAR;
    
}

//--------------------------------------------------------------
void testApp::update() {
	
	ofBackground(100, 100, 100);
	
	kinect.update();
    
	// there is a new frame and we are connected
	if(kinect.isFrameNew()) {
		
		// load grayscale depth image from the kinect source
		grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
            
        // 有効深度内の画像を切り出す
        fsPixels = grayImage.getPixels();
        int width = kinect.width;
        int height = kinect.height;
        for (int j=0; j<height; j++) {
            for (int i=0; i<width; i++) {
                // 有効深度外の画素を0にする
                if( nearThreshold < fsPixels[i+j*width] || fsPixels[i+j*width] < farThreshold){
                    fsPixels[i+j*width] = 0;
                }
            }
        }
    }
    
    // update the cv images
    grayImage.flagImageChanged();
}

//--------------------------------------------------------------
void testApp::draw() {
	
	ofSetColor(255, 255, 255);
	
    // draw from the live kinect
//    kinect.drawDepth(0, 0);

    grayImage.draw(0, 0);
}

//--------------------------------------------------------------
void testApp::exit() {
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
}

//--------------------------------------------------------------
void testApp::keyPressed (int key) {
	switch (key) {
			
		case 'w':
			kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
			break;
			
		case 'o':
			kinect.setCameraTiltAngle(angle); // go back to prev tilt
			kinect.open();
			break;
			
		case 'c':
			kinect.setCameraTiltAngle(0); // zero the tilt
			kinect.close();
			break;
            
        case 'n':
            nearThreshold++;
            break;
			
        case 'N':
            nearThreshold--;
            break;
            
        case 'f':
            farThreshold++;
            break;
            
        case 'F':
            farThreshold--;
            break;
            
		case OF_KEY_UP:
			angle++;
			if(angle>30) angle=30;
			kinect.setCameraTiltAngle(angle);
			break;
			
		case OF_KEY_DOWN:
			angle--;
			if(angle<-30) angle=-30;
			kinect.setCameraTiltAngle(angle);
			break;
            
	}
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{}
