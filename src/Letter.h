#pragma once

#include "ofMain.h"
#include "grid.h"
#include "Body.h"
#include "Collidable.h"
#include "OBB.h"
#include "LetterManager.h"

/*
 * This class controls the letter particles that
 * constitute the letter waterfall animation.
 *
 * There are two parameters that control behavior:
 * 1. Comine State: ALONE, PARTIAL_WORDS, PARTIAL_SENTANCES, and SENTANCES.
 * 2. behavior: WATERFALL, POOL, COMBINE, TEXT_SCROLL
 *
 * Each of these behaviors occurs in a separate division of the animation.
 * as a shorthand, we can use the division variable to see what stage a letter is in.
 */
// The sequence of the waterfall is:
// 1. Letters fall. (ALONE, WATERFALL)
// 2. Letters pool. (PARTIAL_WORDS, POOL)
// 3. Words form.   (PARTIAL
// 4. Words fall.
// 5. Words pool.
// 6. Sentances form.
// 7. Sentances slide to the left.
// 8. Sentances scroll.

// Note: Behavior is a combination of state and and combine_state.
// Their are three main stages: Letters, Words, and Sentances.
// Each of these states has three parts:
// 1. The structures form at the top.
// 2. The structures fall through the middle.
// 3. The strucutres pool at the bottom,
//    waiting for all of the neighbor structures to also be in the pool.
// The structures then move into the forming pool at the top of the next screen.

class Letter : public Body
{
public:
    Letter(
        LetterManager * letter_manager,
        float start_x, float start_y,
        Letter * letter_to_my_left,
        char character,
        float offset_from_left,
        bool space_before,
        int sentance_index);
    ~Letter();

    // -- Public Interface.
    void update(float dt); // Udpates internal state, but not position.
    void move(float dt);   // Second pass updates all of the positions.
    void resolve_collision(float dt);
    void draw();
    bool isDead();

    void kill();

public:
    float getX();
    float getY();

    // Sets the letter to my right field.
    void setRightLetter(Letter * right);

    // Set to a new position, especially when initializing to non-collision location.
    // The teleporting is permanant without allowing for reverting.
    void setPosition(float x, float y);

// Init functions
private:

    // Visuality and Collision geometry.
    void init_texture(char character);


private:

    // Determines how much a letter is affected by wind.
    float mass;


    float time = 0;

    bool dead = false;
    enum Behavior{WATERFALL, POOL, COMBINE, TEXT_SCROLL}; 

    Behavior behavior;

    // The current stage is describes the current action this letter is taking.
    int current_stage = 0;


    // 1.0 is fast, 0.0 is not at all.
    float move_to_left = 1.0;
    float left_scroll_margin = 50;

    int sentance_index;

    bool collision_detection; // Controls whether the letter needs to avoid letters and obstacles.
    void enable_collision_detection();
    void disable_collision_detection();


    char character;
    string str;
    int char_width;
    int char_height;
    bool allocated = false; // Texture and collidable object have been allocated.
    //ofFbo fbo; // 9.12.17 we will no longer be baking characters into textures.
               
    ofFloatColor background_color = ofFloatColor(1.0, 1.0, 1.0);


    // Letters implicitly form doubly linked, non-circular lists.
    // These pointers are used a lot in stage 2 when the letters are being combined into words and then into sentances.
    // Indicates the letter to the left of this one in the final sentance that will be displayed.
    // For garbage collection, etc we will query left.
    // For normal queries either left or right should be fine.
    Letter * letter_to_my_left  = NULL;
    Letter * letter_to_my_right = NULL;
    // TRUE IFF this letter was preceded by a space in the input text.
    // This parameter combined with the letter pointers and helper methods may be used to
    // determine the starting and ending points for words.
    bool space_before; 


    // my x = letter_to_my_left x + width + spacing + kerning.
    // my x = left x + x_offset_from_left
    float x_offset_from_left;

    // -- Routing functions that call sub behavior functions based on 
    void stepAcceleration(float dt);
    void stepVelocity(float dt);
    void stepPosition(float dt);

    // -- Integration based Physics calculations.
    void dynamicsA(float dt);
    void dynamicsV(float dt);
    void dynamicsP(float dt);

    // -- Waterfall behaviour.
    void stepWaterfallA(float dt);
    void stepWaterfallV(float dt);
    void stepWaterfallP(float dt);

    // Return the y bound for the current stage.
    float y_bound_top();
    float y_bound_bottom();


    // -- Pool behavior.
    void stepPoolA(float dt);
    void stepPoolV(float dt);
    void stepPoolP(float dt);

