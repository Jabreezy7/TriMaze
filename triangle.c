/*
 *  Created on: Nov 25, 2025
 *  Author: Mohammed Baled
 */


#ifdef __APPLE__  // include Mac OS X verions of headers

#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>

#else // non-Mac OS X operating systems

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>

#endif  // __APPLE__

#include <stdio.h>
#include <stdlib.h>
#include "myLib.h"
#include "myLib.c"
#include <math.h>
#include <time.h>
#include "initShader.h"
#include <string.h>
#include <float.h>


// Animation Variables
int isAnimating = 0;
int currentStep = 0;
int maxFlyingToStep = 10;
int maxFlyingAroundStep = 10;
int maxFlyingDownStep = 10;
int maxSolvingStep = 10;
int maxTurningStep = 10;
int maxMoveForwardStep = 10;
int maxMoveRightStep = 10;
int maxMoveBackwardStep = 10;
int maxMoveLeftStep = 10;
int maxLookLeftStep = 10;
int maxLookRightStep = 10;

vec4 baseEyePoint;
vec4 baseAtPoint;
GLfloat alpha;
GLfloat xEyePoint;
GLfloat yEyePoint;
GLfloat zEyePoint;
GLfloat xAtPoint;
GLfloat yAtPoint;
GLfloat zAtPoint;
GLfloat yUp;
GLfloat zUp;

// Camera Variables (Initially looking straight down)
float left = -1;
float right = 1;
float bottom = -1;
float top  = 1;
float near = -1;
float far = -50;
vec4 eyePoint = {0, 5, 0, 1};
vec4 atPoint  = {0, -0.5, 0, 1};
vec4 upVector = {0, 0, -1, 0};


// Different animation states
typedef enum
{
    NONE = 0,
    FLYING_TO,
    FLYING_AROUND,
    FLYING_DOWN,
    SOLVING,
    TURNING,
    MOVE_FORWARD,
    MOVE_RIGHT,
    MOVE_BACKWARD,
    MOVE_LEFT,
    LOOK_LEFT,
    LOOK_RIGHT,

} state;

state currenState = NONE;


// Texture Patterns
vec2 floor_tex[6] = {
    {0.50, 1.00},
    {0.00, 0.50},
    {0.00, 1.00},
    {0.50, 1.00},
    {0.50, 0.50},
    {0.00, 0.50}
};

vec2 wall_tex[6] = {
    {0.50, 0.50},
    {0.00, 0.00},
    {0.00, 0.50},
    {0.50, 0.50},
    {0.50, 0.00},
    {0.00, 0.00}
};

vec2 pole_tex[6] = {
    {1.00, 0.50},
    {0.50, 0.00},
    {0.50, 0.50},
    {1.00, 0.50},
    {1.00, 0.00},
    {0.50, 0.00}
};

vec2 spinning_cube_tex[6] = {
    {1.00, 1.00},
    {0.50, 0.50},
    {0.50, 1.00},
    {1.00, 1.00},
    {1.00, 0.50},
    {0.50, 0.50}
};


// Maze Properties
#define MAZE_SIZE 8

typedef struct {
    int north;
    int east;
    int south;
    int west;
    int visited;
} Cell;

Cell maze[MAZE_SIZE][MAZE_SIZE];

int dx[4] = {0, 1, 0, -1};
int dy[4] = {-1, 0, 1, 0};

void initMaze() {
    for (int y = 0; y < MAZE_SIZE; y++)
        for (int x = 0; x < MAZE_SIZE; x++) {
            maze[y][x].north = 1;
            maze[y][x].east  = 1;
            maze[y][x].south = 1;
            maze[y][x].west  = 1;
            maze[y][x].visited = 0;
        }
}

void shuffle(int *array, int n) {
    for (int i = n-1; i > 0; i--) {
        int j = rand() % (i+1);
        int tmp = array[i];
        array[i] = array[j];
        array[j] = tmp;
    }
}

void generateMazeDFS(int x, int y) {
    maze[y][x].visited = 1;

    int dirs[4] = {0,1,2,3};
    shuffle(dirs,4);

    for (int i = 0; i < 4; i++) {
        int dir = dirs[i];
        int nx = x + dx[dir];
        int ny = y + dy[dir];

        if (nx >= 0 && nx < MAZE_SIZE && ny >= 0 && ny < MAZE_SIZE && !maze[ny][nx].visited) {
            if (dir == 0) { maze[y][x].north = 0; maze[ny][nx].south = 0; }
            if (dir == 1) { maze[y][x].east  = 0; maze[ny][nx].west  = 0; }
            if (dir == 2) { maze[y][x].south = 0; maze[ny][nx].north = 0; }
            if (dir == 3) { maze[y][x].west  = 0; maze[ny][nx].east  = 0; }
            generateMazeDFS(nx, ny);
        }
    }

    maze[0][0].west = 0;
    maze[MAZE_SIZE-1][MAZE_SIZE-1].east = 0;
}

void printMaze() {
    printf("+");
    for (int x = 0; x < MAZE_SIZE; x++) {
        if (x == 0 && maze[0][0].north == 0)
            printf("  +");
        else
            printf("--+");
    }
    printf("\n");

    for (int y = 0; y < MAZE_SIZE; y++) {
        if (maze[y][0].west) printf("|");
        else printf(" ");

        for (int x = 0; x < MAZE_SIZE; x++) {
            printf("  ");
            if (maze[y][x].east) printf("|");
            else printf(" ");
        }
        printf("\n");

        printf("+");
        for (int x = 0; x < MAZE_SIZE; x++) {
            if (y == MAZE_SIZE-1 && x == MAZE_SIZE-1 && maze[y][x].south == 0)
                printf("  +");
            else if (maze[y][x].south)
                printf("--+");
            else
                printf("  +");
        }
        printf("\n");
    }
}

int countWalls() {
    int count = 0;

    for (int y = 0; y < MAZE_SIZE; y++)
        for (int x = 0; x < MAZE_SIZE; x++)
            if (maze[y][x].north) count++;

    for (int y = 0; y < MAZE_SIZE; y++)
        for (int x = 0; x < MAZE_SIZE; x++)
            if (maze[y][x].west) count++;

    for (int x = 0; x < MAZE_SIZE; x++)
        if (maze[MAZE_SIZE-1][x].south) count++;

    for (int y = 0; y < MAZE_SIZE; y++)
        if (maze[y][MAZE_SIZE-1].east) count++;

    return count;
}

// This function creates a cube
vec4* cube(int* num) {
    int num_triangles = 3 * 2;
    int num_squares = 6;
    int num_vts = num_triangles * num_squares;
    *num = num_vts;

    vec4* pos = (vec4*) malloc(num_vts * sizeof(vec4));

    // Base face
    vec4 baseSquare[6] = {
    {  0.5, -0.5,  0.5, 1.0 },
    { -0.5,  0.5,  0.5, 1.0 },
    { -0.5, -0.5,  0.5, 1.0 },
    {  0.5, -0.5,  0.5, 1.0 },  
    {  0.5,  0.5,  0.5, 1.0 }, 
    { -0.5,  0.5,  0.5, 1.0 }
    };

    // Rotations needed to create a cube
    mat4 rotations[6] = {
        identity(),
        rotateY(90),
        rotateY(180),
        rotateY(270),
        rotateX(-90),
        rotateX(90)
    };

    int curr = 0;
    for (int r = 0; r < 6; r++) {
        for (int i = 0; i < 6; i++) {
            pos[curr++] = mat_vec_mult(rotations[r], baseSquare[i]);
        }
    }

    return pos;
}

