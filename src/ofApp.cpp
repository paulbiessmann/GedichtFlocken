#include "ofApp.h"

float fps = 25;

/*** Wird rückwärts abgespielt, szenen von 4 -> 1 am Ende ***/
/*     immer < scene. 0 < Szene 1 < scene1; scene1 < Szene 2 < scene2; usw     */
//
//float scene0 = 0;    // explosion reverse - Flocken werden zu Gedichten
//float scene1 = 2600; // so lange steht das Gedicht da
//float scene2 = 4800; // Tex Vec Effekt
//float scene3 = 6600; // zusätzlich einzelne Textflocken
//float scene4 = 8400;

// Reverse gerechnet:
float scene0 = 0;    // Black
float scene1 = 1800; // zusätzlich einzelne Textflocken
float scene2 = 3600; // Tex Vec Effekt
float scene3 = 5800; // so lange steht das Gedicht da
float scene4 = 8400; // explosion reverse - Flocken werden zu Gedichten


float vidPart         = 500; //frames in one vid
float writeWaitTime   = 200; //seconds waiting between recordings to reduce memory
//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetBackgroundColor(255);
    ofEnableAlphaBlending();
    mode = MODE_EXPLODE;
    
    recordedFrame = 0;
    
/**** Setup post-processing chain ****/
    post.init(ofGetWidth(), ofGetHeight());
    post.createPass<DofPass>()->setEnabled(false);
 //   post.createPass<DofPass>()->setFocus(0.99);
//    post.createPass<ConvolutionPass>()->setEnabled(true);
    post.createPass<VerticalTiltShifPass>()->setEnabled(false);
    post.createPass<GodRaysPass>()->setEnabled(false);
    post.createPass<RGBShiftPass>()->setEnabled(false);
    post.createPass<ZoomBlurPass>()->setEnabled(false); //cool
    post.createPass<ContrastPass>()->setEnabled(false);
    
/**** Setup light ****/
    light.setPosition(1000, 1000, 2000);
    
/************ Video Record *********/
    ofSetFrameRate((int) fps);
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    fileName = "Flocken";
    fileExt = ".mov"; // ffmpeg uses the extension to determine the container type. run 'ffmpeg -formats' to see supported formats
    
    // override the default codecs if you like
    // run 'ffmpeg -codecs' to find out what your implementation supports (or -formats on some older versions)
    vidRecorder.setVideoCodec("prores");
    vidRecorder.setVideoBitrate("2000k");
    
    ofAddListener(vidRecorder.outputFileCompleteEvent, this, &ofApp::recordingComplete);
    
    recordFbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGB);
    recordFboFlip.allocate(ofGetWidth(), ofGetHeight(), GL_RGB);
    
    bRecording = false;
    
    keyReleased('r');
    bEnd = false;

/** end recording **/
    

    kuppelGrid.load("Tiefsee_UV_GRID_1.tif");
    
    vPosVerse.resize(6);
    vNum.resize(6);
    versesImg.resize(6);
    versesImg[0].load("Strophen/1.png");
    versesImg[1].load("Strophen/2.png");
    versesImg[2].load("Strophen/3.png");
    versesImg[3].load("Strophen/4.png");
    versesImg[4].load("Strophen/5.png");
    versesImg[5].load("Strophen/6.png");
        
    float imgScaleFac = 4;
    particleResolution = 7;

/**** Explode Particles ****/
    for (int i = 0; i<versesImg.size(); i++){
        int oldWidth   = versesImg[i].getWidth();
        int oldHeight  = versesImg[i].getHeight();
        
        versesImg[i].resize(oldWidth / imgScaleFac, oldHeight / imgScaleFac);
        vNum[i] = (versesImg[i].getWidth() / particleResolution) * (versesImg[i].getHeight()/particleResolution);
        
        vPosVerse[i] = ofVec3f((ofGetWidth()/8)*(i+1), ofRandom((ofGetHeight()/2) - 300, (ofGetHeight()/2) + 10), 0);
        
    }
    p1.assign(vNum[0], customParticle());
    p2.assign(vNum[1], customParticle());
    p3.assign(vNum[2], customParticle());
    p4.assign(vNum[3], customParticle());
    p5.assign(vNum[4], customParticle());
    p6.assign(vNum[5], customParticle());
    
    
 /**** Schnipsel ****/
    float fullTexScaleFac       = 25; // -> origSize/scaleFac
    numFullTexParticles         = 200; // so viele Partikel davon
    
    int numSchnipsel            = 30;  // soviel Bilder sind im Ordner
    p.assign(numFullTexParticles, customParticle());
    schnipselImgs.resize(numSchnipsel);
    
    for (int i=0; i<numSchnipsel; i++){
        schnipselImgs[i].load("Schnipsel/s" + ofToString(i+1) + ".png");
        
        int oldWidth   = schnipselImgs[i].getWidth();
        int oldHeight  = schnipselImgs[i].getHeight();
        schnipselImgs[i].resize(oldWidth/fullTexScaleFac, oldHeight/fullTexScaleFac);

    }
    
