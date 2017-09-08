#include "LetterManager.h"


LetterManager::LetterManager(Grid * grid, float time_per_scroll,
    float pool_y,
    float scroll_y)
{
    this -> time_per_scroll = time_per_scroll;
    this -> grid = grid;

    this -> pool_y_coordinate = pool_y;
    this -> text_scroll_y_coordinate = scroll_y;
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
    time_till_next_scroll = time_per_scroll;
}


// Get y coordinates of the horizontal lines that demarcate the three regions.
float LetterManager::getPoolY()
{
    return this -> pool_y_coordinate;
}

float LetterManager::getScrollY()
{
    return this -> text_scroll_y_coordinate;
}