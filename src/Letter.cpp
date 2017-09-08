#include "Letter.h"

Letter::Letter(
    float x,
    float y,
    Letter * left,
    char character,
    ofTrueTypeFont * font,
    LetterManager * letterManager,
    float offset_from_left,
    bool space_before,
    int sentance_index) : Body(letterManager -> grid)
{

    // FIXME: change back to waterfall after kerning is working.
    this -> state = TEXT_SCROLL;//WATERFALL;

    // Position and Physics.
    this -> position = ofVec2f(x, y);

    // Letters already have side to side rotation behavior when they come into the scene.
    this -> velocity = ofVec2f(ofRandom(5) - 2.5, 0);

    this -> acceleration = ofVec2f(0, this -> gravity);

    // Collision Detection Geometry will be created with the texture later.
    collidable = NULL;

    // FIXME: Remove this once we are ready to retest collisions.
    this -> deactivateCollider();

    // State.
    this -> letter_to_my_left  = left;
    //this -> letter_to_my_right = right;

    this -> x_offset_from_left = offset_from_left;

    this -> font = font;

    this -> character = character;

    this -> letterManager = letterManager;

    this -> dynamic = true;

    this -> last_dt = 0.0;

    this -> space_before = space_before;

    this -> sentance_index = sentance_index;

    // Letters start out with a rotational speed.
    float angular_velocity_variance = PI*5;
    this -> angle_speed = ofRandom(angular_velocity_variance) - angular_velocity_variance / 2;

    // Handle words of length 1.
    if (isStartOfWord() && isEndOfWord())
    {
        setWordComplete();
    }
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

    // Character to nul terminated string.
    string s;
    s.push_back(character);
    s.push_back('\0');

    // Determine character size on screen.
    ofRectangle char_bounds  = font -> getStringBoundingBox(s, this -> position.x, this -> position.y);
    ofRectangle glyph_bounds = font -> getGlyphBBox();
    int width  = char_bounds.getWidth();
    int height = glyph_bounds.getHeight();
    int char_height = char_bounds.getHeight();

    // Allocate a large enough frame buffer object to hold 
    this -> fbo.allocate(width, height + 5, GL_RGBA);

    // OBSERVATIONS: 
    // 1. ofSetColor(white) needs to be called before drawing the fbo's.
    // 2. The background may be set, but drawing the string doesn't seem to be working.
    // 3. drawString is the location of the base line.

    this -> fbo.begin();
    ofClear(0, 0, 0, 0);

    // Draw a string onto the texture at the given left, baseline position.
    // the texture is big enough to hold any glyph and the ascender indicates the distance from the top to the baseline, if we want to provide enough space for any glyph.
    ofSetColor(0, 0, 0, 255);

    // We draw the string at the ascender height to make the glpyhs level with each other.
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
    this -> last_dt = dt;

    // Safe guard against letters
    // FIXME: Remove this and solve it by gurantteeing that letters do not get stuck.
    time += dt;
    if (state == WATERFALL && time > 10)
    {
        ofVec2f center = ofVec2f(960, 500);
        ofVec2f offset = this -> position - center;

        if (offset.length() > 300)
        {
            state = POOL;
            this -> position = center;
            this -> acceleration = ofVec2f(0, 0);
            this -> velocity = ofVec2f(0, 0);
        }
        
    }

    // Bounce off side walls.
    if ((position.x < 10 && velocity.x < 0) || 
        (position.x > ofGetWidth() - 10 && velocity.x > 0))
    {
        velocity.x *= -1;
    }

    stepAcceleration(dt);
    stepVelocity(dt);
}

