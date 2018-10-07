#ifndef WORLD_H
#define WORLD_H

#include <gl/gl.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <iostream>
#include <math.h>
#include "definitions.h"

using namespace std;

class world
{
    public:
        world();

        bool init(int numof_players,vector<st_pos_int>& vec_station_pos);
        bool update(void);
        bool draw(void);
        bool test_for_collision_nofix(float pos_new[2],float pos_old[2]);
        bool test_for_collision(float pos_new[2],float pos_old[2]);//will adjust the new pos
        bool wall_crossed(float pos_new[2],float pos_old[2]);
        bool close_to_wall_edge(int x_node,int y_node);

    private:

        float m_world_size;
        int   m_arr_wall[_world_size][_world_size];
        int   m_arr_connected_pos[_world_size][_world_size];
};

#endif // WORLD_H
