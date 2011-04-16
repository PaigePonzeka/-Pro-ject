#include "handJesture.h"
//#include "zhelpers.hpp"

//the ZMQ socket used to transfer data to the browser
/*
 zmq::context_t context (1);
 zmq::socket_t socket (context, ZMQ_PUB);
 */
//--------------------------------------------------------------
Shape* Shape::board[10];
//screen size
int screen_width= 1024;
int screen_height=768;
float centroidX;//hand location
float centroidY;
float prev_x = 0.0f;//previous location for velocity calc
float prev_y = 0.0f;
float RGB[] = {0.0f,128.0f,255.0f};//limit color options

int checkMouseDownCount = 0;
int cornerCount = 0;

const int MOUSE_CLICK_FRAME = 8;
const int HAND_MODE_NORMAL = 0;
const int HAND_MODE_MOVE = 1;
const int HAND_MODE_CLICK = 2;
const int HAND_MODE_DRAG = 3;
const int POSITION_HISTORY_SIZE = 4;
const int JESTURE_FIRE_BUFFER = 20;

const int SHAPE_SIZE_MAX = 200;//only used for rand()% expr so keep them ints
const int SHAPE_SIZE_MIN = 100;


void HandJesture::initShapeBoard(){
	for(int i = 0; i<10 ; i++){
		
		Shape::board[i] = new Shape(float(rand()%screen_width),float(rand()%screen_height),		//random location 
                                    float(ofRandom(SHAPE_SIZE_MIN, SHAPE_SIZE_MAX)),//random width 
							 float(ofRandom(SHAPE_SIZE_MIN, SHAPE_SIZE_MAX)),			//random height
							 RGB[rand()%3], RGB[rand()%3], RGB[rand()%3], float(rand()%255));//random color
	}
}

void HandJesture::setup() {
    soundClick.loadSound("sound/16582__tedthetrumpet__kettleswitch1.aif");
    soundClick.setVolume(100);
	ofSetLogLevel(0);
	ofLog(OF_LOG_VERBOSE, "Start setup()");
    jestureFiredCount = 0;
	toNormalModeCount = 0;
	
    
    debug = false;
	showConfigUI = true;
	mirror = true;
    showUserFeedback=true;
    
    //intialize the background board sound
    HandJesture::background_sound.loadSound("sound/beat.wav");
    HandJesture::background_sound.setVolume(0.75f);
	HandJesture::background_sound.setMultiPlay(true);
    HandJesture::background_sound.setLoop(true);
    	
	// Setup Kinect
	angle = 5;
	//kinect.init(true);  //shows infrared image
	kinect.init();
	//kinect.setVerbose(true);
	kinect.open();
	kinect.setCameraTiltAngle(angle);
	
	// Setup ofScreen
	//ofSetFullscreen(true);
	ofSetFrameRate(30);
	ofBackground(0, 0, 0);
	ofSetWindowShape(800, 600);
	
	// For GAMEBOARD
	HandJesture::initShapeBoard();
	
	// For images
	grayImage.allocate(kinect.width, kinect.height);
	checkGrayImage.allocate(kinect.width, kinect.height);
	grayThresh.allocate(kinect.width, kinect.height);
	
	// For hand detection *these values are set in calibration*
	nearThreshold = 5;
	farThreshold = 30;
	detectCount = 0;
	detectTwoHandsCount = 0;
	
    //screen display values, increase to increase screen size
	displayWidth = 1280;
	displayHeight = 800;
	
	// Fonts
	msgFont.loadFont("Courier New.ttf",14, true, true);
	msgFont.setLineHeight(20.0f);
    
   	
	/*
     try {
     socket.bind ("tcp:*:14444");
     s_sendmore (socket, "event");
     s_send (socket, "{type:\"up\"}");
     std::cout << "Open Zeromq socket" << endl;
     }
     catch (zmq::error_t e) {
     std::cerr << "Cannot bind to socket: " <<e.what() << endl;
     exit();
     }
     */
	
	// Sounds
	soundDetect.loadSound("sound/16582__tedthetrumpet__kettleswitch1.aif");
	soundDetect.setVolume(100);
	soundRelease.loadSound("sound/2674__dmooney__TAPE32.wav");
	soundRelease.setVolume(100);
	
	// setup config gui
	/*set up infromation for the setup screen*/
	gui.setup();
	gui.config->gridSize.x = 300;
	gui.addTitle("KINECT SETTINGS");
	gui.addSlider("Tilt Angle", angle, -30, 30);
	gui.addToggle("Mirror Mode", mirror);
	gui.addTitle("DETECT RANGE");
	gui.addSlider("Near Distance", nearThreshold, 5, 20);
	gui.addSlider("Far Distance", farThreshold, 20, 60);
	gui.addTitle("MOUSE CONTROLL");
	gui.addSlider("Display Width", displayWidth, 600, 1980);
	gui.addSlider("Display height", displayHeight, 600, 1980);
	gui.setDefaultKeys(true);
	gui.loadFromXML();
	gui.show();

    
}