// Helper function to append a cube to the given positions and texture arrays
void append_cube(vec4 *outPositions, vec2 *outTexCoords, int *writeIdx, vec4 *baseCube, mat4 transformation, vec2 *tex) 
{
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 6; j++) {
            outPositions[*writeIdx] = mat_vec_mult(transformation, baseCube[i*6 + j]);
            outTexCoords[*writeIdx] = tex[j];
            (*writeIdx)++;
        }
    }
}

void buildMaze(vec4 **outPositions, vec2 **outTexCoords, int *outNumVerts)
{
    int baseNumVerts;
    vec4 *baseCube = cube(&baseNumVerts);

    int padding = 4;
    int layers = 2;
    
    int numFloorCubes = ((MAZE_SIZE + 2 * padding) * (MAZE_SIZE + 2 * padding)) * layers;
    int numPoleCubes  = (MAZE_SIZE + 1) * (MAZE_SIZE + 1);
    int maxWallCubes  = 79;
    int totalCubes = numFloorCubes + numPoleCubes + maxWallCubes;
    int totalVerts = totalCubes * baseNumVerts;

    *outPositions = (vec4*) malloc(totalVerts * sizeof(vec4));
    *outTexCoords = (vec2*) malloc(totalVerts * sizeof(vec2));

    int writeIdx = 0;

    // Object Parameters
    float halfGrid = (MAZE_SIZE / 2.0) - 0.5;
    float cellSize = 1.0;
    float floorThickness = 0.1;
    float poleWidth = 0.20; 
    float poleHeight = 1;
    float wallThickness = 0.04;
    float wallHeight = 1;

    // Floor/Platform
    for (int layer = 0; layer < layers; layer++) {
        float yOffset = -0.5 - layer * floorThickness;
        for (int y = -padding; y < MAZE_SIZE + padding; y++) {
            for (int x = -padding; x < MAZE_SIZE + padding; x++) {
                float mazeX = x - halfGrid;
                float mazeZ = y - halfGrid;
                mat4 transformation = mat_mat_mult(translate(mazeX, yOffset, mazeZ), scale(cellSize, floorThickness, cellSize));
                append_cube(*outPositions, *outTexCoords, &writeIdx, baseCube, transformation, floor_tex);
            }
        }
    }


    // Poles
    for (int y = 0; y <= MAZE_SIZE; y++) {
        for (int x = 0; x <= MAZE_SIZE; x++) {
            float mazeX = (x - 0.5) - halfGrid;
            float mazeZ = (y - 0.47) - halfGrid;
            mat4 transformation = mat_mat_mult(translate(mazeX, -0.5 + poleHeight/2.0, mazeZ), scale(poleWidth, poleHeight, poleWidth));
            append_cube(*outPositions, *outTexCoords, &writeIdx, baseCube, transformation, pole_tex);
        }
    }
    
    // Define a visited array for walls so we don't try to create a wall we have already created
    // 0 = north, 1 = south, 2 = west, 3 = east
    int visited[MAZE_SIZE][MAZE_SIZE][4] = {0}; 

    // Walls
    for (int y = 0; y < MAZE_SIZE; y++) {
        for (int x = 0; x < MAZE_SIZE; x++) {
            float mazeX = x - halfGrid;
            float mazeZ = y - halfGrid;

            // North wall
            if(maze[y][x].north && !visited[y][x][0]){
                mat4 transformation = mat_mat_mult(translate(mazeX, -0.5 + wallHeight/2.0, mazeZ - cellSize/2.0 + wallThickness/2.0), scale(cellSize, wallHeight, wallThickness));
                append_cube(*outPositions, *outTexCoords, &writeIdx, baseCube, transformation, wall_tex);
                visited[y][x][0] = 1;
                if(y > 0){
                    // Mark south of neighbor
                    visited[y-1][x][1] = 1;
                }
            }

            // South wall
            if(maze[y][x].south && !visited[y][x][1]){
                mat4 transformation = mat_mat_mult(translate(mazeX, -0.5 + wallHeight/2.0, mazeZ + cellSize/2.0 - wallThickness/2.0), scale(cellSize, wallHeight, wallThickness));
                append_cube(*outPositions, *outTexCoords, &writeIdx, baseCube, transformation, wall_tex);
                visited[y][x][1] = 1;
                if(y < MAZE_SIZE - 1){
                    // Mark north of neighbor
                    visited[y+1][x][0] = 1;
                }
            }

            // West wall
            if(maze[y][x].west && !visited[y][x][2]){
                mat4 transformation = mat_mat_mult(translate(mazeX - cellSize/2.0 + wallThickness/2.0, -0.5 + wallHeight/2.0, mazeZ), scale(wallThickness, wallHeight, cellSize));
                append_cube(*outPositions, *outTexCoords, &writeIdx, baseCube, transformation, wall_tex);
                visited[y][x][2] = 1;
                if(x > 0){
                    // Mark east of neighbor
                    visited[y][x-1][3] = 1;
                } 
            }

            // East wall
            if(maze[y][x].east && !visited[y][x][3]) {
                mat4 transformation = mat_mat_mult(translate(mazeX + cellSize/2.0 - wallThickness/2.0, -0.5 + wallHeight/2.0, mazeZ), scale(wallThickness, wallHeight, cellSize));
                append_cube(*outPositions, *outTexCoords, &writeIdx, baseCube, transformation, wall_tex);
                visited[y][x][3] = 1;
                if(x < MAZE_SIZE - 1){
                    // Mark west of neighbor
                    visited[y][x+1][2] = 1;
                }
            }
        }
    }

    *outNumVerts = writeIdx;
    free(baseCube);
}


void buildSun(vec4 **outPositions, vec2 **outTexCoords, int *outNumVerts){
    int baseNumVerts;
    vec4 *baseCube = cube(&baseNumVerts);
    
    int totalCubes = 1;
    int totalVerts = totalCubes * baseNumVerts;

    *outPositions = (vec4*) malloc(totalVerts * sizeof(vec4));
    *outTexCoords = (vec2*) malloc(totalVerts * sizeof(vec2));

    int writeIdx = 0;
 
    mat4 transformation = mat_mat_mult(translate(0, 10, 0), scale(.5, .5, .5));
    append_cube(*outPositions, *outTexCoords, &writeIdx, baseCube, transformation, pole_tex);

    *outNumVerts = writeIdx;
    free(baseCube);
}

void buildSpinningCubeEntrance(vec4 **outPositions, vec2 **outTexCoords, int *outNumVerts){
    int baseNumVerts;
    vec4 *baseCube = cube(&baseNumVerts);
    
    int totalCubes = 1;
    int totalVerts = totalCubes * baseNumVerts;

    *outPositions = (vec4*) malloc(totalVerts * sizeof(vec4));
    *outTexCoords = (vec2*) malloc(totalVerts * sizeof(vec2));

    int writeIdx = 0;

    mat4 transformation = mat_mat_mult(translate(-3.5, 0, -3.5), scale(.25, .25, .25));
    append_cube(*outPositions, *outTexCoords, &writeIdx, baseCube, transformation, pole_tex);

    *outNumVerts = writeIdx;
    free(baseCube);
}

