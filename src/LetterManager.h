#pragma once

#include "grid.h"
//#include "Letter.h"

class LetterManager
{
public:

    // NOTE: scroll_delay should be no greater than the sentance generation time,
    //       else the number of letters will monototically increase.
    LetterManager(Grid * grid,
                  float scroll_delay,
                  float pool_y,
                  float pool_y_divider_1,
                  float pool_y_divider_2,
                  float scroll_y,
                  float meanderingDamping,
                  float meanderingSpeed,
                  float scroll_speed,
                  float magnet_factor);
    ~LetterManager();

private:
    // -- Fields.
    int next_index = 0;
    float time_per_scroll = 1.0; // Time in Seconds.
    bool scroll_ready = true;    // true iff enough time has passed to allow the next sentance
                                 // into the scroll field.
    float time_till_next_scroll;
    vector<LineSegment *> pool_boundaries;

    float meanderingDamping;
    float meanderingSpeed;
    float scrollSpeed;

    float magnet_factor;

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

    float pool_y_coordinate;

    float pool_y_divider_1;
    float pool_y_divider_2;

    float text_scroll_y_coordinate;

    float bottom_y;


    // Get y coordinates of the horizontal lines that demarcate the three regions.
    float getPoolY();    // Divides waterfall and pool stages.
    float getPoolY_d1(); // Divider between letter and words pool sections.
    float getPoolY_d2(); // Divider between words and sentances sections.
    float getScrollY();  // Divides pool and text scrolling stages.
    
    float getBottomY(); // Bottom y coordinate for the world.

    void generatePoolBoundaries();

    // The user may neither modify the returned vector of the line segments inside.
    vector<LineSegment*> * getPoolBoundaries();

    float getMeanderingDamping();
    float getMeanderingSpeed();
    float getMagnetFactor();

    const float getSpeedLimit();

};