//--------------------------------------------------------------
void HandJesture::update() {
    
	kinect.update();
	checkDepthUpdated();
	//prints gray pixels where the image is detected
	grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
	grayThresh.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
	
	unsigned char * pix = grayImage.getPixels();
	unsigned char * grayThreshPix = grayThresh.getPixels();
	int numPixels = grayImage.getWidth() * grayImage.getHeight();
    
	int maxThreshold = 255 - nearThreshold;// default 250
	int minThreshold = 255 - farThreshold; // default 225
	
	int nearestDepth = 0;
	for(int i = 0; i < numPixels; i++){
		if (minThreshold < pix[i] && pix[i] < maxThreshold && pix[i] > nearestDepth) {
			nearestDepth = pix[i];
		}
	}
    
	for(int i = 0; i < numPixels; i++){
		//if( pix[i] < nearThreshold && pix[i] > farThreshold ){
		if(minThreshold < pix[i] 
           && pix[i] < nearestDepth+2 
           && pix[i] > nearestDepth - 10 ){
			//grayThreshPix[i] = floor((5 - (nearestDepth - pix[i]))/5.0f*255.0f);
			pix[i] = 255; // white
		}else{
			pix[i] = 0;
			//grayThreshPix[i] = 0;
		}
	}
	
	//update the cv image
	grayImage.flagImageChanged();
	//grayThresh.flagImageChanged();
	if (mirror) {
		grayImage.mirror(false, true);
	}
	//grayThresh.mirror(false, true);
	
    contourFinder.findContours(grayImage, 1500, (kinect.width*kinect.height)/4, 2, false);
	
	if (showConfigUI) {
		return;
	}
	
	int detectHands = contourFinder.nBlobs;
	
	if (detectHands == 2) {
		detectTwoHandsCount = min(60, ++detectTwoHandsCount);
	} else {
		detectTwoHandsCount = max(0, --detectTwoHandsCount);
	}
	
	if (detectHands > 0) {
		detectCount = min(50, ++detectCount);
	} else {
		detectCount = max(0, --detectCount);
	}
	
	if (detectingHands) {
		if (detectCount < 10) {
			detectingHands = false;
			sendEvent("UnRegister", "\"mode\":\"single\"");
			for (int j = 0; j < hands.size(); j++) {
				hands[j]->unRegister();
			}
			soundRelease.play();
		}
	} else {
		if (detectCount > 30) {
			detectingHands = true;
			sendEvent("Register", "\"mode\":\"single\"");
			soundDetect.play();
		}
	}
    
	ofLog(OF_LOG_VERBOSE, ofToString(detectTwoHandsCount));
	if (detectingTwoHands) {
		ofLog(OF_LOG_VERBOSE, "detecTwo");
		if (detectTwoHandsCount < 15) {
			detectingTwoHands = false;
			sendEvent("Register", "\"mode\":\"double\"");
		}
	} else {
		//ofLog(OF_LOG_VERBOSE, "Not...");
		if (detectTwoHandsCount > 30) {
			detectingTwoHands = true;
			sendEvent("Register", "\"mode\":\"double\"");
            
            /*Kinect Jesture keyboard events*/
			//CGEventRef keyEv = CGEventCreateKeyboardEvent(NULL, (CGKeyCode)101, true);
			//CGEventPost (kCGHIDEventTap, keyEv);
			//CGEventRef keyEv2 = CGEventCreateKeyboardEvent(NULL, (CGKeyCode)101, false);
			//CGEventPost (kCGHIDEventTap, keyEv2);
            
		}
	}
	
	if (detectingHands && detectHands == 1) {
        
		for (int i = 0; i < contourFinder.nBlobs; i++){
			int centerX = contourFinder.blobs[i].centroid.x;
			int centerY = contourFinder.blobs[i].centroid.y;
            
			// Apply lowpass filter
			float x = centerX;
			float y = centerY;
			cornerCount = contourFinder.blobs[i].nPts;
			
			
			 centroidX = 0;
			 centroidY = 0;
			float addCount = 0;
			for (int j = 0; j < contourFinder.blobs[i].nPts; j+=5){
				addCount++;
				centroidX += contourFinder.blobs[i].pts[j].x;
				centroidY += contourFinder.blobs[i].pts[j].y;
			}
			centroidX = centroidX/addCount;
			centroidY = centroidY/addCount;			
			
            /*Contour finder to find and update the location of the hands*/
			if (hands.size() == 0) {
                Hand *hand = new Hand(true, displayWidth, displayHeight);
				hand->setIsActive(true);
				hand->update(ofPoint(x, y), cornerCount, ofPoint(x, y));
				hands.push_back(hand);
			} else {
				for (int j = 0; j < hands.size(); j++) {
					hands[j]->update(ofPoint(x, y), cornerCount, ofPoint(centroidX, centroidY));
				}
			}
            
			/*
             stringstream ss;
             ss  << "\"x\":"  << x
             << ",\"y\":" << y
             << ",\"z\":" << min(100, (int)kinect.getDistanceAt(centerX, centerY));
             //cout << "move: " << ss.str() << endl;
             sendEvent("Move", ss.str());
             */
		}
	}
	
	if (detectingTwoHands) {
		// scroll
	}
}