void buildSpinningCubeExit(vec4 **outPositions, vec2 **outTexCoords, int *outNumVerts){
    int baseNumVerts;
    vec4 *baseCube = cube(&baseNumVerts);
    
    int totalCubes = 1;
    int totalVerts = totalCubes * baseNumVerts;

    *outPositions = (vec4*) malloc(totalVerts * sizeof(vec4));
    *outTexCoords = (vec2*) malloc(totalVerts * sizeof(vec2));

    int writeIdx = 0;

    mat4 transformation = mat_mat_mult(translate(3.5, 0, 3.5), scale(.25, .25, .25));
    append_cube(*outPositions, *outTexCoords, &writeIdx, baseCube, transformation, spinning_cube_tex);

    *outNumVerts = writeIdx;
    free(baseCube);
}


// Variables for maze traversal algorithms
int solutionDirs[2048];
int solutionLength = 0;
int currentIndex = 0;
int method = 0;

int visitedSolve[MAZE_SIZE][MAZE_SIZE];

int solveDX[4] = {0, 1, 0, -1};
int solveDY[4] = {-1, 0, 1, 0};

// Returns our current eye position as a maze x index
int userMazeX() {
    int mx = (int)(eyePoint.x + (MAZE_SIZE/2.0));
    if(mx < 0){
        mx = 0;
    }
    if(mx >= MAZE_SIZE){
        mx = MAZE_SIZE-1;
    }
    return mx;
}

// Returns our current eye position as a maze y index
int userMazeY() {
    int my = (int)(eyePoint.z + (MAZE_SIZE/2.0));
    if(my < 0){
        my = 0;
    }
    if(my >= MAZE_SIZE){
        my = MAZE_SIZE-1;
    }
    return my;
}

// Check if a wall in a given direction is open
int isOpen(int x, int y, int dir) {
    if((dir == 0 && maze[y][x].north == 0) ||
       (dir == 1 && maze[y][x].east  == 0) ||
       (dir == 2 && maze[y][x].south == 0) ||
       (dir == 3 && maze[y][x].west  == 0)){
        return 1;
       }
    return 0;
}


int backDir[4] = {2, 3, 0, 1};  
// 0→2, 1→3, 2→0, 3→1
// north <-> south, east <-> west

int solveExhaustive(int x, int y) {
    if (x == MAZE_SIZE-1 && y == MAZE_SIZE-1) {
        solutionDirs[solutionLength++] = 1;
        return 1; 
    }

    visitedSolve[y][x] = 1;

    int dirs[4] = {0,1,2,3};

    for (int i = 0; i < 4; i++) {
        int d = dirs[i];

        if (!isOpen(x, y, d)) continue;

        int nx = x + solveDX[d];
        int ny = y + solveDY[d];

        if (nx < 0 || nx >= MAZE_SIZE || ny < 0 || ny >= MAZE_SIZE) continue;
        if (visitedSolve[ny][nx]) continue;

        solutionDirs[solutionLength++] = d;

        if (solveExhaustive(nx, ny)) {
            return 1;
        }

        solutionDirs[solutionLength++] = backDir[d];
    }

    return 0;
}


int solveDFS(int x, int y) {

    if (x == MAZE_SIZE-1 && y == MAZE_SIZE-1) {
         solutionDirs[solutionLength++] = 1;
        return 1;
    }

    visitedSolve[y][x] = 1;

    int dirs[4] = {0,1,2,3};

    for (int i = 0; i < 4; i++) {
        int d = dirs[i];

        if (!isOpen(x, y, d)) continue;

        int nx = x + solveDX[d];
        int ny = y + solveDY[d];

        if (nx < 0 || nx >= MAZE_SIZE || ny < 0 || ny >= MAZE_SIZE) continue;
        if (visitedSolve[ny][nx]) continue;

        solutionDirs[solutionLength++] = d;

        if (solveDFS(nx, ny)) return 1;

        solutionLength--;
    }

    return 0;
}

void printSolutionDirsRaw() {
    for (int i = 0; i < solutionLength; i++) {
        printf("%d ", solutionDirs[i]);
    }
    printf("\n");
}

void startSolving() {
    memset(visitedSolve, 0, sizeof(visitedSolve));
    solutionLength = 0;

    int sx = userMazeX();
    int sy = userMazeY();

    if(method == 0){
        solveExhaustive(sx, sy);
        // printSolutionDirsRaw();
    }

    else if(method == 1){
        solveDFS(sx, sy);
        // printSolutionDirsRaw();
    }

    currenState = SOLVING;
    currentIndex = 0;
}


// Direction we are facing: 0 = North, 1 = East, 2 = South, 3 = West
int facing;

int updateFacing() {
    if(eyePoint.z > atPoint.z && eyePoint.x == atPoint.x){
        facing = 0;
    }
    else if(eyePoint.x < atPoint.x && eyePoint.z == atPoint.z){
        facing = 1;
    } 
    else if(eyePoint.z < atPoint.z && eyePoint.x == atPoint.x){
        facing = 2;
    }
    else if(eyePoint.x > atPoint.x && eyePoint.z == atPoint.z){
        facing = 3;
    }
    else return 0;
    return 1;
}

// Returns 1 (true) if we are inside maze, otherwise returns 0 (false)
int isInsideMaze(){
    if(eyePoint.x >= -MAZE_SIZE/2.0 && eyePoint.x < MAZE_SIZE/2.0 &&
        eyePoint.z >= -MAZE_SIZE/2.0 && eyePoint.z < MAZE_SIZE/2.0 &&
        eyePoint.y == 0){
        return 1;
    }

    return 0;
}

int canMove(int dir) {

    int inside = isInsideMaze();
    float newX = eyePoint.x;
    float newZ = eyePoint.z;

    // Mapping of relative direction to absolute direction
    // to pass to the isOpen() function
    int mapping[4][4] = { {0,1,2,3}, {1,2,3,0}, {2,3,0,1}, {3,0,1,2} };

    int mazeDir = mapping[facing][dir];

    // Compute where player would move
    if(mazeDir == 0){
        newZ -= 1;
    }
    else if(mazeDir == 1){
        newX += 1;
    }
    else if(mazeDir == 2){
        newZ += 1;
    }
    else if(mazeDir == 3){
        newX -= 1; 
    } 

    // Check if new position is inside maze
    int newInside = (newX >= -MAZE_SIZE/2.0 && newX < MAZE_SIZE/2.0 && newZ >= -MAZE_SIZE/2.0 && newZ < MAZE_SIZE/2.0);

    // If we are moving from outside the maze to inside the maze,
    // we need to check if there is a wall
    if(!inside && newInside){
        int newMazeX = (int)(newX + MAZE_SIZE/2.0);
        int newMazeY = (int)(newZ + MAZE_SIZE/2.0);
        
        // Check the wall in the direction we're entering from
        int oppositeDir;

        if(mazeDir == 0){
            oppositeDir = 2;
        }
        else if(mazeDir == 1){
            oppositeDir = 3;
        }
        else if(mazeDir == 2){
            oppositeDir = 0;
        }
        else if(mazeDir == 3){
            oppositeDir = 1;
        }

        return isOpen(newMazeX, newMazeY, oppositeDir);
    }

    // If we are currently inside, we can use normal wall checking
    if(inside){
        int x = userMazeX();
        int y = userMazeY();
        return isOpen(x, y, mazeDir);
    }

    // Otherwise, if we are trying to move from outside to outside,
    // we allow it.
    return 1;
}


