#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofBackground(0);
    
    video.initGrabber(1280, 720);
    
    isf.setup(1280, 720, GL_RGB32F);
    isf.load("Circular Screen");
    isf.setImage("inputImage", video.getTexture());
}

//--------------------------------------------------------------
void ofApp::update(){
    video.update();
    isf.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    isf.draw(0, 0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    isf.setUniform<ofVec2f>("center", ofPoint(x, y));
    isf.setUniform<float>("scale", ofMap(x, 0, ofGetWidth(), 0.0, 1.0));
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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
