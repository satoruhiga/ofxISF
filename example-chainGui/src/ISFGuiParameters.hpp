//
//  ISFGuiRenderer.hpp
//  example-chainGui
//
//  Created by Atsushi Tadokoro on 2/14/16.
//
//

#pragma once
#include "ofMain.h"

class ISFGuiParameters {
public:
    ISFGuiParameters();
    void setup(string name);
    ofParameterGroup parameters;
    vector<string> types;
};
