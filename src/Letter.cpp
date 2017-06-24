#include "Letter.h"


Letter::Letter(float x, float y, float radius, Letter * left)
{
    // Position and Physics.
    this -> px = x;
    this -> py = y;
    this -> radius = radius;

    this -> vx = 0;
    this -> vy = 0;

    this -> ay = this -> gravity;
    this -> ax = 0;

    // State.
    this -> letter_to_my_left = left;

    // FIXME: For now we are assuming that the letters will be of equal width.
    this -> x_offset_from_left = radius * 2;
}


Letter::~Letter()
{
}


/*
 *
 * Public Interface.
 *
 */

void Letter::update(float dt)
{
    stepAcceleration(dt);
    stepVelocity(dt);
    stepPosition(dt);


    // Transition between states.

    if (state == WATERFALL && py > pool_y_coordinate)
    {
        state = POOL;
        //vy *= -1; // Bounce letters off of the line.
    }

    if (state == POOL && py > text_scroll_y_coordinate)
    {
        state = TEXT_SCROLL;
    }
}

void Letter::draw()
{
    // For now we will draw circles to represent these letters on the screen.
    ofSetColor(0, 0, 0);
    ofFill();
    ofDrawCircle(this -> px, this -> py, this -> radius);

    // FIXME: Draw Letters, instead of circles.
}

bool Letter::isDead()
{
    return py > ofGetHeight();
}

float Letter::getX()
{
    return px;
}

float Letter::getY()
{
    return py;
}


/*
 *
 * Internal Methods.
 *
 */

// -- Routing functions that call sub behavior functions based on 
void Letter::stepAcceleration(float dt)
{
    switch (state)
    {
        case WATERFALL:   stepWaterfallA(dt); break;
        case POOL:        stepPoolA(dt);      break;
        case TEXT_SCROLL: stepTextScrollA(dt);break;
    }
}

void Letter::stepVelocity(float dt)
{
    switch (state)
    {
        case WATERFALL:   stepWaterfallV(dt);  break;
        case POOL:        stepPoolV(dt);       break;
        case TEXT_SCROLL: stepTextScrollV(dt); break;
    }
}

void Letter::stepPosition(float dt)
{
    switch (state)
    {
        case WATERFALL:   stepWaterfallP(dt);  break;
        case POOL:        stepPoolP(dt);       break;
        case TEXT_SCROLL: stepTextScrollP(dt); break;
    }
}

// -- Integration based Physics calculations.
void Letter::dynamicsA(float dt)
{
    // FIXME: Add turbulence.
}

void Letter::dynamicsV(float dt)
{
    // FIXME: Use RK5 or some other better integration scheme.
    this -> vx += this -> ax*dt;
    this -> vy += this -> ay*dt;

    // Limit the velocity of letters to ensure collision detection accuracy and for better controlled aesthetics.
    vy = CLAMP(vy, -terminal_velocity, terminal_velocity);
    vx = CLAMP(vx, -terminal_velocity, terminal_velocity);
}

void Letter::dynamicsP(float dt)
{
    // FIXME: Use RK5 or some other better integration scheme.
    this -> px += this -> vx*dt;
    this -> py += this -> vy*dt;
}


// -- Stage 1: Waterfall behavior.

void Letter::stepWaterfallA(float dt)
{
    dynamicsA(dt);
}

void Letter::stepWaterfallV(float dt)
{
    dynamicsV(dt);
}

void Letter::stepWaterfallP(float dt)
{
    dynamicsP(dt);
}


// -- Stage 2: Pool behavior.

void Letter::stepPoolA(float dt)
{

}

void Letter::stepPoolV(float dt)
{
    dynamicsV(dt);

    // Gradually mitigate the gravity acceleration.
    ay *= .999;

    // Oscilate the horizontal movement.
    //vx = 20 * cos(ofGetElapsedTimef());

    // Interpolate the velocity to the pool speed.
    float per = .99;

    vy = vy*per + pool_y_speed * (1.0 - per);
    
    // Move the letter closer towards the center.
    float center_x = ofGetWidth()/2;
    float target_x;

    if (letter_to_my_left == NULL)
    {
        target_x = this -> px;
    }
    else
    {
        target_x = letter_to_my_left -> getX() + this -> x_offset_from_left;
    }
    
    // Velocity is skewed towards the letter's position in the sentance.
    vx = vx*per + (target_x - px)*40*(1.0 - per);

    // FIXME: Coagulate the letters with their leaders.
}

void Letter::stepPoolP(float dt)
{
    dynamicsP(dt);
}


// Stage 3: Text Scroll behavior.

void Letter::stepTextScrollA(float dt)
{
    ay = 0;
}

void Letter::stepTextScrollV(float dt)
{
    vx = 0;
    vy = text_scroll_speed;
}

void Letter::stepTextScrollP(float dt)
{
    dynamicsP(dt);
}