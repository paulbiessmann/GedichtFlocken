// TODO: kleinere Zahl gP (104 ist zu groß). bei gPUpdateSize checken. recordedFrame checken.
//          TexVec um diese Linie kümmern


#include "ofApp.h"

float fps = 25;

// struct that contains the Verses, that will explode to particles
int numGP = 186+34;
groupParticles gP[186+34];

/*** Wird rückwärts abgespielt, szenen von 4 -> 1 am Ende ***/
/*     immer < scene. 0 < Szene 1 < scene1; scene1 < Szene 2 < scene2; usw     */

// Reverse gerechnet:
float scene0 = 0;    // Black
float scene1 = 2400;//2400;//2400; // zusätzlich einzelne Textflocken
float scene2 = 4200;//4200;//4200; // Tex Vec Effekt
float scene3 = 4900;//5000;//6000; // so lange steht das Gedicht da
float scene4 = 9000;//9000;//8400; // explosion reverse - Flocken werden zu Gedichten - mit Fleisch


float vidPart         = 500; //frames in one vid till pause
float writeWaitTime   = 150; //seconds waiting between recordings to reduce memory
//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetBackgroundColor(255);
    ofEnableAlphaBlending();
    mode = MODE_EXPLODE;
    
    recordedFrame = 0;
    
    
/**** Setup light ****/
    light.setPosition(1900, 1000, 2000);
    light2.setPosition(2000, -1000, -2000);
    light2.lookAt(ofVec3f(fullWidth/2, fullHeight/2, 0));
    
/************ Video Record *********/
    ofSetFrameRate((int) fps);
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    fileName = "Flocken_v3-5";
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
    
    
    // soviele Bilder sind in den Ordnern:
    int numVerses       = 6;
    int numSchnipsel    = 30;
    int numVersN        = 34;
    int numVersNadd     = 34;
    
    int numGPart = numGP;//numSchnipsel + numVersN + numVerses;
    vector <string> fileP;
    fileP.resize(numGP);
    for(int i=0; i<numSchnipsel; i++){  fileP[i] = "Schnipsel/s" + ofToString(i+1) + ".png"; }
    for(int i=0; i<numVerses; i++){     fileP[i+numSchnipsel] = "Strophen/" + ofToString(i+1) + ".png"; }
    for(int i=0; i<numVerses; i++){     fileP[i+numVerses+numSchnipsel] = "Strophen/" + ofToString(i+1) + ".png"; }
    for(int i=0; i<numVerses; i++){     fileP[i+2*numVerses+numSchnipsel] = "Strophen/" + ofToString(i+1) + ".png"; }

    for(int i=0; i<numVersN; i++){      fileP[i+3*(numVerses)+numSchnipsel] = "StrophenSchnitt/n" + ofToString(i+1) + ".png"; }

    for(int i=0; i<numVersN; i++){      fileP[i+3*(numVerses)+numSchnipsel+numVersN] = "StrophenSchnitt/n" + ofToString(i+1) + ".png"; }
    for(int i=0; i<numVersNadd; i++){   fileP[i+3*(numVerses)+numSchnipsel+2*numVersN] = "StrophenSchnitt/n" + ofToString(i+1) + ".png"; }
    for(int i=0; i<numSchnipsel; i++){  fileP[i+3*(numVerses)+numSchnipsel+2*numVersN+numVersNadd] = "Schnipsel/s" + ofToString(i+1) + ".png"; }
    for(int i=0; i<numSchnipsel; i++){  fileP[i+3*(numVerses)+numSchnipsel+2*numVersN+2*numVersNadd] = "Schnipsel/s" + ofToString(i+1) + ".png"; }

    for(int i=0; i<numVerses; i++){     fileP[i+3*numVerses+numSchnipsel+2*numVersN+2*numVersNadd+numSchnipsel] = "Strophen/" + ofToString(i+1) + ".png"; }
    

    bFirstCallScene3 = true;
    
    float imgScaleFac = 4; //4
    particleResolution = 10; //8

