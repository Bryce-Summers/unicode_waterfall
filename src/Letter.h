#pragma once

#include "ofMain.h"
#include "grid.h"
#include "Body.h"
#include "Collidable.h"
#include "OBB.h"
#include "LetterManager.h"


class Letter : public Body
{
public:
    Letter(float x, float y,
        Letter * letter_to_my_left,
        char character,
        ofTrueTypeFont * font,
        LetterManager * letter_manager,
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
    void init_texture(char character, ofTrueTypeFont * font);


private:

    float time = 0;


    bool dead = false;
    enum State{WATERFALL, POOL, TEXT_SCROLL};
    State state;

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

    ofTrueTypeFont * font;

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
    bool inPool();
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

    void setGroupState(State state);
    
    enum Combine_Stage {
        // Letters.
        PARTIAL_WORD, // A group of letters that are not yet a full word.

        // Words.
        PARTIAL_SENTANCE, // A group of full words that is not yet an entire sentance.

        // Sentances.
        SENTANCE};
    Combine_Stage combine_stage = PARTIAL_WORD;

    // 3 seconds.
    float time_delay_between_combines = 1;
    float combine_delay = time_delay_between_combines;

    float time_delay_between_sentance_combines = 1;
    float sentance_combine_delay = 0;

    float scroll_delay = 10;

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


    // Calculate the target position based on the left letter.
    ofVec2f getTargetPosition(bool * free);

    // -- Position and movement functions.

    // Returns true if the the conditions have been met for this letter to magnet
    // towards the left letter.
    inline bool pool_goto_right_of_left();
    inline bool pool_goto_left_of_right();

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


// Collision Detection.
private:
    LetterManager * letterManager;

    virtual Collidable * getCollidable();
    virtual bool isDynamic(){return true;}
    OBB * collidable;

    // Updates this body's position from its collidable object.
    virtual void updatePositionFromCollidable();

    virtual void updateCollidableFromPosition();
};