#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	// enable depth->video image calibration
	kinect.setRegistration(true);
    
	kinect.init();
	//kinect.init(true); // shows infrared instead of RGB video image
	//kinect.init(false, false); // disable video image (faster fps)
	
	kinect.open();		// opens first available kinect
	//kinect.open(1);	// open a kinect by id, starting with 0 (sorted by serial # lexicographically))
	//kinect.open("A00362A08602047A");	// open a kinect using it's unique serial #
	
//	kinect2.init();
//	kinect2.open();
	
//    colorImg = new ofxCvColorImage();
	colorImg.allocate(kinect.width, kinect.height);
	grayImage.allocate(kinect.width, kinect.height);
	
	ofSetFrameRate(60);
	
	// zero the tilt on startup
	angle = 0;
	kinect.setCameraTiltAngle(angle);
    
    // kinect2 set
//    kinect2.setCameraTiltAngle(angle);
	
    bBGFlag = true;
    bgImage.allocate(kinect.width, kinect.height);
    maskImage.allocate(kinect.width, kinect.height);    // これだけじゃちゃんと初期化されない
    grayDiffImage.allocate(kinect.width, kinect.height);
    blackImage.allocate(kinect.width, kinect.height);
    detectionImage.allocate(kinect.width, kinect.height);
    
    maskPosition = cvPoint(0, 0);
    maskR = 0;
    
    // 画像を初期化
    cvRectangle(blackImage.getCvImage(), cvPoint(0, 0), cvPoint(blackImage.width, blackImage.height ), CV_RGB(0, 0, 0));
    
    maskImage = grayImage;
    maskedGrayImage = grayImage;
    maskedColorImage = colorImg;
    detectionImage = colorImg;
    
    // ピクセルデータ保存用の領域を確保
    // w * h * 1 (gray scale)
    bgPixels = (unsigned char*)malloc(kinect.width * kinect.height);
    fsPixels = (unsigned char*)malloc(kinect.width * kinect.height);
    
    // Serial通信の初期化
	serial.listDevices();
	vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
    
	serial.setup("/dev/tty.usbmodemfa131",115200); // mac osx example
}

//--------------------------------------------------------------
void testApp::update() {
	
	ofBackground(100, 100, 100);
	
	kinect.update();
//    kinect2.update();
	
	// there is a new frame and we are connected
	if(kinect.isFrameNew()) {
		
		// load grayscale depth image from the kinect source
		grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        
        // load color image from the kinect source
        colorImg.setFromPixels(kinect.getPixels(), kinect.width, kinect.height);
        
        if( bBGFlag ) {
            bgImage = grayImage;
            bgPixels = bgImage.getPixels();
            bBGFlag = false;
        }
        
        // 背景差分
//        grayDiffImage.absDiff(bgImage, grayImage);  // これはやめとこう、増加分だけ欲しい
        fsPixels = grayImage.getPixels();
        int width = kinect.width;
        int height = kinect.height;
        for (int j=0; j<height; j++) {
            for (int i=0; i<width; i++) {
                // 背景よりちょっと以上明るかったらgrayImageの画素を反映する
                if ( bgPixels[i+j*width] > fsPixels[i+j*width] -2 || bgPixels[i+j*width] < fsPixels[i+j*width] -4) {
                    fsPixels[i+j*width] = 0;
                }
            }
        }
        grayDiffImage.setFromPixels(fsPixels, kinect.width, kinect.height);
        
        // マスク画像生成
        createMask(maskPosition, maskR);
        
        // 深度の差分画像のマスキング
        maskedGrayImage = blackImage;
        cvCopy(grayDiffImage.getCvImage(), maskedGrayImage.getCvImage(), maskImage.getCvImage());
        
        // color画像のマスキング
        maskedColorImage = blackImage;
        cvCopy(colorImg.getCvImage(), maskedColorImage.getCvImage(), maskImage.getCvImage() );
        
        // 深度の差分画像のマスキング画像でマスキング
        detectionImage = blackImage;
        cvCopy(maskedColorImage.getCvImage(), detectionImage.getCvImage(), maskedGrayImage.getCvImage() );
        
        
		// update the cv images
		grayImage.flagImageChanged();
        maskedColorImage.flagImageChanged();
        maskedGrayImage.flagImageChanged();
        grayDiffImage.flagImageChanged();
        detectionImage.flagImageChanged();
        
		
	}
	
#ifdef USE_TWO_KINECTS
	kinect2.update();
#endif
}

//--------------------------------------------------------------
void testApp::draw() {
	
	ofSetColor(255, 255, 255);
	
    // draw from the live kinect
    kinect.drawDepth(10, 10, 400, 300);
//    kinect.draw(420, 10, 400, 300);
//    bgImage.draw(10, 320, 400, 300);
    
//    grayDiffImage.draw(10, 10, 400, 300);
    
//    maskedGrayImage.draw(10, 10, 400, 300);
    maskedColorImage.draw(420, 10, 400, 300);
    maskImage.draw(10, 320, 400, 300);
    detectionImage.draw(420, 320, 400, 300);
    
//    kinect2.drawDepth(420, 10, 400, 300);

}

//--------------------------------------------------------------
void testApp::exit() {
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
	
#ifdef USE_TWO_KINECTS
	kinect2.close();
#endif
}

//--------------------------------------------------------------
void testApp::createMask(CvPoint center, int range) {
    
    // 初期化
    maskImage = grayImage;
    
    // 塗りつぶす
    cvRectangle(maskImage.getCvImage(), cvPoint(0, 0), cvPoint(bgImage.width, bgImage.height), CV_RGB(0, 0, 0), -1, 8, 0);
    
    // マスクを作る
    cvCircle(maskImage.getCvImage(), center, range, CV_RGB(255,255,255), -1, 8, 0);
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
			
		case 'h':
            serial.writeByte('h');
			break;
			
		case 'l':
            serial.writeByte('l');
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
            
		case 'b':
            // get background image at next frame
            bBGFlag = true;
			break;
			
	}
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{
    // 表示している画像と座標を合わせる
    // window座標 -> 画像の座標
    int ix = ( x - 10 ) * kinect.width / 400;   // この数字は読み込んだ画像のサイズに合わせる
    int iy = ( y - 320 ) * kinect.height / 300;
    
    // マスクの中心を設定
    maskPosition = cvPoint(ix, iy);
    // ついでに適当な半径も決める
    maskR = 100;
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{}
