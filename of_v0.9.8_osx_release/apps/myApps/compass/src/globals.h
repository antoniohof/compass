#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

class globals
{
public:
    static void setup(string xmlPath);
    
    static ofxXmlSettings xml;
    
    static string compassSerialName;
    static string compassOrientation;
    static int baudrate;
    static string separator;
    
};