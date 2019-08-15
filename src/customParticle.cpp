//
//  customParticle.cpp
//  testDOF
//
//  Created by Paul Bießmann on 09.08.19.
//

#include "customParticle.h"


//------------------------------------------------------------------
customParticle::customParticle(){
    //leaf.load("1-2.png");
    
}

//------------------------------------------------------------------
void customParticle::getImage(){
    //leaf.load("test2.png");
}

//------------------------------------------------------------------
void customParticle::setDrawMode(particleDrawMode newDrawMode){
    drawMode = newDrawMode;
}

//------------------------------------------------------------------
void customParticle::setParticleMode(particleMode newParticleMode){
    pMode = newParticleMode;
}


//------------------------------------------------------------------
void customParticle::setStartingTime(float _time, int _frameNum){
    startingTimef = _time;
    
    startingFrame = _frameNum;
}

//------------------------------------------------------------------
void customParticle::reset(){
    //the unique val allows us to set properties slightly differently for each particle
    uniqueVal = ofRandom(-10000, 10000);
    
    
    if(pMode == PARTICLE_MODE_EXPLODE){
        pos.x = ofRandomWidth();
        pos.y = ofRandom(0, 200);
        pos.z = -4000;
        
        globalPos = ofVec3f(0,0,0);
        
        vel.x = ofRandom(-3.9, 3.9);
        vel.y = ofRandom(-3.9,0);
        vel.z = -2;
        
        frc   = ofPoint(0,0,0);
        
        rotation = 0;
        friction = 0.4;
        
        scale = ofRandom(0.5, 1.0);
        
        if( 1 ){
            drag  = ofRandom(0.97, 0.99);
           // vel.y = fabs(vel.y) * 2.0; //make the particles all be going down
            vel.z = fabs(vel.z) *3.0;
        
        }else{
            drag  = ofRandom(0.95, 0.998);
        }
    
    }
    else if(pMode == PARTICLE_MODE_SNOW){
        
        pos.x = ofRandom(0, fullWidth);
        pos.y = ofRandom(fullHeight, fullHeight - 500 - ofRandom(100));
        pos.z = -10;
        
        globalPos = ofVec3f(0,0,0);
        
        vel.x = 0;
        vel.y = 0;
        vel.z = 0;
        frc   = ofPoint(0,0,0);
        
        rotation    = 0;//ofRandom(-180,180);
        rotFrc      = 0.1;
        friction    = 0.4;
        
    }
    customColor = ofColor(255,255,255,255);
}

//------------------------------------------------------------------
void customParticle::setPos(ofVec3f thisPos){
    
    pos.x = thisPos.x + globalPos.x;
    pos.y = thisPos.y + globalPos.y;
    
    if(thisPos.z){
        pos.z = thisPos.z + globalPos.z;
    }else{
        pos.z = 0;
    }

}

//------------------------------------------------------------------
void customParticle::setParticleSize(int thisParticleSize){
    
    particleSize = thisParticleSize;
    
}
//------------------------------------------------------------------
void customParticle::setParticleSize(int thisParticleSizeX, int thisParticleSizeY){
    particleSizeX = thisParticleSizeX;
    particleSizeY = thisParticleSizeY;
}


//------------------------------------------------------------------
void customParticle::setGlobalPos(ofVec3f _xyz){
    globalPos = _xyz;
}


