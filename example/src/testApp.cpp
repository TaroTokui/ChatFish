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
    
    // 画像領域を確保
    grayImage.allocate(kinect.width, kinect.height);
    bgImage.allocate(kinect.width, kinect.height);
    fsImage.allocate(kinect.width, kinect.height);
    
    // ピクセルデータ保存用の領域を確保
    srcPixels = (unsigned char*)malloc(kinect.width * kinect.height);
    bgPixels = (unsigned char*)malloc(kinect.width * kinect.height);
    fsPixels = (unsigned char*)malloc(kinect.width * kinect.height);
    
    // bgPixelsを0で初期化
    int width = kinect.width;
    int height = kinect.height;
    for (int j=0; j<height; j++) {
        for (int i=0; i<width; i++) {
            bgPixels[i+j*width] = 0;
        }
    }
    
    margin = 5;
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
        srcPixels = grayImage.getPixels();
        int width = kinect.width;
        int height = kinect.height;
        for (int j=0; j<height; j++) {
            for (int i=0; i<width; i++) {
                // 有効深度外の画素を0にする
                if( nearThreshold < srcPixels[i+j*width] || srcPixels[i+j*width] < farThreshold){
                    srcPixels[i+j*width] = 0;
                }
                // 画素値をコピー
                fsPixels[i+j*width] = srcPixels[i+j*width];
            }
        }
        grayImage.setFromPixels(srcPixels, width, height);
        
        switch (sequence) {
            case CALIBRATION:
                // 0を除く最も小さな値を背景とする
                for (int j=0; j<height; j++) {
                    for (int i=0; i<width; i++) {
                        // 背景より奥に物体があったら背景情報を更新(※近くの方が高輝度)
                        if (bgPixels[i+j*width] == 0) {
                            // 背景が0だったら現在の値を採用
                            bgPixels[i+j*width] = fsPixels[i+j*width];
                        }else if(fsPixels[i+j*width] < bgPixels[i+j*width] && fsPixels[i+j*width] != 0){
                            // 背景と現在の値が共に0でなく、背景の値が明るい場合は背景を更新
                            bgPixels[i+j*width] = fsPixels[i+j*width];
                        }
                    }
                }
                bgImage.setFromPixels(bgPixels, width, height);
                break;
                
            case RECOGNITION:
                // 背景より近い領域を切り出す
                for (int j=0; j<height; j++) {
                    for (int i=0; i<width; i++) {
                        if( fsPixels[i+j*width] < bgPixels[i+j*width] + margin){
                            fsPixels[i+j*width] = 0;
                        }
                    }
                }
                fsImage.setFromPixels(fsPixels, width, height);
                // ごま塩ノイズの除去
                fsImage.erode_3x3();
                fsImage.dilate_3x3();
                break;
                
            default:
                break;
        }
    }
    
    // update the cv images
    grayImage.flagImageChanged();
    bgImage.flagImageChanged();
    fsImage.flagImageChanged();
}

//--------------------------------------------------------------
void testApp::draw() {
	
    // draw from the live kinect
//    kinect.drawDepth(0, 0, 320, 240);

    grayImage.draw(0, 0, 320, 240);
    bgImage.draw(330, 0, 320, 240);
    fsImage.draw(0, 250, 320, 240);
}

//--------------------------------------------------------------
void testApp::exit() {
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
}

//--------------------------------------------------------------
void testApp::keyPressed (int key) {
    
    int width = kinect.width;
    int height = kinect.height;
    
	switch (key) {
			
        case 'b':
            sequence = CALIBRATION;
            // bgPixelsを0で初期化
            for (int j=0; j<height; j++) {
                for (int i=0; i<width; i++) {
                    bgPixels[i+j*width] = 0;
                }
            }
            break;
            
		case 'c':
			kinect.setCameraTiltAngle(0); // zero the tilt
			kinect.close();
			break;
            
        case 'f':
            farThreshold++;
            cout << "farThreshold = " << farThreshold << endl;
            break;
            
        case 'F':
            farThreshold--;
            cout << "farThreshold = " << farThreshold << endl;
            break;
            
        case 'm':
            margin++;
            cout << "margin = " << margin << endl;
            break;
            
        case 'M':
            margin--;
            cout << "margin = " << margin << endl;
            break;
            
        case 'n':
            nearThreshold++;
            cout << "nearThreshold = " << nearThreshold << endl;
            break;
			
        case 'N':
            nearThreshold--;
            cout << "nearThreshold = " << nearThreshold << endl;
            break;
            
		case 'o':
			kinect.setCameraTiltAngle(angle); // go back to prev tilt
			kinect.open();
			break;
			
		case 'w':
			kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
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
            
        case OF_KEY_RETURN:
            sequence = RECOGNITION;
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