    // The target speed of circulation for the 
    const float pool_speed = 30;

    // --  Lots of helper functions for the implementation of the 
    // letter -> word -> sentance combining pool.

    // -- Letters.
    bool inZone(); // Returns true if this letter is in its currently indicated zone.

    // TRUE if this letter and the one to the left have 'snapped' together and are linked.
    // Letter level booleans.
    bool connected_left = false;
    bool connected_right = false;

    // Used to explictly allow letters to attract each other.
    bool magnet_left  = false;
    bool magnet_right = false;
    enum Direction { LEFT, RIGHT };
    void setMagnet(Direction direction, bool value);

    // Whether the letters are driven from the front or driven from the back.
    // True if left, false if right.
    bool driving = true;
    void setDriving(Direction direction);// set driving for every element of this component.

    // -- Words.
    bool isStartOfWord();
    bool isEndOfWord();
    bool word_complete = false; // Cached value.
    bool isWordComplete();  // Derives word_complete from letter level booleans.
    void setWordComplete(); // Sets word_complete to true in all letters in this word.
    /*
    Letter * start_of_word = NULL; // Cached start and end letter for sentance level transversal.
    Letter * end_of_word = NULL; // Premature optimization is the root of all evil.
    */

    /// -- Sentances.
    inline bool isStartOfSentance();
    inline bool isEndOfSentance();
    bool sentance_complete = false;
    bool isSentanceComplete(); // Derives sentance complete form the word level booleans.
    void setSentanceComplete();

    void update_word_sentance_connectivity();

    void setGroupBehavior(Behavior Behavior);
    

    // Defined in LetterManager.h
    Combine_Stage combine_stage = ALONE;
   
    // countdown before letters move on to the next combine or stage.
    float combine_delay = 0;
    float scroll_delay = 10;
    float sentance_pooling_delay = 0;

    // FIXME: ensure that the complete sentances are scrolled in the correct order.
    //int my_sentance_index;

    // Returns a pointer to the first letter in this connected group.
    Letter * findStartOfConnectedGroup();
    Letter * findEndOfConnectedGroup();

    // Returns a pointer to the first letter of this word.
    Letter * findStartOfWord();
    // Returns a pointer to the last letter of this word.
    Letter * findEndOfWord();

    // Returns a pointer to the last letter in the sentance.
    Letter * findStartOfSentance();

    // Returns a pointer to the first letter in the sentance.
    Letter * findEndOfSentance();

    void setGroupCombineStage(Combine_Stage stage);


    // Calculate the target position based on the left letter.
    ofVec2f getTargetPosition(bool * free);

    // -- Position and movement functions.

    // Determines whether letters should move to their left or right neighbors.
    inline bool should_goto_right_of_left();
    inline bool should_goto_left_of_right();


    // Puts the result in the output.
    // If the letter to my left stands still,
    // returns the position to the right of it at an appropriate
    // target distance.
    inline bool getOffsetPositionFromLeft(ofVec2f * output);// ASSUMES that we have a left letter.
    // If the letter to my right stands still, returns the location to its left at an
    // appropriate offset.
    inline bool getOffsetPositionFromRight(ofVec2f * output);// ASSUMES that we have a right

    // Connects this letter to the letter to the left or right and updates the letter,
    // word, and sentance connectivity values.
    void connect_to_left();
    void connect_to_right();

    // -- Text Scroll behaviour.
    void stepTextScrollA(float dt);
    void stepTextScrollV(float dt);
    void stepTextScrollP(float dt);

    // FIXME: After kerning works, put this back; 40
    float text_scroll_speed = 10;
    bool left_pool = false; // Signals whether a sentance has left the pool or not.


    // Gracefully adapt the angle speed to the target angle.
    void setAngleSpeed(float target, float percentage);


    // This value should be used to steer letter away from singularities.
    ofVec2f average_position;


    // Extra Helper functions.

    // Sets the velocity to interpolate this letter to the left margin
    void velocityInterpolateToTargetBasedOnY(float y_start, float y_end, float dt);

    void tryToEjectLetterFromPool(float y_start, float dt);

    // Move the angle to an upright position.
    void haveLetterFaceUpwards();

    void nextStage();


// Collision Detection.
private:
    LetterManager * letterManager;

    virtual Collidable * getCollidable();
    virtual bool isDynamic(){return true;}
    OBB * collidable;

    // Updates this body's position from its collidable object.
    virtual void updatePositionFromCollidable();

    virtual void updateCollidableFromPosition();

    void transitionToPool();

    float getStageDelay();

    float getGlyphAngle(ofVec2f & desired_velocity);
};