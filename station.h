#ifndef STATION_H
#define STATION_H

#include <iostream> //temp debug

#include <gl/gl.h>
#include <vector>
#include "model_obj.h"
#include "definitions.h"

using namespace std;

class station
{
    public:
        station();
        station(float x_pos,float y_pos,int direction,int owner_id,model_obj* pModel);

        bool update(void);
        bool draw(void);
        bool cube_place();
        bool in_station_pos(int x_pos,int y_pos);
        bool point_inside_station(float _x,float _y,float _z);
        bool add_cube_to_station(int cube_id,float cube_pos_final[5],float test_point_v[3]);
        bool remove_cube_from_station(int cube_id);
        bool get_list_of_cube_in_station(vector<int>& vec_cube_list);

    private:

        float m_pos_x,m_pos_y;
        int   m_direction;
        int   m_owner_id;
        int   m_slot_taken[9];//stores cube id, -1 = empty
        model_obj* m_pModel;

        vector<int> m_vec_contained_powcubes_id;
};

#endif // STATION_H