void HandJesture::sendEvent(const std::string& etype, const std::string& edata) {
	/*
     s_sendmore (socket, "event");
     stringstream ss;
     ss << "{\"type\":\"" << etype << "\",\"data\":{" << edata << "}}";
     s_send (socket, ss.str());
     //std::cout << ss.str() << endl;
     */
}


/**
 * Check depth data is updated.
 *
 * If not updated, close and reopen the Kinect.
 */
void HandJesture::checkDepthUpdated(){
    if (ofGetFrameNum() % 150 == 0) {
		ofLog(OF_LOG_VERBOSE, "check depth updated");
        unsigned char * nextDepth = kinect.getDepthPixels();
		
        if (ofGetFrameNum() != 150) {
			ofLog(OF_LOG_VERBOSE, "Start compare depth pixels");
			unsigned char * currentDepthPixels = checkGrayImage.getPixels();
			
		    int pixNum = kinect.width * kinect.height;
            for (int i=0; i<pixNum; i++) {
                if (nextDepth[i] != currentDepthPixels[i]) {
                    break;
				}
				if (i > pixNum / 2) {
					ofLog(OF_LOG_ERROR, "Depth pixels could not be refreshed. Reset Kinect");
					kinect.close();
					kinect.open();
					kinect.setCameraTiltAngle(angle);
					break;
				}
			}                  
		}
		checkGrayImage.setFromPixels(nextDepth, kinect.width, kinect.height);
	}
}



