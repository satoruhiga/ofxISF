//
//  Gui.h
//
//  Created by Atsushi Tadokoro on 2/14/16.
//
//

#pragma once
#include "Chain.h"
#include "Uniforms.h"
#include "ofxGui.h"

OFX_ISF_BEGIN_NAMESPACE

class ISFGuiParameters {
public:
    void setup(string name){
        parameters.setName(name);
    }
    
    ofParameterGroup parameters;
    vector<string> types;
};

class Gui {
public:
    Gui(){ }
    
    void setup(Chain *_chain){
        chain = _chain;
        gui.setup();
    }
    
    void addGuiParameters(){
        int j = chain->size() - 1;
        ISFGuiParameters gr;
        gr.setup(chain->getShader(j)->getName());
        
        ofParameter<bool> isEnable;
        isEnable.set("Enable", true);
        gr.parameters.add(isEnable);
        gr.types.push_back("enebale");
        
        for (int i = 0; i < chain->getShader(j)->getInputs().size(); i++) {
            string name = chain->getShader(j)->getInputs().getUniform(i)->getName();
            string type;
            chain->getShader(j)->getInputs().getUniform(i)->useNoralizedValue = true;
            
            if (chain->getShader(j)->getInputs().getUniform(i)->isTypeOf<float>())
            {
                type = "float";
                ofParameter<float> param;
                param.set(name, 0.5, 0.0, 1.0);
                gr.parameters.add(param);
                gr.types.push_back("float");
            }
            else if (chain->getShader(j)->getInputs().getUniform(i)->isTypeOf<ofVec2f>())
            {
                type = "ofVec2f";
                ofParameter<ofVec2f> param;
                param.set(name, ofVec2f(ofGetWidth()*.5,ofGetHeight()*.5), ofVec2f(0,0),ofVec2f(ofGetWidth(), ofGetHeight()));
                gr.parameters.add(param);
                gr.types.push_back("ofVec2f");
            }
            else if (chain->getShader(j)->getInputs().getUniform(i)->isTypeOf<ofFloatColor>())
            {
                type = "ofFloatColor";
                ofParameter<ofFloatColor> param;
                param.set(name, ofFloatColor(0.5, 0.5, 0.5, 1.0), ofFloatColor(0., 0., 0., 0.), ofFloatColor(1., 1., 1., 1.));
                gr.parameters.add(param);
                gr.types.push_back("ofFloatColor");
            }
            else if (chain->getShader(j)->getInputs().getUniform(i)->isTypeOf<bool>())
            {
                type = "bool";
                ofParameter<bool> param;
                param.set(name, true);
                gr.parameters.add(param);
                gr.types.push_back("bool");
            }
            else if (chain->getShader(j)->getInputs().getUniform(i)->isTypeOf<ofTexture*>())
            {
                type = "ofTexture";
            }
        }
        gui.add(gr.parameters);
        guiParams.push_back(gr);
    }
    
    void update(){
        for (int j = 0; j < guiParams.size(); j++) {
            for (int i = 0; i < guiParams[j].parameters.size(); i++) {
                string name = guiParams[j].parameters[i].getName();
                string type = guiParams[j].types[i];
                
                if (type == "enebale") {
                    ofParameter<bool> value = guiParams[j].parameters.getBool(name);
                    chain->setEnable(j, value);
                }
                if (type == "float") {
                    ofParameter<float> value = guiParams[j].parameters.getFloat(name);
                    chain->getShader(j)->setUniform<float>(name, value);
                }
                if (type == "ofVec2f") {
                    ofVec2f value;
                    value.x = guiParams[j].parameters.getVec2f(name)->x;
                    value.y = guiParams[j].parameters.getVec2f(name)->y;
                    chain->getShader(j)->setUniform<ofVec2f>(name, value);
                }
                if (type == "ofFloatColor") {
                    ofVec4f col;
                    col.x = guiParams[j].parameters.getVec4f(name)->x;
                    col.y = guiParams[j].parameters.getVec4f(name)->y;
                    col.z = guiParams[j].parameters.getVec4f(name)->z;
                    col.w = guiParams[j].parameters.getVec4f(name)->w;
                    ofParameter<ofFloatColor> value = ofFloatColor(col.x, col.y, col.z, col.w);
                    chain->getShader(j)->setUniform<ofFloatColor>(name, value);
                }
            }
        }
    }
    
    void draw(){
        gui.draw();
    }
    
    ofxPanel gui;
    vector<ISFGuiParameters> guiParams;
    Chain *chain;
};

OFX_ISF_END_NAMESPACE
