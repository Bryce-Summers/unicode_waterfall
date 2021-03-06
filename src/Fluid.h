#pragma once

#include "ofMain.h"


// Note all arrays are packed. (row, col). e.g. y, x.

class Fluid
{
public:
    Fluid(int numRows, int numColumns, int screen_width, int screen_height);
    ~Fluid();

// Public Interface.
public:
    
    // Take 1 step with screen time dt.
    void step(float dt);

    void setViscocity(float visc);

    // Provides the VX buffer for reading or setting.
    vector<float> * getVX();
    // Provides the VY buffer for reading or setting.
    vector<float> * getVY();

    // Returns the interpolated value from the data buffers.
    // positions and velocities are in screen space coordinates.
    ofVec2f getVelocityAtPosition(ofVec2f pos);
    void    addVelocityAtPosition(ofVec2f position, ofVec2f velocity);

    // Performs conversion from [0-numRows]x[0-numCols] space to
    // [0-numRows+2]x[0-numCols+2] space.
    void setVelocityAtRowCol(int row, int col, ofVec2f velocity);

    // Add random velocities throughout the fluid.
    void addRandomVelocities(float mag);

// Data Buffers.
private:

    // Dimensional values.
    int N;      // The total number of values per data field.
    int rowNum; // The number rows stacked vertically on top of each other.
    int colNum; // The number of columns horizontally sitting next to each other.

    float screenToLocalX;
    float screenToLocalY;

    // Constant boundary codes.
    const int COPY = 0;
    const int HORI = 1;
    const int VERT = 2;

    // Single dimensional packed array.

    // Velocity x. field value 1.
    // horizontal screen pixels / time.
    vector<float> v_x;
    // Velocity y. field value 2.
    // vertical screen pixels / time.
    vector<float> v_y;

    // Temporary buffers.
    vector<float> temp1;
    vector<float> temp2;

    float viscocity;

    void removeDivergence();
    void step(float dt, float visc);

    // -- Data lookup functions.
    float getVX(float row, float col);
    float getVY(float row, float col);
    float lookupValue(vector<float> * array, float row, float col);
    void  addToValue(vector<float> * array, float row, float col, float add);

    // Copies every element from src into dest.
    // ASSUMPTION: src and dest should have the same length.
    void copyFromTo(vector<float> * src, vector<float> * dest);
    void SWAP(vector<float> * a1, vector<float> * a2);

    int IX(int row, int col);
    void project_onto_divergence_free_field(
        vector<float> * u,
        vector<float> * v,
        vector<float> * p,
        vector<float> * div);

    void Fluid::setBoundary(int b, vector<float> * x);

    void lin_solve(int b, vector<float> * x, vector<float> * x0, float a, float c);

    void advect(
        int b,
        vector<float> * d,
        vector<float> * d0,
        vector<float> * u,
        vector<float> * v,
        float dt);
    
    void diffuse(int b, vector<float> * x, vector<float> * x0, float diff, float dt);

};

