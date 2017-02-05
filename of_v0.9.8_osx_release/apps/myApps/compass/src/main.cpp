#include "ofMain.h"
#include "ofApp.h"
#include "globals.h"

//========================================================================
int main( ){
	ofSetupOpenGL(600,800,OF_WINDOW);			// <-------- setup the GL context
    
    globals::setup("config.xml");
    
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
