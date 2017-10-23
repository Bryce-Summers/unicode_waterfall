#pragma once

#include "grid.h"
#include "ofxGui.h"

enum Combine_Stage {
    ALONE,

    // Letters.
    PARTIAL_WORD, // A group of letters that are not yet a full word.

    // Words.
    PARTIAL_SENTANCE, // A group of full words that is not yet an entire sentance.

    // Sentances.
    SENTANCE
};

class LetterManager
{
public:

    // NOTE: scroll_delay should be no greater than the sentance generation time,
    //       else the number of letters will monototically increase.
    LetterManager(Grid * grid,
                  ofxFloatSlider * sentances_per_second,

                  vector<ofxFloatSlider *> * y_dividers,

                  ofxFloatSlider * meanderingDamping_letters,
                  ofxFloatSlider * meanderingSpeed_letters,
                  ofxFloatSlider * meanderingDamping_words,
                  ofxFloatSlider * meanderingSpeed_words,
                  ofxFloatSlider * meanderingDamping_sentances,
                  ofxFloatSlider * meanderingSpeed_sentances,

                  ofxFloatSlider * scroll_speed,
                  ofxFloatSlider * magnet_factor,
                  ofxFloatSlider * pool_turn_speed,
                  ofxFloatSlider * gravity,
                  ofxFloatSlider * terminal_velocity,

                  ofxFloatSlider * combine_delay_letters,
                  ofxFloatSlider * combine_delay_words,
                  ofxFloatSlider * combine_delay_sentances,
                  ofxFloatSlider * max_time_between_scrolls,
                  ofxFloatSlider * getDeadZoneHeight,
                  ofxFloatSlider * getWordToSentancePoolDelay,
                  ofTrueTypeFont * font,
                  int input_num
                 
);
    ~LetterManager();

private:
    // -- Fields.
    int next_index = 0;
    ofxFloatSlider * sentances_per_second; // Time in Seconds.
    bool scroll_ready = true;    // true iff enough time has passed to allow the next sentance
                                 // into the scroll field.
    float time_till_next_scroll;
    vector<LineSegment *> pool_boundaries;


    ofxFloatSlider * meanderingDamping_letters;
    ofxFloatSlider * meanderingSpeed_letters;
    ofxFloatSlider * meanderingDamping_words;
    ofxFloatSlider * meanderingSpeed_words;
    ofxFloatSlider * meanderingDamping_sentances;
    ofxFloatSlider * meanderingSpeed_sentances;


    ofxFloatSlider * combine_delay_letters;
    ofxFloatSlider * combine_delay_words;
    ofxFloatSlider * combine_delay_sentances;
    ofxFloatSlider * max_time_between_scrolls;


    ofxFloatSlider * scrollSpeed;

    ofxFloatSlider * wind_factor;

    ofTrueTypeFont * font;

    const float speed_limit = 300;

public:
    Grid * grid;

    // Increases the count down till the next sentance can come.
    void update(float dt);// INPUT: Time in seconds.

    bool isScrollReady();
    int get_scroll_index();
    void next_scroll();


    // The y - coordinate that letters transition from the waterfall behavior to the pooling behavior.
    // define horizontal lines the separate Stage 1, Stage 2, and Stage 3.

    float top_y = 0;

    vector<ofxFloatSlider *> * y_dividers;

    ofxFloatSlider * turn_speed;

    ofxFloatSlider * gravity;
    ofxFloatSlider * terminal_velocity;

    ofxFloatSlider * deadZoneHeight;
    ofxFloatSlider * wordToSentancePoolDelay;

    float bottom_y;


    // Get y coordinates of the horizontal lines that demarcate the three regions.
    float getYDivider(int index);

    float getTopY();
    float getBottomY(); // Bottom y coordinate for the world.

    float getMeanderingDamping(Combine_Stage stage);
    float getMeanderingSpeed(Combine_Stage stage);


    const float getSpeedLimit();

    float getTurnSpeed();

    float getGravity();
    float getTerminalVelocity();

    float getTextScrollSpeed();

    float getSentancesPerSecond();

    float getWindFactor();


    float get_combine_delay_letters();
    float get_combine_delay_words();
    float get_combine_delay_sentances();
    float get_max_time_between_scrolls();

    
    float getDeadZoneHeight();
    float getWordToSentancePoolDelay();

    ofTrueTypeFont * getFont();

    float getDriverDelay();
    float getRepellingForce();
    float combineThresholdDistance();

    int max_index;
};