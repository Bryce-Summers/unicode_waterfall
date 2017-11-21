#include "Letter.h"

Letter::Letter(
    LetterManager * letterManager,
    float x,
    float y,
    Letter * left,
    char character,    
    float offset_from_left,
    float offset_in_text_scroll,
    bool space_before,
    int sentance_index,
    int font_size_index, // Everything the letter needs to dynamically query its font.
    bool font_italics,
    FontManager * fontManager) : Body(letterManager -> collision_detection_grid)
{

    // Font.
    this -> font_size_index = font_size_index;
    this -> font_italics = font_italics;
    this -> fontManager = fontManager;

    this -> letterManager = letterManager;
    this -> behavior = WATERFALL;

    // Position and Physics.
    this -> position = ofVec2f(x, y);

    // Letters already have side to side rotation behavior when they come into the scene.
    this -> velocity = ofVec2f(ofRandom(5) - 2.5, 0);

    this -> acceleration = ofVec2f(0, this -> letterManager -> getGravity());

    // Collision Detection Geometry will be created with the texture later.
    collidable = NULL;
    collision_detection = true;

    this -> enable_collision_detection();
    //this -> disable_collision_detection();// FIXME: Remove this once we are ready to retest collisions.

    // State.
    this -> letter_to_my_left  = left;
    //this -> letter_to_my_right = right;

    this -> minnimal_offset = offset_from_left;
    this -> maximal_offset  = offset_in_text_scroll;

    this -> character = character;

    this -> dynamic = true;

    this -> space_before = space_before;

    this -> sentance_index = sentance_index;

    // Letters start out with a rotational speed.
    float angular_velocity_variance = PI*5;
    this -> angle_speed = ofRandom(angular_velocity_variance) - angular_velocity_variance / 2;

    this -> init_texture(character);

    this -> scroll_delay = this -> letterManager -> get_max_time_between_scrolls();

    this -> mass = 1 + ofRandom(1);

    average_position = this -> position;

}


Letter::~Letter()
{
    delete collidable;
}

// Firmly teleports the letter.
void Letter::setPosition(float x, float y)
{
    collision_detection_grid -> remove_from_collision_grid(this);
    this -> position.x = x;
    this -> position.y = y;
    this -> previous_position = this->position;

    if(this -> collision_detection && this -> position.y > 0)
    {
        this -> updateCollidableFromPosition();
        collision_detection_grid -> add_to_collision_grid(this);
    }
}

/*
 *
 * Initialization functions.
 *
 */

void Letter::init_texture(char character)
{
    this -> character = character;

    // Character to nul terminated string.
    
    this -> str.push_back(character);
    this -> str.push_back('\0');

    // Determine character size on screen.
    ofTrueTypeFont * font = fontManager -> getFont(font_size_index, font_italics);
    ofRectangle char_bounds  = font -> getStringBoundingBox(this -> str, this -> position.x, this -> position.y);
    ofRectangle glyph_bounds = font -> getGlyphBBox();
    int width  = char_bounds.getWidth();
    int height = glyph_bounds.getHeight();
    int char_height = char_bounds.getHeight();

    this -> char_width  = width;
    this -> char_height = height;

    
    /*
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
    font -> drawString(str, 0, font -> getAscenderHeight());
    this -> fbo.end();

    ofDisableLighting();
    */

    // Initialize Oriented Bounding Box Collision Geometry.
    this -> collidable = new OBB(position.x, position.y, width/2, height/2, this -> angle);

    allocated = true;

}

/*
 *
 * Public Interface.
 *
 */

// I think I should rewrite this to concisely describe what needs to happen.
// Apply forces based on behavior, bound dynamics, transition to the next state if necessary.

void Letter::update(float dt)
{
    // Apply Dynamics.
    stepAcceleration(dt);
    stepVelocity(dt);
    
    // Bound the dynamics.
    boundDynamics(dt);

    // Transition to the next state if necessary.
    transitionToNextStateIfProper(dt);
}

void Letter::boundDynamics(float dt)
{
    // Ensure that letters are travelling towards the visible portion of the screen.
    if (this -> position.y < 0 && this -> velocity.y < 0)
    {
        this -> velocity.y *= -1;
        this -> position.y += 1;
    }

    // Bounce off side walls.
    if ((position.x < 10 && velocity.x < 0) ||
        (position.x > ofGetWidth() - getRemainingLength() && velocity.x > 0))
    {
        velocity.x *= -1;
    }

    // Bound the dynamics in the waterfall to avoid over energizing the letters.
    if (behavior == WATERFALL)
    {
        float mag = this -> acceleration.length();
        if (mag > this -> letterManager -> getSpeedLimit())
        {
            this -> acceleration.normalize();
            this -> acceleration *= letterManager -> getSpeedLimit();
        }

        // Now for velocity.
        mag = this -> velocity.length();
        if (mag > this -> letterManager -> getSpeedLimit())
        {
            this -> velocity.normalize();
            this -> velocity *= letterManager -> getSpeedLimit();
        }

        // Bound the maximum angle speed.

        float sign = angle_speed / abs(angle_speed);
        float max_speed = PI / 10 / dt;
        if (abs(angle_speed) > max_speed)
        {
            angle_speed = sign*max_speed;
        }
    }

    this -> angle_speed *= .99; // Slow down the angle speed.

    // Keep a running update for the average position.
    average_position = this -> position * .01 + .99*average_position;

}

