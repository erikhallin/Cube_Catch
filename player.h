#ifndef PLAYER_H
#define PLAYER_H

#include <gl/gl.h>
#include "model_obj.h"
#include "definitions.h"

#include <iostream> //temp
using namespace std;//temp

class player
{
    public:
        player(model_obj* pModel_part[],int color_id);

        bool set_location(float xpos,float ypos);
        bool get_location(float loc[2]);
        bool set_pos(float pos[8]);//x,y,yaw_body,pitch_body,yaw_eye,pitch_eye,arm_yaw,arm_pitch
        bool get_pos(float pos[8]);//x,y,yaw_body,pitch_body,yaw_eye,pitch_eye,arm_yaw,arm_pitch
        bool update(float time_dif);
        bool draw(bool draw_active_player);
        int  get_arm_status(void);
        bool set_arm_status(int status);
        int  get_battery_status(void);
        bool set_battery_status(int status);
        bool anim_grab(int arm_state,float arm_length_req);
        bool point_inside_player_fast(float _x,float _y,float size_factor=1.0);
        bool get_look_inversion(bool inverse_flag[2]);
        bool set_look_inversion(bool inverse_flag[2]);

    private:

        float m_pos_x,m_pos_y;
        float m_rot_yaw_body,m_rot_pitch_body,m_rot_yaw_eye,m_rot_pitch_eye,m_rot_yaw_arm,m_rot_pitch_arm;
        int   m_arm_carry_id,m_battery_cube_id;//-1 = empty
        int   m_arm_state;
        int   m_color_id;
        float m_arm_length_curr,m_arm_grab_anim_timer,m_arm_length_req;
        bool  m_inverse_look[2];//left, right
        model_obj* m_pModel_body;
        model_obj* m_pModel_shoulder;
        model_obj* m_pModel_hand;
        model_obj* m_pModel_head;
        model_obj* m_pModel_throat;
};

#endif // PLAYER_H
