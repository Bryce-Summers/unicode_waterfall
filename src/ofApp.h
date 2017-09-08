#pragma once

//#define DEBUG
//#define FIXED_TIME Sets this to fixed time step mode.

#include "ofMain.h"
#include "Letter.h"
#include "Obstacle.h"
#include <fstream>
#include "Useful.h" // char to string.
#include "LetterManager.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
        int frame;

        bool bogus = false;

        float time;
        float time_accum = 0;

        float seconds_per_frame;
        int frame_rate;


        void loadFonts();
        // From fonts example.
        bool bFirst;
        string typeStr;

        ofTrueTypeFont  franklinBook14;
        ofTrueTypeFont	verdana14;
        ofTrueTypeFont	verdana30;

        ofTrueTypeFont  franklinBook14A;
        ofTrueTypeFont	verdana14A;

        ofTrueTypeFont font;


        void loadGridAndObstacles();
        // Objects in the scene.
        LetterManager * letter_manager;
        Grid * grid;
        list<Obstacle *> obstacles;
        list<Letter *>   letters;

        void loadInputText();
        vector<string> input; //declare a vector of strings to store data  
        int line_index;

        // These control where the barriers between the stages are.
        float pool_y   = 400; // y coordinate where waterfall transforms into pool.
        float scroll_y = 800; // y coordinate where pool transforms into scroll.

        ofRectangle phase_1;
        ofRectangle phase_2;
        ofRectangle phase_3;

        size_t ofApp::stringLength(string & str);
};
