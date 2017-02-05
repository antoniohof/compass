#include "globals.h"

string globals::compassSerialName;
string globals::compassOrientation;
int globals::baudrate;
string globals::separator;

ofxXmlSettings globals::xml;

void globals::setup(string xmlPath)
{
    if(xml.loadFile(xmlPath)){
        xml.pushTag("root", 0);
        
        compassSerialName = xml.getValue("compassSerialName", compassSerialName);
        compassOrientation = xml.getValue("compassOrientation", compassOrientation);
        baudrate = xml.getValue("baudrate", baudrate);
        separator = xml.getValue("separator", separator);
        xml.popTag();
    }
}