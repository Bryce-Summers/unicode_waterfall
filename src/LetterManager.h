#pragma once

#include "grid.h"
//#include "Letter.h"

class LetterManager
{
public:

    // NOTE: scroll_delay should be no greater than the sentance generation time,
    //       else the number of letters will monototically increase.
    LetterManager(Grid * grid, float scroll_delay, float pool_y, float scroll_y);
    ~LetterManager();

// -- Fields.
    int next_index = 0;
    float time_per_scroll = 1.0; // Time in Seconds.
    bool scroll_ready = true;    // true iff enough time has passed to allow the next sentance
                                 // into the scroll field.
    float time_till_next_scroll;

public:
    Grid * grid;

    // Increases the count down till the next sentance can come.
    void update(float dt);// INPUT: Time in seconds.

    bool isScrollReady();
    int get_scroll_index();
    void next_scroll();


    // The y - coordinate that letters transition from the waterfall behavior to the pooling behavior.
    // define horizontal lines the separate Stage 1, Stage 2, and Stage 3.
    float pool_y_coordinate;
    float text_scroll_y_coordinate;

    // Get y coordinates of the horizontal lines that demarcate the three regions.
    float getPoolY();
    float getScrollY();

};