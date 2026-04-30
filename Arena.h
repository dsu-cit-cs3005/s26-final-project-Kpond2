#ifndef ARENA_H
#define ARENA_H

#include <vector>
#include <string>
#include <iostream>
#include <dlfcn.h>
#include "RobotBase.h"
#include "RadarObj.h"

// This struct tracks the "Master" state of each robot in your game
struct RobotInstance {
    RobotBase* robot;     // Pointer to the loaded robot
    void* handle;         // Handle for the .so file
    int x, y;             // Current position on the grid
    int health;           // Tracked via ri.robot->get_health()
    bool is_alive = true;
    std::string name;
};

class Arena {
private:
    int width, height;
    std::vector<std::vector<char>> grid;
    std::vector<RobotInstance> robots;

    // Helper to update the 2D display grid
    void update_grid();
    
    // Helper to deal damage based on coordinates
    void resolve_damage(int r, int c, int damage);

public:
    Arena(int w, int h);
    ~Arena();

    // Compiles and loads the .cpp robot files
    bool load_robot(std::string cpp_file);
    
    // The main game loop
    void run_game();
    
    // Prints the board to the terminal
    void display();
};

#endif