//------------------------------------------------------------------
void customParticle::update(){
    
    float relTimef = ofGetElapsedTimef() - startingTimef;
    float relFrameNum = ofGetFrameNum() - startingFrame;

    //lets simulate falling snow
    //the fake wind is meant to add a shift to the particles based on where in x they are
    //we add pos.y as an arg so to prevent obvious vertical banding around x values - try removing the pos.y * 0.006 to see the banding
    float fakeWindX = ofSignedNoise(pos.x * 0.003, pos.y * 0.006, relTimef * 0.6);

    
/********** EXPLOSION STROPHEN **********/
    if(pMode == PARTICLE_MODE_EXPLODE){
        frc.x = fakeWindX * 0.25 + ofSignedNoise(uniqueVal, pos.y * 0.04) * 0.8;
        
        friction = 0.29;
        
        if(relFrameNum < (50)){
            friction = 0.9;
            //frc.z = ofSignedNoise(uniqueVal, pos.z * 0.06, relTimef*0.2) * 0.09 + 0.58;
            frc.z = 0;
            frc.x = 0;
            frc.y = 0;
            

        }
        else if(relFrameNum > (25 * 10) ){
            frc.z = ofSignedNoise(uniqueVal, pos.z * 0.06, relTimef*0.2) * 0.09 + 0.58;
            frc.x = fakeWindX * 3.25 + ofSignedNoise(uniqueVal, pos.y * 0.04) * 2.8;

//            if(pos.z > 0){
//                //frc.x *= 1.2;
//                frc.y = ofSignedNoise(uniqueVal, pos.x * 0.006, relTimef*0.2) * 0.09 - 0.15;
//                //frc.y = - frc.y;
//            }

            if(pos.y < 700){
                frc.z = -abs(frc.z) * 4;
                frc.x = fakeWindX * 3.25 + ofSignedNoise(uniqueVal, pos.y * 0.04) * 2.8;
                frc.y = ofSignedNoise(uniqueVal, pos.x * 0.006, relTimef*0.2) * 0.9 - 2.65;
            }
            else if(pos.y > 1500){
                friction = 0.49;
                frc.x = ofSignedNoise(uniqueVal, pos.x * 0.006, relTimef*0.2) * 4.5 + 0.1;
                frc.z = ofSignedNoise(uniqueVal, pos.z * 0.06, relTimef*0.2) * 0.09 + 0.5;
                frc.y = ofSignedNoise(uniqueVal, pos.y * 0.006, relTimef*0.2) * 0.7 - 0.95;

            }
            else{
                frc.x = fakeWindX * 4.25 + ofSignedNoise(uniqueVal, pos.y * 0.04) * 4.8;
                frc.z = ofSignedNoise(uniqueVal, pos.z * 0.06, relTimef*0.02) * 0.09 + 0.01;
                frc.y = ofSignedNoise(uniqueVal, pos.y * 0.006, relTimef*0.2) * 0.7 - 0.95;

            }
        }
        else{
            frc.x = ofSignedNoise(uniqueVal, pos.x * 0.06, relTimef*0.2) * 2.5;
            frc.z = ofSignedNoise(uniqueVal, pos.z * 0.06, relTimef*0.2) * 0.9 - 0.58;
            frc.y = ofSignedNoise(uniqueVal, pos.x * 0.006, relTimef*0.2) * 0.9 + 0.35;

        }
        
        drag  = ofRandom(0.67, 0.99);
        vel *= drag;
        vel += frc * 0.4 * (1.0 - friction);
        
        //2 - UPDATE OUR POSITION
        pos += vel;
        
        //3 - (optional) LIMIT THE PARTICLES TO STAY ON SCREEN
        //we could also pass in bounds to check - or alternatively do this at the ofApp level
//        if( pos.x + pos.z > fullWidth ){
//            pos.x = fullWidth + pos.z;
//            vel.x *= -1.0;
//        }else if( pos.x - pos.z < 0 ){
//            pos.x = 0 + pos.z;
//            vel.x *= -1.0;
//        }
        if( pos.x + vel.x > fullWidth  ){
            pos.x -= fullWidth;
        }
        if( pos.y + pos.z > fullHeight ){
            pos.y = fullHeight + pos.z;
            vel.y *= -1.0;
        }
        else if( pos.y - pos.z< 0 ){
            pos.y = 0  + pos.z;
            vel.y *= -1.0;
        }
        if(pos.z > 100){
            vel.z = -4;
        }
    }
/********** SNOW TEXTURES **********/
    else if(pMode == PARTICLE_MODE_SNOW){
        fakeWindX = ofSignedNoise(pos.x * 0.03, pos.y * 0.06, relTimef * 0.07);
        frc.x = fakeWindX * 0.25 + ofSignedNoise(uniqueVal, pos.y * 0.04) * 0.8;
        friction = 0.89;
        
        rotFrc = ofSignedNoise(uniqueVal, relTimef * 0.004) * 0.0008;
        
        rotation += rotFrc;
        
        frc.x = fakeWindX * 0.25 + ofSignedNoise(uniqueVal, pos.y * 0.04) * 0.8;
        frc.x += ofSignedNoise(uniqueVal, pos.x * 0.06, relTimef*0.2) * 0.5;
        frc.z = ofSignedNoise(uniqueVal, pos.z * 0.06, relTimef*0.2) * 0.2 - 0.18;
        frc.y = ofSignedNoise(uniqueVal, pos.x * 0.006, relTimef*0.2) * 0.2 - 0.15;
        
        
        if(relFrameNum > 1000){
            friction = 0.23;
            frc.x += ofSignedNoise(uniqueVal, pos.x * 0.06, relTimef*0.2) * 2.5;
            frc.z = ofSignedNoise(uniqueVal, pos.z * 0.06, relTimef*0.2) * 0.9 - 0.58;
            frc.y = ofSignedNoise(uniqueVal, pos.x * 0.006, relTimef*0.2) * 0.9 - 0.75;
            
            if(pos.y < 1000){
                frc.z = -abs(frc.z) * 2;
                frc.x = fakeWindX * 0.25 + ofSignedNoise(uniqueVal, pos.y * 0.04) * 2.8;
                frc.y = ofSignedNoise(uniqueVal, pos.x * 0.006, relTimef*0.2) * 0.9 - 0.65;
            }
            else if(pos.y > 1500){
                friction = abs(sin(relTimef *0.001)) * 0.39 + 0.39 ;
                frc.x = ofSignedNoise(uniqueVal, pos.x * 0.006, relTimef*0.2) * 0.05 + 0.5;
                frc.z = ofSignedNoise(uniqueVal, pos.z * 0.06, relTimef*0.2) * 0.09 + 0.15;
            }
            else{
                frc.x = ofSignedNoise(uniqueVal, pos.x * 0.06, relTimef*0.2) * 2.5;
                frc.z = ofSignedNoise(uniqueVal, pos.z * 0.06, relTimef*0.2) * 0.9 - 0.18;
                frc.y = ofSignedNoise(uniqueVal, pos.x * 0.006, relTimef*0.2) * 0.9 - 0.75;
            }
        }
        
        
        drag  = ofRandom(0.40, 0.99);
        vel *= drag * 1.24;
        vel += frc * 0.6 * (1.0 - friction);
        
        //2 - UPDATE OUR POSITION
        pos += vel;
        
        //3 - (optional) LIMIT THE PARTICLES TO STAY ON SCREEN
        //we could also pass in bounds to check - or alternatively do this at the ofApp level
//        if( pos.x  > fullWidth ){
//            pos.x = fullWidth ;
//            vel.x *= -1.0;
//        }else if( pos.x < 0 ){
//            pos.x = 0 ;
//            vel.x *= -1.0;
//        }
        if( pos.x + vel.x > fullWidth  ){
            pos.x -= fullWidth;
        }
        if( pos.y  > fullHeight ){
            pos.y = fullHeight ;
            vel.y *= -1.0;
        }
        else if( pos.y < 0 ){
            pos.y = 0  ;
            vel.y *= -1.0;
        }
        if(pos.z > 100){
            vel.z = -4;
        }
     
    }
    

    //we do this so as to skip the bounds check for the bottom and make the particles go back to the top of the screen
//    if( pos.y + vel.y > fullHeight ){
//        pos.y -= fullHeight;
//    }
//    if( pos.x + vel.x > fullWidth  ){
//        pos.x -= fullWidth;
//    }

 
    

  
    

}


