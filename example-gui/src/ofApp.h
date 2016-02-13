#pragma once

#include "ofMain.h"
#include "ofxISF.h"
#include "ofxGui.h"

class floatInput {
public:
    string name;
    ofParameter<float> value;
};

class vec2fInput {
public:
    string name;
    ofParameter<ofVec2f> value;
};

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void setupGui();
    
    ofxISF::Shader isf;
    ofVideoGrabber video;
    
    ofxPanel gui;
    vector<floatInput> floatInputs;
    vector<vec2fInput> vec2fInputs;

};
