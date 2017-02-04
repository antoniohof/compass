#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
   ofLog() << "[SETUP] - initializing hofmeister compass software...";
    
    debug = false;
    
    //intro
    logo.load("logo.png");
    
    setupGui();
    
    checkMidiListInterval = 0;
}

//--------------------------------------------------------------
void ofApp::update(){
    logo.update();
    midiGui->update();
    
    //update midi out devices every 10s
    checkMidiListInterval += ofGetLastFrameTime();
    
    if(checkMidiListInterval > 10){
        checkMidiListInterval = 0;
        updateMidiList();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if(ofGetElapsedTimeMillis() < 2500) {
        ofClear(255);
        ofSetWindowShape(logo.getWidth(), logo.getHeight());
        ofSetColor(255);
        logo.draw(0,0, logo.getWidth(), logo.getHeight());
    } else{
        ofClear(0);
        ofSetWindowShape(400,800);
        
        midiGui->draw();
    }
    
    
    if(debug){
        ofSetColor(255);
        ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate()), 20, 100);
        ofDrawBitmapString("frame: " +  ofToString(checkMidiListInterval) + " / " + ofToString(ofGetLastFrameTime()), 20, 120);

    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case ' ':
            debug = !debug;
            break;
            
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::setupGui(){
    //midi
    midiGui = new ofxDatGui(0,0);
    midiGui->addHeader("MIDI", true);
    
    midiGui->addDropdown("select out device", midiOut.getPortList());
    
    midiGui->setAutoDraw(false);
}


void ofApp::updateMidiList(){
    ofLog() << "[MIDI] updating out devices...";

}