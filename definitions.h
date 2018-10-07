#ifndef DEF_H
#define DEF_H

#include <vector>

const float  _version=1.11;
const double _pi=3.1415926;
const double _deg2rand=0.0174532925;
const int    _world_size=10;
const int    _station_numof_slots=9;
const float  _world_height=1.0;
const float  _world_wall_size=1.0;
const float  _world_wall_thickness=0.4;
const float  _world_wall_thickness_drawn=0.05;
const float  _play_time_per_turn_static=10.0;
const float  _play_time_per_turn_dynamic_coef=5.0;
const float  _play_time_per_turn_dynamic_const=5.0;
const float  _player_swap_time=4.0;
const float  _player_swap_height=1.5;
const float  _cube_transfer_time=0.4;
const float  _cube_size=0.05;
const float  _cube_spacing=0.025;
const float  _station_size=0.4;
const float  _station_thickness=0.1;
const float  _station_spaceing=0.3;
const float  _delay_arm_pickup=0.5;
const float  _arm_length_short=0.21;
const float  _arm_length_long=0.5;
const float  _arm_length_reach=0.6;
const float  _arm_grab_anim_length=0.22;
const float  _player_size=0.6;
const float  _player_col_move_speed=0.01;
const float  _player_head_offset_max=0.1;
const float  _look_inversion_toggle_delay_time=0.5;
const float  _intro_shade_time=4.0;
const float  _key_delay_time=0.17;
const float  _game_over_time=6.0;
const float  _ingame_exit_time=1.0;
const float  _wall_edge_push_limit=0.4;
const float  _wall_edge_push_sens=0.02;
const float  _time_warning_sound_time=1.5;//played when x seconds left
const float  _intro_menu_zoom_time=0.7;
const float  _music_fade_time=0.7;
const float  _music_fade_min=0.2;
const float  _music_fade_max=0.5;
const float  _music_vol_menu=0.8;

const float  _player_color[9][3]=//color array for 9 players
{
    {1,0,0},//red
    {0,1,0},//green
    {0,0,1},//blue
    {1,1,0},//yellow
    {0.1,0.1,0.1},//black
    {1,0,1},//magenta
    {1,0.5,0},//orange
    {0,1,1},//cyan
    {0.5,0.5,1}//blue light
};

enum powcube_states
{
    pcs_error=0,
    pcs_in_station,
    pcs_carried_in_arm,
    pcs_in_battery_slot
};

enum player_arm_states
{
    pas_idle=0,
    pas_extend,
    pas_retract
};

struct st_pos_int
{
    st_pos_int()
    {
        x=0, y=0;
        flag=false;
        value=0;
    }
    st_pos_int(int _x,int _y)
    {
        x=_x, y=_y;
        flag=false;
        value=0;
    }
    st_pos_int(int _x,int _y,int _val)
    {
        x=_x, y=_y;
        flag=false;
        value=_val;
    }

    int x,y;
    bool flag;
    int value;
};

enum game_states
{
    gs_init=0,
    gs_menu,
    gs_ingame
};

enum draw_types
{
    dt_error=0,
    dt_player,
    dt_station,
    dt_menu_robot,
    dt_menu_station
};

#endif