void Letter::transitionToNextStateIfProper(float dt)
{
    // Handle degenerate cases.

    // Handle words of length 1.
    if (!word_complete && isStartOfWord() && isEndOfWord())
    {
        setWordComplete();
    }

    // Handle Sentances of Length 1.
    if (!sentance_complete && isStartOfSentance() && isEndOfSentance())
    {
        setSentanceComplete();
    }


    // Waterfall to meander pool.
    float y_divider = getCurrentYDivider();
    if (behavior == WATERFALL && position.y > y_divider)
    {
        this -> transitionToMeanderPool();
        incrementStage();
    }


    // Meander Pools to Combine Pools.
    if ((behavior == POOL || behavior == COMBINE))// && action_delay > 0)
    {
        action_delay -= dt;

        // LETTER POOL.
        if (current_stage == LETTER_POOL && entire_word_in_stage())
        {
            transitionToCombinePool();
            incrementStage(); // On the first divider, so no increment.
        }

        // COMBINE POOL.
        if (current_stage == LETTER_COMBINE && word_complete)
        {
            transitionToWaterfall();
            incrementStage();
        }

        // WORD POOL.
        if (current_stage == WORD_POOL && entire_sentance_in_stage())
        {
            transitionToCombinePool();
            incrementStage();
        }

        // WORD COMBINE -> Sentance scroll.
        if (current_stage == WORD_COMBINE && sentance_complete)
        {
            scroll_delay -= dt;
            if(letterManager -> isScrollReady() &&
               this -> sentance_index == letterManager -> get_scroll_index() && (scroll_delay < 0))
            {
                transitionToScroll();
                incrementStage();
                letterManager -> setScrollDelay(1.5*char_height / letterManager -> getTextScrollSpeed());
            }
        }
    }

}

void Letter::incrementStage()
{
    Letter * search = findStartOfConnectedGroup();
    Letter * last_search;

    // Set elements from left to right.
    do
    {
        last_search = search;
        search -> current_stage++;
        search = search  -> letter_to_my_right;
    } while (last_search -> connected_right);

}

void Letter::avoidOthers(float dt)
{
    std::set<Body *> neighbors;

    collision_detection_grid -> findNeighbors(this, neighbors);
    
    float repelling_force = letterManager -> getRepellingForce();

    for (auto iter = neighbors.begin(); iter != neighbors.end(); ++iter)
    {
        Body * b = *iter;

        // Avoid body if not magnetting.
        if(b != letter_to_my_left || !magnet_left)
        {
            ofVec2f offset = this -> position - b -> getPosition();

            float mag = offset.length();
            offset *= repelling_force*dt / mag / mag;
            this -> velocity += offset;
        }
    }
}

void Letter::move(float dt)
{
    // Simply test out fluid movement.
    //fluidTest(dt);

    storePosition();

    // Remove the old references to this object stored in the grid.
    if (collision_detection)
    {
        collision_detection_grid -> remove_from_collision_grid(this);
    }

    angle += this -> angle_speed*dt;
   

    stepPosition(dt);

    // Safeguard against bad positioning.
    if (isnan(this->position.x))
    {
        this->revertToPrevious();
        this->velocity = ofVec2f(0, 0);
        angle_speed = 0;
    }

    // Manually enforce spacing.
    if (connected_left)
    {
        ofVec2f offset = position - letter_to_my_left -> position;
        float max_offset = getOffsetFromLeft() * 1.2;
        float min_offset = getOffsetFromLeft() * .8;
        float current_offset = offset.length();
        if (current_offset > max_offset)
        {
            float factor = max_offset / current_offset;
            factor = MAX(factor, .7);
            offset *= factor;
        }
        if (current_offset < min_offset)
        {
            float factor = min_offset / current_offset;
            factor = MIN(factor, 1.3);
            offset *= factor;
        }
        position = letter_to_my_left -> position + offset;
    }

    /*
    if (!connected_left)
    {
        angle = 0;
    }*/

    boundMovement(dt);

    if(collision_detection && this -> position.y > -char_height)
    {
        updateCollidableFromPosition();

        // Delay entering the screen if it is coming to a collision with another letter.
        if(collision_detection_grid -> detect_collision(this))
        {
            this -> position.y -= -this -> char_height*2;
        }
        else
        {
            // Add the new references to this object to the grid.
            collision_detection_grid -> add_to_collision_grid(this);
        }
    }


    //letterManager -> addVelocityToMeander(this -> position, this -> velocity);

}

void Letter::fluidTest(float dt)
{
    if (position.y < 0)
    {
        position.y = ofGetHeight() / 2;
    }

    velocity = letterManager->getMeanderVelocityAtPosition(position);
    position += velocity*dt;
    //letterManager->addVelocityToMeander(this->position, this->velocity);
    return;
}

void Letter::storePosition()
{
    // Store the current position, so that it may be reinstated as necessary.
    this -> previous_position = this -> position;
    this -> previous_angle = this -> angle;
}

void Letter::boundMovement(float dt)
{
    // -- bound the movement, to avoid letters escaping velocity.
    if (position.x < 0)
    {
        position.x = 0;
    }

    // Don't allow meandering to scrunch the letters against the right wall.

    float right_bound = ofGetWidth() - getRemainingLength();
    if (position.x > right_bound)
    {
        position.x = right_bound;
    }
}