/**** Explode Particles ****/
    for (int i=0; i< numGP; i++){
        gP[i].versesImg.load( fileP[i] );
        int oldWidth   =  gP[i].versesImg.getWidth();
        int oldHeight  =  gP[i].versesImg.getHeight();
        
        if(i>numSchnipsel+numVersN+numVersN){
            gP[i].versesImg.resize(oldWidth / 3, oldHeight / 3);
            gP[i].vPosVerse = ofVec3f(ofRandom(fullWidth-500), ofRandom(fullHeight- 500), ofRandom(5,10));
        }
        else{
            gP[i].versesImg.resize(oldWidth / imgScaleFac, oldHeight / imgScaleFac);
            gP[i].vPosVerse = ofVec3f(ofRandom(fullWidth-300), ofRandom(fullHeight- 300), ofRandom(-50,5));
        }
        
        gP[i].vNum = ( gP[i].versesImg.getWidth() / particleResolution) * ( gP[i].versesImg.getHeight()/particleResolution);
        
        gP[i].p.assign( gP[i].vNum, customParticle() );
        gP[i].bInit     = false;
    }
    
    
 /**** Schnipsel ****/
    float fullTexScaleFac       = 25; // -> origSize/scaleFac
    numFullTexParticles         = 2; // so viele Partikel davon
    
    p.assign(numFullTexParticles, customParticle());
    schnipselImgs.resize(numSchnipsel);
    
    for (int i=0; i<numSchnipsel; i++){
        schnipselImgs[i].load("Schnipsel/s" + ofToString(i+1) + ".png");
        
        int oldWidth   = schnipselImgs[i].getWidth();
        int oldHeight  = schnipselImgs[i].getHeight();
        schnipselImgs[i].resize(oldWidth/fullTexScaleFac, oldHeight/fullTexScaleFac);

    }

    
    
/**** Snowflakes ****/
    int numSnowflakes = 10000;
    pSnowFlakes.resize(numSnowflakes);
    snowFlake.load("snow.png");
    snowFlake.resize(10,10);
    
    resetParticles();
    resetSchnipsel();
    
    relativeFr = 0;
    
/**** Init Particles *****/

    bInitSchnipsel = false;
    if(!bInitSchnipsel){
        initFullTexParticles(p, schnipselImgs);
        bInitSchnipsel = true;
    }
    initSnowFlakes(pSnowFlakes, snowFlake);
    

/** Tex Vec **/
    texVecGetter.allocate(ofGetWidth(), ofGetHeight(), GL_RGB);
    texVecNum = 10000;
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
        texVecsPosX[i]   = ofRandom((int) 0, fullWidth );
        texVecsPosY[i]   = ofRandom((int) 0, fullHeight);
        texVecsPosZ[i]   = ofRandom((int) -10, 1000);
        colTexVecs[i]    = ofColor(ofRandom(70, 75), ofRandom(70,75), ofRandom(70,75), ofRandom(170, 230));
        
        texVecPosDraw[i].x = ofRandom(0, fullWidth) ;
        texVecPosDraw[i].y = ofRandom(0, fullHeight) ;
        texVecPosDraw[i].z = ofRandom( - 1000, 1000);
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
        pThis[i].setDrawMode(PARTICLE_MODE_POINTS);
        pThis[i].setParticleMode(PARTICLE_MODE_LAYER);
        pThis[i].setParticleSize(13);
        
        pThis[i].setColor(ofColor(ofRandom(100,220), ofRandom(10,100)));
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

                pThis[pCount].setColor(ofColor(255,255,255,180));
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
  
    relativeFr = recordedFrame;
    for(int j=0; j<numGP; j++){
        for(unsigned int i = 0; i < gP[j].p.size(); i++){
            gP[j].bInit     = false;
            gP[j].vel       = ofVec3f(0, 0, 0);
            gP[j].uniqueVal = ofRandom(-10000, 10000);
            gP[j].p[i].reset();
            gP[j].p[i].setGlobalPos(gP[j].vPosVerse);
            gP[j].p[i].setDrawMode(PARTICLE_MODE_TEXTURES);
            gP[j].p[i].pMode = PARTICLE_MODE_EXPLODE;
            gP[j].relVerseAge = 0;
        }
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
        pSnowFlakes[i].setColor(ofColor(ofRandom(100,150), ofRandom(20, 150)));
    }
    
    
    
}