//------------------------------------------------------------------
void customParticle::draw(){

    
    
    
    ofSetColor(customColor);
    
    
    if(drawMode == PARTICLE_MODE_POINTS){
        ofDrawRectangle(pos.x,pos.y, pos.z, particleSize, particleSize);
    }
    else if(drawMode == PARTICLE_MODE_TEXTURES){
  
        ofRotateZ(rotation);
        particleTexture.draw(pos.x, pos.y, pos.z, particleSizeX, particleSizeY);


    }
}


//------------------------------------------------------------------
void customParticle::setColor(ofColor thisColor){
    
    //blinky
    if(0){
        float newAlpha = sin(ofGetElapsedTimef() * 100 + uniqueVal )* 100 + 150;
        thisColor.r = newAlpha;
    }
    
    if(thisColor != NULL){
        customColor = thisColor;
    }
    else{
        customColor = ofColor(255,255,255,200);
    }
    
}


//------------------------------------------------------------------
void customParticle::addBlinky(float blinkyness){
    
    ofColor color = customColor;
    
    color.a = abs(ofNoise(ofGetElapsedTimef() * 0.2 + uniqueVal) * blinkyness) + blinkyness ;
    
 //   color.a -= ofMap(pos.z, 0, 255, 0, 2000);
    
    customColor = color;

}

//------------------------------------------------------------------
void customParticle::setParticleImg(ofImage &thisImage){
    

    
    particleTexture = thisImage;
    
    ofSetColor(255);
}


