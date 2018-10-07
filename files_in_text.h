#ifndef FILES_IN_TEXT_H
#define FILES_IN_TEXT_H

#include <string>
#include <iostream>

using namespace std;

//Data files converted with Base64 to text

enum data_files//list of all files
{
    file_texture_extratext,
    file_texture_help,
    file_texture_title,

    file_model_robot2,
    file_model_station,
    file_model_robot_body,
    file_model_robot_head,
    file_model_robot_shoulder,
    file_model_robot_hand,
    file_model_robot_throat,

    file_sound_arm_pickup,
    file_sound_arm_pickup_nothing,
    file_sound_cube_from_station,
    file_sound_end_turn_warning,
    file_sound_engine_arm,
    file_sound_engine_body,
    file_sound_engine_head,
    file_sound_match_start,
    file_sound_menu_click,
    file_sound_turn_end,
    file_sound_turn_start,
    file_sound_win,
    file_sound_music_intro,
    file_sound_music_loop
};

string load_base64_file(int file_id);//will return encoded text

#endif
