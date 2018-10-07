#ifndef MENU_H
#define MENU_H

#include <iostream>
#include <gl/gl.h>
#include <gl/glu.h>
#include <math.h>
#include "model_obj.h"
#include "definitions.h"

class menu
{
    public:
        menu();

        bool init(float fov,float ratio,int textures[3],model_obj* pModel_robot,model_obj* pModel_station);
        bool update(float m_time_diff_cycle);
        bool draw(void);
        bool inc_player_counter(void);
        bool dec_player_counter(void);
        int  get_numof_players(void);
        bool show_help(bool flag);

    private:

        bool  m_show_help;
        int   m_player_counter;
        int   m_tex_title,m_tex_help,m_tex_extra_text;
        float m_intro_zoom_timer;
        float m_screen_fov,m_screen_aspect_ratio,m_screen_aspect_ratio_real;//real is the given value, other is culled to max 16:9
        model_obj* m_pModel_robot;
        model_obj* m_pModel_station;
};

#endif // MENU_H
