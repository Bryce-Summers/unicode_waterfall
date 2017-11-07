#include "Fluid.h"


Fluid::Fluid(int nRows, int nColumns, int screen_width, int screen_height)
{
    this -> viscocity = .5;

    this -> rowNum = nRows;
    this -> colNum = nColumns;
    this -> N = (rowNum + 2) * (colNum + 2);

    // Preallocate all of the memory 
    // that the data buffers will need.
    v_x.reserve(N);
    v_y.reserve(N);
    temp1.reserve(N);
    temp2.reserve(N);

    // 0 out all of the fields.
    for (int i = 0; i < N; i++)
    {
        v_x.push_back(0);
        v_y.push_back(0);
        temp1.push_back(0);
        temp2.push_back(0);
    }

    // Compute the dimensional space translations.
    this -> screenToLocalX = 1.0*nColumns/screen_width;
    this -> screenToLocalY = 1.0*nRows   /screen_width;

}


Fluid::~Fluid()
{
}


// -- Public interface.
void Fluid::step(float dt)
{
    step(dt, this -> viscocity);
}

void Fluid::setViscocity(float visc)
{
    this -> viscocity = visc;
}

// Provides the VX buffer for reading or setting.
vector<float> * Fluid::getVX()
{
    return &v_x;
}

// Provides the VY buffer for reading or setting.
vector<float> * Fluid::getVY()
{
    return &v_y;
}

// Returns the interpolated value from the data buffers.
// ASSUMES position is in local space.
ofVec2f Fluid::getVelocityAtPosition(ofVec2f pos)
{
    // Convert to local position.
    float row = pos.y * this -> screenToLocalY;
    float col = pos.x * this -> screenToLocalX;

    // Get Screen pixels / time velocities.
    float vx = getVX(row, col);
    float vy = getVY(row, col);

    return ofVec2f(vx, vy);
}

// Position given in Screen coordinates.
void Fluid::addVelocityAtPosition(ofVec2f position, ofVec2f velocity)
{
    float x = position.x * screenToLocalX;
    float y = position.y * screenToLocalY;

    addToValue(&v_x, y, x, velocity.x);
    addToValue(&v_y, y, x, velocity.y);
}

void Fluid::setVelocityAtRowCol(int row, int col, ofVec2f velocity)
{
    // Conversion between user space and fluid computation space.
    int index = IX(row + 1, col + 1);
    v_x[index] = velocity.x;
    v_y[index] = velocity.y;
}


// -- Private functions.


void Fluid::removeDivergence()
{
    project_onto_divergence_free_field(&v_x, &v_y, &temp1, &temp2);
}

// Steps the velocity field by timestep dt.
void Fluid::step(float dt, float visc)
{

    //dt /= ;
    float dtx = dt*this -> screenToLocalX;
    float dty = dt*this -> screenToLocalY;

    // Diffuse the velocity.
    diffuse(1, &temp1, &v_x, visc, dtx);
    diffuse(2, &temp2, &v_y, visc, dty);
    copyFromTo(&temp1, &v_x);
    copyFromTo(&temp2, &v_y);

    removeDivergence();

    // Advect v_x along velocity field v_x,v_y, store in temp1.
    advect(1, &temp1, &v_x, &v_x, &v_y, dtx);
    // Advect v_y along velocity field v_x,v_y, store in temp2.
    advect(2, &temp2, &v_y, &v_x, &v_y, dty);
    copyFromTo(&temp1, &v_x);// I could optimize this be swaping pointers if I wanted to...
    copyFromTo(&temp2, &v_y);

    removeDivergence();

}

float Fluid::getVX(float row, float col)
{
    return lookupValue(&v_x, row, col);
}

float Fluid::getVY(float row, float col)
{
    return lookupValue(&v_y, row, col);
}

float Fluid::lookupValue(vector<float> * array, float row, float col)
{
    if (row >= rowNum)
    {
        row = rowNum - 1;
    }

    if (col >= colNum)
    {
        col = colNum - 1;
    }

    if (row < .5)
    {
        row = .5;
    }

    if (col < .5)
    {
        col = .5;
    }

    // Bilinear interpolation.
    int y0 = (int)row;
    float yp = row - y0;
    int y1 = y0 + 1;

    int x0 = (int)col;
    float xp = col - x0;
    int x1 = x0 + 1;

    int i00 = IX(y0, x0);
    int i01 = IX(y0, x1);
    int i10 = IX(y1, x0);
    int i11 = IX(y1, x1);

    float v00 = array -> at(i00);
    float v01 = array -> at(i01);
    float v10 = array -> at(i10);
    float v11 = array -> at(i11);

    // Complement of x percentage.
    float xpc = 1.0 - xp;
    float ypc = 1.0 - yp;

    // Linear Interpolation.
    float v0 = v00*xpc + v01*xp;
    float v1 = v10*xpc + v11*xp;

    // Bilinear Interpolation.
    float val = v0*ypc + v1*yp;

    return val;
}