void Letter::move(float dt)
{
    // Remove the old references to this object stored in the grid.
    grid -> remove_from_collision_grid(this);

    angle += this->angle_speed*dt;
    this -> angle_speed *= .99; // Slow down the angle speed.

    stepPosition(dt);

    // Transition between states.
    float pool_y = this -> letterManager -> getPoolY();
    if (state == WATERFALL && position.y > pool_y)
    {
        state = POOL;

        // Collisions are no longer used in stage 2.
        this -> deactivateCollider();
        //vy *= -1; // Bounce letters off of the line.
    }

    // Transition to text scrolling.
    if (state == POOL &&
        this -> sentance_complete == true &&
        letterManager -> isScrollReady() &&
        this -> sentance_index == letterManager -> get_scroll_index())
    {
        this -> setGroupState(TEXT_SCROLL);
        this -> letterManager -> next_scroll();

        cout << "Index scrolled = " << this -> sentance_index << endl;
    }

    updateCollidableFromPosition();

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
    if (dead)
    {
        return true;
    }

    // all letters should die off at once.
    // The left hand letter is allowed to die.
    if (this -> letter_to_my_left == NULL)
    {
        // Caching the dead value makes dead collection O(n) for an n letter list.
        dead = (state == TEXT_SCROLL && position.y > ofGetHeight() || dead);
        return dead;
    }
    else
    {
        return this -> letter_to_my_left -> isDead(); // Deadness is determined at the sentance level.
    }
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
    // Constantly Dampen Accelerations.
    //acceleration.y *= .999;
    //acceleration.x *= .999;

    // Oscilate the horizontal movement.
    //vx = 20 * cos(ofGetElapsedTimef());

    // Interpolate the velocity to the pool speed.

    // Move the letter closer towards the center.
    bool free;
    ofVec2f target_position = this -> getTargetPosition(&free);

    if (!free)
    {
        float snap_percentage = 1.0; // 0.0 - 1.0

        ofVec2f desired_velocity = (target_position - this->position);

        // Velocity is skewed towards the letter's position in the sentance.
        acceleration = desired_velocity - this -> velocity;
        acceleration *= 50;
    }

    
    if (free)
    {
        ofVec2f center = ofVec2f(960, 500);
        ofVec2f toCenter = center - this -> position;
        ofVec2f perpCenter = toCenter;
        float temp = -perpCenter.y;
        perpCenter.y = perpCenter.x;
        perpCenter.x = temp;
        this -> acceleration = toCenter*40 + perpCenter*40;
    }

}

void Letter::stepPoolV(float dt)
{
    dynamicsV(dt);

    
    bool free;
    ofVec2f target_position = this -> getTargetPosition(&free);

    
    if (!free)
    {
        ofVec2f desired_velocity = (target_position - this -> position)*.01/dt;
        //this -> velocity = desired_velocity;
    }
}

void Letter::stepPoolP(float dt)
{
    dynamicsP(dt);

    // -- Make connected words rigid.
    bool free;
    ofVec2f target_position = this -> getTargetPosition(&free);
    if (connected_left && driving == LEFT) 
    {
        this -> position = target_position;
    }
    else if(connected_right && driving == RIGHT)
    {
        this->position = target_position;
    }
}