//--------------------------------------------------------------
void ofApp::update(){
    
    if(!bPause){
        recordedFrame = vidRecorder.getNumVideoFramesRecorded();// + vidRecorder.getVideoQueueSize();
    }

    std::stringstream strm;
    strm << "fps: " << ofGetFrameRate();
    ofSetWindowTitle(strm.str());
	
    
if(!bPause){
      if(recordedFrame > scene3 && !bPause){

          if(bFirstCallScene3){
              gPUpdateSize = 1;
              relativeFr = recordedFrame;
              bFirstCallScene3 = false;
          }else{
              gPUpdateSize = (recordedFrame - relativeFr) * 0.06; //0.07
          }
          if(gPUpdateSize >= numGP) {gPUpdateSize = numGP;}
          if(gPUpdateSize < 2) {gPUpdateSize = 1;}

          
          updateFullVerses();
          if(0 < numGP){
              for(int j=0; j<numGP; j++){
                  for(int i=0; i < gP[j].p.size(); i++){
                      gP[j].p[i].setGlobalPos(gP[j].vPosVerse);
                  }
              }
          }
          for(int j=0; j<gPUpdateSize; j++){
                int pUpSize;
                gP[j].relVerseAge++;
                pUpSize = (gP[j].relVerseAge) * 100;
                if(pUpSize >= gP[j].p.size()) {pUpSize = gP[j].p.size();}
                if(pUpSize < 0) {pUpSize = 0;}

                for(unsigned int i = 0; i < pUpSize; i++){
                    gP[j].p[i].update();
                    gP[j].p[i].addBlinky(15, 150);
                }
            }
          
        }
    
        if(recordedFrame > scene0 && !bPause){
            
            int pSnowSize ;
            pSnowSize = recordedFrame * 2;
            if(pSnowSize >= pSnowFlakes.size()) {pSnowSize = pSnowFlakes.size();}
            for(unsigned int i = 0; i < pSnowFlakes.size(); i++){
                pSnowFlakes[i].addBlinky(100, 80);
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
}

//--------------------------------------------------------------
void ofApp::draw(){

    if(0){
        cout << "recFrame " << recordedFrame << "\n";
    }

    ofEnableAlphaBlending();

    recordFbo.begin();
    ofClear(255,255,255,250);

    float t = ofGetElapsedTimef();
   
    if(0){
        cout << "frame " << ofGetFrameNum() << "\n";
    }
    if(0){
        cout << "sec " << ofGetElapsedTimef() << "\n";
    }
    
    if(recordedFrame == scene3 && !bPause){
        keyReleased('r');
        bPause = true;
    }
    
//    light.enable();
//    light2.enable();
  
    if(!bPause){
/** Scene 1  -  Voll mit Tex Vex, einzelne Flocken schneien runter (weiß auf Schwarz, als Fonts)**/
        if(recordedFrame > scene0 && recordedFrame <= scene1){
            
            texVecGetter.begin();
            ofClear(255,255,255,255);

            /*** schnipsel ***/
            for(unsigned int i = 0; i < pSnowFlakes.size()-1; i++){ pSnowFlakes[i].draw(); }
            
            /** Strophen global move **/
            updateFullVerses();
            drawFullVerses(0);
            
            texVecGetter.end();
            texVecGetter.draw(0,0);
            
            if ((int)recordedFrame % 30 == 0){
                iColorUpdate++;
                iAlphaUpdate--;
            }
            
            
            /** Tex Vecs **/
            growing++;
            int texVecNumNew = ofMap(recordedFrame, scene0, scene2, texVecNum, 0 );
            
            for (int i=0; i < texVecNumNew; i++){
               // dirX =  (texVecsPosX[i] - center.x) * growing * 0.0001;
               // dirY =  (texVecsPosY[i] - center.y) * growing * 0.0001;
                float fakeWindX = ofSignedNoise(texVecPosDraw[i].x * 0.03, texVecPosDraw[i].y * 0.06, recordedFrame * 0.02);
                dirX = ofSignedNoise(texVecPosDraw[i].y, texVecPosDraw[i].z * 0.06, recordedFrame*0.2) * 0.59 + 0.01;
                dirY = ofSignedNoise(texVecPosDraw[i].y, texVecPosDraw[i].z * 0.02, recordedFrame*0.2) * 0.59 - 0.78;
                dirZ = ofSignedNoise(recordedFrame * 0.02 * i) * 1.5 - 3.0;
                
                if (texVecPosDraw[i].x + dirX > fullWidth +100){
                    dirX = -dirX;
                    texVecPosDraw[i].x = -100;
                }
                if (texVecPosDraw[i].x + dirX < -100 ){
                    dirX = -dirX;
                    texVecPosDraw[i].x = fullWidth + 100;
                }
                
                if (texVecPosDraw[i].y + dirY > fullHeight + 0){
                    //dirY = -dirY;
                    texVecPosDraw[i].y= 0;
                }
                if (texVecPosDraw[i].y + dirY < 0){
                    //dirY = -dirY;
                    texVecPosDraw[i].y = fullHeight + 0;
                }
                if (texVecPosDraw[i].z < - 1000){
                    texVecPosDraw[i].z = 1000;
                }
                
                float texFill = recordedFrame * 1;
                if (recordedFrame > 100){
                    texFill = recordedFrame - 100;
                }
                else{
                    texFill = 0;
                }
                if(texVecPosDraw[i].y > texFill){
                    dirY = 0;
                    dirX = 0;
                    if (texVecPosDraw[i].z > -300){
                        dirZ = 0;
                    }
                }
                
                
                float drag = ofRandom(0.8, 0.99);
                dirX *= drag;
                dirY *= drag;
                texVecPosDraw[i].x += dirX;// + fakeWindX;
                texVecPosDraw[i].y += dirY;// + fakeWindX;
                texVecPosDraw[i].z += dirZ;
                
                
                if ((int)recordedFrame % 29 == 0){
                    colTexVecs[i].r++;
                    colTexVecs[i].g++;
                    colTexVecs[i].b++;
                    colTexVecs[i].a--;
                }
                
                if (colTexVecs[i].r > 240) { colTexVecs[i].r = 240;}
                if (colTexVecs[i].g > 240) { colTexVecs[i].g = 240;}
                if (colTexVecs[i].b > 240) { colTexVecs[i].b = 240;}
                if (colTexVecs[i].a < 50) { colTexVecs[i].a = 50;}
                
                ofSetColor(colTexVecs[i]);
               
                texVecGetter.getTexture().drawSubsection(texVecPosDraw[i].x, texVecPosDraw[i].y, texVecPosDraw[i].z, texVecSize, texVecSize, texVecsPosX[i], texVecsPosY[i], texVecSize, texVecSize);
                
                ofSetColor(255);
                
            }
            
        }
/** Scene 2   -  Tex Vex Effect, von dicht/dunkel nach wenig dicht/hell**/
        if(recordedFrame > scene1 && recordedFrame <= scene2){
            texVecGetter.begin();
            ofClear(255,255,255,255);
            
            /*** schnipsel ***/
            for(unsigned int i = 0; i < pSnowFlakes.size()-1; i++){ pSnowFlakes[i].draw(); }
            
            
            /** Strophen global move **/
            updateFullVerses();
            drawFullVerses(0);
            
            texVecGetter.end();
            texVecGetter.draw(0,0);
            
            /** Tex Vecs **/
            growing++;
            int texVecNumNew = ofMap((float) recordedFrame, scene0, scene2, (float) texVecNum, 0 );
            
            if ((int)recordedFrame % 25 == 0){
                iColorUpdate++;
                iAlphaUpdate--;
            }
            for (int i=0; i < texVecNumNew; i++){
                // dirX =  (texVecsPosX[i] - center.x) * growing * 0.0001;
                // dirY =  (texVecsPosY[i] - center.y) * growing * 0.0001;
                float fakeWindX = ofSignedNoise(texVecPosDraw[i].x * 0.03, texVecPosDraw[i].y * 0.06, ofGetElapsedTimef() * 0.6);
                dirX = ofSignedNoise(texVecPosDraw[i].y, texVecPosDraw[i].z * 0.06, recordedFrame*0.02) * 0.59 + 0.18;
                dirY = ofSignedNoise(texVecPosDraw[i].y, texVecPosDraw[i].z * 0.02, recordedFrame*0.02) * 0.59 - 0.78;
                
                
                if (texVecPosDraw[i].x + dirX > fullWidth + 100){
                    dirX = -dirX;
                    texVecPosDraw[i].x = -100;
                }
                if (texVecPosDraw[i].x + dirX < -100 ){
                    dirX = -dirX;
                    texVecPosDraw[i].x = fullWidth + 100;
                }
                
                if (texVecPosDraw[i].y + dirY > fullHeight + 100){
                    dirY = -dirY;
                    texVecPosDraw[i].y= -100;
                }
                if (texVecPosDraw[i].y + dirY < -100){
                    dirY = -dirY;
                    texVecPosDraw[i].y = fullHeight + 100;
                }
                if (texVecPosDraw[i].z < - 1000){
                    texVecPosDraw[i].z = 1000;
                }

                float drag = ofRandom(0.8, 0.99);
                dirX *= drag;
                dirY *= drag;
                
                texVecPosDraw[i].x += dirX;// + fakeWindX;
                texVecPosDraw[i].y += dirY;// + fakeWindX;
                texVecPosDraw[i].z -= 0.8;
                
                if ((int)recordedFrame % 25 == 0){
                    colTexVecs[i].r++;
                    colTexVecs[i].g++;
                    colTexVecs[i].b++;
                    colTexVecs[i].a--;
                }
                
                if (colTexVecs[i].r > 240) { colTexVecs[i].r = 240;}
                if (colTexVecs[i].g > 240) { colTexVecs[i].g = 240;}
                if (colTexVecs[i].b > 240) { colTexVecs[i].b = 240;}
                if (colTexVecs[i].a < 50) { colTexVecs[i].a = 50;}
                
                ofSetColor(colTexVecs[i].r, colTexVecs[i].g, colTexVecs[i].b, colTexVecs[i].a);
                texVecGetter.getTexture().drawSubsection(texVecPosDraw[i].x, texVecPosDraw[i].y, texVecPosDraw[i].z, texVecSize, texVecSize, texVecsPosX[i], texVecsPosY[i], texVecSize, texVecSize);
                
                ofSetColor(255);
                
            }
            
          
        }
        
/** Scene 3   -  Strophen bewegen sich, leichte Vis Effects**/
        if(recordedFrame > scene2 && recordedFrame <= scene3){
            /*** schnee ***/
            for(unsigned int i = 0; i < pSnowFlakes.size()-1; i++){ pSnowFlakes[i].draw(); }

            /** Strophen global move **/
            updateFullVerses();
            drawFullVerses(0);

        }
        

/** Scene 4   -  Strophen Explodieren, fliegen weg als Schnee   **/
        if(recordedFrame > scene3 ){
            
            /*** schnipsel ***/
            for(unsigned int i = 0; i < pSnowFlakes.size()-1; i++){ pSnowFlakes[i].draw(); }
            
            //  for(unsigned int i = 0; i < pSnowFlakes.size()-1; i++){ pSnowFlakes[i].draw(); }
            
            if(!bInit){
                resetParticles();
//                for(int i=0; i<numGP; i++){
//                    initParticles(gP[i].p, gP[i].versesImg);
//                }
                bInit = true;
            }
            
            
            /* verse explodieren */
            for(int j=0; j<numGP; j++){
                if(gP[j].bInit){
                    for (int i=0; i<gP[j].p.size(); i++){
                        gP[j].p[i].setGlobalPos(gP[j].vPosVerse);
                    }
                }
            }
            
            for(int j=0; j<gPUpdateSize; j++){
                
                if(!gP[j].bInit){
                    initParticles(gP[j].p, gP[j].versesImg);
                    for (int i=0; i<gP[j].p.size(); i++){
                        gP[j].p[i].setGlobalPos(gP[j].vPosVerse);
                    }
                    gP[j].bInit = true;
                    gP[j].relVerseAge = 0;
                }
                
                for(unsigned int i = 0; i < gP[j].p.size()-particleResolution; i++){
                    if(gP[j].p[i].particleTexture.isAllocated()){
                        gP[j].p[i].draw();
                    }
                }
            }
            drawFullVerses(gPUpdateSize);
        }
    }
    recordFbo.end();
    recordFbo.draw(0,0, fullWidth/10, fullHeight/10);
    
//    ofSetColor(255,255,255,10);
//    kuppelGrid.draw(0,0, fullWidth, fullHeight);
    
    /* recording pause */
    if((int)recordedFrame % (int)vidPart == 0 && !bPause && recordedFrame > 1){
        bPause = true;
        waitCounter = ofGetElapsedTimef();
        keyReleased('r');
    }
    
    /* recording fortsetzen */
    if(vidRecorder.getVideoQueueSize() < 5 && bPause && recordedFrame < scene4){
        bPause = false;
        keyReleased('r');
    }
    
    /** END Recording **/
    if(recordedFrame - vidRecorder.getVideoQueueSize() > scene4 && !bEnd ){
        keyReleased('r');
        bEnd = true;
        bPause = true;
    }
    /* Close Recordign */
    if(vidRecorder.getVideoQueueSize() < 1 && recordedFrame > scene4 && bEnd){
        exit();
    }
}

//--------------Strophen Global Move:-------------------------------------
void ofApp::updateFullVerses(){
    
    float t = recordedFrame/fps;
    
    for(int i=0; i<numGP; i++){
        float       friction = 0.79 + i * 0.001;
        ofVec3f     frc;
        float       fakeWindX;
        ofVec3f     pos = gP[i].vPosVerse;
        float       uniqueVal = gP[i].uniqueVal;
        ofVec3f     vel = gP[i].vel;
        
        fakeWindX = ofSignedNoise(pos.x * 0.04, pos.y * 0.09, ofGetElapsedTimef() * 0.07);
        
        
        frc.x = fakeWindX * 0.25 + ofSignedNoise(uniqueVal, pos.y * 0.04) * 0.8;
        frc.x += ofSignedNoise(uniqueVal, pos.x * 0.06 + i, t*0.2) * 0.5;
        frc.z = ofSignedNoise(uniqueVal, pos.z * 0.06 * i, t*0.002 * i) * 0.9 + 0.78;
        frc.y = fakeWindX * 0.1 + ofSignedNoise(uniqueVal, pos.x * 0.006 + i, t*0.2) * 0.5 - 0.9;
        
        
        float rate = 0.02 + ofNoise(recordedFrame * 0.002, 10 * i) * 0.015;
        gP[i].vPosVerse += ofSignedNoise(ofSignedNoise(recordedFrame * 0.001, rate), ofSignedNoise(recordedFrame * 0.001, rate));
        
        frc.x += rate + sin(recordedFrame * 0.001 + 10 + i) * 0.5;
        frc.y -= rate + sin(recordedFrame * 0.001 + 10 + i) * 0.1;

        
        float drag  = ofRandom(0.87, 0.99);
        vel *= drag;
        vel += frc * 0.4 * (1.0 - friction);
        
        if(gP[i].vPosVerse.z + vel.z > 10){
            vel.z = -vel.z;
        }
        if(gP[i].vPosVerse.z + vel.z< -20){
            vel.z = -vel.z;
        }
        
        if(gP[i].vPosVerse.x > fullWidth + gP[i].versesImg.getWidth()){
            gP[i].vPosVerse.x = -gP[i].versesImg.getWidth();
        }
        if(gP[i].vPosVerse.x  < - gP[i].versesImg.getWidth()){
            gP[i].vPosVerse.x = fullWidth + gP[i].versesImg.getWidth();
        }
        if(gP[i].vPosVerse.y > fullHeight + gP[i].versesImg.getHeight()){
            gP[i].vPosVerse.y = -gP[i].versesImg.getHeight();
        }
        if(gP[i].vPosVerse.y  < - gP[i].versesImg.getHeight()){
            gP[i].vPosVerse.y = fullHeight;
        }
        
        
        //2 - UPDATE OUR POSITION
        gP[i].vPosVerse += vel;
        gP[i].vel = vel;
        
       
    }
}
//--------------------------------------------------------------
void ofApp::drawFullVerses(int startNum){
    float t = recordedFrame/fps;
    
    for(int i=startNum;i< numGP;i++){
        ofSetColor(255, 180);
        gP[i].versesImg.draw(gP[i].vPosVerse.x  , gP[i].vPosVerse.y , gP[i].vPosVerse.z );
        
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
