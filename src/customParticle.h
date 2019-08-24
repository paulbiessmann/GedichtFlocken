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

enum particleMode{
    PARTICLE_MODE_EXPLODE,
    PARTICLE_MODE_SNOW,
    PARTICLE_MODE_TEX_GLITCH,
    PARTICLE_MODE_LAYER
};


class customParticle{
    
public:
    
    customParticle();
    
    void setDrawMode(particleDrawMode newMode);
    void setParticleMode(particleMode newParticleMode);

    void getImage();
    void setColor(ofColor color);
    void setPos(ofVec3f position);
    void setStartingTime(float _time, int _frameNum);
    void setParticleSize(int thisParticleSize);
    void setParticleSize(int thisParticleSizeX, int thisParticleSizeY);

    void setParticleImg(ofImage &image);
    void setGlobalPos(ofVec3f _xyz);
    
    void addBlinky(float blinkyness, float staticVal);

    void reset();
    void update();
    void draw();
    
    ofVec3f globalPos;
    ofVec3f pos;
    ofVec3f vel;
    ofVec3f frc;
    float   rotFrc;
    
    float rotation;
    float friction;
    
    ofVec3f gravity;
    
    float age;
    
    float drag;
    float uniqueVal;
    float scale;
    
    ofImage particleTexture;
    ofColor customColor;
        
    int     particleSize = 3;
    int     particleSizeX = 10;
    int     particleSizeY = 10;
    
    //particleDrawMode drawMode = PARTICLE_MODE_POINTS;
    particleDrawMode drawMode = PARTICLE_MODE_TEXTURES;
    
    particleMode        pMode = PARTICLE_MODE_SNOW;
    
    int fullWidth = 3840;
    int fullHeight = 2160;    
 //   int fullWidth = 1920;
//    int fullHeight = 1080;

    int     startingFrame;
   // int     relFrameNum;
    float   startingTimef;
    
};
