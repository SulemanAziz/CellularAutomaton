#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include"raylib.h"

#define WIDTH 800
#define HEIGHT 600

struct Cell{
    int state;  // 0 = dead/off, 1 = alive/on, 2 = refractory (for Brian's Brain)
};

// Linked list node for automaton functions
typedef struct AutomatonNode {
    char* name;
    void (*function)(struct Cell grid[][HEIGHT]);
    struct AutomatonNode* next;
    struct AutomatonNode* prev;
} AutomatonNode;

float rand_float(){
    return (float) rand()/RAND_MAX;
}

void FillGrid(struct Cell grid[][HEIGHT]){

    for (size_t x = 0; x < WIDTH; x++)
    {
        for (size_t y = 0; y < HEIGHT; y++)
        {
            grid[x][y].state = rand_float()>0.82?1:0;
        }
    }
}

void Neighbours(struct Cell grid[][HEIGHT]){

    /*
    RULES:
        1. Any live cell with fewer than two live neighbours dies, as if by underpopulation.
        2. Any live cell with two or three live neighbours lives on to the next generation.
        3. Any live cell with more than three live neighbours dies, as if by overpopulation.

        4. Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
    */

    // Create a temporary grid to store the next generation
    struct Cell newGrid[WIDTH][HEIGHT];
    
    for (int x = 0; x < WIDTH; x+=10) {
        for (int y = 0; y < HEIGHT; y+=10) {
            int liveNeighbors = 0;
            
            // Count all 8 neighbors
            for (int dx = -10; dx <= 10; dx += 10) {
                for (int dy = -10; dy <= 10; dy += 10) {
                    if (dx == 0 && dy == 0) continue; // Skip the cell itself
                    
                    int nx = (x + dx + WIDTH) % WIDTH;   // Wrap edges
                    int ny = (y + dy + HEIGHT) % HEIGHT;
                    
                    if (grid[nx][ny].state==1) liveNeighbors++;
                }
            }
            
            // Apply Game of Life rules
            if (grid[x][y].state == 1) {
                // Rules 1, 2, 3: Live cell
                newGrid[x][y].state = (liveNeighbors == 2 || liveNeighbors == 3) ? 1 : 0;
            } else {
                // Rule 4: Dead cell
                newGrid[x][y].state = (liveNeighbors == 3) ? 1 : 0;
            }
        }
    }
    
    // Copy newGrid back to grid
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            grid[x][y] = newGrid[x][y];
        }
    }
}

// MORLEY - Creates slowly moving spaceship patterns: birth on 3-4, survive on 4-6
void Morley(struct Cell grid[][HEIGHT]){
    struct Cell newGrid[WIDTH][HEIGHT];
    
    for (int x = 0; x < WIDTH; x+=10) {
        for (int y = 0; y < HEIGHT; y+=10) {
            int liveNeighbors = 0;
            
            for (int dx = -10; dx <= 10; dx += 10) {
                for (int dy = -10; dy <= 10; dy += 10) {
                    if (dx == 0 && dy == 0) continue;
                    
                    int nx = (x + dx + WIDTH) % WIDTH;
                    int ny = (y + dy + HEIGHT) % HEIGHT;
                    
                    if (grid[nx][ny].state == 1) liveNeighbors++;
                }
            }
            
            // Morley rules: birth on 3-4 neighbors, survive on 4-6 neighbors
            if (grid[x][y].state == 1) {
                newGrid[x][y].state = (liveNeighbors >= 4 && liveNeighbors <= 6) ? 1 : 0;
            } else {
                newGrid[x][y].state = (liveNeighbors == 3 || liveNeighbors == 4) ? 1 : 0;
            }
        }
    }
    
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            grid[x][y] = newGrid[x][y];
        }
    }
}

// ASSIMILATION - Growing thick blobs: birth on 3-5 neighbors, survive on 4-7
void Assimilation(struct Cell grid[][HEIGHT]){
    struct Cell newGrid[WIDTH][HEIGHT];
    
    for (int x = 0; x < WIDTH; x+=10) {
        for (int y = 0; y < HEIGHT; y+=10) {
            int liveNeighbors = 0;
            
            for (int dx = -10; dx <= 10; dx += 10) {
                for (int dy = -10; dy <= 10; dy += 10) {
                    if (dx == 0 && dy == 0) continue;
                    
                    int nx = (x + dx + WIDTH) % WIDTH;
                    int ny = (y + dy + HEIGHT) % HEIGHT;
                    
                    if (grid[nx][ny].state == 1) liveNeighbors++;
                }
            }
            
            // Assimilation rules: birth on 3-5 neighbors, survive on 4-7 neighbors
            if (grid[x][y].state == 1) {
                newGrid[x][y].state = (liveNeighbors >= 4 && liveNeighbors <= 7) ? 1 : 0;
            } else {
                newGrid[x][y].state = (liveNeighbors >= 3 && liveNeighbors <= 5) ? 1 : 0;
            }
        }
    }
    
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            grid[x][y] = newGrid[x][y];
        }
    }
}

