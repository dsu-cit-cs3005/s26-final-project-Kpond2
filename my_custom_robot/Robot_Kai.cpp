#include "RobotBase.h"

class KaiBot : public RobotBase {
private:
    std::vector<RadarObj> m_last_results;
    int m_sweep_dir = 1;

public:
    KaiBot() : RobotBase(3, 5, WeaponType::railgun) {
        m_name = "Kai_Apex_V4";
        m_character = 'K';
    }

    void get_radar_direction(int& radar_direction) override {
        radar_direction = m_sweep_dir;
        m_sweep_dir = (m_sweep_dir % 8) + 1; 
    }

    void process_radar_results(const std::vector<RadarObj>& radar_results) override {
        m_last_results = radar_results;
    }

    bool get_shot_location(int& shot_row, int& shot_col) override {
        if (!m_last_results.empty()) {
            shot_row = m_last_results[0].m_row;
            shot_col = m_last_results[0].m_col;
            return true; 
        }
        return false; 
    }

    void get_move_direction(int &direction, int &distance) override {
        int r, c; get_current_location(r, c);
        // Head for the corner (0,0) then patrol the top row
        if (r > 0) direction = 1;
        else if (c > 0) direction = 7;
        else direction = (rand() % 2 == 0) ? 3 : 7; // Patrol left/right
        distance = get_move_speed(); 
    }
};

extern "C" RobotBase* create_robot() { return new KaiBot(); }
extern "C" const char* robot_summary() { return "Systematic Railgunner with corner-patrol logic."; }
