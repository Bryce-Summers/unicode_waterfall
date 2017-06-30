#include "grid.h"



Grid::Grid(int rows, int columns, int screen_w, int screen_h)
{
    this -> createGrid(rows, columns, screen_w, screen_h);
}


Grid::~Grid()
{
}

void Grid::draw()
{
    for(int row = 0; row < numRows; row++)
    for (int col = 0; col < numCols; col++)
    {
        int index = indexAtRowColumn(row, col);
        GridCell cell = gridCells[index];
        ofSetColor(0);
        ofNoFill();
        ofRect(cell.screen_x, cell.screen_y, gridX2screen, gridY2screen);
        ofDrawBitmapString(cell.size(), cell.screen_x, cell.screen_y + 20);
    }

}

// -- Setup methods.

void Grid::createGrid(int numRows, int numColumns, int screen_w, int screen_h)
{
    this -> numRows = numRows;
    this -> numCols = numColumns;

    float cell_width = 1.0*screen_w / numColumns;
    float cell_height = 1.0*screen_h / numRows;

    this->screenX2grid = 1.0 / cell_width;
    this->screenY2grid = 1.0 / cell_height;
    this->gridX2screen = cell_width;
    this->gridY2screen = cell_height;

    int len = numRows*numColumns;

    for (int i = 0; i < len; i++)
    {
        GridCell cell = GridCell();
    
        cell.grid_x = index2Col(i);
        cell.grid_y = index2Row(i);
        cell.screen_x = gridX2screen*cell.grid_x;
        cell.screen_y = gridY2screen*cell.grid_y;

        gridCells.push_back(cell);
    }

    return;
}


// -- Public interface.

// Adds the given collidable object to this grid.
void Grid::add_to_collision_grid(Body * obj)
{
    Collidable * collidable = obj -> getCollidable();
    if (collidable == NULL)
    {
        return;
    }

    int r0, r1, c0, c1;
    populate_grid_bounds(collidable, &r0, &r1, &c0, &c1);

    for(int row = r0; row <= r1; row++)
    for (int col = c0; col <= c1; col++)
    {
        GridCell * cell = lookupCell(row, col);
        cell -> addCollidable(obj);
    }
}

// Removes the given collidable object from this grid.
void Grid::remove_from_collision_grid(Body * obj)
{

    Collidable * collidable = obj -> getCollidable();
    if (collidable == NULL)
    {
        return;
    }

    int r0, r1, c0, c1;
    populate_grid_bounds(collidable, &r0, &r1, &c0, &c1);

    for (int row = r0; row <= r1; row++)
    for (int col = c0; col <= c1; col++)
    {
        GridCell * cell = lookupCell(row, col);
        cell -> removeCollidable(obj);
    }
}

// Resolves the collisions for the given dynamic object.
// Tests the object against all objects in it's vicinity
// Returns true if collisions were detected and resolved.
bool Grid::resolve_collisions(Body * dynamic_obj)
{
    // Nothing needs doing if the input object is not dynamic.
    if(dynamic_obj -> isDynamic() == false)
    {
        return false;
    }

    Collidable * collidable = dynamic_obj -> getCollidable();
    if(collidable == NULL)
    {
       return false;
    }

    // Look up all candidates minus the object itself.
    ofRectangle aabb = dynamic_obj -> getCollidable() -> getBoundingBox();

    std::set<Body *> candidates;
    this ->lookupBodiesInBox(aabb, candidates);

    candidates.erase(dynamic_obj);

    // For now return true if we have encountered another object.
    if (candidates.size() > 0)
    {
        return true;
    }

    return false;
}


// Utility functions
void Grid::populate_grid_bounds(Collidable * obj, 
    int * r0,
    int * r1,
    int * c0,
    int * c1)
{
    // Add a collidable to every cell within the AABB 
    ofRectangle aabb = obj -> getBoundingBox();
    populate_grid_bounds(aabb, r0, r1, c0, c1);
}

void Grid::populate_grid_bounds(ofRectangle & aabb,
    int * r0,
    int * r1,
    int * c0,
    int * c1)
{
    int min_index = indexAtPoint(aabb.getTopLeft());
    int max_index = indexAtPoint(aabb.getBottomRight());

    *r0 = index2Row(min_index);
    *r1 = index2Row(max_index);

    *c0 = index2Col(min_index);
    *c1 = index2Col(max_index);

    return;
}

GridCell * Grid::lookupCell(int row, int column)
{
    int index = this -> indexAtRowColumn(row, column);
    return &(this -> gridCells[index]);
}

int Grid::indexAtPoint(ofPoint pt)
{
    float sx = pt.x;
    float sy = pt.y;

    int col = (int)(sx*this -> screenX2grid);
    int row = (int)(sy*this -> screenY2grid);

    col = ofClamp(col, 0, numCols - 1);
    row = ofClamp(row, 0, numRows - 1);

    return indexAtRowColumn(row, col);
}

inline int Grid::indexAtRowColumn(int row, int col)
{
    return row*this -> numCols + col;
}

int Grid::index2Row(int index)
{
    return index / this -> numCols;
}

int Grid::index2Col(int index)
{
    return index % this -> numCols;
}


// -- Collision detection methods.

void Grid::lookupBodiesInBox(ofRectangle aabb, std::set<Body *> & collison_set)
{
    collison_set.clear();

    int r0, r1, c0, c1;
    populate_grid_bounds(aabb, &r0, &r1, &c0, &c1);

    // Adds all collidables found to the set.
    for (int row = r0; row <= r1; row++)
    for (int col = c0; col <= c1; col++)
    {
       GridCell * cell = lookupCell(row, col);
       cell -> addAllCollidablesToSet(collison_set);
    }
}


// FIXME: This could be a namespace level function or may already be implemented by ofRectangle.
bool Grid::detect_isect_box_box(ofRectangle aabb1, ofRectangle aabb2)
{
    return aabb1.intersects(aabb2);
}


bool Grid::detect_isect_rect_rect(ofRectangle rect1, ofRectangle rect2)
{
    // NAIVE: Find intersection between any 2 edges. elso see if the rectangles are nested on top of each other by checking 1 point for inclusion.
    // Less NAIVE: shift the frame of reference to have one box be aligned, then do some less expensive tests.
    return rect1.intersects(rect2);
}

// Returns true iff the line segments a1a2 and b1b2 intersect.
bool Grid::detect_isect_segment_segment(ofVec2f a1, ofVec2f a2, ofVec2f b1, ofVec2f b2)
{
    cout << "Grid:detect_isect_seg not yet implemented";
    return false;
}

// Returns <0 if c is to the left of ray a1a2.
// Returns  0 if c is on the line a1a2.
// Returns >0 if c is on the right of ray a1a2.
int Grid::line_side_test(ofVec2f a1, ofVec2f a2, ofVec2f c)
{
    cout << "Grid:line_side_test not yet implemented";
    return -1;
}

 ///
 // Grid Cell.
///

void GridCell::addCollidable(Body * obj)
{
    current_objects.insert(obj);
}

void GridCell::removeCollidable(Body * obj)
{
    current_objects.erase(obj);
}

void GridCell::addAllCollidablesToSet(std::set<Body *> & collision_set)
{
    for (auto iter = current_objects.begin(); iter != current_objects.end(); ++iter)
    {
        collision_set.insert(*iter);
    }
    return;
}

int GridCell::size()
{
    return current_objects.size();
}