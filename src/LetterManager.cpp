#include "LetterManager.h"


LetterManager::LetterManager(Grid * grid, float time_per_scroll)
{
    this -> time_per_scroll = time_per_scroll;
    this -> grid = grid;
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