//
//  customParticle.hpp
//  testDOF
//
//  Created by Paul Bießmann on 09.08.19.
//

#ifndef customParticle_h
#define customParticle_h

#include <stdio.h>
#include "ofMain.h"

#endif /* customParticle_h */

enum particleDrawMode{
    PARTICLE_MODE_POINTS,
    PARTICLE_MODE_TEXTURES
};


class customParticle{
    
public:
    
    customParticle();
    
    void setDrawMode(particleDrawMode newMode);

    void getImage();
    void setColor(ofColor color);
    void setPos(ofVec3f position);
    void setStartingTime(float _time);
    void setParticleSize(int thisParticleSize);
    void setParticleImg(ofImage image);
    void setGlobalPos(ofVec3f _xyz);
    void reset();
    void update();
    void draw();
    
    ofVec3f globalPos;
    ofVec3f pos;
    ofVec3f vel;
    ofVec3f frc;
    
    float rotation;
    float friction;
    
    ofVec3f gravity;
    
    float drag;
    float uniqueVal;
    float scale;
    
    ofImage particleTexture;
    ofColor customColor;
    
    int particleSize = 3;
    
    //particleDrawMode drawMode = PARTICLE_MODE_POINTS;
    particleDrawMode drawMode = PARTICLE_MODE_TEXTURES;
    
    int fullWidth = 3840;
    int fullHeight = 2160;
    
    float startingTimef;
    
};
