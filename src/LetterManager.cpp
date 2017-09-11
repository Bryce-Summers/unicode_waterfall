#include "LetterManager.h"


LetterManager::LetterManager(Grid * grid, float time_per_scroll,
    float pool_y,
    float pool_y_divider_1,
    float pool_y_divider_2,
    float scroll_y,

    float meanderingDamping,
    float meanderingSpeed,
    float scrollSpeed,
    float magnet_factor)
{
    this -> time_per_scroll = time_per_scroll;
    this -> grid = grid;

    this -> pool_y_coordinate = pool_y;
    this -> pool_y_divider_1  = pool_y_divider_1;
    this -> pool_y_divider_2  = pool_y_divider_2,

    this -> text_scroll_y_coordinate = scroll_y;

    this -> meanderingDamping  = meanderingDamping;
    this -> meanderingSpeed    = meanderingSpeed;
    this -> scrollSpeed        = scrollSpeed;

    this -> magnet_factor = magnet_factor;

    this -> generatePoolBoundaries();

    // The bottom of the world.
    bottom_y = ofGetHeight() + 50;

}


LetterManager::~LetterManager()
{
}

// Increases the count down till the next sentance can come.
// INPUT: Time in seconds.
void LetterManager::update(float dt)
{
    if (!scroll_ready)
    {
        time_till_next_scroll -= dt;
        if (time_till_next_scroll < 0)
        {
            scroll_ready = true;
        }
    }
}

bool LetterManager::isScrollReady()
{
    return scroll_ready;
}

int LetterManager::get_scroll_index()
{
    return next_index;
}

void LetterManager::next_scroll()
{
    next_index++;
    scroll_ready = false;
    
    // As of 9.8.17 4:32 pm, we are triggering this as soon as a sentance leaves the pool.
    time_till_next_scroll = time_per_scroll;
}


// Get y coordinates of the horizontal lines that demarcate the three regions.
float LetterManager::getPoolY()
{
    return this -> pool_y_coordinate;
}

float LetterManager::getPoolY_d1()
{
    return this -> pool_y_divider_1;
}

float LetterManager::getPoolY_d2()
{
    return this -> pool_y_divider_2;
}

float LetterManager::getScrollY()
{
    return this -> text_scroll_y_coordinate;
}

float LetterManager::getBottomY()
{
    return this -> bottom_y;
}

float LetterManager::getMeanderingDamping()
{
    return this -> meanderingDamping;
}

float LetterManager::getMeanderingSpeed()
{
    return this -> meanderingSpeed;
}

void LetterManager::generatePoolBoundaries()
{
    float x0 = 0;
    float x1 = ofGetWidth();
    float y0 = pool_y_coordinate;
    float y1 = text_scroll_y_coordinate;

    ofVec2f xy00 = ofVec2f(x0, y0);
    ofVec2f xy01 = ofVec2f(x0, y1);
    ofVec2f xy10 = ofVec2f(x1, y0);
    ofVec2f xy11 = ofVec2f(x1, y1);

    LineSegment * top    = new LineSegment(xy00, xy10);
    LineSegment * right  = new LineSegment(xy10, xy11);
    LineSegment * bottom = new LineSegment(xy11, xy01);
    LineSegment * left   = new LineSegment(xy01, xy00);

    pool_boundaries.push_back(top);
    pool_boundaries.push_back(right);
    pool_boundaries.push_back(bottom);
    pool_boundaries.push_back(left);
}

vector<LineSegment*> * LetterManager::getPoolBoundaries()
{    
   // FIXME: Consider clearing a vector and adding pointers to another output.
   return &pool_boundaries;
}

const float LetterManager::getSpeedLimit()
{
    return this -> speed_limit;
}

float LetterManager::getMagnetFactor()
{
    return this -> magnet_factor;
}