/**** Snowflakes ****/
    int numSnowflakes = 100;
    pSnowFlakes.resize(numSnowflakes);
    snowFlake.load("snow.png");
    snowFlake.resize(10,10);
    
    resetParticles();
    resetSchnipsel();

    
/**** Init Particles *****/
    
        //resetParticles();
        initParticles(p1, versesImg[0]);
        initParticles(p2, versesImg[1]);
        initParticles(p3, versesImg[2]);
        initParticles(p4, versesImg[3]);
        initParticles(p5, versesImg[4]);
        initParticles(p6, versesImg[5]);

    bInitSchnipsel = false;
    if(!bInitSchnipsel){
        initFullTexParticles(p, schnipselImgs);
        bInitSchnipsel = true;
    }
    initSnowFlakes(pSnowFlakes, snowFlake);
    

/** Tex Vec **/
    texVecGetter.allocate(ofGetWidth(), ofGetHeight(), GL_RGB);
    texVecNum = 1500;
    initTexVecs();
    
    
}
//--------------------------------------------------------------
void ofApp::initTexVecs(){
    center = ofVec2f(ofGetWidth()/2, ofGetHeight()/2);
    
    growing = 5;
    
    //texVecNum = 200;
    texVecSize = 50;
    //texVecs.resize(texVecNum);
    
    colTexVecs.resize(texVecNum);
    
    texVecsPosX.resize(texVecNum);
    texVecsPosY.resize(texVecNum);
    texVecsPosZ.resize(texVecNum);
    
    texVecPosDraw.resize(texVecNum);
    
    
    for(int i = 0; i < texVecNum; i++) {
        //texVecs[i].allocate(texVecSize,texVecSize, GL_RGB);
        texVecsPosX[i]   = ofRandom((int) fullWidth/4, 3*fullWidth/4 );
        texVecsPosY[i]   = ofRandom((int) (fullHeight/2) - 200, (fullHeight/2) +200);
        texVecsPosZ[i]   = ofRandom((int) -1000, 1000);
        colTexVecs[i]    = ofColor(ofRandom(150, 155), ofRandom(150,155), ofRandom(150,155), ofRandom(200));
        
        texVecPosDraw[i].x = ofRandom(0, fullWidth) ;
        texVecPosDraw[i].y = ofRandom(0, fullHeight) ;
        texVecPosDraw[i].z = ofRandom( - 500, 500);
    }
    
}
//--------------------------------------------------------------
void ofApp::initSnowFlakes(vector <customParticle> &pThis, ofImage &imgThis){

    int numFlakes = pThis.size();
    int imgWidth = imgThis.getWidth();
    int imgHeight = imgThis.getHeight();
    
    for(unsigned int i=0; i < pThis.size(); i++){

        pThis[i].setParticleImg(imgThis);
        pThis[i].setParticleSize(imgWidth, imgHeight);
        pThis[i].setDrawMode(PARTICLE_MODE_TEXTURES);
        pThis[i].setParticleMode(PARTICLE_MODE_SNOW);
    }

}
//--------------------------------------------------------------
void ofApp::initFullTexParticles(vector <customParticle> &pThis, vector <ofImage> &imgThis){
    
    int numImages = imgThis.size();
    int particleSizeMax = 100; // Pixelsize of Particles
    
   // if(mode == MODE_SNOW){
        for(unsigned int i=0; i < pThis.size(); i++){
            int randNum = ofRandom(numImages);
            int imgWidth = imgThis[randNum].getWidth();
            int imgHeight = imgThis[randNum].getHeight();
            
            imgThis[randNum].rotate90(ofRandom(360));
            
            pThis[i].setParticleImg(imgThis[randNum]);
            pThis[i].setParticleSize(imgWidth, imgHeight);
            pThis[i].setDrawMode(PARTICLE_MODE_TEXTURES);
            pThis[i].setParticleMode(PARTICLE_MODE_SNOW);
        }
   // }

}
//--------------------------------------------------------------
void ofApp::initParticles(vector <customParticle> &pThis, ofImage &imgThis){
    int textWidth   = imgThis.getWidth();
    int textHeight  = imgThis.getHeight();
    ofPixels pixels = imgThis.getPixels();
    int pCount = 0;
    
    
    
    //points
    if(pThis[0].drawMode == PARTICLE_MODE_POINTS){
        for(unsigned int i = 0; i < textWidth - particleResolution; i += particleResolution){
            for(unsigned int j = 0; j < textHeight - particleResolution; j += particleResolution){
    
                pThis[pCount].setStartingTime(ofGetElapsedTimef(), recordedFrame);
                
                ofColor pxColor = pixels.getColor(i, j);

//                pThis[pCount].setGlobalPos();

                
                pThis[pCount].setColor(pxColor);
                pThis[pCount].setPos(ofVec3f(i, j, ofRandom(-100,100)));
                pThis[pCount].setParticleSize(particleResolution);
                pCount++;
    
            }
        }
    }
    else if(pThis[0].drawMode == PARTICLE_MODE_TEXTURES){ //textures
        for(unsigned int i = 0; i <= textWidth-particleResolution ; i += particleResolution){
            for(unsigned int j = 0; j <= textHeight-particleResolution; j += particleResolution){
                
                pThis[pCount].setStartingTime(ofGetElapsedTimef() + 2.0, recordedFrame);

                ofColor pxColor = pixels.getColor(i, j);
                ofImage pxImage = imgThis;
                pxImage.crop(i,j,particleResolution,particleResolution);

                //pThis[pCount].setColor(ofColor(220,220,220,200));
                pThis[pCount].setParticleImg(pxImage);
                pThis[pCount].setPos(ofVec3f(i, j, 0));//ofRandom(-100,100)));
                pThis[pCount].setParticleSize(particleResolution);
                pCount++;
            }
        }
        
    }

}
//--------------------------------------------------------------
void ofApp::resetParticles(){
  
    for(unsigned int i = 0; i < p1.size(); i++){
        p1[i].reset();
        p1[i].setGlobalPos(vPosVerse[0]);
        p1[i].setDrawMode(PARTICLE_MODE_TEXTURES);
        p1[i].pMode = PARTICLE_MODE_EXPLODE;
    }
    for(unsigned int i = 0; i < p2.size(); i++){
        p2[i].reset();
        p2[i].setGlobalPos(vPosVerse[1]);
        p2[i].setDrawMode(PARTICLE_MODE_TEXTURES);
        p2[i].pMode = PARTICLE_MODE_EXPLODE;
    }
    for(unsigned int i = 0; i < p3.size(); i++){
        p3[i].reset();
        p3[i].setGlobalPos(vPosVerse[2]);
        p3[i].setDrawMode(PARTICLE_MODE_TEXTURES);
        p3[i].pMode = PARTICLE_MODE_EXPLODE;
    }
    for(unsigned int i = 0; i < p4.size(); i++){
        p4[i].reset();
        p4[i].setGlobalPos(vPosVerse[3]);
        p4[i].setDrawMode(PARTICLE_MODE_TEXTURES);
        p4[i].pMode = PARTICLE_MODE_EXPLODE;
    }
    for(unsigned int i = 0; i < p5.size(); i++){
        p5[i].reset();
        p5[i].setGlobalPos(vPosVerse[4]);
        p5[i].setDrawMode(PARTICLE_MODE_TEXTURES);
        p5[i].pMode = PARTICLE_MODE_EXPLODE;
    }
    for(unsigned int i = 0; i < p6.size(); i++){
        p6[i].reset();
        p6[i].setGlobalPos(vPosVerse[5]);
        p6[i].setDrawMode(PARTICLE_MODE_TEXTURES);
        p6[i].pMode = PARTICLE_MODE_EXPLODE;
    }
    
    
}