void Fluid::addToValue(vector<float> * array, float row, float col, float add)
{
    if (row >= rowNum)
    {
        row = rowNum - 1;
    }

    if (col >= colNum)
    {
        col = colNum - 1;
    }

    if (row < .5)
    {
        row = .5;
    }

    if (col < .5)
    {
        col = .5;
    }

    // Bilinear interpolation.
    int y0 = (int)row;
    float yp = row - y0;
    int y1 = y0 + 1;

    int x0 = (int)col;
    float xp = col - x0;
    int x1 = x0 + 1;

    int i00 = IX(y0, x0);
    int i01 = IX(y0, x1);
    int i10 = IX(y1, x0);
    int i11 = IX(y1, x1);

    // Complement of x percentage.
    float xpc = 1.0 - xp;
    float ypc = 1.0 - yp;

    // Deposit the value amongst the 4 regions.
    array -> at(i00) += add*ypc*xpc;
    array -> at(i01) += add*ypc*xpc;
    array -> at(i10) += add*yp *xpc;
    array -> at(i11) += add*yp *xp;
}


// ASSUMPTION: src and dest have the same length.
void Fluid::copyFromTo(vector<float> * src, vector<float> * dest)
{
    for (int i = 0; i < src -> size(); i++)
    {
        (*dest)[i] = (*src)[i];
    }
}

// Returns the index of the given row and column.
// ASSUMPTION: row and col are in bounds.
int Fluid::IX(int row, int col)
{
    return (colNum + 2)*row + col;
}

// Using the Helmholtz decomposition,
  // Projects this vector field onto a divergence free field.
  // This is done be subtracting the gradient of pressure,
  // pressure is the diffusion of the -divergence.
  // the divergence is the gradient of the velocity field.
  void Fluid::project_onto_divergence_free_field(
      vector<float> * u,
      vector<float> * v,
      vector<float> * p,
      vector<float> * div)
  {
    
      // Computes the divergence field as the negative gradient of the velocity field.
      for(int j = 1; j <= rowNum; j++)
      for(int i = 1; i <= colNum; i++)
      {
          div -> at(IX(j, i)) =
              this -> screenToLocalX*
                               (
                                u -> at(IX(j, i + 1))
                               -u -> at(IX(j, i - 1))
                               )/2 +
              this -> screenToLocalY*
                               (
                                v -> at(IX(j + 1, i))
                               -v -> at(IX(j - 1, i))
                               )/2;
          p -> at(IX(j, i)) = 0; // Pressure field is initialized to 0.
      }

      // divergence = localX, localY / time.
      
      
      setBoundary(0, div);
      setBoundary(0, p);
      
      // Computes pressure as the diffusion of the divergence.
      lin_solve(0, p, div, 1, 4); // 1 part original divergence, 1.0 / 4 parts each neighbor's divergence.
      
      // Subtracts from the velocity field the gradient of the pressure field. 
      // for every cell.
      for(int j = 1; j <= rowNum; j++)
      for(int i = 1; i <= colNum; i++)
      {
          u -> at(IX(j, i)) -= 0.5f/screenToLocalX*(p -> at(IX(j, i + 1)) - p -> at(IX(j,     i - 1)));
          v -> at(IX(j, i)) -= 0.5f/screenToLocalY*(p -> at(IX(j + 1, i)) - p -> at(IX(j - 1, i    )));
      }
      
      setBoundary(1, u);
      setBoundary(2, v);
    }

  // Set the boundary.
  // b == 0 --> Boundaries do exactly what their neighbors do.
  //      1 --> Do exactly the opposite of what horizontal neighbors do.
  //      2 --> Do exactly the opposite of what vertical neighbors do.
  void Fluid::setBoundary(int b, vector<float> * x)
  {
    for(int j = 1; j <= rowNum + 1; j++)
    {
      x -> at(IX(j,     0         )) = b == 1 ? -x -> at(IX(j, 1))      : x -> at(IX(j, 1));
      x -> at(IX(j,     colNum + 1)) = b == 1 ? -x -> at(IX(j, colNum)) : x -> at(IX(j, colNum));
    }
    
    for(int i = 1; i <= colNum + 1; i++)
    {
      x -> at(IX(0,          i    )) = b == 2 ? -x -> at(IX(1,      i)) : x -> at(IX(1,      i));
      x -> at(IX(rowNum + 1, i    )) = b == 2 ? -x -> at(IX(rowNum, i)) : x -> at(IX(rowNum, i));
    }
    
    // Corners are the weighted average of their orthogonal neighbors, weighted by how close they are.
    (*x)[IX(0,                   0)] = 0.5f*((*x)[IX(0,               1)] + (*x)[IX(1,               0 )]);
    (*x)[IX(rowNum + 1,          0)] = 0.5f*((*x)[IX(rowNum + 1,      1)] + (*x)[IX(rowNum,          0 )]);
    (*x)[IX(0,          colNum + 1)] = 0.5f*((*x)[IX(0,          colNum)] + (*x)[IX(1,      colNum + 1 )]);
    (*x)[IX(rowNum + 1, colNum + 1)] = 0.5f*((*x)[IX(rowNum + 1, colNum)] + (*x)[IX(rowNum, colNum + 1 )]);
  }

