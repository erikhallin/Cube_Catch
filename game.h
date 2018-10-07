#ifndef GAME_H
#define GAME_H

#include <SOIL/SOIL.h>
#include <gl/gl.h>
#include <vector>
#include <time.h>
#include <math.h>
#include <iostream>
#include "files_in_text.h"
#include "base64.h"
#include "world.h"
#include "player.h"
#include "gamepad.h"
#include "powcube.h"
#include "station.h"
#include "model_obj.h"
#include "menu.h"
#include "sound.h"
#include "definitions.h"

using namespace std;

class game
{
    public:
        game();

        bool init(bool* pKeys,int numof_players,float time_per_turn,float player_swap_time,float fov,float ratio);
        bool update(void);
        bool draw(void);

    private:

        world* m_pWorld;
        vector<player>  m_vec_player;
        vector<powcube> m_vec_powcube;
        vector<station> m_vec_station;
        float m_player_height,m_player_arm_height,m_player_shoulder_length;
        int   m_curr_player_ind;
        float m_play_time_curr_turn;

        //Input
        bool*   m_pKeys;
        gamepad m_gamepad[4];
        float   m_look_inversion_toggle_delay;
        int     m_selected_gamepad_ind;
        float   m_key_input_delay_timer;
        float   m_screen_aspect_ratio;

        //Time
        float m_time_last_cycle,m_time_this_cycle,m_time_diff_cycle;
        float m_delay_timer_pickup,m_play_time_per_turn;
        float m_player_swap_time,m_player_swap_timer,m_intro_shade_timer,m_game_over_timer,m_ingame_exit_timer;

        //Sound
        bool   m_time_warning_sound_played,m_music_intro_done,m_music_fade_out,m_music_fade_in;
        float  m_music_fade_timer;
        int    m_sound_channel_motor,m_sound_channel_arm,m_sound_channel_head,m_sound_channel_music;
        sound* m_pSoundEngine;
        bool   load_sounds(void);

        //Misc
        menu* m_pMenu;
        int   m_game_state;
        bool  m_player_swap_mode;
        float m_player_swap_cam_pos_start[5];//x,y,z,yaw,pitch
        float m_player_swap_cam_pos_end[5];

        bool  m_game_over;
        int   game_over_test(void);//returns winner ind
        bool  reset_game(int numof_players);

        //Textures
        int   m_tex_title,m_tex_help,m_tex_extra_text;
        bool  load_textures(void);

        model_obj* m_pModel_robot_menu;
        model_obj* m_pModel_station_menu;
        model_obj* m_pModel_player_body;
        model_obj* m_pModel_player_shoulder;
        model_obj* m_pModel_player_hand;
        model_obj* m_pModel_player_head;
        model_obj* m_pModel_station;
        model_obj* m_pModel_player_throat;
};

#endif // GAME_H
