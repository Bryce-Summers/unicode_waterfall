#pragma once

#include "grid.h"
//#include "Letter.h"

class LetterManager
{
public:

    // NOTE: scroll_delay should be no greater than the sentance generation time,
    //       else the number of letters will monototically increase.
    LetterManager(Grid * grid, float scroll_delay);
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
};

