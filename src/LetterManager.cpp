#include "LetterManager.h"


LetterManager::LetterManager(Grid * collision_detection_grid,
    ofxFloatSlider * sentances_per_second,
    vector<ofxFloatSlider *> * y_dividers,

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
    ofxFloatSlider * max_time_between_scrolls,
    ofxFloatSlider * deadZoneHeight,
    ofxFloatSlider * wordToSentancePoolDelay,
    ofxFloatSlider * coef_of_restitution,
    int input_count,
    Grid * fluid_dynamics_grid,
    int bottom_y
)
{

    this -> sentances_per_second = sentances_per_second;

    this -> collision_detection_grid = collision_detection_grid;
    this -> fluid_dynamics_grid = fluid_dynamics_grid;

    this -> y_dividers = y_dividers;

    this -> meanderingDamping_letters   = meanderingDamping_letters;
    this -> meanderingSpeed_letters     = meanderingSpeed_letters;
    this -> meanderingDamping_words     = meanderingDamping_words;
    this -> meanderingSpeed_words       = meanderingSpeed_words;
    this -> meanderingDamping_sentances = meanderingDamping_sentances;
    this -> meanderingSpeed_sentances   = meanderingSpeed_sentances;
    this -> coef_of_restitution = coef_of_restitution;


    this -> scrollSpeed        = scrollSpeed;

    this -> wind_factor = wind_factor;

    // The bottom of the world.
    this -> bottom_y = bottom_y;

    this -> turn_speed = pool_turn_speed;

    this -> gravity = gravity;
    this -> terminal_velocity = terminal_velocity;

    this -> combine_delay_letters    = combine_delay_letters;
    this -> combine_delay_words      = combine_delay_words;
    this -> combine_delay_sentances  = combine_delay_sentances;
    this -> max_time_between_scrolls = max_time_between_scrolls;

    this -> deadZoneHeight          = deadZoneHeight;
    this -> wordToSentancePoolDelay = wordToSentancePoolDelay;

    this -> max_index = input_count;
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

    if (next_index >= max_index)
    {
        next_index = 0;
    }
}


// Get y coordinates of the horizontal lines that demarcate the three regions.
float LetterManager::getYDivider(int index)
{
    if (index < 0)
    {
        return getTopY();
    }

    if (index >= y_dividers -> size())
    {
        return getBottomY();
    }

    return *this -> y_dividers -> at(index);
}

float LetterManager::getBottomY()
{
    return this -> bottom_y;
}

float LetterManager::getTopY()
{
    return 0;
}

float LetterManager::getMeanderingDamping(Combine_Stage stage)
{
    switch(stage)
    {
        case ALONE: return *this -> meanderingDamping_letters;
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

float LetterManager::getDeadZoneHeight()
{
    return *this -> deadZoneHeight;
}

float LetterManager::getWordToSentancePoolDelay()
{
    return *this -> wordToSentancePoolDelay;
}

float LetterManager::getDriverDelay()
{
    return 2; // FIXME: Add this to GUI.
}

float LetterManager::getRepellingForce()
{
    return 1000;
}

float LetterManager::combineThresholdDistance()
{
    return 300;
}

ofVec2f LetterManager::getWindVelocityAtPosition(ofVec2f position)
{
    return fluid_dynamics_grid -> getWindVelocityAtPosition(position);
}

ofVec2f LetterManager::getMeanderVelocityAtPosition(ofVec2f position)
{
    return fluid_dynamics_grid -> getMeanderVelocityAtPosition(position);
}

float LetterManager::getRestitutionCoef()
{
    return *coef_of_restitution;
}

void LetterManager::addVelocityToMeander(ofVec2f position, ofVec2f velocity)
{
    fluid_dynamics_grid -> addVelocityToMeander(position, velocity);
}
