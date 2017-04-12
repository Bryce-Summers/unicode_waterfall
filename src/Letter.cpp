#include "Letter.h"



Letter::Letter(float x, float y, float radius)
{
    this -> px = x;
    this -> py = y;
    this -> radius = radius;

    this -> vx = 0;
    this -> vy = 0;

    this -> ay = this -> gravity;
    this -> ax = 0;
}


Letter::~Letter()
{
}

void Letter::update(float dt)
{
    stepAcceleration(dt);
    stepVelocity(dt);
    stepPosition(dt);


    // Transition between states.

    if (state == WATERFALL && py > pool_y_coordinate)
    {
        state = POOL;
        vy *= -1;
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
}

bool Letter::isDead()
{
    return py > ofGetHeight();
}

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

    vy = CLAMP(vy, -terminal_velocity, terminal_velocity);
    vx = CLAMP(vx, -terminal_velocity, terminal_velocity);
}

void Letter::dynamicsP(float dt)
{
    // FIXME: Use RK5 or some other better integration scheme.
    this -> px += this -> vx*dt;
    this -> py += this -> vy*dt;
}


// -- Waterfall behavior.
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


// -- Pool behavior.

void Letter::stepPoolA(float dt)
{

}

void Letter::stepPoolV(float dt)
{
    dynamicsV(dt);

    // Gradually mitigate the gravity acceleration.
    ay *= .999;
    vx = 20 * cos(ofGetElapsedTimef());

    float per = .99;

    vy = vy*per + pool_y_speed * (1.0 - per);

    vx = vx*per + (ofGetWidth()/2 - px)*40*(1.0 - per);

    // FIXME: Coagulate the letters with their leaders.
}

void Letter::stepPoolP(float dt)
{
    dynamicsP(dt);
}

// -- Text Scroll behavior.

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