//----------------------------------------------------------------------------------------------------------------
void HandJesture::draw() {
    	ofSetColor(255, 255, 255);
	
	if (showConfigUI ==true ) {
        //draw a black background for the g ui
        ofBackground(0,0,0);	
		kinect.drawDepth(400, 0, 400, 300);
		gui.draw();
		
		msgFont.drawString("Press Space Key to start.", 20, ofGetHeight()-60);
		
		ofPushMatrix();
		ofTranslate(400, 300, 0);
		glScalef(.6, .6, 1.0f); 
        for (int i = 0; i < contourFinder.nBlobs; i++){
            ofPushMatrix();
            contourFinder.blobs[i].draw(0,0);
            //draws the hand centering dot
			ofSetColor(255, 0 , 0);
            ofFill();
            ofEllipse(contourFinder.blobs[i].centroid.x, contourFinder.blobs[i].centroid.y, 4, 4);
			
			 centroidX = 0;
			 centroidY = 0;
			float addCount = 0;
			for (int j = 0; j < contourFinder.blobs[i].nPts; j+=5){
				addCount++;
				centroidX += contourFinder.blobs[i].pts[j].x;
				centroidY += contourFinder.blobs[i].pts[j].y;
			}
			centroidX = centroidX/addCount;
			centroidY = centroidY/addCount;
			ofCircle(centroidX, centroidY, 10);

			ofPopMatrix();
        }
		ofPopMatrix();
	} 
	else {
        //draw a white background
        ofBackground(255,255,255);	
        //if showUserFeedback is on show the kinect input
        if(showUserFeedback)
        {
            grayImage.draw(screen_width-200, screen_height-150, 200, 150);
        }
        

        
		//msgFont.drawString("Press Space Key to start.", 20, ofGetHeight()-60);
		
		ofPushMatrix();
		//ofTranslate(400, 300, 0);
		glScalef(1, 1, 1.0f); 

        /*loop through each blob (hand) found*/
        for (int i = 0; i < contourFinder.nBlobs; i++){
            ofPushMatrix();
            
            //contourFinder.blobs[i].draw(0,0);
            
            //set the hand centering dot color (red)
			ofSetColor(255, 0 , 0);
            ofFill();
            
            //this prints an box around the found "hand" or blob
            //ofEllipse(contourFinder.blobs[i].centroid.x, contourFinder.blobs[i].centroid.y, 4, 4);
			
			 centroidX = 0;
			 centroidY = 0;
			float addCount = 0;
            //sets the x and y value for the contours
			for (int j = 0; j < contourFinder.blobs[i].nPts; j+=5){
				addCount++;
				centroidX += contourFinder.blobs[i].pts[j].x;
				centroidY += contourFinder.blobs[i].pts[j].y;
                //check the speed of the hand movement
                HandJesture::checkSpeedMove(centroidX, centroidY);
			}
            /*These are the values */
			centroidX = centroidX/addCount;
			centroidY = centroidY/addCount;
			
			/*scales points to the screen size*/
			centroidX = screen_width*(centroidX/620);
			centroidY = screen_height*(centroidY/460);
			
			if((prev_x ==0.0f)&&(prev_y==0.0f)){
				prev_x = centroidX;
				prev_y = centroidY;
			}
			
			//draw circle
			ofCircle(centroidX, centroidY, 10);
			HandJesture::drawShapes();
		
			
			prev_x = centroidX;
			prev_y = centroidY;
			
			
			
            /*Gets a boolean value that indicates whether the hand has been made into a fist "clicked"*/
			HandJesture::checkClick(cornerCount);
            
            
            /*For test purposes only - print the location of the the X and Y location of each hand*/
    //      printf("Hand Location X: %f Y: %f \n", centroidX,centroidY);
            ofPopMatrix();
        }
		//draw interactive shapes
			if(contourFinder.nBlobs==0)HandJesture::drawShapes();
		ofPopMatrix();
        
	}
    //set the font color
	ofSetColor(255, 255, 255);
    //print the videos frames per second
	msgFont.drawString("fps: "+ ofToString(ofGetFrameRate()), 20, ofGetHeight()-40);
    
	ofNoFill();
}
//-----------------------------------------------------------------------------------
/*print all the shape objects */
void HandJesture::drawShapes()
{
    //for every shape in the static shape array
        //generate a random colom (from an array of colors)
        //draw the shape to the screen
	
	for(int i = 0; i<10 ; i++)
	{
		ofSetColor(Shape::board[i]->getRed(), Shape::board[i]->getGreen(), Shape::board[i]->getBlue(),Shape::board[i]->getAlpha());
		ofFill();
		
		//move with the hand if its grabbed
		if((Shape::board[i]->isGrabbed())) 
		{
			Shape::board[i]->setLocation(centroidX-(Shape::board[i]->getWidth()/2), centroidY-(Shape::board[i]->getHeight()/2));
			Shape::board[i]->setVelocity(centroidX-prev_x+1, centroidY-prev_y+1);
			Shape::board[i]->checkCollision(i);
		}
		else {
			Shape::board[i]->move();
			Shape::board[i]->slow();
		}
		
		ofRect(Shape::board[i]->getLocation_x(),Shape::board[i]->getLocation_y(),
			   Shape::board[i]->getWidth(), Shape::board[i]->getHeight());
	}
}
//-------------------------------------------------------------
//Do on program exit
void HandJesture::exit(){
	kinect.close();
	ofLog(OF_LOG_NOTICE, "Close Kinect and exit");
}