int num_vertices;

int num_vertices_maze;
int num_vertices_sun;
int num_vertices_spinning_entrance;
int num_vertices_spinning_exit;


mat4 maze_ctm = {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}};

mat4 sun_ctm = {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}};

mat4 spinning_entrance_ctm = {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}};

mat4 spinning_exit_ctm = {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}};

GLuint ctm_location;

mat4 model_view = {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}};
GLuint model_view_location;

mat4 projection = {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}};
GLuint projection_location;

// Current position of the sun
vec4 light_position = {0.0, 10.0, 0.0, 1.0};
GLuint light_position_location;

// Lighting options (1 = on, 0 = off) 
// (x = ambient, y = diffuse, z = specular, w = lighting effect)
vec4 light_options = {1.0, 1.0, 1.0, 0.0};
GLuint light_options_location;

// Light source coming from sun = 1
// Light source coming from viewer = 0

//New sun = 0, viewer = 1, flashlight = 2;
int light_source = 0;
GLuint light_source_location;

vec4 aim_direction = {0.0, 0.0, -1.0, 0.0};
GLuint aim_direction_location;

void init(void)
{
    vec4 *positionsMaze;
    vec2 *texturesMaze;
    buildMaze(&positionsMaze, &texturesMaze, &num_vertices_maze);

    vec4 *positionsSun;
    vec2 *texturesSun;
    buildSun(&positionsSun, &texturesSun, &num_vertices_sun);

    vec4 *positionsSpinningEntrance;
    vec2 *texturesSpinningEntrance;
    buildSpinningCubeEntrance(&positionsSpinningEntrance, &texturesSpinningEntrance, &num_vertices_spinning_entrance);

    vec4 *positionsSpinningExit;
    vec2 *texturesSpinningExit;
    buildSpinningCubeExit(&positionsSpinningExit, &texturesSpinningExit, &num_vertices_spinning_exit);

    num_vertices = num_vertices_maze + num_vertices_sun + num_vertices_spinning_entrance + num_vertices_spinning_exit;

    vec4 *positions = malloc(num_vertices * sizeof(vec4));
    vec2 *tex_coords = malloc(num_vertices * sizeof(vec2));
    vec4 *normals = malloc(num_vertices * sizeof(vec4));


    memcpy(positions, positionsMaze, sizeof(vec4) * num_vertices_maze);
    memcpy(positions + num_vertices_maze, positionsSun, sizeof(vec4) * num_vertices_sun);
    memcpy(positions + num_vertices_maze + num_vertices_sun, positionsSpinningEntrance, sizeof(vec4) * num_vertices_spinning_entrance);
    memcpy(positions + num_vertices_maze + num_vertices_sun + num_vertices_spinning_entrance, positionsSpinningExit, sizeof(vec4) * num_vertices_spinning_exit);

    memcpy(tex_coords, texturesMaze, sizeof(vec2) * num_vertices_maze);
    memcpy(tex_coords + num_vertices_maze, texturesSun, sizeof(vec2) * num_vertices_sun);
    memcpy(tex_coords + num_vertices_maze + num_vertices_sun, texturesSpinningEntrance, sizeof(vec2) * num_vertices_spinning_entrance);
    memcpy(tex_coords + num_vertices_maze + num_vertices_sun + num_vertices_spinning_entrance, texturesSpinningExit, sizeof(vec2) * num_vertices_spinning_exit);

    free(positionsMaze);
    free(texturesMaze);

    free(positionsSun);
    free(texturesSun);

    free(positionsSpinningEntrance);
    free(texturesSpinningEntrance);

    free(positionsSpinningExit);
    free(texturesSpinningExit);


    for(int i = 0; i < num_vertices; i = i + 3){

        vec4 p0 = positions[i];
        vec4 p1 = positions[i+1];
        vec4 p2 = positions[i+2];

        vec4 normal = cross_product((vec_sub(p1, p0)), (vec_sub(p2, p0)));

        normals[i] = normal;
        normals[i+1] = normal;
        normals[i+2] = normal;
    }


    int tex_width = 800;
    int tex_height = 800;

    GLubyte my_texels[tex_width][tex_height][3];

    FILE *fp = fopen("p2texture04.raw", "rb");

    fread(my_texels, tex_width * tex_height * 3, 1, fp);
    fclose(fp);

    GLuint program = initShader("vshader.glsl", "fshader.glsl");
    glUseProgram(program);

    GLuint mytex[1];
    glGenTextures(1, mytex);
    glBindTexture(GL_TEXTURE_2D, mytex[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, my_texels);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

    int param;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &param);

    GLuint vao;
    #ifdef __APPLE__
    glGenVertexArraysAPPLE(1, &vao);
    glBindVertexArrayAPPLE(vao);
    #else
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    #endif

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(vec4) * num_vertices + sizeof(vec2) * num_vertices, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, num_vertices * sizeof(vec4), positions);
    glBufferSubData(GL_ARRAY_BUFFER, num_vertices * sizeof(vec4), num_vertices * sizeof(vec2), tex_coords);
    glBufferSubData(GL_ARRAY_BUFFER, num_vertices * sizeof(vec4) + num_vertices * sizeof(vec2), num_vertices * sizeof(vec4), normals);

    GLuint vPosition = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(vPosition);
    glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0);

    GLuint vTexCoord = glGetAttribLocation(program, "vTexCoord");
    glEnableVertexAttribArray(vTexCoord);
    glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid *) (num_vertices * sizeof(vec4)));

    GLuint vNormal = glGetAttribLocation(program, "vNormal");
    glEnableVertexAttribArray(vNormal);
    glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *) (num_vertices * sizeof(vec4)) + (num_vertices * sizeof(vec2)));

    ctm_location = glGetUniformLocation(program, "ctm");
    model_view_location = glGetUniformLocation(program, "model_view");
    projection_location = glGetUniformLocation(program, "projection_matrix");
    light_position_location = glGetUniformLocation(program, "light_position");
    light_options_location = glGetUniformLocation(program, "light_options");
    GLuint texture_location = glGetUniformLocation(program, "texture");
    glUniform1i(texture_location, 0);
    light_source_location = glGetUniformLocation(program, "light_source");
    aim_direction_location = glGetUniformLocation(program, "aim_direction");


    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.5, 1.0, 1.0);
    glDepthRange(1,0);

    free(positions);
    free(tex_coords);
    free(normals);
}