float Letter::getRemainingLength()
{
    if (letter_to_my_right == NULL)
    {
        return 0;
    }
    return letter_to_my_right -> getOffsetFromLeft() + letter_to_my_right -> getRemainingLength();
}

void Letter::resolve_collision(float dt)
{
    // Grid will broadly detect colliding bodies.
    // the body will rever the position to 'previous_position' if necessary.
    this -> collision_detection_grid -> resolve_collisions(this);
}

void Letter::draw()
{

    // Perform texture allocation here in the serial loop to prevent multiple binding problems.
    if (!allocated)
    {
        this -> init_texture(this -> character);
    }

    // We retrieve a snapshot of the values at this point in time, so that drawing is consistent.
    float x = position.x;
    float y = position.y;
    float angle = this -> angle;

    
    ofPushMatrix();//will isolate the transform
    ofTranslate(x, y); // Move the 0 coordinate to location (px, py)
    ofRotate(ofRadToDeg(angle));
    int w = this -> char_width;  //fbo.getWidth();
    int h = this -> char_height; //fbo.getHeight();
    
    //int h = this -> char_height; //fbo.getHeight();

    // Draws this glyph with the origin at its center point.
    // draws in local space, because of ofTranslate call.
    // It is then automatically translated to world space.
    // This needs to be drawn aligned with the left border for text scrolling,
    // but the middle alignment works better for rotations.
    //this -> fbo.draw(0, -h/2);//-w/2 This is what I used when baking textures.

    //this -> fbo.draw(x, y -h / 2);

    // -- With matrices is too slow.
    ofTrueTypeFont * font = fontManager -> getFont(font_size_index, font_italics);
    font -> drawString(this -> str, 0, 0);//font -> getAscenderHeight() + h);

    // Without matrices is too slow.
    //font -> drawString(this -> str, x, y + font->getAscenderHeight() - h / 2);

    // Bitmap string is too slow.
    //ofDrawBitmapString(this->str, x, y + font->getAscenderHeight() - h / 2);

    //ofDrawCircle(ofVec2f(x, y), 5);

    
    ofPopMatrix();//will stop other things from being drawn rotated
    

    
    #ifdef DEBUG
    this->collidable->draw(x, y, (angle));
    #endif
    //this->collidable->draw(x, y, (angle));
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
        dead = (behavior == TEXT_SCROLL && position.y > letterManager -> getBottomY() || dead);
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
    switch (behavior)
    {
        case WATERFALL:   stepWaterfallA(dt); break;
        case POOL:        stepPoolA(dt);      break;
        case COMBINE:     stepCombineA(dt);   break;
        case TEXT_SCROLL: stepTextScrollA(dt);break;
    }
}

void Letter::stepVelocity(float dt)
{
    switch (behavior)
    {
        case WATERFALL:   stepWaterfallV(dt);  break;
        case POOL:        stepPoolV(dt);       break;
        case COMBINE:     stepCombineV(dt);    break;
        case TEXT_SCROLL: stepTextScrollV(dt); break;
    }
}