//--------------------------------------------------------------
void HandJesture::keyPressed (int key)
{
	ofLog(OF_LOG_VERBOSE, ofToString(key));
	switch (key)
	{
	
		case '>':
		case '.':
			farThreshold ++;
			if (farThreshold > 255) farThreshold = 255;
			break;
		case '<':		
		case ',':		
			farThreshold --;
			if (farThreshold < 0) farThreshold = 0;
			break;
			
		case '+':
		case '=':
			nearThreshold ++;
			if (nearThreshold > 255) nearThreshold = 255;
			break;
		case '-':		
			nearThreshold --;
			if (nearThreshold < 0) nearThreshold = 0;
			break;
        case 'd':
            //debug mode on or off
            debug ? debug=false : debug=true;
            break;
		case 'r':
			// reboot kinect
			kinect.close();
			kinect.open();
			kinect.setCameraTiltAngle(angle);
            break;
		case ' ':
			showConfigUI = !showConfigUI;
			if (showConfigUI) {
                //set the calibration window shape
				ofSetWindowShape(800, 600);
                //set the background sound to stop
                HandJesture::background_sound.stop();
			} else {
				ofSetWindowShape(1024, 768);
                //set the camera tilt
				kinect.setCameraTiltAngle(angle);
                //set the background sound to play
                HandJesture::background_sound.play();
			}
			break;
        //show the corner user feedback screen
        case 's':
			showUserFeedback=!showUserFeedback;
			break;
        //move the kinect camera
		case OF_KEY_UP:
			angle++;
			if(angle>30) angle=30;
			kinect.setCameraTiltAngle(angle);
			break;
        //move the kinect camera
		case OF_KEY_DOWN:
			angle--;
			if(angle<-30) angle=-30;
			kinect.setCameraTiltAngle(angle);
			break;
	}
}
//-----------------------------------------------------------------------
/*Checks to see if the hand has been clicked*/
void HandJesture::checkClick(int cornerCount) {
	cornerCountHistory.push_back(cornerCount);
	if (cornerCountHistory.size() > 6) {
		cornerCountHistory.erase(cornerCountHistory.begin());
	} else {
		return;
	}
    
	int oldCornerNums = 0;
	int cornerNums = 0;
	for (int i=0; i<cornerCountHistory.size(); i++) {
		if (i < 4) {
			oldCornerNums += cornerCountHistory[i];
		} else {
			cornerNums += cornerCountHistory[i];
		}
	}
	oldCornerNums = oldCornerNums/4;
	cornerNums = cornerNums/2;
    
	
	if (handMode == HAND_MODE_NORMAL && cornerNums + 150 < oldCornerNums) {
		// mouse down
		currentCornerNums = cornerNums;
        printf("\n MOUSE DOWN \n \n");
		handMode = HAND_MODE_CLICK;
		//checkMouseDownCount = 0;
		for( int s=0; s<10; s++){
			if(!(Shape::board[s]->isGrabbed())&&(Shape::board[s]->hoveredOver(centroidX,centroidY)))
			Shape::board[s]->grabShape();
		}
		return;
	}
	if (cornerNums > currentCornerNums + 150) {
		if (handMode == HAND_MODE_DRAG) {
			//fireMouseUp();
             printf("\n MOUSE UP \n \n");
		//	soundClick.play();
			handMode = HAND_MODE_NORMAL;
			for( int s=0; s<10; s++)
				Shape::board[s]->releaseShape();
			return;
		} else if (handMode == HAND_MODE_CLICK) {
			//fireMouseClick();
			soundClick.play();
			printf("\n MOUSE UP \n \n");
			for( int s=0; s<10; s++)
				Shape::board[s]->releaseShape();
			handMode = HAND_MODE_NORMAL;
			return;
		}
	}
	if (handMode == HAND_MODE_CLICK) {
		checkMouseDownCount++;
		if (checkMouseDownCount > MOUSE_CLICK_FRAME) {
			handMode = HAND_MODE_DRAG;
             printf("\n MOUSE DOWN \n \n");
			soundClick.play();
			checkMouseDownCount = 0;
			for( int s=0; s<10; s++){
				if(!(Shape::board[s]->isGrabbed())&&(Shape::board[s]->hoveredOver(centroidX,centroidY)))
					Shape::board[s]->grabShape();
			}
		}
	}
	return;
}
/*Check The Speed of the hand movement*/
void HandJesture::checkSpeedMove(float x, float y) {
    float slowest_beat_speed = .75;
    float fastest_beat_speed = 1.25;
    float widthStep = ofGetWidth() / 3.0f;
	if (x >= widthStep && x < widthStep*2){
        float speed = 0.5f + ((float)(ofGetHeight() - y) / (float)ofGetHeight())*0.75f;
        //make sure speed doesn't fall above or below a certain threshold
        if(speed < slowest_beat_speed)
        {
            speed = slowest_beat_speed;
        }
        if(speed > fastest_beat_speed)
        {
            speed = fastest_beat_speed;
        }

		background_sound.setSpeed(speed);
        printf("setting Background Beat Speed: %f",speed);
	} 

}
/*void HandJesture::setBeatSpeed(int x, int y, int button){
	// continuously control the speed of the beat sample via drag, 
	// when in the "beat" region:
	float widthStep = ofGetWidth() / 3.0f;
	if (x >= widthStep && x < widthStep*2){
		beats.setSpeed( 0.5f + ((float)(ofGetHeight() - y) / (float)ofGetHeight())*1.0f);
	} 
    
}*/

//--------------------------------------------------------------
void HandJesture::mouseMoved(int x, int y)
{}

//--------------------------------------------------------------
void HandJesture::mouseDragged(int x, int y, int button)
{}

//--------------------------------------------------------------
void HandJesture::mousePressed(int x, int y, int button)
{
}

//--------------------------------------------------------------
void HandJesture::mouseReleased(int x, int y, int button)
{
}

//--------------------------------------------------------------
void HandJesture::windowResized(int w, int h)
{}

