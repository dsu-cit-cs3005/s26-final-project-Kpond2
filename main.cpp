#include "Arena.h"
#include <ctime>

int main() {
    srand(time(NULL));
    
    // 15x15 is easier to see in a terminal than 20x20
    Arena game(15, 15);

    if(!game.load_robot("Robot_Ratboy.cpp")) {
        std::cout << "Failed to load Ratboy" << std::endl;
    }
    
    if(!game.load_robot("Robot_Kai.cpp")) {
        std::cout << "Failed to load KaiBot" << std::endl;
    }

    game.run_game();
    
    return 0;
}
