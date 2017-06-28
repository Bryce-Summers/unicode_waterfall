#pragma once
#include "ofMain.h"

/*
 * A grid that drives several behaviors in the scene.
 *
 * The grid has the following features:
 * Broad pass collision detection:
 *  - Objects in the scene keep an update of which grid cells they are located in
 *    Narrow pass polygon - polygon collision detection only needs to be performed between polygons that share a grid cell.
 */

class GridCell
{
    std::set<ofPolyline> current_objects;

    // Physics properties that are stored along the grid.
    
    // The Letters are requested to have the following behavior:
    // buoyancy, weight of the particles, degree of turbulence, strength of the wind

    ofVec2f wind_velocity;

    // The location of the grid cell.
    int x;
    int y;
    
};

class Grid
{
public:
    Grid(int rows, int cols);
    ~Grid();

    std::vector<GridCell> gridCells;

    void createGrid(int numRows, int numColumns);
};