// Returns an appropriate position based on the stage.
ofVec2f Letter::getTargetPosition(bool * free)
{
    *free = false;

    // Just fall if in waterfall.
    if (state == WATERFALL)
    {
        *free = true;
        return this -> position;
    }
    
    ofVec2f output;

    if (state == POOL)
    {
        // -- Handle Magnet logic.
        /* The algorithm tries out 4 discrete results as follows:
         *  1. Move magnetized letters towards each other,
         *     connect them when they are close enough.
         *  2. Magnetize unconnected unmagnetized groups of letters that fullfill the connection requirements.
         *     i.e. letters attract neighbors in their word, complete words attract each other.
         *  3. Connected letters follow their group in the driving direction.
         *  4. Set driving ends to free motion.
         */


        // -- Magnets attract towards their opposite.
        
        // Follow, connect, then demagnetize left.
        if (magnet_left)
        {
            getOffsetPositionFromLeft(&output);

            if(!connected_left)
            {
                // NOTE: We don't need dist_sqr if connected_left is already true.
                // same for symmetrical case below.
                ofVec2f offset = output - this -> position;
                float dist_sqr = offset.lengthSquared();
                float threshold = 100;
                if (dist_sqr < threshold) // 10^2
                {
                    setMagnet(LEFT, false);
                    letter_to_my_left -> setMagnet(RIGHT, false);

                    connect_to_left(); // Update letter, word, and sentance connectivity values.
                
                    setDriving(LEFT);
                }
            }

            return output;
        }

        // Follow, connect, then demagnetize right.
        if (magnet_right)
        {
            getOffsetPositionFromRight(&output);

            if (!connected_right)
            {
                ofVec2f offset = output - this -> position;
                float dist_sqr = offset.lengthSquared();
                float threshold = 100;
                if (dist_sqr < threshold) // 10^2
                {
                    setMagnet(RIGHT, false);
                    letter_to_my_right -> setMagnet(LEFT, false);

                    connect_to_right(); // Update letter, word, and sentance connectivity values.
                    setDriving(LEFT);
                }
            }

            return output;
        }

        // -- Magnetize letters if they meet the requirements for connection.

        // Magnetize left.
        if (!connected_left && pool_goto_right_of_left())
        {
            setMagnet(LEFT, true);
            letter_to_my_left -> setMagnet(RIGHT, true);
            setDriving(LEFT);
            letter_to_my_left -> setDriving(RIGHT);

            getOffsetPositionFromLeft(&output);
            return output;
        }

        // Magnetize right.
        if(!connected_right && pool_goto_left_of_right())
        {
            setMagnet(RIGHT, true);
            this -> letter_to_my_right -> setMagnet(LEFT, true);
            setDriving(RIGHT);
            letter_to_my_right -> setDriving(LEFT);

            getOffsetPositionFromRight(&output);
            return output;
        }

        // Follow left connection.
        if (connected_left && driving == LEFT)
        {
            getOffsetPositionFromLeft(&output);
            return output;
        }

        // Follow right connection.
        if (connected_right && driving == RIGHT)
        {
            getOffsetPositionFromRight(&output);
            return output;
        }

        // Default Circulation behavior.
        *free = true; // Calling function can freely move this letter.
        return this -> position;
    }

    // If this letter is the leader then it wants to be right where it is.
    if (letter_to_my_left == NULL)
    {
        if (state == TEXT_SCROLL)
        {
            return ofVec2f(left_scroll_margin, this -> position.y + text_scroll_speed);
        }

        return this -> position;
    }    
    
    bool free1;
    //return this -> letter_to_my_left -> getTargetPosition(&free1) + ofVec2f(this -> x_offset_from_left, 0);
    return this -> letter_to_my_left -> position + ofVec2f(this->x_offset_from_left, 0);
}

void Letter::setDriving(Direction direction)
{
    this -> driving = direction;
    
    Letter * search = this;

    // backkwards.
    while (search -> connected_left)
    {
        search = search -> letter_to_my_left;
        search -> driving = direction;
    }

    search = this;

    // Forwards.
    while (search -> connected_right)
    {
        search = search -> letter_to_my_right;
        search -> driving = direction;
    }
}

void Letter::setMagnet(Direction direction, bool value)
{  
    Letter * search = findStartOfConnectedGroup();
    Letter * last_search;
    
    // Set elements from left to right.
    do
    {
        last_search = search;

        switch (direction)
        {
            case LEFT:  search -> magnet_left  = value; break;
            case RIGHT: search -> magnet_right = value; break;
        }

        search = search -> letter_to_my_right;
    }
    while(last_search -> connected_right);
}

void Letter::setGroupState(State state)
{
    Letter * search = findStartOfConnectedGroup();
    Letter * last_search;

    // Set elements from left to right.
    do
    {
        last_search = search;
        search -> state = state;
        search = search -> letter_to_my_right;
    } while (last_search -> connected_right);
}

inline bool Letter::pool_goto_right_of_left()
{
    if (this -> letter_to_my_left == NULL)
    {
        return false;
    }

    // ASSUMPTION: a left letter exists.

    if (connected_left)
    {
        return true;
    }

    // ASSUMPTION: we are not yet connected to it.

    if(this -> letter_to_my_left -> inPool() == false)
    {
        return false;
    }

    // ASSUMPTION: The left letter is in the pool searching and swimming for its mates.

    if (combine_stage == PARTIAL_WORD)
    {
        if(!isStartOfWord())
        {
            return true; // going towards left.
        }

        return false; // Start of word, not yet connected to the whole word.
    }

    // ASSUMPTION: We have a complete word now that should move towards its left neighbbor when it is ready.

    // letters follow leaders within complete words and follow the ends of the
    // words to their left when they are complete.    
    if(!isStartOfSentance() && this -> letter_to_my_left -> isWordComplete())
    {
        // -- FIXME: Remove me.
        if (this -> isStartOfWord())
        {
            return true;
        }

        return true;
    }

    // complete SENTANCE state letters should not need to move.
    return false;
}

