#pragma once

/*
 * Collision Information Data Structure.
 * Written by Bryce Summers on Jun.30.2017
 * 
 * Purpose: Provide some universal data fields for describing pertinant information
 *          about a collision.
 */

#include "ofMain.h"

class CollideInfo
{
public:
    CollideInfo();
    ~CollideInfo();

    // Geometric Information.
    ofVec2f location1;
    ofVec2f location2;
    ofVec2f collision_normal;

    float time_till_collision;

    // Physics Dynamics information.
    /*
    bool dynamic1;
    bool dynamic2;
    float mass1;
    float mass2;
    */
};