// int input = 0;
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // glPolygonMode(GL_FRONT, GL_FILL);
    // glPolygonMode(GL_BACK, GL_LINE);

    model_view = look_at(eyePoint, atPoint, upVector);
    projection = frustum(left, right, bottom, top, near, far);

    glUniformMatrix4fv(model_view_location, 1, GL_FALSE, (GLfloat *) &model_view);
    glUniformMatrix4fv(projection_location, 1, GL_FALSE, (GLfloat *) &projection);
    glUniform4fv(light_position_location, 1, (GLfloat *) &light_position);
    glUniform4fv(light_options_location, 1, (GLfloat *) &light_options);
    glUniform1i(light_source_location, light_source);
    glUniform4fv(aim_direction_location, 1, (GLfloat *) &aim_direction);

    glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *) &maze_ctm);
    glDrawArrays(GL_TRIANGLES, 0, num_vertices_maze);

    glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *) &sun_ctm);
    glDrawArrays(GL_TRIANGLES, num_vertices_maze, num_vertices_sun);

    glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *) &spinning_entrance_ctm);
    glDrawArrays(GL_TRIANGLES, num_vertices_maze + num_vertices_sun, num_vertices_spinning_entrance);

    glUniformMatrix4fv(ctm_location, 1, GL_FALSE, (GLfloat *) &spinning_exit_ctm);
    glDrawArrays(GL_TRIANGLES, num_vertices_maze + num_vertices_sun + num_vertices_spinning_entrance, num_vertices_spinning_exit);
    
    glutPostRedisplay();

    glutSwapBuffers();

}


mat4 prevSun =  {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}};
mat4 prevSpinEntrance =  {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}};
mat4 prevSpinExit =  {{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,0,1}};
void keyboard(unsigned char key, int mousex, int mousey)
{

    if(key == 'q')
    	exit(0);

    else if(key == ' ' && isAnimating == 0){
        isAnimating = 1;
        currenState = FLYING_TO;
    }

    else if(key == 't' && isAnimating == 0 && isInsideMaze() == 1){
        updateFacing();
        method = 0;
        startSolving();
        isAnimating = 1;
    }

    else if(key == 'y' && isAnimating == 0 && isInsideMaze() == 1){
        updateFacing();
        isAnimating = 1;
        method = 1;
        startSolving();
    }


    else if(key == 'w' && isAnimating == 0 && eyePoint.y == 0){
        updateFacing();
        if(canMove(0)){
            isAnimating = 1;
            currenState = MOVE_FORWARD;
        }
    }

    else if(key == 'd' && isAnimating == 0 && eyePoint.y == 0){
        updateFacing();
        if(canMove(1)){
            isAnimating = 1;
            currenState = MOVE_RIGHT;
        }
    }

    else if(key == 's' && isAnimating == 0 && eyePoint.y == 0){
        updateFacing();
        if(canMove(2)){
            isAnimating = 1;
            currenState = MOVE_BACKWARD;
        }
    }

    else if(key == 'a' && isAnimating == 0 && eyePoint.y == 0){
        updateFacing();
        if(canMove(3)){
            isAnimating = 1;
            currenState = MOVE_LEFT;
        }
    }

    else if(key == 'z' && isAnimating == 0 && eyePoint.y == 0){
        updateFacing();
        isAnimating = 1;
        currenState = LOOK_LEFT;
    }

    else if(key == 'x' && isAnimating == 0 && eyePoint.y == 0){
        updateFacing();
        isAnimating = 1;
        currenState = LOOK_RIGHT;
    }

    else if(key == 'v' && isAnimating == 0){
        sun_ctm = mat_mat_mult(rotateX(-5), prevSun);
        prevSun = sun_ctm;
        light_position = mat_vec_mult(rotateX(-5), light_position);
    }

    else if(key == 'b' && isAnimating == 0){
        sun_ctm = mat_mat_mult(rotateX(5), prevSun);
        prevSun = sun_ctm;
        light_position = mat_vec_mult(rotateX(5), light_position);
    }

    else if(key == 'n' && isAnimating == 0){
        sun_ctm = mat_mat_mult(rotateZ(5), prevSun);
        prevSun = sun_ctm;
        light_position = mat_vec_mult(rotateZ(5), light_position);
    }

    else if(key == 'm' && isAnimating == 0){
        sun_ctm = mat_mat_mult(rotateZ(-5), prevSun);
        prevSun = sun_ctm;
        light_position = mat_vec_mult(rotateZ(-5), light_position);
    }

    // Toggle ambient light
    else if(key == '1'){
        if(light_options.x == 1.0){
            light_options.x = 0.0;
        }
        else{
            light_options.x = 1.0;
        }
    }

    // Toggle diffuse light
    else if(key == '2'){
        if(light_options.y == 1.0){
            light_options.y = 0.0;
        }
        else{
            light_options.y = 1.0;
        }
    }

    // Toggle specular light
    else if(key == '3'){
        if(light_options.z == 1.0){
            light_options.z = 0.0;
        }
        else{
            light_options.z = 1.0;
        }
    }

    // Toggle lighting effect
    else if(key == '4'){
        if(light_options.w == 1.0){
            light_options.w = 0.0;
        }
        else{
            light_options.w = 1.0;
        }
    }

    // Toggle between sun and viewer light source
    // else if(key == '5'){
    //     if(light_source == 1){
    //         light_source = 0;
    //     }
    //     else{
    //         light_source = 1;
    //     }
    // }
    else if(key == '5'){
        if(light_source == 0){
            light_source = 1;
        }
        else if(light_source == 1){
            light_source = 2;
        }
        else{
            light_source = 0;
        }
    }

    glutPostRedisplay();
}

vec4 v1;
vec4 v2;
int outOfBounds;
float boundDetector;


void mouse(int button, int state, int x, int y){
    // printf("Button: %d, State: %d, Coordinate(%d, %d)\n", button, state, x, y);

    // float glx = (x / 400.0) - 1;
    // float gly = 1 - (y / 400.0);

    // if(state == 0){
    //     float glz = calcZ(glx, gly);
    //     v1 = (vec4){glx, gly, glz, 0};
    // }

    // if(state == 1){
    //     prevSun = sun_ctm;
    // }

    glutPostRedisplay();
}

void motion(int x, int y){

    // Turn window coordinate into openGL coordinate
    float glx = (x / 400.0) - 1;
    float gly = 1 - (y / 400.0);

    if(light_source == 2){
        aim_direction.x = glx;
        aim_direction.y = gly;
    }

    glutPostRedisplay();
}


int direction;