inline bool Letter::getOffsetPositionFromLeft(ofVec2f * output) // ASSUMES that we have a left letter.
{
    if (this -> letter_to_my_left == NULL)
    {
        return false;
    }

    // FIXME: Include rotations.
    *output = this -> letter_to_my_left -> position + ofVec2f(this -> x_offset_from_left, 0);
    return true;
}

inline bool Letter::getOffsetPositionFromRight(ofVec2f * output) // ASSUMES that we have a left letter.
{
    if (this -> letter_to_my_right == NULL)
    {
        return false;
    }

    // FIXME: Include rotations.
    *output = this -> letter_to_my_right -> position - ofVec2f(this -> letter_to_my_right -> x_offset_from_left, 0);
    return true;
}

inline bool Letter::pool_goto_left_of_right()
{
    if (this -> letter_to_my_right == NULL)
    {
        return false;
    }

    // ASSUMPTION: a right letter exists.

    if (connected_right)
    {
        return true;
    }

    // ASSUMPTION: we are not yet connected to it.

    if(this -> letter_to_my_right -> inPool() == false)
    {
        return false;
    }

    // ASSUMPTION: The right letter is in the pool searching and swimming for its mates.

    if (combine_stage == PARTIAL_WORD)
    {
        if(!isEndOfWord())
        {
            return true; // going towards right.
        }

        return false; // Start of word, not yet connected to the whole word.
    }

    // Word to word.
    if (combine_stage == PARTIAL_SENTANCE)
    {
        if (isEndOfWord() && this -> letter_to_my_right -> combine_stage == PARTIAL_SENTANCE)
        {
            return true;
        }
        return false;
    }

    // At the word to sentance stage words hunt down their left neighbor.
    return false;

    /*
    // ASSUMPTION: The left letter is the last letter of a word and this is the first letter.

    // FIXME: Differentiate different length sentances.
    if (combine_stage == PARTIAL_SENTANCE)
    {
        
        getOffsetPositionFromRight(output);
        return true;
    }

    // complete SENTANCE state letters should not need to move.
    return false;
    */
}

// Stage 3: Text Scroll behavior.

void Letter::stepTextScrollA(float dt)
{
    acceleration = ofVec2f(0, 0);
}