// BRIAN'S BRAIN - 3-state system: 0=off, 1=on, 2=refractory
void BriansBrain(struct Cell grid[][HEIGHT]){
    struct Cell newGrid[WIDTH][HEIGHT];
    
    for (int x = 0; x < WIDTH; x+=10) {
        for (int y = 0; y < HEIGHT; y+=10) {
            int liveNeighbors = 0;
            
            for (int dx = -10; dx <= 10; dx += 10) {
                for (int dy = -10; dy <= 10; dy += 10) {
                    if (dx == 0 && dy == 0) continue;
                    
                    int nx = (x + dx + WIDTH) % WIDTH;
                    int ny = (y + dy + HEIGHT) % HEIGHT;
                    
                    if (grid[nx][ny].state == 1) liveNeighbors++;
                }
            }
            
            // Brian's Brain rules:
            if (grid[x][y].state == 1) {
                // On cell -> Refractory
                newGrid[x][y].state = 2;
            } else if (grid[x][y].state == 2) {
                // Refractory -> Off
                newGrid[x][y].state = 0;
            } else {
                // Off cell -> On if exactly 2 neighbors are on
                newGrid[x][y].state = (liveNeighbors == 2) ? 1 : 0;
            }
        }
    }
    
    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            grid[x][y] = newGrid[x][y];
        }
    }
}

void Start(struct Cell grid[][HEIGHT], struct Cell prevGrid[][HEIGHT], Sound blip){

    for (int x = 0; x < WIDTH; x+=10)
    {
        for (int y = 0; y < HEIGHT; y+=10)
        {
            if(grid[x][y].state == 1){
                DrawRectangle(x,y,10,10,BLUE);
            } else if(grid[x][y].state == 2){
                // Refractory state (for Brian's Brain)
                DrawRectangle(x,y,10,10,YELLOW);
            }
        }
    }
}

int main(void){

    InitWindow(WIDTH,HEIGHT, "Game of Life");
    SetTargetFPS(15);

    const char* soundfile = "blip.mp3";
    InitAudioDevice();
    Sound blip = LoadSound(soundfile);
    SetSoundVolume(blip, 0.5f);

    srand(time(0));
    struct Cell grid[WIDTH][HEIGHT] = {};

    FillGrid(grid);
    bool paused = false;

    // Create circular linked list of automatons
    AutomatonNode* gameOfLife = (AutomatonNode*)malloc(sizeof(AutomatonNode));
    AutomatonNode* morley = (AutomatonNode*)malloc(sizeof(AutomatonNode));
    AutomatonNode* assimilation = (AutomatonNode*)malloc(sizeof(AutomatonNode));
    AutomatonNode* briansBrain = (AutomatonNode*)malloc(sizeof(AutomatonNode));

    // Initialize Game of Life node
    gameOfLife->name = "Game of Life";
    gameOfLife->function = Neighbours;

    // Initialize Morley node
    morley->name = "Morley";
    morley->function = Morley;

    // Initialize Assimilation node
    assimilation->name = "Assimilation";
    assimilation->function = Assimilation;

    // Initialize Brian's Brain node
    briansBrain->name = "Brian's Brain";
    briansBrain->function = BriansBrain;

    // Create circular links
    gameOfLife->next = morley;
    gameOfLife->prev = briansBrain;

    morley->next = assimilation;
    morley->prev = gameOfLife;

    assimilation->next = briansBrain;
    assimilation->prev = morley;

    briansBrain->next = gameOfLife;
    briansBrain->prev = assimilation;

    // Start with Game of Life
    AutomatonNode* current = gameOfLife;
    
    // Create previous grid to track state changes
    struct Cell prevGrid[WIDTH][HEIGHT] = {};

    while(!WindowShouldClose()){

        BeginDrawing();
        ClearBackground((Color) BLACK);

        if(IsKeyPressed(KEY_SPACE)){
            FillGrid(grid);
        }
        if(IsKeyPressed(KEY_P)){
            paused = !paused;
        }
        // Navigate with arrow keys
        if(IsKeyPressed(KEY_RIGHT)){
            current = current->next;
        }
        if(IsKeyPressed(KEY_LEFT)){
            current = current->prev;
        }
        if(!paused){
            current->function(grid);
        }

        Start(grid, prevGrid, blip);
        
        // Copy current grid to prevGrid for next frame
        for(int x = 0; x < WIDTH; x++){
            for(int y = 0; y < HEIGHT; y++){
                prevGrid[x][y] = grid[x][y];
            }
        }

        if(paused){
          DrawText("PAUSED - Press P to Resume", 10, 10, 30, YELLOW);
        }

        // Display current automaton name
        DrawText(TextFormat("Current: %s", current->name), 10, 50, 20, GREEN);
        DrawText("Use LEFT/RIGHT arrows to switch", 10, 80, 16, GRAY);
        
        DrawText("Press Spacebar To Regenerate Randomly",130,HEIGHT/1.1,22,GRAY);

        EndDrawing();
    }

    // Free allocated memory
    free(gameOfLife);
    free(morley);
    free (assimilation);
    free(briansBrain);

    CloseWindow();
}