void Letter::stepPosition(float dt)
{
    switch (behavior)
    {
        case WATERFALL:   stepWaterfallP(dt);  break;
        case POOL:        stepPoolP(dt);       break;
        case COMBINE:     stepCombineP(dt);    break;
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

    float terminal_velocity = letterManager -> getTerminalVelocity();

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

    this -> acceleration = ofVec2f(0, this -> letterManager -> getGravity());

    // Add wind.
    if(this -> position.y > 0)
    {
        ofVec2f wind = letterManager -> getWindVelocityAtPosition(this -> position);
        this -> acceleration += wind * this -> letterManager -> getWindFactor() * mass;
    }
    else
    {
        //velocity.x = 0;
    }

    dynamicsA(dt);
}

void Letter::stepWaterfallV(float dt)
{
    dynamicsV(dt);

    if (velocity.y < 0)
    {
        velocity.y *= .9;
    }

    // Chase leader if it has one.
    if(connected_left)
        chaseLeaderV(dt);
}

void Letter::stepWaterfallP(float dt)
{
    // Ensure that the letters keep moving.
    if (this -> velocity.length() < 1)
    {
        this -> velocity.y += .01;
        this -> velocity.normalize();
    }
    dynamicsP(dt);
}


// -- Stage 2: Pool behavior.

void Letter::stepPoolA(float dt)
{


    ofVec2f velocity = letterManager -> getMeanderVelocityAtPosition(this -> position);
    this -> acceleration = velocity * this -> letterManager -> getMeanderingDamping(combine_stage);

    bool free;
    ofVec2f target_position = this -> getTargetPosition(&free, dt);

    // Apply singularity for the leader.
    if (free)
    {
        this -> acceleration = ofVec2f(ofRandom(20) - 10, 0);
    }

    // Don't accelerate the letter if we are near a dead zone.
    float dead_zone = letterManager->getDeadZoneHeight();
    if (abs(this->position.x - 0) < dead_zone ||
        abs(this->position.x - ofGetWidth() - getRemainingLength()) < dead_zone)
    {
        this->acceleration = ofVec2f(0, 0);
    }

    // Special boundary avoidance could be put here, but it has created mysterious bugs in the past.
    
    return;



    // Constantly Dampen Accelerations.
    //acceleration.y *= .999;
    //acceleration.x *= .999;

    // Oscilate the horizontal movement.
    //vx = 20 * cos(ofGetElapsedTimef());

    // Interpolate the velocity to the pool speed.

    // Move the letter closer towards the center.

    target_position = this -> getTargetPosition(&free, dt);

    // Letter is in a sentance. i.e. not free.
    if (!free)
    {
        /*
        float snap_percentage = 1.0; // 0.0 - 1.0

        ofVec2f desired_velocity = (target_position - this -> position);

        // Velocity is skewed towards the letter's position in the sentance.
        acceleration = desired_velocity - this -> velocity;
        acceleration *= 50;
        */

        ofVec2f velocity = letterManager -> getMeanderVelocityAtPosition(this -> position);
        this -> acceleration = velocity * this -> letterManager -> getMeanderingDamping(combine_stage);

    }

    // letter is free roaming or the leader of a word.
    if (free)
    {

        // This has the letter rotate around the center of the pool.
        /*
        float center_y = (letterManager -> getScrollY() + letterManager -> getPoolY())/2;
        ofVec2f center = ofVec2f(ofGetWidth()/2, center_y);
        ofVec2f toCenter = center - this -> position;
        ofVec2f perpCenter = toCenter;
        float temp = -perpCenter.y;
        perpCenter.y = perpCenter.x;
        perpCenter.x = temp;
        this -> acceleration = toCenter*40 + perpCenter*40;
        */

        // Meandering behavior.

        // Simply move the letter along a divergence - free vector field 
        // with no slip conditions on the boundaries.
        ofVec2f velocity = letterManager -> getMeanderVelocityAtPosition(this -> position);
        this -> acceleration = velocity * this -> letterManager -> getMeanderingDamping(combine_stage);

        // Accelerate letters away from singularities.
        ofVec2f singularity_avoidance = this -> position - this -> average_position;
        float len = singularity_avoidance.length();
        float mag = this -> letterManager -> getMeanderingSpeed(combine_stage) *
                    this -> letterManager -> getMeanderingDamping(combine_stage)/len/len;
        singularity_avoidance *= mag;

        this -> acceleration += singularity_avoidance;


        /* To meander the word should maintain a constant velocity,
         * perhaps slower as the word gets longer.
         * 
         * The main change will be in direction.
         * To change the direction, I will choose a random direction and then shoot 2 rays out from
         * the letter, the direction of the word will be changed weighted by the longer distance
         * found.
         */

        /*
        // Try just rotating the angle by counter - clockwise by a random amount.
        //float angle = atan2(velocity.y, velocity.x);
    
        // First we obtain a list of relevant linesegments to cast the ray upon.
        vector<LineSegment*> * segments = letterManager -> getPoolBoundaries();

        // Generate a Random 2D direction in radians.
        float angle = ofRandom(PI*2);
        float dx = cos(angle);
        float dy = sin(angle);
        ofVec2f direction = ofVec2f(dx, dy);

        // Create the two rays along this direction.
        Ray * ray1, * ray2;

        ray1 = new Ray(this -> position, direction);
        ray2 = new Ray(this -> position, -direction);

        // Find the intersection point of farthest distance from this letter.
        ofVec2f far_point;
        float far_dist = 0;

        for (auto iter = segments -> begin(); iter != segments -> end(); ++iter)
        {
            LineSegment seg = **iter;

            float dist1 = ray1 -> dist_to_intersection(seg);
            float dist2 = ray2 -> dist_to_intersection(seg);

            // Note: can only be greater if it has been found and positive anyways...
            if (dist1 > far_dist)
            {
                far_dist = dist1;

                // We need the point for the position,
                // in addition to the directionality.
                far_point = ray1 -> getPointAtTime(far_dist);
            }

            if (dist2 > far_dist)
            {
                far_dist = dist2;
                
                // We need the point for the position,
                // in addition to the directionality.
                far_point = ray2 -> getPointAtTime(far_dist);
            }
        }

        // ASSUMPTION: far point is defined,
        // because the line segments represented a
        // bounded region (no gaps).
        // and our letter will not be travelling outside of this region.

        /*
        // Now we compute the acceleration vector.
        this -> acceleration = (far_point - this -> position)*letterManager -> getMeanderingDamping();
        */

        /*
        // Current direction the letter is heading in.
        angle = atan2(velocity.y, velocity.x);
        angle += ofRandom(letterManager -> getTurnSpeed());//far_dist / 20000);
        dx = cos(angle);
        dy = sin(angle);
        direction = ofVec2f(dx, dy);

        velocity = direction;
        acceleration = ofVec2f(0, 0);

        // The acceleration vector will be applied in stepPoolV().
        */
    }
}

void Letter::chaseLeaderV(float dt)
{
    bool free;
    ofVec2f target_position = this->getTargetPosition(&free, dt);

    float turn_speed = this->letterManager->getTurnSpeed();

    if (!free)
    {
        ofVec2f desired_velocity = (target_position - this->position) / dt;

        float speed = desired_velocity.length();

        if (speed > .001)
        {
            // Chasing letters, should be able to exceed the meander speed to catch up with the
            // word they want to connect to or follow.
            float chase_factor = 1.5;

            float max_speed = letterManager->getMeanderingSpeed(combine_stage) * chase_factor;

            float speedup = min(speed, max_speed) / speed;

            //this -> velocity = desired_velocity*speedup;
            //this -> velocity = desired_velocity * speedup + this -> acceleration * dt;
            // Interpolate between the meandering velocity and the following velocity.
            this -> velocity = this -> velocity * .1 + desired_velocity*speedup * .9;

            // Update angle to match where this letter is going.
            float angle = getGlyphAngle(desired_velocity);

            // Gradually alter the angle to desired.
            setAngleSpeed(angle, turn_speed / dt);
            //this -> angle_speed = angle;
        }
    }
}

void Letter::stepPoolV(float dt)
{
    dynamicsV(dt);


    bool free;
    ofVec2f target_position = this -> getTargetPosition(&free, dt);

    if (!free)
    {
        //ofVec2f desired_velocity = ofVec2f((target_position.x - this -> position.x) / dt, 0);
        ofVec2f desired_velocity = (target_position - this -> position)/dt;

        /*
        float per = .01;
        float per_c = 1.0 - per;
        float desired_position = getCurrentYDivider();

        float vel = (desired_position - this->position.y);
        vel = vel / abs(vel)*MIN(abs(vel), abs(letterManager->getTerminalVelocity()))*dt;
        */

        float speed = desired_velocity.length();

        float chase_factor = 1.5;
        float max_speed = letterManager->getMeanderingSpeed(combine_stage) * chase_factor;

        if(speed > max_speed)
        {
            // Chasing letters, should be able to exceed the meander speed to catch up with the
            // word they want to connect to or follow.
            float speedup = min(speed, max_speed) / speed;

            //this -> velocity = desired_velocity*speedup;
            //this -> velocity = desired_velocity * speedup + this -> acceleration * dt;
            // Interpolate between the meandering velocity and the following velocity.
            this -> velocity = this -> velocity * .1 + desired_velocity*speedup * .9;
        }

        this -> velocity.y = desired_velocity.y;

    }

    
    // -- The Leader Meanders using the acceleration vector described in stepPoolA()
    if (free)
    {       

        // dynamics has already applied the acceleration,
        // so all that remains is to clip the velocity to the meandering velocity.


        this -> velocity.normalize();
        this -> velocity *= letterManager -> getMeanderingSpeed(combine_stage);

        // Move letters towards the line.
        if (abs(target_position.y - this->position.y) > 1)
        {
            float toY = (target_position.y - position.y);
            this -> velocity.y = MIN(toY/ dt, toY/abs(toY)/dt);
        }
        else
        {
            this -> velocity.y = 0;
        }


        if(ofRandom(1) < .01)
        {
            this -> velocity *= -1;
        }
    }
}

void Letter::stepPoolP(float dt)
{
    dynamicsP(dt);
    
    //this -> position.y; += vel;
    haveLetterFaceUpwards();
}

void Letter::stepCombineA(float dt)
{
    stepPoolA(dt);
}

void Letter::stepCombineV(float dt)
{
    stepPoolV(dt);
}

void Letter::stepCombineP(float dt)
{
    stepPoolP(dt);
}


// Returns an appropriate position based on the stage.
ofVec2f Letter::getTargetPosition(bool * free, float dt)
{
    *free = false;

    ofVec2f output;

    // Chase neighbors if in combine stage.
    if (behavior == COMBINE)
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

            if (!connected_left)
            {
                // NOTE: We don't need dist_sqr if connected_left is already true.
                // same for symmetrical case below.
                ofVec2f offset = output - this->position;
                float dist_sqr = offset.lengthSquared();
                float threshold = letterManager -> combineThresholdDistance()*dt;
                threshold = threshold*threshold;
                if (dist_sqr < threshold) // 10^2
                {
                    setMagnet(LEFT, false);
                    letter_to_my_left->setMagnet(RIGHT, false);

                    connect_to_left(); // Update letter, word, and sentance connectivity values.

                    setDriving(LEFT);

                    action_delay = getStageDelay();
                    letter_to_my_left -> action_delay = action_delay;
                    // FIXME: shoudl the letter to the right have a combine delay as well.
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
                float offset = output.x - this -> position.x;
                float threshold = letterManager -> getMeanderingSpeed(combine_stage);
                
                if (offset < threshold)
                {
                    setMagnet(RIGHT, false);
                    letter_to_my_right->setMagnet(LEFT, false);

                    connect_to_right(); // Update letter, word, and sentance connectivity values.
                    setDriving(LEFT);
                    action_delay = getStageDelay();
                    letter_to_my_right -> action_delay = action_delay;
                }
            }

            return output;
        }

        // -- Magnetize letters if they meet the requirements for connection.
        // but delay combines to allow for proper circulation.


        // Magnetize left if the delays are proper and the object is in the correct pool.
        if (!connected_left &&
            ((current_stage == LETTER_COMBINE && !isStartOfWord()) || 
             (current_stage == WORD_COMBINE  && !isStartOfSentance())) &&
             action_delay < 0
           )
        {
            setMagnet(LEFT, true);

            // We are no longer allowing right leaders.
            //letter_to_my_left -> setMagnet(RIGHT, true);
            setDriving(LEFT);

            // Don't drive right, because it is broken.
            //letter_to_my_left->setDriving(RIGHT);

            getOffsetPositionFromLeft(&output);
            return output;
        }

    }


    // Follow left or right connections.

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


    // If this letter is the leader then it wants to be right where it is.
    if (letter_to_my_left == NULL)
    {
        if (behavior == TEXT_SCROLL)
        {
            return ofVec2f(left_scroll_margin + getOffsetFromLeft(), this -> position.y);
        }

        // The Leader is free however.
        *free = true;

        if (behavior == COMBINE || behavior == POOL)
        {
            return ofVec2f(this -> position.x, this -> getCurrentYDivider());
        }

        // The Leader is free however.
        *free = true;
        return this -> position;
    }    
    

    // - Text Scroll follower behavior.
    if (behavior == TEXT_SCROLL)
    {
        bool free1;
        return this->letter_to_my_left->getTargetPosition(&free1, dt) + ofVec2f(getOffsetFromLeft(), 0);
    }
    
    // Default Circulation behavior.
    *free = true; // Calling function can freely move this letter.
    return this -> position;
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

void Letter::setGroupBehavior(Behavior behavior)
{
    Letter * search = findStartOfConnectedGroup();
    Letter * last_search;

    // Set elements from left to right.
    do
    {
        last_search = search;
        search -> behavior = behavior;
        search = search -> letter_to_my_right;
    } while (last_search -> connected_right);
}

void Letter::setGroupCombineStage(Combine_Stage stage)
{
    Letter * search = findStartOfConnectedGroup();
    Letter * last_search;

    // Set elements from left to right.
    do
    {
        last_search = search;
        search -> combine_stage = stage;
        search = search -> letter_to_my_right;
    } while (last_search -> connected_right);
}

inline bool Letter::getOffsetPositionFromLeft(ofVec2f * output) // ASSUMES that we have a left letter.
{
    if (this -> letter_to_my_left == NULL)
    {
        return false;
    }


    // Get angle leader to this.
    ofVec2f offset = this -> position - this -> letter_to_my_left -> position;

    // NOTE: Using offset produces snake.
    // Using left's angle produces leaf like.
    float angle = this -> letter_to_my_left -> angle; //atan2(offset.y, offset.x);
    float dx = cos(angle);
    float dy = sin(angle);

    // Ensure left to right orientation.
    /*
    if (dx < 0)
    {
        dx *= -1;
    }*/

    float mag = this -> getOffsetFromLeft();

    offset = ofVec2f(dx*mag, dy*mag);

    *output = this -> letter_to_my_left -> position + offset;
    return true;
}

inline bool Letter::getOffsetPositionFromRight(ofVec2f * output) // ASSUMES that we have a left letter.
{
    if (this -> letter_to_my_right == NULL)
    {
        return false;
    }

    // for leader to this angle.
    ofVec2f offset = this -> position - this -> letter_to_my_right -> position;
    float angle = this -> letter_to_my_right -> angle; //atan2(offset.y, offset.x);

    float dx = cos(angle);
    float dy = sin(angle);
    float mag = this -> letter_to_my_right -> getOffsetFromLeft();

    offset = ofVec2f(dx*mag, dy*mag);

    // FIXME: Include rotations.
    *output = this -> letter_to_my_right -> position - offset;
    return true;
}

inline bool Letter::should_goto_right_of_left()
{

    if (combine_stage == ALONE)
    {
        return false;
    }

    if (this->letter_to_my_left == NULL)
    {
        return false;
    }

    // ASSUMPTION: a left letter exists.

    if (connected_left)
    {
        return true;
    }

    // ASSUMPTION: we are not yet connected to it.

    if (this->letter_to_my_left->inZone() == false)
    {
        return false;
    }

    // ASSUMPTION: The left letter is in the pool searching and swimming for its mates.

    if (combine_stage == PARTIAL_WORD)
    {
        if (!isStartOfWord())
        {
            return true; // going towards left.
        }

        return false; // Start of word, not yet connected to the whole word.
    }

    // ASSUMPTION: We have a complete word now that should move towards its left neighbbor when it is ready.

    // letters follow leaders within complete words and follow the ends of the
    // words to their left when they are complete.    
    if (!isStartOfSentance() && this->letter_to_my_left->isWordComplete())
    {
        // -- FIXME: Remove me.
        if (this->isStartOfWord())
        {
            return true;
        }

        return true;
    }

    // complete SENTANCE state letters should not need to move.
    return false;
}


inline bool Letter::should_goto_left_of_right()
{

    return false;

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

    if(this -> letter_to_my_right -> inZone() == false)
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
    // Letters to default scroll behavior.
    velocity.x = 0;
    velocity.y = this -> letterManager -> getTextScrollSpeed();

    
    // Then correct the behavior of the letters.
    //float scroll_start = letterManager -> getYDivider(SENTANCE_LEFT_ALIGN);
    //float scroll_end   = letterManager -> getYDivider(SENTANCE_SCROLL);

    // Have the text simply come out of the pool, not aligning to the left outside of the pool line.
    float scroll_start = getCurrentYDivider();
    float scroll_end   = scroll_start + 1;

    velocityInterpolateToTargetBasedOnY(scroll_start, scroll_end, dt);

    tryToEjectLetterFromPool(scroll_start, dt);

    haveLetterFaceUpwards();

}

void Letter::velocityInterpolateToTargetBasedOnY(float y_start, float y_end, float dt)
{
    ofVec2f leader_position = this -> findStartOfSentance() -> position;


    if (leader_position.y > y_start)
    {
        // Interpolate based on progress of the leader.
        float y_dist = leader_position.y - y_start;
        float offset = y_end - y_start;

        float percentage_x = MAX(0, MIN(y_dist / offset, 1));
        float percentage_y = MAX(0, MIN(y_dist / offset, 1));

        bool free;

        ofVec2f target = getTargetPosition(&free, dt);

        velocity.x += (target.x - this->position.x) / dt*(percentage_x*percentage_x*percentage_x);
        velocity.y += (target.y - this->position.y) / dt*percentage_y;
        // We want the sentance to gradually interpolate by the time it reaches the to scroll stage.

    }

}

void Letter::tryToEjectLetterFromPool(float y_start, float dt)
{

    // Quickly eject the sentance form the pool, then scroll at constant scroll speed.
    float dist_to_pool_end_y = y_start - this -> position.y;

    if (dist_to_pool_end_y > 0)
    {
        velocity.y *= 3;
    }
    // Next scroll once per sentance.
    else if (!left_pool && isStartOfSentance())
    {
        // Let the letter Manager scoll another sentance.
        this->letterManager->next_scroll();
        left_pool = true;
    }
}

void Letter::haveLetterFaceUpwards()
{

    while (angle < 0)
    {
        angle += PI*2;
    }

    while (angle > 2 * PI)
    {
        angle -= 2*PI;
    }

    //angle = angle % PI*2;

    // Bring the letter facing upwards.
    float angle_speed1 = -angle*move_to_left;
    float angle_speed2 = (PI * 2 - angle)*move_to_left;

    // Set angle speed to the direction with lowest absolute value.
    if (-angle_speed1 < angle_speed2)
    {
        this->angle_speed = angle_speed1;
    }
    else
    {
        this->angle_speed = angle_speed2;
    }

    //angle = 0;
}

// ASSUMPTIONS: target is positive and < 2*PI.
// angle is positive and < 2*PI
// percentage is between 0 and 1.
void Letter::setAngleSpeed(float target, float percentage)
{

    // find both angular directions.
    float angle_speed1 = abs(target - angle);
    float angle_speed2 = 2*PI - angle_speed1;

    float sign = 1;
    if (target < angle)
    {
        sign = -1;
    }

    // Set angle speed to the direction with lowest absolute value.
    if (angle_speed1 < angle_speed2)
    {
        this -> angle_speed = sign*angle_speed1*percentage;
    }
    else
    {
        this -> angle_speed = -sign*angle_speed2*percentage;
    }
}

void Letter::stepTextScrollP(float dt)
{
  dynamicsP(dt);

  bool free;
  ofVec2f target_position = getTargetPosition(&free, dt);
  this -> position = target_position;

  //this -> position.y = (int)(this -> position.y);

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

/*
void Letter::revertToPrevious()
{
    // Rewind time.
    float dt = -.01;//this -> last_dt;

    grid -> remove_from_collision_grid(this);
    this -> position += this -> velocity * dt;
    this -> angle    += this -> angle_speed * dt;
    this -> updateCollidableFromPosition();
    grid -> add_to_collision_grid(this);

}*/

void Letter::updateCollidableFromPosition()
{   
    if(this -> collidable != NULL)
    {
        float width_half = this->char_width / 2;
        float dx = cos(this -> angle) * width_half;
        float dy = sin(this -> angle) * width_half;

        this -> collidable -> updatePositionRotation(position.x + dx, position.y + dy, this -> angle);
    }
}


// -- Stage 2 Methods.

void Letter::setRightLetter(Letter * right)
{
    this -> letter_to_my_right = right;
}

bool Letter::inZone()
{
    return true;
}


void Letter::connect_to_left()
{
    if (combine_stage == ALONE)
    {
        combine_stage = PARTIAL_WORD;
    }
    if (letter_to_my_left -> combine_stage == ALONE)
    {
        combine_stage = PARTIAL_WORD;
    }
    

    connected_left = true;
    letter_to_my_left -> connected_right = true;
    update_word_sentance_connectivity();

    return;
}

void Letter::connect_to_right()
{
    if (combine_stage == ALONE)
    {
        combine_stage = PARTIAL_WORD;
    }
    if (letter_to_my_right -> combine_stage == ALONE)
    {
        combine_stage = PARTIAL_WORD;
    }

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
        //search -> combine_stage = PARTIAL_SENTANCE;
        search -> sentance_pooling_delay = letterManager -> getWordToSentancePoolDelay();
        //cout << search -> character;

        search -> action_delay = getStageDelay();

        // Demagnetize all of the elements, so that the word flows to the sentance phase before continuing to combine.
        search -> magnet_left = false;
        search -> magnet_right = false;

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
        search -> action_delay = getStageDelay();

        if(search -> letter_to_my_left != NULL)
        {
            search -> connected_left  = true;
        }
        if(search-> letter_to_my_right != NULL)
        {
            search -> connected_right = true;
        }

        // Iterate.
        search = search -> letter_to_my_left;
    } while (latest_search -> isStartOfSentance() == false);


    // Forwards.
    search = this;
    do
    {
        latest_search = search;
        search -> sentance_complete = true;
        search -> action_delay = getStageDelay();

        if (search-> letter_to_my_left != NULL)
        {
            search -> connected_left  = true;
        }
        if (search -> letter_to_my_right != NULL)
        {
            search -> connected_right = true;
        }

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

void Letter::enable_collision_detection()
{
    if(!collision_detection)
    {
        collision_detection = true;
        //grid -> add_to_collision_grid(this);

        // From Body.h
        this -> activateCollider();
    }
}

void Letter::disable_collision_detection()
{
    if(collision_detection)
    {
        collision_detection = false;
        collision_detection_grid -> remove_from_collision_grid(this);

        // From body.h
        this -> deactivateCollider();
    }
}

void Letter::transitionToMeanderPool()
{
    setGroupBehavior(POOL);

    action_delay = this -> getStageDelay();// ALONE.

    // Letters in the pool no longer care about collision detection.
    disable_collision_detection();

    this -> acceleration = ofVec2f(0, 0);
    this -> velocity = ofVec2f(0, 0);

    this -> angle_speed = 0;
}

void Letter::transitionToCombinePool()
{
    setGroupBehavior(COMBINE);
    action_delay = this -> getStageDelay();// ALONE.
}

void Letter::transitionToWaterfall()
{
    enable_collision_detection();
    setGroupBehavior(WATERFALL);
}

void Letter::transitionToScroll()
{
    // We need to artificially combine the words if they haven't yet merged.
    this -> setSentanceComplete();
    this -> setGroupBehavior(TEXT_SCROLL);
    cout << "Index scrolled = " << this -> sentance_index << endl;

    // In the entire sentance,
    // we need to convert letters into words,
    // so that kerning is proper.
    combineWordGroups();
}


float Letter::getStageDelay()
{
    switch (combine_stage)
    {
        case ALONE:        return letterManager -> get_combine_delay_letters();
        case PARTIAL_WORD: return letterManager -> get_combine_delay_words();
        case PARTIAL_SENTANCE: return letterManager -> get_combine_delay_sentances();
        case SENTANCE: return letterManager -> get_max_time_between_scrolls();
    }

    return 0;
}

float Letter::getGlyphAngle(ofVec2f & desired_velocity)
{
    // Update angle to match where this letter is going.
    float angle_x = desired_velocity.x;
    float angle_y = desired_velocity.y;
    if (angle_x < 0)
    {
        angle_x *= -1;
        angle_y *= -1;
    }
    float angle = atan2(angle_y, angle_x);
    return angle;
}

// Transition to the next stage.
void Letter::nextStage()
{
    current_stage += 1;
}

bool Letter::entire_word_in_stage()
{
    Letter * search = this -> findStartOfWord();
    Letter * last_search;

    int stage = this -> current_stage;
    
    do
    {        
        if (search -> current_stage < stage)
        {
            return false;
        }
        last_search = search;
        search = search -> letter_to_my_right;
    }
    while(!last_search -> isEndOfWord());

    return true;
}

bool Letter::entire_sentance_in_stage()
{
    Letter * search = this -> findStartOfSentance();
    Letter * last_search;

    int stage = this -> current_stage;

    do
    {
        if (search -> current_stage < stage)
        {
            return false;
        }

        last_search = search;
        search = search -> letter_to_my_right;

    } while (!search -> isEndOfSentance());

    return true;
}

float Letter::getOffsetFromLeft()
{
    if (behavior != TEXT_SCROLL)
    {
        // Hides spaces and indentation.
        return minnimal_offset;
    }
    else
    {
        return minnimal_offset + (maximal_offset - minnimal_offset)*fontManager -> getSpaceSizeFactor(font_size_index);
    }
}

float Letter::getRestitutionCoef()
{
    return letterManager -> getRestitutionCoef();
}

float Letter::getCurrentYDivider()
{
    if(current_stage < WORD_WATERFALL)
    {
        return letterManager -> getYDivider(0); // divider 1.
    }
    else
    {
        return letterManager -> getYDivider(1);
    }
}

void Letter::combineWordGroups()
{
    // We start at the very beginning. Thats a very good place to start.
    Letter * start = findStartOfSentance();

    // Combine every word.
    while (start != NULL)
    {
        Letter * end = start -> findEndOfWord() -> letter_to_my_right;
        Letter * current = start;
        string s;
        while (current != end)
        {
            s += current -> str;
            if (current != start)
            {
                current -> dead = true;
            }
            current = current -> letter_to_my_right;
        }
        
        start -> str = s;
        start -> letter_to_my_right = end;
        if (end != NULL)
        {
            end -> letter_to_my_left = start;
            
            float min_offset = fontManager -> getWordOffset(start -> str,
                                                            start -> font_size_index,
                                                            font_italics);
            
            string stringWithSpace = start -> str + "."; // period is size of space.
            float max_offset = fontManager -> getWordOffset(stringWithSpace,
                                                            start -> font_size_index,
                                                            font_italics);
            // Set a new offset.
            end -> minnimal_offset = min_offset;
            end -> maximal_offset  = max_offset;
        }
        else // If we've combined the last word, we need to remove ghost right connection pointers.
        {
            start -> connected_right = false;
        }

        // Transition to next word.
        start = end;
    }
}