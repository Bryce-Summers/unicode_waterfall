#pragma once
#include "ofMain.h"
#include "CollideHeaders.h"
#include "Body.h"

/*
 * A grid that drives several behaviors in the scene.
 *
 * The grid has the following features:
 * Broad pass collision detection:
 *  - Objects in the scene keep an update of which grid cells they are located in
 *    Narrow pass polygon - polygon collision detection only needs to be performed between polygons that share a grid cell.
 */

class Body;

class GridCell
{

private:
    std::set<Body *> current_objects;

    // Physics properties that are stored along the grid.
    
    // The Letters are requested to have the following behavior:
    // buoyancy, weight of the particles, degree of turbulence, strength of the wind

    ofVec2f wind_velocity;

public:

    // The location of the grid cell.
    int screen_x;
    int screen_y;
    int grid_x;
    int grid_y;

    // Add a collidable to this grid cell.
    void addCollidable(Body * obj);
    
    // Remove a collidable from this grid cell.
    void removeCollidable(Body * obj);

    // unions every collidable in this grid cell to the given collision set.
    void addAllCollidablesToSet(std::set<Body *> & collision_set);

    int size();
};

class Grid
{
public:
    // Deocmposes a region [0,0] x [screen_w, screen_h]
    // rectangular equal regions with such that the grid
    // has the indicated rows and columns.
    Grid(int rows, int cols, int screen_w, int screen_h);
    ~Grid();

    // Visualize the grid for debugging.
    void draw();

private:
    int numRows;
    int numCols;
    std::vector<GridCell> gridCells;
    float screenX2grid;
    float screenY2grid;
    float gridX2screen;
    float gridY2screen;


// The public facing interfacial services that this grid provides.
public:

    // Removes the given collidable object from this grid.
    void remove_from_collision_grid(Body * obj);

    // Adds the given collidable object to this grid.
    void add_to_collision_grid(Body * obj);

    // Resolves the collisions for the given dynamic object.
    // Tests the object against all objects in it's vicinity
    // RETURNS true iff collisions were detected and resolved.
    bool resolve_collisions(Body * dynamic_obj);

    // Detects whether the given body collides with any other objects in the grid.
    bool detect_collision(Body * obj);

// Internal Functions that operate the grid.
private:
    
    // Grid space setup and transforms between grid index space and screen space.
    void createGrid(int numRows, int numColumns, int screen_w, int screen_h);

    // Returns the integer min and max iteration bounds for the gridCells
    // intersecting the bounding box of the given dynamic object.
    void populate_grid_bounds(Collidable * dynamic_obj, int * r0, int * r1, int * c0, int * c1);
    void populate_grid_bounds(ofRectangle & aabb, int * r0, int * r1, int * c0, int * c1);

    GridCell * lookupCell(int row, int column);

    // screen pt to grid index.
    int indexAtPoint(ofPoint pt);
    inline int indexAtRowColumn(int row, int col);

    int index2Row(int index);
    int index2Col(int index);

    // Looks up a set of all Collidable Objects on the grid that could
    // intersect the given axis aligned bounding box.
    void lookupBodiesInBox(ofRectangle aabb, std::set<Body *> & output);

    // Detect Intersection of 2 arbitrarily rotated rectangles.
    bool detect_isect_rect_rect(ofRectangle rect1, ofRectangle rect2);

    // FIXME: This could be a namespace level function or may already be implemented by ofRectangle.
    bool detect_isect_box_box(ofRectangle aabb1, ofRectangle aabb2);

    // Returns true iff the line segments a1a2 and b1b2 intersect.
    bool detect_isect_segment_segment(ofVec2f a1, ofVec2f a2, ofVec2f b1, ofVec2f b2);

    // Returns <0 if c is to the left of ray a1a2.
    // Returns  0 if c is on the line a1a2.
    // Returns >0 if c is on the right of ray a1a2.
    int line_side_test(ofVec2f a1, ofVec2f a2, ofVec2f c);

};

