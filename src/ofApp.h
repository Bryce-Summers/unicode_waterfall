#pragma once

//#define DEBUG
//#define FIXED_TIME //Sets this to fixed time step mode.

#include "ofMain.h"
#include "Letter.h"
#include "Obstacle.h"
#include <fstream>
#include "Useful.h" // char to string.
#include "LetterManager.h"
#include "FontManager.h"

#include "ofxGui.h"
//#include "ofxUnicode.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

        // Creates a new sentance worth of letters to be added to the animation.
        void spawnSentance();

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

        int frame_rate;


        void loadFonts();
        // From fonts example.
        bool bFirst;
        string typeStr;

        // A vector of up to 10 sizes of fonts.
        vector<ofTrueTypeFont *> normal_fonts; // Non stylized.
        vector<ofTrueTypeFont *> italic_fonts; // Stylized in an italic manner.
        FontManager * fontManager;

        void loadGridAndObstacles();
        // Objects in the scene.
        LetterManager * letter_manager;

        // Use one grid for collision detection.
        Grid * collision_detection_grid;

        // Use the other grid for the calculation of fluid dynamics.
        Grid * fluid_dynamics_grid;

        list<Obstacle *> obstacles;
        list<Letter *>   letters;

        void loadInputText();
        vector<string> input; //declare a vector of strings to store data  
        int line_index;


        ofRectangle phase_1A;
        ofRectangle phase_1B;
        ofRectangle phase_2A;
        ofRectangle phase_2B;
        ofRectangle phase_2C;
        ofRectangle phase_3A;
        ofRectangle phase_3B;
        ofRectangle phase_3C;

    public:

    

    ofxFloatSlider divide_y1;
    ofxFloatSlider divide_y2;
    ofxFloatSlider divide_y3;
    ofxFloatSlider divide_y4;
    ofxFloatSlider divide_y5;
    ofxFloatSlider divide_y6;
    ofxFloatSlider divide_y7;
    ofxFloatSlider divide_y8;

    vector<ofxFloatSlider *> y_dividers;
    
    ofxFloatSlider sentances_per_second;// = .3; //.3;
    
    // Amount that a given accelaration is damped for altering meandering direction.
    ofxFloatSlider meanderingDamping_letters;// = .5;
    
    // The constant speed that a pool word will meander at.
    // This should be faster if the pool is larger.
    //float meanderingSpeed = 50; // Pixels per second.
    ofxFloatSlider meanderingSpeed_letters;// = 100;
    // 50 works for 800 wide pool.
    // 100 for 1920 wide pool.
    
    
    // -- More meandering Sliders for each phase.
    ofxFloatSlider meanderingDamping_words;//     =  .5;
    ofxFloatSlider meanderingSpeed_words;//       = 100;
    ofxFloatSlider meanderingDamping_sentances;// =  .5;
    ofxFloatSlider meanderingSpeed_sentances;//   = 100;
    
    ofxFloatSlider combine_delay_letters;//   = 1;
    ofxFloatSlider combine_delay_words;
    ofxFloatSlider combine_delay_sentances;// = 1;
    ofxFloatSlider max_scroll_delay;//  = 1;
    
    
    // How much the wind affects the letters.
    ofxFloatSlider wind_factor;// = 0.01;
    
    // Rate at which sentances scroll down the screen in the Stage 3: text_scroll.
    ofxFloatSlider scrollSpeed;// = 40;
    
    ofxFloatSlider poolTurnSpeed;// = .1;
    
    ofxFloatSlider gravity;// = 30;// pixels / time / time.
    ofxFloatSlider terminal_velocity;// = 80;

    ofxFloatSlider dead_zone_height;
    ofxFloatSlider wordToSentancePoolDelay;
   
    ofxFloatSlider bounce_energy;    
    ofxToggle show_grid;


        // -- GUI Sliders.        

        ofxPanel gui;

        int timing = 0;

};
