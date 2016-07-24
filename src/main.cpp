#include "ofApp.h" // ofxBlackMagicを使うときofMain.hより先に定義する (See https://github.com/elliotwoods/ofxBlackmagic2)
#include "ofMain.h"
//#include "ofApp.h"
#include "ofAppGLFWWindow.h"

//========================================================================
int main( ){
//	ofSetupOpenGL(800, 600,OF_WINDOW);			// <-------- setup the GL context

    ofAppGLFWWindow window;
    ofSetupOpenGL(&window, 1200, 600, OF_WINDOW);
//    ofSetupOpenGL(&window, 800, 600, OF_FULLSCREEN);

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
