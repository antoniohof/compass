#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
   ofLog() << "[SETUP] - initializing hofmeister compass software...";
    std::vector<ofx::IO::SerialDeviceInfo> devicesInfo = ofx::IO::SerialDeviceUtils::listDevices();
    
    ofLog() << "connected serial devices: ";
    
    for (std::size_t i = 0; i < devicesInfo.size(); ++i)
    {
        ofLog() << "\t" << devicesInfo[i].getPort();
    }
    
    debug = false;
    
    //intro
    logo.load("logo.png");
    
    
    //setup compass
    bConnected = false;
    compassValues = ofVec3f(0,0,0);
    
    selectedMidiDevice = "";
    
    
    setupInterface();
}

//--------------------------------------------------------------
void ofApp::update(){
    logo.update();
    midiMenu->update();
    serialMenu->update();

    //search if not connected;
    searchInterval += ofGetLastFrameTime();
    if(!bConnected && searchInterval > 3){
        searchInterval = 0;
        searchForCompass();
    }
    
    if(bConnected){
        stringstream data;
        try
        {
            // Read all bytes from the device;
            uint8_t buffer[1024];
            
            while (compassSerial.available() > 0)
            {
                std::size_t sz = compassSerial.readBytes(buffer, 1024);
                
                for (std::size_t i = 0; i < sz; ++i)
                {
                    if(buffer[i] == ofToChar(globals::separator)) {
                        ofLog() << "[SERIAL]" << "received: " << data.str();
                       data.clear();
                    }else{
                        data << buffer[i];
                    }
                }
            }
        }
        catch (const std::exception& exc)
        {
            ofLogError("ofApp::update") << exc.what();
        }
        
        
    }
    
    serialMenu->getTextInput("status")->setText(bConnected ? "connected" : "disconnected");
    serialMenu->getTextInput("status")->setBackgroundColor(bConnected ? ofColor(0,150,0) : ofColor(150,0,0));
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofClear(0);
    
    midiMenu->draw();
    serialMenu->draw();
    
    if(debug){
        ofSetColor(255);
        ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate()), 20, 750);
        ofDrawBitmapString("selected midi device: " + selectedMidiDevice, 20, 770);

    }
    

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {

        case ' ':
            debug = !debug;
            break;
        
        case 's':
            // Send some new bytes to the device to have them echo'd back.
            string text = "coconut";
            
            ofx::IO::ByteBuffer textBuffer(text);
            
            compassSerial.writeBytes(textBuffer);
            
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

void ofApp::searchForCompass(){
    ofLog() << "[UPDATE] - searching...";
    
    std::vector<ofx::IO::SerialDeviceInfo> devicesInfo = ofx::IO::SerialDeviceUtils::listDevices();

    for (std::size_t i = 0; i < devicesInfo.size(); ++i){
        string deviceName = devicesInfo[i].getPort();
        if(deviceName == globals::compassSerialName){
            
            bool success = compassSerial.setup(globals::compassSerialName, globals::baudrate);
            
            if(success)
            {
                //serialIn.registerAllEvents(this);
                ofLog() << "[SERIAL] successfull connected to " << globals::compassSerialName << " at " << globals::baudrate;
            }
            else
            {
                ofLog() << "[SERIAL] unable to connect to " << globals::compassSerialName << " at " << globals::baudrate;
            }
            bConnected = success;
            
        }
    }
}

void ofApp::saveCompassConfig(){
    globals::xml.setValue("root:compassSerialName", globals::compassSerialName);
    globals::xml.setValue("root:baudrate", globals::baudrate);
    globals::xml.setValue("root:compassOrientation", globals::compassOrientation);
    globals::xml.saveFile();
}


void ofApp::setupInterface(){
    //midi
    midiMenu = new ofxDatGui(300,0);
    midiMenu->setAutoDraw(false);
    midiMenu->addHeader("OUT: midi", true);
    midiMenu->addButton("refresh");
    midiMenu->addLabel("select below your output device");
    midiMenu->addDropdown("devices", midiOut.getPortList());
    
    midiMenu->onButtonEvent(this, &ofApp::onButtonEvent);
    midiMenu->onDropdownEvent(this, &ofApp::onDropdownEvent);
    
    
    //serial
    serialMenu = new ofxDatGui(0,0);
    serialMenu->setAutoDraw(false);
    serialMenu->addHeader("IN: compass");
    serialMenu->addTextInput("status", "disconnected");

    serialMenu->addTextInput("bluetooth", globals::compassSerialName);
    serialMenu->addTextInput("baudrate", ofToString(globals::baudrate));
    vector<string> orientations;
    orientations.push_back("horizontal");
    orientations.push_back("vertical");
    serialMenu->addDropdown("orientation", orientations);
    serialMenu->addButton("save bluetooth config");
    serialMenu->addBreak();
    serialMenu->addValuePlotter("x", 0, 255);
    serialMenu->addValuePlotter("y", 0, 255);
    serialMenu->addValuePlotter("z", 0, 255);


    serialMenu->onButtonEvent(this, &ofApp::onButtonEvent);
    serialMenu->onTextInputEvent(this, &ofApp::onTextInputEvent);
    serialMenu->onDropdownEvent(this, &ofApp::onDropdownEvent);
}

void ofApp::refreshMidiList(){
    ofLog() << "[MIDI] refreshing midi out devices...";
    for(int i = 0; i < midiMenu->items.size(); i++){
        if(midiMenu->items.at(i) == midiMenu->getDropdown("devices")){
            midiMenu->items.erase(midiMenu->items.begin() + i);
            midiMenu->addDropdown("devices", midiOut.getPortList());
        }
    }
    midiMenu->getDropdown("devices")->expand();
}

void ofApp::onButtonEvent(ofxDatGuiButtonEvent e){
    if(e.target->getName() == "refresh") refreshMidiList();
    else if(e.target->getName() == "save bluetooth config") saveCompassConfig();
}

void ofApp::onDropdownEvent(ofxDatGuiDropdownEvent e){
    if(e.target->getName() == "devices"){
        if(e.target->getSelected()->getLabel() != selectedMidiDevice){
            midiOut.closePort();
            selectedMidiDevice = e.target->getSelected()->getLabel();
            midiOut.openPort(selectedMidiDevice);
            ofLog() << "[MIDI] selected midi device: " + selectedMidiDevice;
        }
    }else if(e.target->getName() == "orientation"){
        globals::compassOrientation = e.target->getSelected()->getLabel();
    }
}

void ofApp::onTextInputEvent(ofxDatGuiTextInputEvent e){
    globals::compassSerialName = serialMenu->getTextInput("bluetooth")->getText();
    globals::baudrate = ofToInt(serialMenu->getTextInput("baudrate")->getText());
    globals::compassOrientation = serialMenu->getDropdown("orientation")->getSelected()->getLabel();
}