//--------------------------------------------------------------
void ofApp::resetSchnipsel(){
    
    /**** Schnipsel ****/
    for(unsigned int i = 0; i < p.size(); i++){
        p[i].reset();
        p[i].setGlobalPos(ofVec3f(0,0,0));
        p[i].setDrawMode(PARTICLE_MODE_POINTS);
    }
    /**** Snow Flakes ****/
    for(unsigned int i = 0; i < pSnowFlakes.size(); i++){
        pSnowFlakes[i].reset();
        pSnowFlakes[i].setGlobalPos(ofVec3f(0,0,0));
        pSnowFlakes[i].setDrawMode(PARTICLE_MODE_POINTS);
        pSnowFlakes[i].setColor(ofColor(ofRandom(100,250)));
    }
    
    
    
}



//--------------------------------------------------------------
void ofApp::update(){
    
    if(!bPause){
        recordedFrame = vidRecorder.getNumVideoFramesRecorded() - vidRecorder.getVideoQueueSize();
    }

    std::stringstream strm;
    strm << "fps: " << ofGetFrameRate();
    ofSetWindowTitle(strm.str());
	

//    if(ofGetFrameNum() > scene1){
        bUpdateSnow = true;
//    }
    bUpdateExplode = true;
    
    
//    if(mode == MODE_EXPLODE){
  if(recordedFrame > scene3 && !bPause){
        for(unsigned int i = 0; i < p1.size(); i++){
            p1[i].update();
            p1[i].addBlinky(150);
        }
        for(unsigned int i = 0; i < p2.size(); i++){
            p2[i].update();
            p2[i].addBlinky(150);
        }
        for(unsigned int i = 0; i < p3.size(); i++){
            p3[i].update();
            p3[i].addBlinky(150);
        }
        for(unsigned int i = 0; i < p4.size(); i++){
            p4[i].update();
            p4[i].addBlinky(150);
        }
        for(unsigned int i = 0; i < p5.size(); i++){
            p5[i].update();
            p5[i].addBlinky(150);
        }
        for(unsigned int i = 0; i < p6.size(); i++){
            p6[i].update();
            p6[i].addBlinky(150);
        }
    }
    if(recordedFrame > scene2 && !bPause){
        int pSize ;
        pSize = recordedFrame / 1;
        if(pSize >= p.size()) {pSize = p.size();}
        for(unsigned int i = 0; i < pSize; i++){
            p[i].update();
        }
        
        int pSnowSize ;
        pSnowSize = recordedFrame*2;
        if(pSnowSize >= pSnowFlakes.size()) {pSnowSize = pSnowFlakes.size();}
        for(unsigned int i = 0; i < pSnowFlakes.size(); i++){
            pSnowFlakes[i].addBlinky(200);
        }
        for(unsigned int i = 0; i < pSnowSize; i++){
            pSnowFlakes[i].update();
        }
    }
    
    recordFbo.getTexture().readToPixels(recordPixels);
    if(bRecording){
        bool success = vidRecorder.addFrame(recordPixels);
       
        if (!success) {
            ofLogWarning("This frame was not added!");
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){

    if(0){
        cout << "recFrame " << recordedFrame << "\n";
    }

    // copy enable part of gl state
   // glPushAttrib(GL_ENABLE_BIT);
    ofEnableAlphaBlending();
   // ofEnableDepthTest();
//    // setup gl state
    
    //glEnable(GL_CULL_FACE);
  //  light.enable();
//    ofEnableDepthTest();

   // cam.setPosition(0,0,0);

    recordFbo.begin();
    ofClear(255,255,255,250);
    
    // begin scene to post process
//    post.begin(cam);
//    ofTranslate(-fullWidth/2, -fullHeight/2, 0);
//    cam.setScale(1, -1, 1);
//    ofRotateZ(180);

//    ofSetColor(255);
//    ofDrawRectangle(0,0,fullWidth, fullHeight);
    
    
    float t = ofGetElapsedTimef();
   
    if(0){
        cout << "frame " << ofGetFrameNum() << "\n";
    }
    if(0){
        cout << "sec " << ofGetElapsedTimef() << "\n";
    }
    
    
       

   
        
/** Scene 1  -  Voll mit Tex Vex, einzelne Flocken schneien runter (weiß auf Schwarz, als Fonts)**/
        if(recordedFrame > scene0 && recordedFrame <= scene1 && !bPause){
            
            texVecGetter.begin();
            ofClear(255,255,255,255);

            /*** schnipsel ***/
            for(unsigned int i = 0; i < p.size()-1; i++){ p[i].draw(); }
            for(unsigned int i = 0; i < pSnowFlakes.size()-1; i++){ pSnowFlakes[i].draw(); }
            
            /** Strophen global move **/
            for(int i=0;i< versesImg.size();i++){
                vPosVerse[i] += ofSignedNoise(ofSignedNoise(ofGetElapsedTimef() * 0.01, i * 10), ofSignedNoise(ofGetElapsedTimef() * 0.01, (i+2) * 10));

                vPosVerse[i].x += ofGetElapsedTimef() * 0.01 + sin(ofGetElapsedTimef() * 0.001 * i ) * 3;
                vPosVerse[i].y += cos(ofGetElapsedTimef() * 0.006 * i ) * 0.02;

                if(vPosVerse[i].x > fullWidth + versesImg[i].getWidth()){
                    vPosVerse[i].x = -versesImg[i].getWidth();
                }


                ofSetColor(255);
                glPushMatrix();
                ofTranslate(-vPosVerse[i].x, -vPosVerse[i].y);
                ofRotateZ(sin(ofGetElapsedTimef() * 0.001)*0.5);
                ofRotateX(sin(ofGetElapsedTimef() * 0.002)*0.3);
                versesImg[i].draw(vPosVerse[i].x *2 , vPosVerse[i].y *2);
                glPopMatrix();
            }
            texVecGetter.end();
            texVecGetter.draw(0,0);
            
            /** Tex Vecs **/
            growing++;
            int texVecNumNew = ofMap(recordedFrame, scene0, scene2, texVecNum, 0 );
            
            for (int i=0; i < texVecNumNew; i++){
               // dirX =  (texVecsPosX[i] - center.x) * growing * 0.0001;
               // dirY =  (texVecsPosY[i] - center.y) * growing * 0.0001;
                float fakeWindX = ofSignedNoise(texVecPosDraw[i].x * 0.03, texVecPosDraw[i].y * 0.06, ofGetElapsedTimef() * 0.6);
                dirX = ofSignedNoise(texVecPosDraw[i].y, texVecPosDraw[i].z * 0.06, ofGetElapsedTimef()*0.2) * 0.59 + 0.58;
                dirY = ofSignedNoise(texVecPosDraw[i].y, texVecPosDraw[i].z * 0.02, ofGetElapsedTimef()*0.2) * 0.59 - 0.78;
                
                
                if (texVecPosDraw[i].x + dirX > fullWidth ){
                    dirX = -dirX;
                    texVecPosDraw[i].x = 0;
                }
                if (texVecPosDraw[i].x + dirX < 0 ){
                    dirX = -dirX;
                    texVecPosDraw[i].x = fullWidth;
                }
                
                if (texVecPosDraw[i].y + dirY > fullHeight){
                    dirY = -dirY;
                    texVecPosDraw[i].y= 0;
                }
                if (texVecPosDraw[i].y + dirY < 0){
                    dirY = -dirY;
                    texVecPosDraw[i].y = fullHeight;
                }
                if (texVecPosDraw[i].z < - 1000){
                    texVecPosDraw[i].z = 1000;
                }
                
                float drag = ofRandom(0.8, 0.99);
                dirX *= drag;
                dirY *= drag;
                texVecPosDraw[i].x += dirX + fakeWindX;
                texVecPosDraw[i].y += dirY + fakeWindX;
                texVecPosDraw[i].z -= ofSignedNoise(ofGetElapsedTimef()*0.2) * 1.5 + 2.5;
                
                ofSetColor(colTexVecs[i]);
               
                texVecGetter.getTexture().drawSubsection(texVecPosDraw[i].x, texVecPosDraw[i].y, texVecPosDraw[i].z, texVecSize, texVecSize, texVecsPosX[i], texVecsPosY[i], texVecSize, texVecSize);
                
                ofSetColor(255);
                
            }
            
        }
/** Scene 2   -  Tex Vex Effect, von dicht/dunkel nach wenig dicht/hell**/
        if(recordedFrame > scene1 && recordedFrame <= scene2 && !bPause){
            texVecGetter.begin();
            ofClear(255,255,255,255);
            
            /*** schnipsel ***/
            for(unsigned int i = 0; i < p.size()-1; i++){ p[i].draw(); }
            for(unsigned int i = 0; i < pSnowFlakes.size()-1; i++){ pSnowFlakes[i].draw(); }
            
            
            /** Strophen global move **/
            for(int i=0;i< versesImg.size();i++){
                vPosVerse[i] += ofSignedNoise(ofSignedNoise(ofGetElapsedTimef() * 0.01, i * 10), ofSignedNoise(ofGetElapsedTimef() * 0.01, (i+2) * 10));
                
                vPosVerse[i].x += ofGetElapsedTimef() * 0.01 + sin(ofGetElapsedTimef() * 0.001 * i ) * 3;
                vPosVerse[i].y += cos(ofGetElapsedTimef() * 0.006 * i ) * 0.02;
                
                if(vPosVerse[i].x > fullWidth + versesImg[i].getWidth()){
                    vPosVerse[i].x = -versesImg[i].getWidth();
                }
                
                
                ofSetColor(255);
                glPushMatrix();
                ofTranslate(-vPosVerse[i].x, -vPosVerse[i].y);
                ofRotateZ(sin(ofGetElapsedTimef() * 0.001)*0.5);
                ofRotateX(sin(ofGetElapsedTimef() * 0.002)*0.3);
                versesImg[i].draw(vPosVerse[i].x *2 , vPosVerse[i].y *2);
                glPopMatrix();
            }
            texVecGetter.end();
            texVecGetter.draw(0,0);
            
            /** Tex Vecs **/
            growing++;
            int texVecNumNew = ofMap((float) recordedFrame, scene0, scene2, (float) texVecNum, 0 );
            
            for (int i=0; i < texVecNumNew; i++){
                // dirX =  (texVecsPosX[i] - center.x) * growing * 0.0001;
                // dirY =  (texVecsPosY[i] - center.y) * growing * 0.0001;
                float fakeWindX = ofSignedNoise(texVecPosDraw[i].x * 0.03, texVecPosDraw[i].y * 0.06, ofGetElapsedTimef() * 0.6);
                dirX = ofSignedNoise(texVecPosDraw[i].y, texVecPosDraw[i].z * 0.06, ofGetElapsedTimef()*0.2) * 0.59 + 0.58;
                dirY = ofSignedNoise(texVecPosDraw[i].y, texVecPosDraw[i].z * 0.02, ofGetElapsedTimef()*0.2) * 0.59 - 0.78;
                
                
                if (texVecPosDraw[i].x + dirX > fullWidth ){
                    dirX = -dirX;
                    texVecPosDraw[i].x = 0;
                }
                if (texVecPosDraw[i].x + dirX < 0 ){
                    dirX = -dirX;
                    texVecPosDraw[i].x = fullWidth;
                }
                
                if (texVecPosDraw[i].y + dirY > fullHeight){
                    dirY = -dirY;
                    texVecPosDraw[i].y= 0;
                }
                if (texVecPosDraw[i].y + dirY < 0){
                    dirY = -dirY;
                    texVecPosDraw[i].y = fullHeight;
                }
                if (texVecPosDraw[i].z < - 1000){
                    texVecPosDraw[i].z = 1000;
                }

                float drag = ofRandom(0.8, 0.99);
                dirX *= drag;
                dirY *= drag;
                
                texVecPosDraw[i].x += dirX + fakeWindX;
                texVecPosDraw[i].y += dirY + fakeWindX;
                texVecPosDraw[i].z -= 0.8;
                
                ofSetColor(colTexVecs[i]);
                
                texVecGetter.getTexture().drawSubsection(texVecPosDraw[i].x, texVecPosDraw[i].y, texVecPosDraw[i].z, texVecSize, texVecSize, texVecsPosX[i], texVecsPosY[i], texVecSize, texVecSize);
                
                ofSetColor(255);
                
            }
            
          
        }
        
/** Scene 3   -  Strophen bewegen sich, leichte Vis Effects**/
        if(recordedFrame > scene2 && recordedFrame <= scene3 && !bPause){
            /** Strophen global move **/
            for(int i=0;i< versesImg.size();i++){
                vPosVerse[i] += ofSignedNoise(ofSignedNoise(ofGetElapsedTimef() * 0.01, i * 10), ofSignedNoise(ofGetElapsedTimef() * 0.01, (i+2) * 10));
                
                vPosVerse[i].x += ofGetElapsedTimef() * 0.01 + sin(ofGetElapsedTimef() * 0.001 * i ) * 3;
                vPosVerse[i].y += cos(ofGetElapsedTimef() * 0.006 * i ) * 0.02;

                if(vPosVerse[i].x > fullWidth + versesImg[i].getWidth()){
                    vPosVerse[i].x = -versesImg[i].getWidth();
                }
                
                
                ofSetColor(255);
                glPushMatrix();
                    ofTranslate(-vPosVerse[i].x, -vPosVerse[i].y);
                    ofRotateZ(sin(ofGetElapsedTimef() * 0.001)*0.5);
                    ofRotateX(sin(ofGetElapsedTimef() * 0.002)*0.3);
                    versesImg[i].draw(vPosVerse[i].x *2 , vPosVerse[i].y *2);
                glPopMatrix();
            }
           
            /*** schnipsel ***/
            for(unsigned int i = 0; i < p.size()-1; i++){ p[i].draw(); }
            for(unsigned int i = 0; i < pSnowFlakes.size()-1; i++){ pSnowFlakes[i].draw(); }

        
        }
        

/** Scene 4   -  Strophen Explodieren, fliegen weg als Schnee   **/
        if(recordedFrame > scene3 && recordedFrame <= scene4 && !bPause){
            
            /*** schnipsel ***/
            for(unsigned int i = 0; i < p.size()-1; i++){
                p[i].draw();
            }
            //  for(unsigned int i = 0; i < pSnowFlakes.size()-1; i++){ pSnowFlakes[i].draw(); }
            
            if(!bInit){
                resetParticles();
                initParticles(p1, versesImg[0]);
                initParticles(p2, versesImg[1]);
                initParticles(p3, versesImg[2]);
                initParticles(p4, versesImg[3]);
                initParticles(p5, versesImg[4]);
                initParticles(p6, versesImg[5]);
                bInit = true;
            }
            for(unsigned int i = 0; i < p1.size()-particleResolution; i++){ p1[i].draw(); }
            for(unsigned int i = 0; i < p2.size()-particleResolution; i++){ p2[i].draw(); }
            for(unsigned int i = 0; i < p3.size()-particleResolution; i++){ p3[i].draw(); }
            for(unsigned int i = 0; i < p4.size()-particleResolution; i++){ p4[i].draw(); }
            for(unsigned int i = 0; i < p5.size()-particleResolution; i++){ p5[i].draw(); }
            for(unsigned int i = 0; i < p6.size()-particleResolution; i++){ p6[i].draw(); }
        }
    
/** END Recording **/
        if(recordedFrame > scene4 && !bEnd ){
            keyReleased('r');
            bEnd = true;
        }

    
    
    
    // end scene and draw
//    post.end();

    // set gl state back to original
   // glPopAttrib();
    
    recordFbo.end();
    recordFbo.draw(0,0);
    
//    ofSetColor(255,255,255,10);
//    kuppelGrid.draw(0,0, fullWidth, fullHeight);

    if((int)recordedFrame % (int)vidPart == 0 && !bPause && recordedFrame > 1){
        bPause = true;
        waitCounter = ofGetElapsedTimef();
        keyReleased('r');
    }
    
    if(ofGetElapsedTimef() - waitCounter > writeWaitTime && bPause){
        bPause = false;
        keyReleased('r');
    }
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
//    unsigned idx = key - '0';
//    if (idx < post.size()) post[idx]->setEnabled(!post[idx]->getEnabled());
//
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

    
    if(key=='r'){
        bRecording = !bRecording;
        if(bRecording && !vidRecorder.isInitialized()) {
            //insert your path in the following row and your settings 1920x1080 60fps - the last 2 parms are zero because in my case I don't want to record audio..
            ofFilePath f;
            vidRecorder.setup( fileName+"_"+ofGetTimestampString()+".mov", recordFbo.getWidth(),recordFbo.getHeight(), (int) fps, 0, 0);
            
            //Start recording
            vidRecorder.start();
        }
        else if(!bRecording && vidRecorder.isInitialized()) {
            vidRecorder.setPaused(true);
        }
        else if(bRecording && vidRecorder.isInitialized()) {
            vidRecorder.setPaused(false);
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

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
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void ofApp::exit(){
    ofRemoveListener(vidRecorder.outputFileCompleteEvent, this, &ofApp::recordingComplete);
    vidRecorder.close();
    
    
}

//--------------------------------------------------------------
void ofApp::recordingComplete(ofxVideoRecorderOutputFileCompleteEventArgs& args){
    cout << "The recoded video file is now complete." << endl;
}
