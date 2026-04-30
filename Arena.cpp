#include "Arena.h"
#include <cmath>
#include <algorithm>
#include <thread>
#include <chrono>
#include <iomanip>

Arena::Arena(int w, int h) : width(w), height(h) {
    grid.resize(height, std::vector<char>(width, ' '));
}

Arena::~Arena() {
    for (auto& ri : robots) {
        delete ri.robot;
        dlclose(ri.handle);
    }
}

bool Arena::load_robot(std::string cpp_file) {
    std::string so_file = "./lib" + cpp_file.substr(0, cpp_file.find_last_of('.')) + ".so";
    std::string cmd = "g++ -fPIC -shared " + cpp_file + " RobotBase.cpp -o " + so_file;
    if (system(cmd.c_str()) != 0) return false;

    void* handle = dlopen(so_file.c_str(), RTLD_NOW);
    if (!handle) return false;

    auto create = (RobotBase* (*)())dlsym(handle, "create_robot");
    if (!create) return false;

    RobotInstance ri;
    ri.robot = create();
    ri.handle = handle;
    ri.name = ri.robot->m_name; 
    ri.x = rand() % width;
    ri.y = rand() % height;
    ri.robot->move_to(ri.y, ri.x); 
    ri.robot->set_boundaries(height, width);
    ri.health = ri.robot->get_health();
    robots.push_back(ri);
    return true;
}

void Arena::resolve_damage(int r, int c, int damage) {
    if (r < 0 || r >= height || c < 0 || c >= width) return;
    for (auto& ri : robots) {
        if (ri.is_alive && ri.y == r && ri.x == c) {
            ri.health = ri.robot->take_damage(damage);
            ri.robot->reduce_armor(1); // Armor degrades on every hit
            if (ri.health <= 0) ri.is_alive = false;
        }
    }
}

void Arena::run_game() {
    int round = 1;
    while (true) {
        int alive_count = 0;
        for (auto& r : robots) if (r.is_alive) alive_count++;
        std::cout << "\033[2J\033[1;1H" << "--- ROUND " << round++ << " ---" << std::endl;
        display();
        std::cout << "\nSTATUS:" << std::endl;
        for (auto& ri : robots) {
            std::cout << (ri.is_alive ? "[LIVE] " : "[DEAD] ") 
                      << std::left << std::setw(18) << ri.name 
                      << " HP: " << std::setw(3) << ri.health 
                      << " Armor: " << ri.robot->get_armor() << std::endl;
        }
        if (alive_count <= 1) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        for (auto& ri : robots) {
            if (!ri.is_alive) continue;
            char tile = grid[ri.y][ri.x];
            if (tile == 'F') ri.health = ri.robot->take_damage(10);
            if (tile == 'P') ri.robot->disable_movement();

            int r_dir = 0; ri.robot->get_radar_direction(r_dir);
            std::vector<RadarObj> scans;
            for(auto& other : robots) {
                if(&ri == &other || !other.is_alive) continue;
                scans.push_back(RadarObj('R', other.y, other.x));
            }
            ri.robot->process_radar_results(scans);

            int sr, sc, md, mt;
            if (ri.robot->get_shot_location(sr, sc)) {
                WeaponType w = ri.robot->get_weapon();
                if (w == WeaponType::grenade) {
                    for(int i = -1; i <= 1; ++i)
                        for(int j = -1; j <= 1; ++j)
                            resolve_damage(sr + i, sc + j, 20);
                    ri.robot->decrement_grenades();
                } else if (w == WeaponType::hammer) {
                    if (abs(sr - ri.y) <= 1 && abs(sc - ri.x) <= 1) resolve_damage(sr, sc, 40);
                } else { resolve_damage(sr, sc, 25); }
            } else {
                ri.robot->get_move_direction(md, mt);
                if (md >= 1 && md <= 8) {
                    int nr = ri.y + (directions[md].first * mt);
                    int nc = ri.x + (directions[md].second * mt);
                    if (nr >= 0 && nr < height && nc >= 0 && nc < width && grid[nr][nc] != 'M') {
                        ri.y = nr; ri.x = nc;
                        ri.robot->move_to(ri.y, ri.x);
                    }
                }
            }
        }
    }
}

void Arena::display() {
    for (auto& row : grid) std::fill(row.begin(), row.end(), ' ');
    grid[5][5] = 'M'; grid[3][10] = 'F'; grid[12][4] = 'P';
    for (auto& ri : robots) {
        char icon = ri.robot->m_character;
        if (icon == ' ' || icon == '\0') icon = (ri.name.find("Kai") != std::string::npos) ? 'K' : 'R';
        grid[ri.y][ri.x] = ri.is_alive ? icon : 'X';
    }
    std::cout << "+" << std::string(width, '-') << "+" << std::endl;
    for (auto& row : grid) {
        std::cout << "|"; for (char c : row) std::cout << c; std::cout << "|" << std::endl;
    }
    std::cout << "+" << std::string(width, '-') << "+" << std::endl;
}
