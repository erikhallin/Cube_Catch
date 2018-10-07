#ifndef POWCUBE_H
#define POWCUBE_H

#include <gl/gl.h>
#include <math.h>
#include "definitions.h"

#include <iostream>
using namespace std;

class powcube
{
    public:

        powcube();

        bool init(int owner_id);
        bool update(float time_dif);
        bool draw(void);
        bool set_pos(float pos[5]);//x,y,z,yaw,pitch
        bool get_pos(float pos[5]);//x,y,z,yaw,pitch
        bool point_inside_cube_fast(float _x,float _y, float _z,float size_factor);
        int  get_status(void);
        bool set_status(int status,int id);
        int  get_carrier_id(void);
        bool set_station_id(int station_id);
        int  get_station_id(void);
        bool set_animation(float start_pos[5],float end_pos[5]);
        bool set_animation_from_curr_pos(float end_pos[5]);
        int  get_owner_id(void);

    private:

        int   m_owner_id,m_carried_id,m_at_station_id;
        int   m_state;
        float m_pos_x,m_pos_y,m_pos_z;
        float m_rot_yaw,m_rot_pitch;
        float m_anim_progress,m_anim_speed;
        float m_anim_start_pos_x,m_anim_start_pos_y,m_anim_start_pos_z,m_anim_start_rot_yaw,m_anim_start_rot_pitch;
        float m_anim_end_pos_x,m_anim_end_pos_y,m_anim_end_pos_z,m_anim_end_rot_yaw,m_anim_end_rot_pitch;
};

#endif // POWCUBE_H