void idle(void){

    spinning_entrance_ctm = mat_mat_mult(mat_mat_mult(translate(-3.5, 0, -3.5),(mat_mat_mult(rotateY(1), translate(3.5, 0, 3.5)))), prevSpinEntrance);
    prevSpinEntrance = spinning_entrance_ctm;

    spinning_exit_ctm = mat_mat_mult(mat_mat_mult(translate(3.5, 0, 3.5),(mat_mat_mult(rotateY(1), translate(-3.5, 0, -3.5)))), prevSpinExit);
    prevSpinExit = spinning_exit_ctm;
    

    if(isAnimating == 1){
        currentStep++;

        if(currenState == FLYING_TO){

            if(currentStep == maxFlyingToStep){
                alpha = 1.0;

                xEyePoint = (alpha * -6) + 0;
                yEyePoint = (alpha * -2) + 5;
                zEyePoint = (alpha * -3.5) + 0;
                yUp = (alpha * 1.0) + 0;
                zUp = (alpha * 1.0) - 1;
                eyePoint = (vec4) {xEyePoint, yEyePoint, zEyePoint, 1};
                upVector = (vec4) {0, yUp, zUp, 0};

                currenState = FLYING_AROUND;
                currentStep = 0;
                baseEyePoint = eyePoint;
            }

            else{
                alpha = (float) currentStep / maxFlyingToStep;

                xEyePoint = (alpha * -6) + 0;
                yEyePoint = (alpha * -2) + 5;
                zEyePoint = (alpha * -3.5) + 0;
                yUp = (alpha * 1.0) + 0;
                zUp = (alpha * 1.0) - 1;
                eyePoint = (vec4) {xEyePoint, yEyePoint, zEyePoint, 1};
                upVector = (vec4) {0, yUp, zUp, 0};
            }
        }


        else if(currenState == FLYING_AROUND){

            if(currentStep == maxFlyingAroundStep){
                alpha = 1.0;

                eyePoint = mat_vec_mult(rotateY(alpha * 360), baseEyePoint);

                currentStep = 0;
                currenState = FLYING_DOWN;
            }

            else{
                alpha = (float) currentStep / maxFlyingAroundStep;

                eyePoint = mat_vec_mult(rotateY(alpha * 360), baseEyePoint);
            }
            
        }


        else if(currenState == FLYING_DOWN){

            if(currentStep == maxFlyingDownStep){
                alpha = 1.0;

                xEyePoint = (alpha * 1.5) - 6;
                yEyePoint = (alpha * -3) + 3;
                xAtPoint = (alpha * -4) + 0;
                yAtPoint = (alpha * .5)  - .5;
                zAtPoint = (alpha * -3.5) + 0;


                eyePoint = (vec4) {xEyePoint, yEyePoint, zEyePoint, 1};
                atPoint = (vec4) {xAtPoint, yAtPoint, zAtPoint, 1};

                left = (alpha * .7) - 1;
                right = (alpha * -.7) + 1;
                bottom = (alpha * .7) - 1;
                top = (alpha * -.7) + 1;
                near = (alpha * .7) - 1;

                // left = -.3;
                // right = .3;
                // bottom = -.3;
                // top = .3;
                // near = -.3;
                // far = -100;

                currenState = NONE;
                currentStep = 0;
                baseEyePoint = eyePoint;
                baseAtPoint = atPoint;

            }

            else{
                alpha = (float) currentStep / maxFlyingDownStep;

                xEyePoint = (alpha * 1.5) - 6;
                yEyePoint = (alpha * -3) + 3;
                xAtPoint = (alpha * -4) + 0;
                yAtPoint = (alpha * .5)  - .5;
                zAtPoint = (alpha * -3.5) + 0;

                eyePoint = (vec4) {xEyePoint, yEyePoint, zEyePoint, 1};
                atPoint = (vec4) {xAtPoint, yAtPoint, zAtPoint, 1};

                left = (alpha * .7) - 1;
                right = (alpha * -.7) + 1;
                bottom = (alpha * .7) - 1;
                top = (alpha * -.7) + 1;
                near = (alpha * .7) - 1;
                
            }
        }


        // When turning we have to take into account the direction we are facing,
        // and the direction we intend to move in
        else if(currenState == TURNING){

            if(currentStep == maxTurningStep){
                alpha = 1.0;

                // Whenever the direction we are facing and the direction
                // we want to move in differs by 2, we want to do a 180 turn
                if(abs(direction - facing) == 2){
                    vec4 lookDir = (vec4) {baseAtPoint.x - eyePoint.x, baseAtPoint.y - eyePoint.y, baseAtPoint.z - eyePoint.z, 1};
                
                    lookDir = mat_vec_mult(rotateY(alpha * 180), lookDir);

                    atPoint.x = eyePoint.x + lookDir.x;
                    atPoint.y = eyePoint.y + lookDir.y;
                    atPoint.z = eyePoint.z + lookDir.z;
                }

                else if(direction == 0){

                    if(facing == 1){
                        atPoint.x = (alpha * -.5) + baseAtPoint.x;
                        atPoint.z = (alpha * -.5) + baseAtPoint.z;
                    }
                    
                    else if(facing == 3){
                        atPoint.x = (alpha * .5) + baseAtPoint.x;
                        atPoint.z = (alpha * -.5) + baseAtPoint.z;
                    }
                } 

                else if(direction == 1){

                    if(facing == 0){
                        atPoint.x = (alpha * .5) + baseAtPoint.x;
                        atPoint.z = (alpha * .5) + baseAtPoint.z;
                    }
                    
                    else if(facing == 2){
                        atPoint.x = (alpha * .5) + baseAtPoint.x;
                        atPoint.z = (alpha * -.5) + baseAtPoint.z;
                    }

                } 

                else if(direction == 2){

                    if(facing == 1){
                        atPoint.x = (alpha * -.5) + baseAtPoint.x;
                        atPoint.z = (alpha * .5) + baseAtPoint.z;
                    }

                    else if(facing == 3){
                        atPoint.x = (alpha * .5) + baseAtPoint.x;
                        atPoint.z = (alpha * .5) + baseAtPoint.z;
                    }

                }

                else if(direction == 3){

                    if(facing == 0){
                        atPoint.x = (alpha * -.5) + baseAtPoint.x;
                        atPoint.z = (alpha * .5) + baseAtPoint.z;
                    }

                    else if(facing == 2){
                        atPoint.x = (alpha * -.5) + baseAtPoint.x;
                        atPoint.z = (alpha * -.5) + baseAtPoint.z;
                    }

                }

                facing = direction;
                baseAtPoint = atPoint;
                currenState = SOLVING;
                currentStep = 0;
            }

            else{

                alpha = (float) currentStep / maxTurningStep;

                if(abs(direction - facing) == 2){
                    vec4 lookDir = (vec4) {baseAtPoint.x - eyePoint.x, baseAtPoint.y - eyePoint.y, baseAtPoint.z - eyePoint.z, 1};
                
                    lookDir = mat_vec_mult(rotateY(alpha * 180), lookDir);

                    atPoint.x = eyePoint.x + lookDir.x;
                    atPoint.y = eyePoint.y + lookDir.y;
                    atPoint.z = eyePoint.z + lookDir.z;
                }

                else if(direction == 0){

                    if(facing == 1){
                        atPoint.x = (alpha * -.5) + baseAtPoint.x;
                        atPoint.z = (alpha * -.5) + baseAtPoint.z;
                    }
                    
                    else if(facing == 3){
                        atPoint.x = (alpha * .5) + baseAtPoint.x;
                        atPoint.z = (alpha * -.5) + baseAtPoint.z;
                    }
                    
                } 

                else if(direction == 1){

                    if(facing == 0){
                        atPoint.x = (alpha * .5) + baseAtPoint.x;
                        atPoint.z = (alpha * .5) + baseAtPoint.z;
                    }
                    
                    else if(facing == 2){
                        atPoint.x = (alpha * .5) + baseAtPoint.x;
                        atPoint.z = (alpha * -.5) + baseAtPoint.z;
                    }

                } 

                else if(direction == 2){

                    if(facing == 1){
                        atPoint.x = (alpha * -.5) + baseAtPoint.x;
                        atPoint.z = (alpha * .5) + baseAtPoint.z;
                    }

                    else if(facing == 3){
                        atPoint.x = (alpha * .5) + baseAtPoint.x;
                        atPoint.z = (alpha * .5) + baseAtPoint.z;
                    }

                }

                else if(direction == 3){

                    if(facing == 0){
                        atPoint.x = (alpha * -.5) + baseAtPoint.x;
                        atPoint.z = (alpha * .5) + baseAtPoint.z;
                    }

                    else if(facing == 2){
                        atPoint.x = (alpha * -.5) + baseAtPoint.x;
                        atPoint.z = (alpha * -.5) + baseAtPoint.z;
                    }

                }

            }
            
        }


        else if(currenState == SOLVING){

            if(currentIndex >= solutionLength){
                currenState = NONE;
                return;
            }

            direction = solutionDirs[currentIndex];

            if(direction != facing){
                baseAtPoint = atPoint;
                baseEyePoint = eyePoint;
                currenState = TURNING;
                return;
            }

            if(currentStep == maxSolvingStep){
                alpha = 1.0;

                if(direction == 0){
                    eyePoint.z = (alpha * -1) + baseEyePoint.z;
                    atPoint.z = (alpha * -1) + baseAtPoint.z;
                } 

                else if(direction == 1){
                    eyePoint.x = (alpha * 1) + baseEyePoint.x;
                    atPoint.x = (alpha * 1) + baseAtPoint.x;
                } 

                else if(direction == 2){
                    eyePoint.z = (alpha * 1) + baseEyePoint.z;
                    atPoint.z = (alpha * 1) + baseAtPoint.z;
                }

                else if(direction == 3){
                    eyePoint.x = (alpha * -1) + baseEyePoint.x;
                    atPoint.x = (alpha * -1) + baseAtPoint.x;
                }

                updateFacing();
                currentStep = 0;
                currentIndex++;
                baseEyePoint = eyePoint;
                baseAtPoint = atPoint;
            }

            else{
                alpha = (float) currentStep / maxSolvingStep;

                if(direction == 0){
                    eyePoint.z = (alpha * -1) + baseEyePoint.z;
                    atPoint.z = (alpha * -1) + baseAtPoint.z;
                } 

                else if(direction == 1){
                    eyePoint.x = (alpha * 1) + baseEyePoint.x;
                    atPoint.x = (alpha * 1) + baseAtPoint.x;
                } 

                else if(direction == 2){
                    eyePoint.z = (alpha * 1) + baseEyePoint.z;
                    atPoint.z = (alpha * 1) + baseAtPoint.z;
                }

                else if(direction == 3){
                    eyePoint.x = (alpha * -1) + baseEyePoint.x;
                    atPoint.x = (alpha * -1) + baseAtPoint.x;
                }
                
            }
            
        }


        else if(currenState == MOVE_FORWARD){

            if(currentStep == maxMoveForwardStep){
                alpha = 1.0;

                if(facing == 0){
                    eyePoint.z = (alpha * -1) + baseEyePoint.z;
                    atPoint.z = (alpha * -1) + baseAtPoint.z;
                }

                else if(facing == 1){
                    eyePoint.x = (alpha * 1) + baseEyePoint.x;
                    atPoint.x = (alpha * 1) + baseAtPoint.x;
                }

                else if(facing == 2){
                    eyePoint.z = (alpha * 1) + baseEyePoint.z;
                    atPoint.z = (alpha * 1) + baseAtPoint.z;
                }

                else if(facing == 3){
                    eyePoint.x = (alpha * -1) + baseEyePoint.x;
                    atPoint.x = (alpha * -1) + baseAtPoint.x;
                }

                currentStep = 0;
                currenState = NONE;
                baseEyePoint = eyePoint;
                baseAtPoint = atPoint;
            }

            else{
                alpha = (float) currentStep / maxMoveForwardStep;

                if(facing == 0){
                    eyePoint.z = (alpha * -1) + baseEyePoint.z;
                    atPoint.z = (alpha * -1) + baseAtPoint.z;
                }

                else if(facing == 1){
                    eyePoint.x = (alpha * 1) + baseEyePoint.x;
                    atPoint.x = (alpha * 1) + baseAtPoint.x;
                }

                else if(facing == 2){
                    eyePoint.z = (alpha * 1) + baseEyePoint.z;
                    atPoint.z = (alpha * 1) + baseAtPoint.z;
                }

                else if(facing == 3){
                    eyePoint.x = (alpha * -1) + baseEyePoint.x;
                    atPoint.x = (alpha * -1) + baseAtPoint.x;
                }

            }
        }


        else if(currenState == MOVE_RIGHT){

            if(currentStep == maxMoveRightStep){
                alpha = 1.0;

                if(facing == 0){
                    eyePoint.x = (alpha * 1) + baseEyePoint.x;
                    atPoint.x = (alpha * 1) + baseAtPoint.x;
                }

                else if(facing == 1){
                    eyePoint.z = (alpha * 1) + baseEyePoint.z;
                    atPoint.z = (alpha * 1) + baseAtPoint.z;
                }

                else if(facing == 2){
                    eyePoint.x = (alpha * -1) + baseEyePoint.x;
                    atPoint.x = (alpha * -1) + baseAtPoint.x;
                }

                else if(facing == 3){
                    eyePoint.z = (alpha * -1) + baseEyePoint.z;
                    atPoint.z = (alpha * -1) + baseAtPoint.z;
                }

                currentStep = 0;
                currenState = NONE;
                baseEyePoint = eyePoint;
                baseAtPoint = atPoint;
            }

            else{
                alpha = (float) currentStep / maxMoveRightStep;

                if(facing == 0){
                    eyePoint.x = (alpha * 1) + baseEyePoint.x;
                    atPoint.x = (alpha * 1) + baseAtPoint.x;
                }

                else if(facing == 1){
                    eyePoint.z = (alpha * 1) + baseEyePoint.z;
                    atPoint.z = (alpha * 1) + baseAtPoint.z;
                }

                else if(facing == 2){
                    eyePoint.x = (alpha * -1) + baseEyePoint.x;
                    atPoint.x = (alpha * -1) + baseAtPoint.x;
                }

                else if(facing == 3){
                    eyePoint.z = (alpha * -1) + baseEyePoint.z;
                    atPoint.z = (alpha * -1) + baseAtPoint.z;
                }

            }
        }


        else if(currenState == MOVE_BACKWARD){

            if(currentStep == maxMoveBackwardStep){
                alpha = 1.0;

                if(facing == 0){
                    eyePoint.z = (alpha * 1) + baseEyePoint.z;
                    atPoint.z = (alpha * 1) + baseAtPoint.z;
                }

                else if(facing == 1){
                    eyePoint.x = (alpha * -1) + baseEyePoint.x;
                    atPoint.x = (alpha * -1) + baseAtPoint.x;
                }

                else if(facing == 2){
                    eyePoint.z = (alpha * -1) + baseEyePoint.z;
                    atPoint.z = (alpha * -1) + baseAtPoint.z;
                }

                else if(facing == 3){
                    eyePoint.x = (alpha * 1) + baseEyePoint.x;
                    atPoint.x = (alpha * 1) + baseAtPoint.x;
                }

                currentStep = 0;
                currenState = NONE;
                baseEyePoint = eyePoint;
                baseAtPoint = atPoint;
            }

            else{
                alpha = (float) currentStep / maxMoveBackwardStep;

                if(facing == 0){
                    eyePoint.z = (alpha * 1) + baseEyePoint.z;
                    atPoint.z = (alpha * 1) + baseAtPoint.z;
                }

                else if(facing == 1){
                    eyePoint.x = (alpha * -1) + baseEyePoint.x;
                    atPoint.x = (alpha * -1) + baseAtPoint.x;
                }

                else if(facing == 2){
                    eyePoint.z = (alpha * -1) + baseEyePoint.z;
                    atPoint.z = (alpha * -1) + baseAtPoint.z;
                }

                else if(facing == 3){
                    eyePoint.x = (alpha * 1) + baseEyePoint.x;
                    atPoint.x = (alpha * 1) + baseAtPoint.x;
                }

            }
        }


        else if(currenState == MOVE_LEFT){

            if(currentStep == maxMoveLeftStep){
                alpha = 1.0;

                if(facing == 0){
                    eyePoint.x = (alpha * -1) + baseEyePoint.x;
                    atPoint.x = (alpha * -1) + baseAtPoint.x;
                }

                else if(facing == 1){
                    eyePoint.z = (alpha * -1) + baseEyePoint.z;
                    atPoint.z = (alpha * -1) + baseAtPoint.z;
                }

                else if(facing == 2){
                    eyePoint.x = (alpha * 1) + baseEyePoint.x;
                    atPoint.x = (alpha * 1) + baseAtPoint.x;
                }

                else if(facing == 3){
                    eyePoint.z = (alpha * 1) + baseEyePoint.z;
                    atPoint.z = (alpha * 1) + baseAtPoint.z;
                }

                currentStep = 0;
                currenState = NONE;
                baseEyePoint = eyePoint;
                baseAtPoint = atPoint;
            }

            else{
                alpha = (float) currentStep / maxMoveLeftStep;

                if(facing == 0){
                    eyePoint.x = (alpha * -1) + baseEyePoint.x;
                    atPoint.x = (alpha * -1) + baseAtPoint.x;
                }

                else if(facing == 1){
                    eyePoint.z = (alpha * -1) + baseEyePoint.z;
                    atPoint.z = (alpha * -1) + baseAtPoint.z;
                }

                else if(facing == 2){
                    eyePoint.x = (alpha * 1) + baseEyePoint.x;
                    atPoint.x = (alpha * 1) + baseAtPoint.x;
                }

                else if(facing == 3){
                    eyePoint.z = (alpha * 1) + baseEyePoint.z;
                    atPoint.z = (alpha * 1) + baseAtPoint.z;
                }

            }
        }


        else if(currenState == LOOK_LEFT){

            if(currentStep == maxLookLeftStep){
                alpha = 1.0;

                if(facing == 0){
                    atPoint.x = (alpha * -.5) + baseAtPoint.x;
                    atPoint.z = (alpha * .5) + baseAtPoint.z;
                } 

                else if(facing == 1){
                    atPoint.x = (alpha * -.5) + baseAtPoint.x;
                    atPoint.z = (alpha * -.5) + baseAtPoint.z;
                }

                else if(facing == 2){
                    atPoint.x = (alpha * .5) + baseAtPoint.x;
                    atPoint.z = (alpha * -.5) + baseAtPoint.z;
                }

                else if(facing == 3){
                    atPoint.x = (alpha * .5) + baseAtPoint.x;
                    atPoint.z = (alpha * .5) + baseAtPoint.z;
                }

                baseAtPoint = atPoint;
                currenState = NONE;
                currentStep = 0;
            }

            else{

                alpha = (float) currentStep / maxLookLeftStep;

                if(facing == 0){
                    atPoint.x = (alpha * -.5) + baseAtPoint.x;
                    atPoint.z = (alpha * .5) + baseAtPoint.z;
                } 

                else if(facing == 1){
                    atPoint.x = (alpha * -.5) + baseAtPoint.x;
                    atPoint.z = (alpha * -.5) + baseAtPoint.z;
                }

                else if(facing == 2){
                    atPoint.x = (alpha * .5) + baseAtPoint.x;
                    atPoint.z = (alpha * -.5) + baseAtPoint.z;
                }

                else if(facing == 3){
                    atPoint.x = (alpha * .5) + baseAtPoint.x;
                    atPoint.z = (alpha * .5) + baseAtPoint.z;
                }

            }
            
        }


        else if(currenState == LOOK_RIGHT){

            if(currentStep == maxLookRightStep){
                alpha = 1.0;

                if(facing == 0){
                    atPoint.x = (alpha * .5) + baseAtPoint.x;
                    atPoint.z = (alpha * .5) + baseAtPoint.z;
                } 

                else if(facing == 1){
                    atPoint.x = (alpha * -.5) + baseAtPoint.x;
                    atPoint.z = (alpha * .5) + baseAtPoint.z;
                }

                else if(facing == 2){
                    atPoint.x = (alpha * -.5) + baseAtPoint.x;
                    atPoint.z = (alpha * -.5) + baseAtPoint.z;
                }

                else if(facing == 3){
                    atPoint.x = (alpha * .5) + baseAtPoint.x;
                    atPoint.z = (alpha * -.5) + baseAtPoint.z;
                }

                baseAtPoint = atPoint;
                currenState = NONE;
                currentStep = 0;
            }

            else{

                alpha = (float) currentStep / maxLookRightStep;

                if(facing == 0){
                    atPoint.x = (alpha * .5) + baseAtPoint.x;
                    atPoint.z = (alpha * .5) + baseAtPoint.z;
                } 

                else if(facing == 1){
                    atPoint.x = (alpha * -.5) + baseAtPoint.x;
                    atPoint.z = (alpha * .5) + baseAtPoint.z;
                }

                else if(facing == 2){
                    atPoint.x = (alpha * -.5) + baseAtPoint.x;
                    atPoint.z = (alpha * -.5) + baseAtPoint.z;
                }

                else if(facing == 3){
                    atPoint.x = (alpha * .5) + baseAtPoint.x;
                    atPoint.z = (alpha * -.5) + baseAtPoint.z;
                }

            }
            
        }


        else if(currenState == NONE){
            isAnimating = 0;
        }

        glutPostRedisplay();
    }
}

int main(int argc, char **argv)
{

    printf("\nMain Menu:\n\n");
    printf("Spacebar - Fly around maze and go to entrance\n");
    printf("w - Move forward\n");
    printf("a - Move left\n");
    printf("s - Move right\n");
    printf("d - Move backward\n");
    printf("z - Look left\n");
    printf("x - Look right\n");
    printf("t - Go to exit (not necessarily shortest path)\n");
    printf("y - Go to exit (shortest path)\n");
    printf("1 - Toggle ambient lighting\n");
    printf("2 - Toggle diffuse lighting\n");
    printf("3 - Toggle specular lighting\n");
    printf("4 - Toggle lighting effect\n");
    printf("5 - Toggle between sun, viewer, and flashlight light source (control flashlight with cursor)\n");
    printf("v - Move sun from west to east\n");
    printf("b - Move sun from east to west\n");
    printf("n - Move sun from north to south\n");
    printf("m - Move sun from south to north\n\n");


    

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Triangle");
    #ifndef __APPLE__
    glewInit();
    #endif

    srand(time(NULL));
    initMaze();
    generateMazeDFS(0,0);
    printMaze();
    int walls = countWalls();
    printf("numWalls: %d\n", walls);

    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutMainLoop();

    return 0;
}
