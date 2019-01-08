//
//  Gui.h
//
//  Created by Atsushi Tadokoro on 2/14/16.
//
//

#pragma once
#include "Chain.h"
#include "Uniforms.h"
#include "ofxFluidUi.h"

OFX_ISF_BEGIN_NAMESPACE

class ISFGuiParameters {
public:
    void setup(string name){
        parameters.setName(name);
    }
    
    ofParameterGroup parameters;
    vector<string> types;
};

class Fluid {
public:
    Fluid(){ }
    FluidUi * ui;
    vector<FluidGroup *> fluidGroups;
    
    void setup(Chain *_chain, FluidUi * u){
        chain = _chain;
        ui = u;
    }
    
    void addGuiParameters(){
        int j = chain->size() - 1;
        
        string name = chain->getShader(j)->getName();
        FluidGroup * group = &ui->addGroup(name);
        group->addLabel(name);
        fluidGroups.push_back(group);
        FluidUi * fluidEnable = &group->addToggle("Enable", false);
        
        
        for (int i = 0; i < chain->getShader(j)->getInputs().size(); i++) {
            string name = chain->getShader(j)->getInputs().getUniform(i)->getName();
            string type;
            chain->getShader(j)->getInputs().getUniform(i)->useNoralizedValue = true;
            
            
            
            if (chain->getShader(j)->getInputs().getUniform(i)->isTypeOf<float>())
            {
//                Uniform::Ref = chain->getShader(j)->getInputs().getMin(i);
                group->addSlider(name, 0.5, 0, 1);
                
            }
            else if (chain->getShader(j)->getInputs().getUniform(i)->isTypeOf<ofVec2f>())
            {
                int ww = ofGetWidth();
                int hh = ofGetHeight();
                group->addAxisSlider(name, ww/2, hh/2, 0, ww, 0, hh);
            }
            else if (chain->getShader(j)->getInputs().getUniform(i)->isTypeOf<ofFloatColor>())
            {
                
                group->addKnob(name, 0.5, 0, 1);
                
            }
            else if (chain->getShader(j)->getInputs().getUniform(i)->isTypeOf<bool>())
            {
                group->addToggle(name,true);
            }
            else if (chain->getShader(j)->getInputs().getUniform(i)->isTypeOf<ofTexture*>())
            {
                type = "ofTexture";
            }
        }
    }
    
    void update(){
        
        int i = 0;
        for (auto & group : fluidGroups) {
            for (auto & ele : group->pattern) {
                
                string name = ele->getName();
                if (name == "Enable") chain->setEnable(i, ele->getState());
                
                if (ele->isSubType(FLUID_SLIDER)) { // float
                    chain->getShader(i)->setUniform<float>(name, ele->getValue());
                }
                if (ele->isSubType(FLUID_AXISSLIDER)) { // ofVec2f
                    ofVec2f value;
                    value.x = ele->getValueX();
                    value.y = ele->getValueY();
                    chain->getShader(i)->setUniform<ofVec2f>(name, value);
                }
                if (ele->isSubType(FLUID_KNOB)) {
                    ofFloatColor value = ofFloatColor(ele->getValue());
                    chain->getShader(i)->setUniform<ofFloatColor>(name, value);
                }
            }
           i += 1;
        }
        
    }
    
    Chain *chain;
};

OFX_ISF_END_NAMESPACE