void Letter::stepTextScrollV(float dt)
{
    velocity.x = 0;
    velocity.y = text_scroll_speed;

    bool free;
    velocity += (getTargetPosition(&free) - this -> position)*move_to_left;

    // Bring the letter facing upwards.
    float angle_speed1 = -angle*move_to_left;
    float angle_speed2 = (PI*2 - angle)*move_to_left;

    // Set angle speed to the direction with lowest absolute value.
    if (-angle_speed1 < angle_speed2)
    {
        this -> angle_speed = angle_speed1;
    }
    else
    {
        this -> angle_speed = angle_speed2;
    }

    angle = 0;
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

void Letter::revertToPrevious()
{
    // Rewind time.
    float dt = -.01;//this -> last_dt;

    grid -> remove_from_collision_grid(this);
    this -> position += this -> velocity * dt;
    this -> angle    += this -> angle_speed * dt;
    this -> updateCollidableFromPosition();
    grid -> add_to_collision_grid(this);

}

void Letter::updateCollidableFromPosition()
{   
    if(this -> collidable != NULL)
    {
        this -> collidable -> updatePositionRotation(position.x, position.y, this -> angle);
    }
}


// -- Stage 2 Methods.

void Letter::setRightLetter(Letter * right)
{
    this -> letter_to_my_right = right;
}

bool Letter::inPool()
{
    return state == POOL;
}


void Letter::connect_to_left()
{
    connected_left = true;
    letter_to_my_left -> connected_right = true;
    update_word_sentance_connectivity();
    return;
}

void Letter::connect_to_right()
{
    connected_right = true;
    letter_to_my_right -> connected_left = true;
    update_word_sentance_connectivity();
    return;
}

void Letter::update_word_sentance_connectivity()
{
    if (!word_complete && isWordComplete())
    {
        setWordComplete();
    }

    if (word_complete && !sentance_complete && isSentanceComplete())
    {
        setSentanceComplete();
    }

    return;
}

void Letter::setWordComplete()
{

    // Start at beginning and set left to right.
    // This is maintainable, because the logic is only written once.
    Letter * search = findStartOfWord();
    Letter * latest_search;

    do
    {
        latest_search = search;
        search -> word_complete = true;
        search -> combine_stage = PARTIAL_SENTANCE;
        //cout << search -> character;

        // Iterate.
        search = search -> letter_to_my_right;
    }while(latest_search -> isEndOfWord() == false);

    //cout << endl;

}

void Letter::setSentanceComplete()
{
    // Start at beginning and set left to right.
    Letter * search = this;
    Letter * latest_search;

    // Backwards.
    do
    {
        latest_search = search;
        search -> sentance_complete = true;
        search -> combine_stage = SENTANCE;

        // Iterate.
        search = search -> letter_to_my_left;
    } while (latest_search -> isStartOfSentance() == false);

    // Forwards.
    search = this;
    do
    {
        latest_search = search;
        search -> sentance_complete = true;

        // Iterate.
        search = search -> letter_to_my_right;
    } while (latest_search -> isEndOfSentance() == false);
}

inline bool Letter::isStartOfSentance()
{
    return this -> letter_to_my_left == NULL;
}

inline bool Letter::isEndOfSentance()
{
    return letter_to_my_right == NULL;
}

bool Letter::isStartOfWord()
{
    return this -> letter_to_my_left == NULL || space_before;
}

bool Letter::isEndOfWord()
{
    return this -> letter_to_my_right == NULL || this -> letter_to_my_right -> space_before;
}

bool Letter::isWordComplete()
{
    // Cached early affirmation.
    if (word_complete)
    {
        return true;
    }

    // Search for incompleteness.
    Letter * search = this;
    while (!(search -> isStartOfWord()))
    {
        if (!search -> connected_left)
        {
            return false;
        }

        search = search -> letter_to_my_left;
    }

    search = this;

    while (!(search -> isEndOfWord()))
    {
        if (!search -> connected_right)
        {
            return false;
        }
        search = search -> letter_to_my_right;
    }

    // Set once, safe for parrallelism.
    word_complete = true;
    return true;
}

bool Letter::isSentanceComplete()
{
    if (sentance_complete)
    {
        return true;
    }

    Letter * search = findStartOfSentance();

    while(search -> isEndOfSentance() == false)
    {
        if (search -> connected_right == false)
        {
            return false;
        }
        search = search -> letter_to_my_right;
    }

    return true;    
}

Letter * Letter::findStartOfWord()
{
    Letter * search = this;
    while (!(search -> isStartOfWord()))
    {
        search = search -> letter_to_my_left;
    }

    return search;
}

Letter * Letter::findStartOfConnectedGroup()
{
    Letter * search = this;
    while (search -> connected_left)
    {
        search = search -> letter_to_my_left;
    }

    return search;
}

Letter * Letter::findEndOfConnectedGroup()
{
    Letter * search = this;
    while (search -> connected_right)
    {
        search = search -> letter_to_my_right;
    }

    return search;
}

Letter * Letter::findEndOfWord()
{

    Letter * search = this;
    while (!(search -> isEndOfWord()))
    {
        search = search -> letter_to_my_right;
    }

    return search;
}

Letter * Letter::findStartOfSentance()
{
    Letter * search = this;
    while (search -> letter_to_my_left != NULL)
    {
        search = search -> letter_to_my_left;
    }

    // ASSUMPTION: search has a NULL left letter pointer.
    return search;
}

Letter * Letter::findEndOfSentance()
{
    Letter * search = this;
    while (search -> letter_to_my_right != NULL)
    {
        search = search -> letter_to_my_left;
    }

    // ASSUMPTION: search has a NULL left letter pointer.
    return search;
}