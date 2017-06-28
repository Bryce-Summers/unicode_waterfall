#pragma once

#include "ofMain.h"
#include "grid.h"

class Letter
{
public:
    Letter(float x, float y, float radius, Letter * letter_to_my_left, char character, ofTrueTypeFont * font);
    ~Letter();

    // -- Public Interface.
    void update(float dt);
    void draw();
    bool isDead();

public:
    float getX();
    float getY();


// Init functions
private:

    void init_texture(char character, ofTrueTypeFont * font);


private:
    enum State{WATERFALL, POOL, TEXT_SCROLL};
    State state = WATERFALL;

    bool collision_detection; // Controls whether the letter needs to avoid letters and obstacles.

    char character;
    ofFbo fbo;
    ofFloatColor background_color = ofFloatColor(1.0, 1.0, 1.0);

    ofTrueTypeFont * font;


    // Indicates the letter to the left of this one in the final sentance that will be displayed.
    Letter * letter_to_my_left = NULL;

    // my x = letter_to_my_left x + width + spacing + kerning.
    // my x = left x + x_offset_from_left
    float x_offset_from_left;
    
    float radius;

    // Angle the letter texture is rotated.
    float angle = 0;
    

    // -- Physics dynamics.
    float px, py;
    float vx, vy;
    float ax, ay;

    // The y - coordinate that letters transition from the waterfall behavior to the pooling behavior.
    // define horizontal lines the separate Stage 1, Stage 2, and Stage 3.
    float pool_y_coordinate = 400;
    float text_scroll_y_coordinate = 600;

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

    float gravity = 30;// pixels / time / time.
    float terminal_velocity = 80;


    // -- Pool behavior.
    void stepPoolA(float dt);
    void stepPoolV(float dt);
    void stepPoolP(float dt);

    float pool_y_speed = 30;

    // Text Scroll behaviour.
    void stepTextScrollA(float dt);
    void stepTextScrollV(float dt);
    void stepTextScrollP(float dt);

    float text_scroll_speed = 40;


// Collision Detetion Functions.
private:
    Grid grid;

    // Removes this Letter from every grid cell that it itersects.
    void remove_from_collision_grid();

    // Adds this letter to every grid cell that the letter's bounding box intersects.
    void add_to_collision_grid();

    // Update's this letter's physical values, such as velocity,
    // to reflect the results of any collisions that this letter experiences.
    void resolve_collisions();
    std::vector<Letter *> report_collisions();

};

