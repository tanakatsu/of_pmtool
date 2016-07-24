#pragma once

//#include "ofxBlackmagic.h" // ofMain.hより先に定義する (See https://github.com/elliotwoods/ofxBlackmagic2)
#include "ofMain.h"
#include "ofxUI.h"
#include "ofxStateMachine.h"
#include "SharedData.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void exit(); // ofxUI

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

        ofxUISuperCanvas *ddlGui[2];
        ofxUICanvas *btnGui[2];
        void guiEvent(ofxUIEventArgs &e);
        void onDeviceSelected(int cameraNo, int deviceId);

        void changeState(string name, ...);
        void saveUserSettings();
        void loadUserSettings();
        void loadSystemSettings();

    private:
        itg::ofxStateMachine<SharedData> stateMachine;

        unsigned long long changeStateFiredAt;
        int tmpActiveCameraNo;
};
