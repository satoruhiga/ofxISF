#include "ofApp.h"

void ofApp::setup(){
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofBackground(0);
    
    video.initGrabber(1280, 720);
    
    chain.setup(1280, 720, GL_RGB32F);
    chain.setImage(video.getTexture());
    chain.load("isf-test");

    gui.setup(&chain);
    gui.addGuiParameters();
}

void ofApp::update(){
    gui.update();
    video.update();
    chain.update();
}

void ofApp::draw(){
    if (chain.size() > 0) {
        ofSetColor(255);
        chain.draw(0, 0);
        gui.draw();
    }
}

void ofApp::dragEvent(ofDragInfo dragInfo){
    for(unsigned int i = 0; i < dragInfo.files.size(); i++){
        string extention = dragInfo.files[i].substr(dragInfo.files[i].length()-3, dragInfo.files[i].length());
        string shaderName = dragInfo.files[i].substr(0,dragInfo.files[i].length()-3);
        if (extention == ".fs" || extention == ".vs") {
            chain.load(shaderName);
            gui.addGuiParameters();
        }
    }
}