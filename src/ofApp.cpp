#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    sampleRate = 96000;
    channels = 1;
    
    ofSetFrameRate(60);
    ofSetLogLevel(OF_LOG_VERBOSE);
    vidGrabber.setDesiredFrameRate(30);
    vidGrabber.initGrabber(640, 480);
    //    vidRecorder.setFfmpegLocation(ofFilePath::getAbsolutePath("ffmpeg")); // use this is you have ffmpeg installed in your data folder
    
    fileName = "testMovie";
    fileExt = ".mov"; // ffmpeg uses the extension to determine the container type. run 'ffmpeg -formats' to see supported formats
    
    // override the default codecs if you like
    // run 'ffmpeg -codecs' to find out what your implementation supports (or -formats on some older versions)
    vidRecorder.setVideoCodec("mpeg4");
    vidRecorder.setVideoBitrate("800k");
    vidRecorder.setAudioCodec("mp3");
    vidRecorder.setAudioBitrate("192k");
    
    ofAddListener(vidRecorder.outputFileCompleteEvent, this, &ofApp::recordingComplete);
    
    soundStream.printDeviceList();
    vector<ofSoundDevice> soundDevices = soundStream.getDeviceList();
    for (int i = 0; i < soundDevices.size(); i++){
        cout << "deviceID:\t\t" << soundDevices[i].deviceID << endl;
        cout << "device name:\t\t" << soundDevices[i].name << endl;
        cout << "inputChannels:\t" << soundDevices[i].inputChannels << endl;
        cout << "outputChannels:\t" << soundDevices[i].outputChannels << endl;
        cout << "isDefaultInput:\t" << soundDevices[i].isDefaultInput << endl;
        cout << "isDefaultOutput:\t" << soundDevices[i].isDefaultOutput << endl;
        cout << "sample rates:\t\t";
        for (int j = 0; j < (int)soundDevices[i].sampleRates.size(); j++) {
            cout  << soundDevices[i].sampleRates[j] << " ";
        }
        cout << "\n\n";
    }
    cout << soundDevices << endl;
    
    
    
    soundStream.setDeviceID(2);
    
    soundStream.setup(this, 0, channels, sampleRate, 256, 4);
    
    ofSetWindowShape(vidGrabber.getWidth(), vidGrabber.getHeight()	);
    bRecording = false;
    ofEnableAlphaBlending();
}

//--------------------------------------------------------------
void ofApp::exit(){
    ofRemoveListener(vidRecorder.outputFileCompleteEvent, this, &ofApp::recordingComplete);
    vidRecorder.close();
}

//--------------------------------------------------------------
void ofApp::update(){
    vidGrabber.update();
    if(vidGrabber.isFrameNew() && bRecording){
        bool success = vidRecorder.addFrame(vidGrabber.getPixels());
        if (!success) {
            ofLogWarning("This frame was not added!");
        }
    }
    
    // Check if the video recorder encountered any error while writing video frame or audio smaples.
    if (vidRecorder.hasVideoError()) {
        ofLogWarning("The video recorder failed to write some frames!");
    }
    
    if (vidRecorder.hasAudioError()) {
        ofLogWarning("The video recorder failed to write some audio samples!");
    }
    
    for (int i = 0; i < (int)players.size(); i++) {
        players[i]->update();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(255, 255, 255);
    vidGrabber.draw(0,0);
    
    stringstream ss;
    ss << "video queue size: " << vidRecorder.getVideoQueueSize() << endl
    << "audio queue size: " << vidRecorder.getAudioQueueSize() << endl
    << "FPS: " << ofGetFrameRate() << endl
    << (bRecording?"pause":"start") << " recording: r" << endl
    << (bRecording?"close current video file: c":"") << endl;
    
    ofSetColor(0,0,0,100);
    ofDrawRectangle(0, 0, 260, 75);
    ofSetColor(255);
    ofDrawBitmapString(ss.str(),15,15);
    
    if(bRecording){
        ofSetColor(255, 0, 0);
        ofDrawCircle(ofGetWidth() - 20, 20, 5);
        ofSetColor(255);
    }
    
    for (int i = 0; i < (int)players.size(); i++) {
        players[i]->draw(players[i]->getWidth()/3 * i, 0, players[i]->getWidth()/3, players[i]->getHeight()/3);
    }
}

//--------------------------------------------------------------
void ofApp::audioIn(float *input, int bufferSize, int nChannels){
    if(bRecording)
        vidRecorder.addAudioSamples(input, bufferSize, nChannels);
}

//--------------------------------------------------------------
void ofApp::recordingComplete(ofxVideoRecorderOutputFileCompleteEventArgs& args){
    cout << "The recoded video file is now complete." << endl;
    shared_ptr<ofVideoPlayer> player = std::make_shared<ofVideoPlayer>();
    while (!player->isLoaded()) {
        player->load(args.fileName); // error loading asset tracks: Cannot Open
    }
    player->setLoopState(OF_LOOP_NORMAL);
    player->play();
    players.push_back(player);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
    if(key=='r'){
        bRecording = !bRecording;
        if(bRecording && !vidRecorder.isInitialized()) {
            vidRecorder.setup(fileName+ofGetTimestampString()+fileExt, vidGrabber.getWidth(), vidGrabber.getHeight(), 30, sampleRate, channels);
            //          vidRecorder.setup(fileName+ofGetTimestampString()+fileExt, vidGrabber.getWidth(), vidGrabber.getHeight(), 30); // no audio
            //            vidRecorder.setup(fileName+ofGetTimestampString()+fileExt, 0,0,0, sampleRate, channels); // no video
            //          vidRecorder.setupCustomOutput(vidGrabber.getWidth(), vidGrabber.getHeight(), 30, sampleRate, channels, "-vcodec mpeg4 -b 1600k -acodec mp2 -ab 128k -f mpegts udp://localhost:1234"); // for custom ffmpeg output string (streaming, etc)
            
            // Start recording
            vidRecorder.start();
        }
        else if(!bRecording && vidRecorder.isInitialized()) {
            vidRecorder.setPaused(true);
        }
        else if(bRecording && vidRecorder.isInitialized()) {
            vidRecorder.setPaused(false);
        }
    }
    if(key=='c'){
        bRecording = false;
        vidRecorder.close();
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
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
