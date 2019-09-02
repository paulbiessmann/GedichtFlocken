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
    age = 0;
    
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
        pos.y = ofRandom(fullHeight + 100, fullHeight - 30 - ofRandom(100));
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
    else if(pMode == PARTICLE_MODE_LAYER){
        
        pos.x = ofRandom(0, fullWidth);
        pos.y = ofRandom(fullHeight, fullHeight - 100 - ofRandom(10));
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
    
    pos.x = thisPos.x ;//+ globalPos.x;
    pos.y = thisPos.y ;//+ globalPos.y;
    
    if(thisPos.z){
        pos.z = thisPos.z ;//+ globalPos.z;
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
    
   // setPos(pos);
}


//------------------------------------------------------------------
void customParticle::update(){
    
    age++;
    
    float relTimef = ofGetElapsedTimef() - startingTimef;
    float relFrameNum = ofGetFrameNum() - startingFrame;

    //lets simulate falling snow
    //the fake wind is meant to add a shift to the particles based on where in x they are
    //we add pos.y as an arg so to prevent obvious vertical banding around x values - try removing the pos.y * 0.006 to see the banding
    float fakeWindX = ofSignedNoise(pos.x * 0.003, pos.y * 0.006, relTimef * 0.6);
    float fakeWindZ = ofSignedNoise(uniqueVal + pos.z * 0.005, pos.y * 0.06, relTimef * 0.1);
    
/********** EXPLOSION STROPHEN **********/
    if(pMode == PARTICLE_MODE_EXPLODE){
        frc.x = fakeWindX * 0.25 + ofSignedNoise(uniqueVal, pos.y * 0.004) * 0.8;
        
        
        friction = 0.49;
        
        if(relFrameNum < (3)){
            friction = 0.9;
            //frc.z = ofSignedNoise(uniqueVal, pos.z * 0.06, relTimef*0.2) * 0.09 + 0.58;
            frc.z = 0;
            frc.x = 0;
            frc.y = 0;
            
        }
        else if(relFrameNum < 300){
            friction = 0.89;
            //frc.z = ofSignedNoise(uniqueVal, pos.z * 0.06, relTimef*0.2) * 0.09 + 0.58;
            frc.z = fakeWindZ * 4.1 + ofSignedNoise(uniqueVal, pos.z * 0.06, relTimef*0.2) * 0.9 + 5.68;
            frc.x = fakeWindX * 4.25 + ofSignedNoise(uniqueVal, pos.y * 0.04) * 2.8;
            frc.y = fakeWindZ * 1.25 + ofSignedNoise(uniqueVal, pos.x * 2.6, relTimef*0.2) * 2.9 + 1.05;

        }
        else{
            friction = 0.69;

            frc.z = ofSignedNoise(uniqueVal, pos.z * 0.06, relTimef*0.2) * 0.09 + 0.58;
            frc.x = fakeWindX * 0.55 + ofSignedNoise(uniqueVal, pos.y * 0.04) * 2.8;

            if(pos.y < 900){
                friction = 0.9;
                frc.z = fakeWindZ * 3.1 + ofSignedNoise(uniqueVal, pos.z * 0.06, relTimef*0.2) * 1.9 - 5.58;
                frc.x = fakeWindX * 3.25 + ofSignedNoise(uniqueVal, pos.y * 0.04) * 2.8 ;
                frc.y = fakeWindX * 0.25 + ofSignedNoise(uniqueVal, pos.x * 0.26, relTimef*0.0002) * 20.9 - 2.65;

            }
            else if(pos.y > 1500){
                friction = 0.79;
                frc.x = fakeWindX * 1.25 + ofSignedNoise(uniqueVal, pos.x * 0.06, relTimef*0.2) * 4.5 + 2.1;
                frc.z = ofSignedNoise(uniqueVal, pos.z * 0.06, relTimef*0.2) * 0.09 + 0.5;
                frc.y = ofSignedNoise(uniqueVal, pos.y * 0.6, relTimef*0.2) * 1.9 - 1.9;

            }
            else{
                
                frc.x = fakeWindX * 5.25 + ofSignedNoise(uniqueVal, pos.y * 3.4) * 8.8 + 2.5;
                frc.z = ofSignedNoise(uniqueVal, pos.z * 0.06, relTimef*0.02) * 0.09 + 0.01;
                frc.y = fakeWindX * 1.25 + ofSignedNoise(uniqueVal, pos.y * 1.2, relTimef*1.2) * 1.9 - 1.91;

            }
        }
//        else{
//            frc.x = ofSignedNoise(uniqueVal, pos.x * 0.06, relTimef*0.2) * 2.5;
//            frc.z = ofSignedNoise(uniqueVal, pos.z * 0.06, relTimef*0.2) * 0.9 - 0.58;
//            frc.y = fakeWindX * 1.25 + ofSignedNoise(uniqueVal, pos.x * 0.006, relTimef*0.2) * 1.9 + 1.25;
//        }
        
        drag  = ofRandom(0.77, 0.99);
        vel *= drag;
        vel += frc * 0.4 * (1.0 - friction);
        

        if( pos.x + vel.x + globalPos.x < -20  ){
            pos.x = fullWidth + 20;
        }
        if( pos.x + vel.x + globalPos.x > fullWidth + 20  ){
            pos.x -= fullWidth - 20;
        }
        if( pos.y + vel.y + pos.z + globalPos.y > fullHeight ){
            //pos.y = fullHeight + pos.z;
            vel.y *= -1.0;
        }
        else if( pos.y + vel.y - pos.z + globalPos.y < 0 ){
            //pos.y = 0  + pos.z;
            vel.y *= -1.0;
        }
        if(pos.z + vel.z + globalPos.z > 400){
            vel.z = -4;
        }
        
        //2 - UPDATE OUR POSITION
        pos += vel;
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
            frc.y = ofSignedNoise(uniqueVal, pos.x * 0.006, relTimef*0.2) * 0.9 - 3.85;
            
            if(pos.y < 1000){
                frc.z = -abs(frc.z);
                frc.x = fakeWindX * 0.25 + ofSignedNoise(uniqueVal, pos.y * 0.04) * 2.8;
                frc.y = fakeWindX * 0.2 + ofSignedNoise(uniqueVal, pos.x * 0.006, relTimef*0.002) * 80.9 - 2.65;
            }
            else if(pos.y > 1500){
                friction = abs(sin(relTimef *0.001)) * 0.39 + 0.39 ;
                frc.x = ofSignedNoise(uniqueVal, pos.x * 0.006, relTimef*0.2) * 0.05 + 0.5;
                frc.z = ofSignedNoise(uniqueVal, pos.z * 0.06, relTimef*0.2) * 0.09 + 0.15;
            }
            else{
                frc.x = ofSignedNoise(uniqueVal, pos.x * 0.06, relTimef*0.2) * 2.5;
                frc.z = ofSignedNoise(uniqueVal, pos.z * 0.06, relTimef*0.2) * 0.9 - 0.18;
                frc.y = ofSignedNoise(uniqueVal, pos.x * 0.006, relTimef*0.2) * 0.9 - 2.75;
            }
        }
        
        if( pos.x + vel.x + globalPos.x > fullWidth  ){
            pos.x -= fullWidth;
        }
        if( pos.x + vel.x + globalPos.x < 0  ){
            pos.x = 0;
        }
       
        if( pos.y + globalPos.y < 0 ){
            pos.y = fullHeight  ;
        }
        if( pos.y + globalPos.y > fullHeight ){
            pos.y = - 300  ;
        }
        if(pos.z + globalPos.z > 100){
            pos.z = -4;
        }
        if(pos.z + globalPos.z < -1000){
            pos.z = -100;
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
       
     
    }
    
    
    /*** LAYER 2 Double Snow ***/
    else if(pMode == PARTICLE_MODE_LAYER){
        fakeWindX = ofSignedNoise(pos.x * 0.04, pos.y * 0.09, relTimef * 0.07);
        friction = 0.49;
        
        
        frc.x = fakeWindX * 0.25 + ofSignedNoise(uniqueVal, pos.y * 0.04) * 0.8;
        frc.x += ofSignedNoise(uniqueVal, pos.x * 0.06, relTimef*0.2) * 0.5;
        frc.z = ofSignedNoise(uniqueVal, pos.z * 0.06, relTimef*0.2) * 0.9 + 0.78;
        frc.y = ofSignedNoise(uniqueVal, pos.x * 0.006, relTimef*0.2) * 0.2 - 0.15;
        
        
        if(relFrameNum > 100){
            
            if(pos.y < 1000){
                frc.z = ofSignedNoise(uniqueVal, pos.z * 0.06, relTimef*0.2) * 0.9 - 2.38;
                frc.x = fakeWindX * 0.25 + ofSignedNoise(uniqueVal, pos.y * 0.04) * 1.8;
                frc.y = fakeWindX  * 0.2 +  ofNoise(uniqueVal, pos.x * 0.006, relTimef*0.002) * 5.9 - 6.25;
            }
            else{
                frc.x = fakeWindX * 1.5 +ofSignedNoise(uniqueVal, pos.x * 0.06, relTimef*0.2) * 1.9;
                frc.z = ofSignedNoise(uniqueVal, pos.z * 0.06, relTimef*0.2) * 0.9 + 0.08;
                frc.y = fakeWindX * ofSignedNoise(uniqueVal, pos.x * 0.006, relTimef*0.02) * 1.9 - 2.75;
            }
        }
        
        if( pos.z + vel.z < -3000 ){
            pos.z = 0;
            pos.y = fullHeight;
        }
        if( pos.y + globalPos.y + vel.y > fullHeight + 400 ){
            pos.y = 0;
        }
        if( pos.y + globalPos.y + vel.y < -300 ){
            pos.y = fullHeight;
        }
        if( pos.x + vel.x + globalPos.x > fullWidth  ){
            pos.x -= fullWidth;
        }
        
        
        drag  = ofRandom(0.40, 0.99);
        vel *= drag * 1.19;
        vel += frc * 0.5 * (1.0 - friction);
        
        //2 - UPDATE OUR POSITION
        pos += vel;
        
    
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
  
        //ofRotateZ(rotation);
        particleTexture.draw(pos.x + globalPos.x, pos.y + globalPos.y, pos.z + globalPos.z, particleSizeX, particleSizeY);


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
void customParticle::addBlinky(float blinkyness, float staticVal){
    
    ofColor color = customColor;
    
    color.a = abs(ofNoise(ofGetElapsedTimef() * 0.2 + uniqueVal) * blinkyness) + staticVal  ;
    
 //   color.a -= ofMap(pos.z, 0, 255, 0, 2000);
    
    customColor = color;

}

//------------------------------------------------------------------
void customParticle::setParticleImg(ofImage &thisImage){
    

    
    
    particleTexture = thisImage;
    
    ofSetColor(255);
}


