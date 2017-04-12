#pragma once

#include "ofMain.h"

class Letter
{
public:
    Letter(float x, float y, float radius);
    ~Letter();

    // -- Public Interface.
    void update(float dt);
    void draw();
    bool isDead();

private:
    enum State{WATERFALL, POOL, TEXT_SCROLL};
    State state = WATERFALL;

    ofTexture texture;
    Letter * letter;

    // Specifies the proper offset x location from the leader.
    // Leader_x + leader width + spacing + kerning.
    float x_offset_from_leader;
    
    float radius;

    // -- Physics dynamics.
    float px, py;
    float vx, vy;
    float ax, ay;

    // The y - coordinate that letters transition from the waterfall behavior to the pooling behavior.
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

};

