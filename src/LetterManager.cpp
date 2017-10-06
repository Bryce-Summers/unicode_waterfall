#include "LetterManager.h"


LetterManager::LetterManager(Grid * grid,
    ofxFloatSlider * sentances_per_second,
    ofxFloatSlider * pool_y,
    ofxFloatSlider * pool_y_divider_1,
    ofxFloatSlider * pool_y_divider_2,
    ofxFloatSlider * scroll_y_start,
    ofxFloatSlider * scroll_y_end,

    ofxFloatSlider * meanderingDamping_letters,
    ofxFloatSlider * meanderingSpeed_letters,
    ofxFloatSlider * meanderingDamping_words,
    ofxFloatSlider * meanderingSpeed_words,
    ofxFloatSlider * meanderingDamping_sentances,
    ofxFloatSlider * meanderingSpeed_sentances,


    ofxFloatSlider * scrollSpeed,
    ofxFloatSlider * wind_factor,
    ofxFloatSlider * pool_turn_speed,
    ofxFloatSlider * gravity,
    ofxFloatSlider * terminal_velocity,

    ofxFloatSlider * combine_delay_letters,
    ofxFloatSlider * combine_delay_words,
    ofxFloatSlider * combine_delay_sentances,
    ofxFloatSlider * max_time_between_scrolls)
{

    this -> sentances_per_second = sentances_per_second;

    this -> grid = grid;

    this -> pool_y_coordinate = pool_y;
    this -> pool_y_divider_1  = pool_y_divider_1;
    this -> pool_y_divider_2  = pool_y_divider_2,

    this -> text_scroll_y_coordinate_start = scroll_y_start;
    this -> text_scroll_y_coordinate_end   = scroll_y_end;

    this -> meanderingDamping_letters   = meanderingDamping_letters;
    this -> meanderingSpeed_letters     = meanderingSpeed_letters;
    this -> meanderingDamping_words     = meanderingDamping_words;
    this -> meanderingSpeed_words       = meanderingSpeed_words;
    this -> meanderingDamping_sentances = meanderingDamping_sentances;
    this -> meanderingSpeed_sentances   = meanderingSpeed_sentances;



    this -> scrollSpeed        = scrollSpeed;

    this -> wind_factor = wind_factor;

    this -> generatePoolBoundaries();

    // The bottom of the world.
    bottom_y = ofGetHeight() + 50;

    this -> turn_speed = pool_turn_speed;

    this -> gravity = gravity;
    this -> terminal_velocity = terminal_velocity;

    this -> combine_delay_letters    = combine_delay_letters;
    this -> combine_delay_words      = combine_delay_words;
    this -> combine_delay_sentances  = combine_delay_sentances;
    this -> max_time_between_scrolls = max_time_between_scrolls;
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
    time_till_next_scroll = 1.0 / *sentances_per_second * .25;
}


// Get y coordinates of the horizontal lines that demarcate the three regions.
float LetterManager::getPoolY()
{
    return *this -> pool_y_coordinate;
}

float LetterManager::getPoolY_d1()
{
    return *this -> pool_y_divider_1;
}

float LetterManager::getPoolY_d2()
{
    return *this -> pool_y_divider_2;
}

float LetterManager::getScrollYStart()
{
    return *this -> text_scroll_y_coordinate_start;
}

float LetterManager::getScrollYEnd()
{
    return *this -> text_scroll_y_coordinate_end;
}

float LetterManager::getBottomY()
{
    return this -> bottom_y;
}

float LetterManager::getMeanderingDamping(Combine_Stage stage)
{
    switch(stage)
    {
        case PARTIAL_WORD: return *this -> meanderingDamping_letters;
        case PARTIAL_SENTANCE: return *this -> meanderingDamping_words;
        case SENTANCE:
        default:
            return *this -> meanderingDamping_sentances;
    }
    
}

float LetterManager::getMeanderingSpeed(Combine_Stage stage)
{
    switch (stage)
    {
        case ALONE:            return *this -> meanderingSpeed_letters;
        case PARTIAL_WORD:     return *this -> meanderingSpeed_letters;
        case PARTIAL_SENTANCE: return *this -> meanderingSpeed_words;
        case SENTANCE:
        default:
            return *this -> meanderingSpeed_sentances;
    }
}


void LetterManager::generatePoolBoundaries()
{
    float x0 = 0;
    float x1 = ofGetWidth();
    float y0 = *pool_y_coordinate;
    float y1 = *text_scroll_y_coordinate_start;

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

float LetterManager::getTurnSpeed()
{
    return *this -> turn_speed;
}

float LetterManager::getGravity()
{
    return *this -> gravity;
}

float LetterManager::getTerminalVelocity()
{
    return *this -> terminal_velocity;
}

float LetterManager::getTextScrollSpeed()
{
    return *this -> scrollSpeed;
}

float LetterManager::getSentancesPerSecond()
{
    return *this -> sentances_per_second;
}

float LetterManager::getWindFactor()
{
    return *this -> wind_factor;
}

float LetterManager::get_combine_delay_letters()
{
    return *this -> combine_delay_letters;
}

float LetterManager::get_combine_delay_words()
{
    return *this -> combine_delay_words;
}

float LetterManager::get_combine_delay_sentances()
{
    return *this -> combine_delay_sentances;
}

float LetterManager::get_max_time_between_scrolls()
{
    return *this -> max_time_between_scrolls;
}