void Fluid::lin_solve(int b, vector<float> * x, vector<float> * x0, float a, float c)
{
     int ITERATIONS = 20;
     for(int n = 0; n < ITERATIONS; n++)
     {
       for(int j = 1; j <= rowNum; j++)// For each cell.
       for(int i = 1; i <= colNum; i++)
       {

         x -> at(IX(j, i)) =
                          (x0 -> at(IX(j,     i    )) +
                        a*(
                           x -> at(IX(j,     i - 1)) +
                           x -> at(IX(j,     i + 1)) +
                           x -> at(IX(j - 1, i    )) +
                           x -> at(IX(j + 1, i    ))
                          ))/c;
       }
       setBoundary(b, x);
     }
}

  // advects d0 along velocity field uv with the timestep dt.
  // Stores result in d.
  void Fluid::advect(
    int b,
    vector<float> * d,
    vector<float> * d0,
    vector<float> * u,
    vector<float> * v,
    float dt)
  {
      int i0, j0, i1, j1;
      float x, y, s0, t0, s1, t1;
      
      float dtx = 1.0*colNum/ofGetScreenWidth();
      float dty = 1.0*rowNum/ofGetScreenHeight();

      // for every cell.
      for(int j = 1; j <= rowNum; j++)
      for(int i = 1; i <= colNum; i++)
      {
        x = i - dtx*(*u)[IX(j, i)];
        y = j - dty*(*v)[IX(j, i)];
        
        // Bound the implicit integration.
        if(x < 0.5f)
        {
          x = .5f;
        }
          
        if(x > colNum + .5f)
        {
          x = colNum + .5f;
        }
           
        i0 = (int)x;
        i1 = i0 + 1;
        
        if(y < 0.5f)
        {
          y = 0.5f;
        }
        if(y > rowNum + 0.5f)
        {
          y = rowNum + .5f;
        }
         
        j0 = (int)y;
        j1 = j0 + 1;
         
        // Bilinear Interpolation.
        s1 = x - i0; s0 = 1 - s1; t1 = y - j0; t0 = 1 - t1;
        (*d)[IX(j, i)] = s0*(t0*(*d0)[IX(j0, i0)] + t1*(*d0)[IX(j1, i0)]) +
                         s1*(t0*(*d0)[IX(j0, i1)] + t1*(*d0)[IX(j1, i1)]);
      }
      
      setBoundary(b, d);
  }
  
  void Fluid::diffuse(int b, vector<float> * x, vector<float> * x0, float diff, float dt)
  {
    float a = dt*diff*rowNum*colNum;
    lin_solve(b, x, x0, a, 1 + 4*a );
  }

  // Add random velocities throughout the fluid.
  void Fluid::addRandomVelocities(float mag)
  {

      //for (int j = 1; j <= rowNum; j++)
      for (int i = 1; i <= colNum; i++)
      {
        int j = 1;

        // Screen x pixels/time.
        //float vx = ofRandom(mag*2) - mag;
        //float vy = ofRandom(mag*2) - mag;
        float vx = ofRandom(mag*2);
        float vy = 0;

        float y = j/screenToLocalY;
        float x = i/screenToLocalX;

        ofVec2f vel = ofVec2f(vx, vy);
        ofVec2f pos = ofVec2f(x, y);
        
        this -> addVelocityAtPosition(pos, vel);
      }
  }

