#include "Letter.h"


Letter::Letter(
    float x,
    float y,
    float radius,
    Letter * left,
    char character,
    ofTrueTypeFont * font,
    Grid * grid)
{
    // Position and Physics.
    this -> position = ofVec2f(x, y);

    this -> velocity = ofVec2f(0, 0);

    this -> acceleration = ofVec2f(0, this -> gravity);

    // Collision Detection Geometry will be created with the texture later.
    collidable = NULL;

    // State.
    this -> letter_to_my_left = NULL;//left; //

    // FIXME: For now we are assuming that the letters will be of equal width.
    this -> x_offset_from_left = radius * 2;

    this -> font = font;

    this -> character = character;

    this -> grid = grid;
}


Letter::~Letter()
{
    delete collidable;
}

/*
 *
 * Initialization functions.
 *
 */

void Letter::init_texture(char character, ofTrueTypeFont * font)
{
    this -> character = character;

    // Determine character size on screen.
    ofRectangle bounds = font -> getGlyphBBox();
    int width  = bounds.getWidth();
    int height = bounds.getHeight();

    // Allocate a large enough frame buffer object to hold 
    this -> fbo.allocate(width, height, GL_RGBA);

    // OBSERVATIONS: 
    // 1. ofSetColor(white) needs to be called before drawing the fbo's.
    // 2. The background may be set, but drawing the string doesn't seem to be working.
    // 3. drawString is the location of the base line.

    this -> fbo.begin();
    ofClear(0, 0, 0, 0);

    // Draw a string onto the texture at the given left, baseline position.
    // the texture is big enough to hold any glyph and the ascender indicates the distance from the top to the baseline, if we want to provide enough space for any glyph.
    ofSetColor(0, 0, 0, 255);

    // Character to nul terminated string.
    string s;
    s.push_back(character);
    s.push_back('\0');    

    font -> drawString(s, 0, font -> getAscenderHeight());
    this -> fbo.end();

    ofDisableLighting();

    // Initialize Oriented Bounding Box Collision Geometry.
    this -> collidable = new OBB(position.x, position.y, width/2, height/2, this -> angle);

}

/*
 *
 * Public Interface.
 *
 */

void Letter::update(float dt)
{
    // Remove the old references to this object stored in the grid.
    grid -> remove_from_collision_grid(this);

    angle += .01;

    stepAcceleration(dt);
    stepVelocity(dt);
    stepPosition(dt);


    // Transition between states.

    if (state == WATERFALL && position.y > pool_y_coordinate)
    {
        state = POOL;
        //vy *= -1; // Bounce letters off of the line.
    }

    if (state == POOL && position.y > text_scroll_y_coordinate)
    {
        state = TEXT_SCROLL;
    }

    if (this -> collidable != NULL)
    {
        this -> collidable -> updatePositionRotation(position.x, position.y, this -> angle);
    }

    // Add the new references to this object to the grid.
    grid -> add_to_collision_grid(this);
}

void Letter::draw()
{

    // Perform texture allocation here in the serial loop to prevent multiple binding problems.
    if (this -> fbo.isAllocated() == false)
    {
        this -> init_texture(this -> character, this -> font);
    }

    // For now we will draw circles to represent these letters on the screen.
    ofSetColor(0, 0, 0);
    ofFill();
    //ofDrawCircle(this -> px, this -> py, this -> radius);

    // FIXME: Draw textures Letters, instead of circles.
    //this -> texture.draw(this -> px, this -> py);

    // We retrieve a snapshot of the values at this point in time, so that drawing is consistent.
    float x = position.x;
    float y = position.y;
    float angle = this -> angle;

    ofPushMatrix();//will isolate the transform
    ofTranslate(x, y); // Move the 0 coordinate to location (px, py)
    ofRotate(ofRadToDeg(angle));
    //ofTranslate(px, py);
    int w = fbo.getWidth();
    int h = fbo.getHeight();

    // Set Alpha to 255, without this line the fbo draws as black.
    ofSetColor(255, 255, 255, 255);
    this -> fbo.draw(-w/2, -h/2);// Drawn at origin, because of ofTranslate call.
    //ofDrawCircle(this -> px, this -> py, this -> radius);
    //ofDrawCircle(0, 0, this -> radius);

    ofPopMatrix();//will stop other things from being drawn rotated

    //this -> fbo.draw(50, 50);
    #ifdef DEBUG
    this -> collidable -> draw(x, y, (angle));
    #endif
}

bool Letter::isDead()
{
    return position.y > ofGetHeight() || dead;
}

void Letter::kill()
{
    dead = true;
}

float Letter::getX()
{
    return position.x;
}

float Letter::getY()
{
    return position.y;
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
    this -> velocity += this -> acceleration*dt;

    // Limit the velocity of letters to ensure collision detection accuracy and for better controlled aesthetics.
    velocity.x = CLAMP(velocity.x, -terminal_velocity, terminal_velocity);
    velocity.y = CLAMP(velocity.y, -terminal_velocity, terminal_velocity);
    
}

void Letter::dynamicsP(float dt)
{
    // FIXME: Use RK5 or some other better integration scheme.
    this -> position += this -> velocity*dt;
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
    acceleration.y *= .999;

    // Oscilate the horizontal movement.
    //vx = 20 * cos(ofGetElapsedTimef());

    // Interpolate the velocity to the pool speed.
    float per = .99;

    velocity.y = velocity.y*per + pool_y_speed * (1.0 - per);
    
    // Move the letter closer towards the center.
    float center_x = ofGetWidth()/2;
    float target_x;

    if (letter_to_my_left == NULL)
    {
        target_x = this -> position.x;
    }
    else
    {
        target_x = letter_to_my_left -> getX() + this -> x_offset_from_left;
    }
    
    // Velocity is skewed towards the letter's position in the sentance.
    velocity.x = velocity.x*per + (target_x - position.x)*40*(1.0 - per);

    // FIXME: Coagulate the letters with their leaders.
}

void Letter::stepPoolP(float dt)
{
    dynamicsP(dt);
}


// Stage 3: Text Scroll behavior.

void Letter::stepTextScrollA(float dt)
{
    acceleration.y = 0;
}

void Letter::stepTextScrollV(float dt)
{
    velocity.x = 0;
    velocity.y = text_scroll_speed;
}

void Letter::stepTextScrollP(float dt)
{
    dynamicsP(dt);
}



/*
 * Collision Detection Methods
 * Collidable Methods
 *
 */

Collidable * Letter::getCollidable()
{
    return this -> collidable;
}

void Letter::updatePositionFromCollidable()
{
    this -> position = collidable -> getCenterPoint();
}