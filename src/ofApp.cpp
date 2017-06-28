#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    frame = 0;

    //string unicode_test = "Testing «ταБЬℓσ» : 1<2 & 4 + 1>3, now 20 % off!";

    /*
    int frame_rate = 30;
    this -> seconds_per_frame = 1.0 / frame_rate
    */

    int sentances_per_second = 1;
    this -> seconds_per_frame = 1.0 / sentances_per_second;

    loadFonts();
}

void ofApp::loadFonts()
{
    ofBackground(54, 54, 54, 255);

    //old OF default is 96 - but this results in fonts looking larger than in other programs. 
    ofTrueTypeFont::setGlobalDpi(72);

    verdana14.load("verdana.ttf", 14, true, true);
    verdana14.setLineHeight(18.0f);
    verdana14.setLetterSpacing(1.037);

    verdana30.load("verdana.ttf", 30, true, true);
    verdana30.setLineHeight(34.0f);
    verdana30.setLetterSpacing(1.035);

    verdana14A.load("verdana.ttf", 14, false);
    verdana14A.setLineHeight(18.0f);
    verdana14A.setLetterSpacing(1.037);

    franklinBook14.load("frabk.ttf", 14);
    franklinBook14.setLineHeight(18.0f);
    franklinBook14.setLetterSpacing(1.037);

    franklinBook14A.load("frabk.ttf", 14, false);
    franklinBook14A.setLineHeight(18.0f);
    franklinBook14A.setLetterSpacing(1.037);

    bFirst = true;
    typeStr = "ABCDEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqrstuvwxyz\n0123456789,:&!?";
}

//--------------------------------------------------------------
void ofApp::update(){

    // Handle time and differences.
    float new_time = ofGetElapsedTimef(); // Time in seconds.
    float dt = new_time - time;
    time = new_time;

    // Used to trigger the creation of new sentances.
    time_accum += dt;

    // Test String.
    if (time_accum > this -> seconds_per_frame)
    {
        time_accum -= this -> seconds_per_frame;
    
        // Create a sentance of letters.
        int letters_per_sentance = 5;
        Letter * previous_letter = NULL;
        for(int i = 0; i < letters_per_sentance; i++)
        {
            
            letters.push_back(Letter(ofRandom(ofGetWidth()), -20, 10, previous_letter, 'L', &verdana14));
            previous_letter = &letters.back();
            
        }
        
    }

    vector<list<Letter>::iterator> dead_letters;

    for (auto iter = letters.begin(); iter != letters.end(); ++iter)
    {
        (*iter).update(dt);
        if ((*iter).isDead())
        {
            dead_letters.push_back(iter);
        }
    }

    // Remove all dead letters from the update list and deallocate them.
    for (auto iter = dead_letters.begin(); iter != dead_letters.end(); ++iter)
    {
        letters.erase(*iter);
    }
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofBackground(ofColor(255));


    // Draw all of the letters to the screen.
    for (auto iter = letters.begin(); iter != letters.end(); ++iter)
    {
        (*iter).draw();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

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
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

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
