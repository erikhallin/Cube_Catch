#include "game.h"

game::game()
{
    m_game_state=gs_init;
}

bool game::init(bool* pKeys,int numof_players,float time_per_turn,float player_swap_time,float fov,float ratio)
{
    //calc playtime per turn
    m_play_time_per_turn=time_per_turn=numof_players*_play_time_per_turn_dynamic_coef+_play_time_per_turn_dynamic_const;

    cout<<"Software started, version "<<_version<<endl;
    cout<<"Number of Players: "<<numof_players<<endl;
    cout<<"Play time per turn: "<<time_per_turn<<" seconds\n";
    cout<<"Player swap time: "<<player_swap_time<<" seconds\n";

    //load textures
    if( !load_textures() )
    {
        cout<<"ERROR: Could not load textures\n";
        return false;
    }

    //load models
    m_pModel_robot_menu=new model_obj(file_model_robot2);
    m_pModel_robot_menu->draw_settings(dt_menu_robot);
    m_pModel_station_menu=new model_obj(file_model_station);
    m_pModel_station_menu->draw_settings(dt_menu_station);
    m_pModel_player_body=new model_obj(file_model_robot_body);
    m_pModel_player_body->draw_settings(dt_player);
    m_pModel_player_head=new model_obj(file_model_robot_head);
    m_pModel_player_head->draw_settings(dt_player);
    m_pModel_player_shoulder=new model_obj(file_model_robot_shoulder);
    m_pModel_player_shoulder->draw_settings(dt_player);
    m_pModel_player_hand=new model_obj(file_model_robot_hand);
    m_pModel_player_hand->draw_settings(dt_player);
    m_pModel_player_throat=new model_obj(file_model_robot_throat);
    m_pModel_player_throat->draw_settings(dt_player);
    m_pModel_station=new model_obj(file_model_station);
    m_pModel_station->draw_settings(dt_station);

    /*//load models OLD
    m_pModel_robot_menu=new model_obj("robot2.obj");
    m_pModel_robot_menu->draw_settings(dt_menu_robot);
    m_pModel_station_menu=new model_obj("station.obj");
    m_pModel_station_menu->draw_settings(dt_menu_station);
    m_pModel_player_body=new model_obj("robot_body.obj");
    m_pModel_player_body->draw_settings(dt_player);
    m_pModel_player_head=new model_obj("robot_head.obj");
    m_pModel_player_head->draw_settings(dt_player);
    m_pModel_player_shoulder=new model_obj("robot_shoulder.obj");
    m_pModel_player_shoulder->draw_settings(dt_player);
    m_pModel_player_hand=new model_obj("robot_hand.obj");
    m_pModel_player_hand->draw_settings(dt_player);
    m_pModel_player_throat=new model_obj("robot_throat.obj");
    m_pModel_player_throat->draw_settings(dt_player);
    m_pModel_station=new model_obj("station.obj");
    m_pModel_station->draw_settings(dt_station);*/

    model_obj* player_model_parts[]={m_pModel_player_body,
                                     m_pModel_player_shoulder,
                                     m_pModel_player_hand,
                                     m_pModel_player_head,
                                     m_pModel_player_throat};

    m_pKeys=pKeys;

    //load sound
    m_pSoundEngine=new sound();
    if( !load_sounds() )
    {
        cout<<"ERROR: Could not load sounds\n";
        return false;
    }
    m_sound_channel_motor=10;
    m_sound_channel_arm=11;
    m_sound_channel_head=12;
    m_sound_channel_music=20;
    //init motor sounds
    m_pSoundEngine->playSimpleSound(wav_engine_body,0.0,m_sound_channel_motor);
    m_pSoundEngine->playSimpleSound(wav_engine_arm,0.0,m_sound_channel_arm);
    m_pSoundEngine->playSimpleSound(wav_engine_head,0.0,m_sound_channel_head);
    //start music intro
    m_pSoundEngine->playSimpleSound(wav_music_intro,_music_vol_menu,m_sound_channel_music,false);
    m_music_intro_done=false;

    //temp, test sound
    //m_pSoundEngine->playSimpleSound(wav_beep1,1.0);

    m_pWorld=new world();
    vector<st_pos_int> vec_station_pos;
    m_pWorld->init(numof_players,vec_station_pos);

    //init menu
    int textures[3]={m_tex_title,m_tex_help,m_tex_extra_text};
    m_pMenu=new menu();
    m_pMenu->init(fov,ratio,textures,m_pModel_robot_menu,m_pModel_station_menu);
    m_screen_aspect_ratio=ratio;

    m_play_time_curr_turn=0.0;
    m_player_height=0.7;
    m_player_arm_height=0.38;
    m_player_shoulder_length=0.28;
    m_curr_player_ind=0;
    m_play_time_per_turn=time_per_turn;
    m_player_swap_time=player_swap_time;
    m_player_swap_mode=true;
    m_player_swap_timer=0.0;
    m_game_over=false;
    m_look_inversion_toggle_delay=0.0;
    m_selected_gamepad_ind=0;
    m_intro_shade_timer=_intro_shade_time;
    m_key_input_delay_timer=0.0;
    m_game_over_timer=_game_over_time;
    m_ingame_exit_timer=_ingame_exit_time;
    m_time_warning_sound_played=false;
    m_music_fade_in=m_music_fade_out=false;
    m_music_fade_timer=0.0;

    //add players and stations, share the same index
    for(int player_i=0;player_i<numof_players;player_i++)
    {
        //the player
        m_vec_player.push_back( player(player_model_parts,player_i) );
        float rotation_body=0;
        switch(vec_station_pos[player_i].value)//direction
        {
            case 0: rotation_body=0; break;
            case 1: rotation_body=270; break;
            case 2: rotation_body=180; break;
            case 3: rotation_body=90; break;
        }
        float player_pos[8]={vec_station_pos[player_i].x+0.5,vec_station_pos[player_i].y+0.5,
                             rotation_body,0,rotation_body,0,rotation_body,0};
        m_vec_player[player_i].set_pos(player_pos);

        //the station
        m_vec_station.push_back( station( vec_station_pos[player_i].x,
                                          vec_station_pos[player_i].y,
                                          vec_station_pos[player_i].value,
                                          player_i,
                                          m_pModel_station ) );

        //add owners cube to that station
        m_vec_powcube.push_back( powcube() );
        m_vec_powcube.back().init(player_i);
        float final_cube_pos[5];
        float temp_test_point_v[3]={0,0,1};
        m_vec_station.back().add_cube_to_station( (int)m_vec_powcube.size()-1, final_cube_pos, temp_test_point_v );
        m_vec_powcube.back().set_pos(final_cube_pos);
        m_vec_powcube.back().set_status(pcs_in_station,-1);
        m_vec_powcube.back().set_station_id(player_i);
    }

    //set startpos for camera
    float player_pos[8];
    m_vec_player[m_curr_player_ind].get_pos(player_pos);
    m_player_swap_cam_pos_end[0]=player_pos[0];
    m_player_swap_cam_pos_end[1]=player_pos[1];
    m_player_swap_cam_pos_end[2]=m_player_height;
    m_player_swap_cam_pos_end[3]=player_pos[4];
    m_player_swap_cam_pos_end[4]=player_pos[5];
    m_player_swap_cam_pos_start[0]=_world_size*0.5;
    m_player_swap_cam_pos_start[1]=_world_size*0.5;
    m_player_swap_cam_pos_start[2]=5;
    m_player_swap_cam_pos_start[3]=361;
    m_player_swap_cam_pos_start[4]=-90;

    /*//temp extra players
    m_vec_player.push_back( player() );
    m_vec_player.push_back( player() );
    float player_pos2[8]={6.5,5.5,10,0,10,0,10,10};
    float player_pos3[8]={7.5,5.5,10,0,10,10,0,0};
    m_vec_player[1].set_pos(player_pos2);
    m_vec_player[2].set_pos(player_pos3);*/

    //temp powcubes
    /*m_vec_powcube.push_back( powcube() );
    float powcube_pos1[5]={3,3,0.5,0,0};
    m_vec_powcube.back().set_pos( powcube_pos1 );
    m_vec_powcube.back().init(0);
    m_vec_powcube.push_back( powcube() );
    float powcube_pos2[5]={3.1,3,0.5,0,0};
    m_vec_powcube.back().set_pos( powcube_pos2 );
    m_vec_powcube.back().init(0);
    m_vec_powcube.push_back( powcube() );
    float powcube_pos3[5]={3.2,4,0.5,0,0};
    m_vec_powcube.back().set_pos( powcube_pos3 );
    m_vec_powcube.back().init(0);
    m_vec_powcube.push_back( powcube() );
    float powcube_pos4[5]={4,4,0.5,0,0};
    m_vec_powcube.back().set_pos( powcube_pos4 );
    m_vec_powcube.back().init(0);
    m_vec_powcube.push_back( powcube() );
    float powcube_pos5[5]={3.5,3,0.5,0,0};
    m_vec_powcube.back().set_pos( powcube_pos5 );
    m_vec_powcube.back().init(0);*/

    /*//temp stations
    m_vec_station.push_back( station( 0,0,0,0 ) );
    m_vec_station.push_back( station( 1,0,1,0 ) );
    m_vec_station.push_back( station( 2,0,2,0 ) );
    m_vec_station.push_back( station( 3,0,3,0 ) );*/


    //init gamepads (but only one in use)
    for(int i=0;i<4;i++)
    {
        m_gamepad[i]=gamepad(i);
    }

    m_time_last_cycle=m_time_this_cycle=(float)clock()/CLOCKS_PER_SEC;
    m_time_diff_cycle=0.0;
    m_delay_timer_pickup=0.0;
    m_game_state=gs_menu;

    return true;
}

bool game::update(void)//returns false if game restarts
{
    //time
    m_time_last_cycle=m_time_this_cycle;//store time for last cycle
    m_time_this_cycle=(float)clock()/CLOCKS_PER_SEC;//get time now, in sec
    m_time_diff_cycle=m_time_this_cycle-m_time_last_cycle;

    //FPS
    //cout<<1.0/m_time_diff_cycle<<endl;
    /*static float fps_sum=0.0;
    static int fps_counter=0;
    float max_counter=20.0;
    if(fps_counter<max_counter)
    {
        fps_sum+=m_time_diff_cycle;
        fps_counter++;
    }
    else
    {
        fps_counter=0;
        float fps=1.0/(fps_sum/max_counter);
        cout<<"FPS: "<<fps<<endl;
    }*/

    //delays
    if(m_delay_timer_pickup>0) m_delay_timer_pickup-=m_time_diff_cycle;
    if(m_look_inversion_toggle_delay>0) m_look_inversion_toggle_delay-=m_time_diff_cycle;
    if(m_intro_shade_timer>0) m_intro_shade_timer-=m_time_diff_cycle;
    if(m_key_input_delay_timer>0) m_key_input_delay_timer-=m_time_diff_cycle;

    //music update
    if(!m_music_intro_done)
    {
        if( m_pSoundEngine->get_source_status(m_sound_channel_music) )//true = done
        {
            m_music_intro_done=true;
            //start to play music loop
            if(m_game_state==gs_ingame)
             m_pSoundEngine->playSimpleSound(wav_music_loop,_music_fade_max,m_sound_channel_music,true);
            else
             m_pSoundEngine->playSimpleSound(wav_music_loop,_music_vol_menu,m_sound_channel_music,true);
        }
    }
    if(m_music_fade_out)
    {
        m_music_fade_timer+=m_time_diff_cycle;
        if(m_music_fade_timer>_music_fade_time)
        {
            m_music_fade_out=false;
            m_music_fade_timer=0.0;
            //set music vol
            m_pSoundEngine->set_volume(m_sound_channel_music,_music_fade_min);
        }
        else//set music vol
        {
            float vol=(1.0-m_music_fade_timer/_music_fade_time)*(_music_fade_max-_music_fade_min)+_music_fade_min;
            m_pSoundEngine->set_volume(m_sound_channel_music,vol);
        }
    }
    if(m_music_fade_in)
    {
        m_music_fade_timer+=m_time_diff_cycle;
        if(m_music_fade_timer>_music_fade_time)
        {
            m_music_fade_in=false;
            //set music vol
            m_pSoundEngine->set_volume(m_sound_channel_music,_music_vol_menu);
        }
        else//set music vol
        {
            float vol=(m_music_fade_timer/_music_fade_time)*(_music_vol_menu-_music_fade_min)+_music_fade_min;
            m_pSoundEngine->set_volume(m_sound_channel_music,vol);
        }
    }

    switch(m_game_state)
    {
        case gs_init:
        {
            //should not happend
        }break;

        case gs_menu:
        {
            m_pMenu->update(m_time_diff_cycle);

            //wait for controller to be chosen
            st_gamepad_data gamepad_data;
            for(int pad_ind=0;pad_ind<4;pad_ind++)
            {
                if( m_gamepad[pad_ind].IsConnected() )
                {
                    gamepad_data=m_gamepad[pad_ind].GetState();

                    //help button, hold to show
                    if(gamepad_data.button_back)
                    {
                        m_pMenu->show_help(true);
                        break;//if others was to be tested, it would hide the help again
                    }
                    else m_pMenu->show_help(false);

                    //test if numof player changed
                    //INC if:
                    if(m_key_input_delay_timer<=0.0 && (gamepad_data.button_A ||
                                                        gamepad_data.button_right_trigger ||
                                                        gamepad_data.button_RB ||
                                                        gamepad_data.button_X ||
                                                        gamepad_data.dpad_up ||
                                                        gamepad_data.dpad_right ||
                                                        gamepad_data.thumbstick_left_x>10000 ||
                                                        gamepad_data.thumbstick_left_y>10000 ||
                                                        gamepad_data.thumbstick_right_x>10000 ||
                                                        gamepad_data.thumbstick_right_y>10000 ) )
                    {
                        if( m_pMenu->inc_player_counter() ) m_pSoundEngine->playSimpleSound(wav_menu_click,1.0);//play sound if val changed
                        m_key_input_delay_timer=_key_delay_time;

                    }
                    //DEC if:
                    if(m_key_input_delay_timer<=0.0 && (gamepad_data.button_B ||
                                                        gamepad_data.button_left_trigger ||
                                                        gamepad_data.button_LB ||
                                                        gamepad_data.button_Y ||
                                                        gamepad_data.dpad_down ||
                                                        gamepad_data.dpad_left ||
                                                        gamepad_data.thumbstick_left_x<-10000 ||
                                                        gamepad_data.thumbstick_left_y<-10000 ||
                                                        gamepad_data.thumbstick_right_x<-10000 ||
                                                        gamepad_data.thumbstick_right_y<-10000 ) )
                    {
                        if( m_pMenu->dec_player_counter() ) m_pSoundEngine->playSimpleSound(wav_menu_click,1.0);//play sound if val changed
                        m_key_input_delay_timer=_key_delay_time;
                    }

                    //test if game should start
                    if(gamepad_data.button_start)
                    {
                        m_selected_gamepad_ind=pad_ind;
                        cout<<"Selected gamepad for input: "<<pad_ind+1<<endl;

                        //reset the game with correct number of players
                        reset_game( m_pMenu->get_numof_players() );
                        //start the game
                        m_game_state=gs_ingame;
                        m_intro_shade_timer=_intro_shade_time;
                        //set music volume to ingame volume
                        m_pSoundEngine->set_volume(m_sound_channel_music,_music_fade_max);
                        //play sound
                        m_pSoundEngine->playSimpleSound(wav_match_start,1.0);
                        break;
                    }
                }
            }

        }break;

        case gs_ingame:
        {
            //update powcubes
            for(int powcube_ind=0;powcube_ind<(int)m_vec_powcube.size();powcube_ind++)
            {
                m_vec_powcube[powcube_ind].update(m_time_diff_cycle);
            }

            //update players
            for(int player_i=0;player_i<(int)m_vec_player.size();player_i++)
            {
                m_vec_player[player_i].update(m_time_diff_cycle);
            }

            //next player input, temp
            static float key_delay_dext_player=0.0;
            if(key_delay_dext_player>0.0) key_delay_dext_player-=m_time_diff_cycle;
            /*if(m_pKeys[32] && key_delay_dext_player<=0)
            {
                key_delay_dext_player=0.5;
                m_curr_player_ind++;
                if(m_curr_player_ind>=(int)m_vec_player.size()) m_curr_player_ind=0;
            }*/
            //next player time
            if(!m_player_swap_mode && !m_game_over && (int)m_vec_player.size()>1)//time swap ON/OFF
            {
                m_play_time_curr_turn+=m_time_diff_cycle;
                if(m_play_time_curr_turn>=m_play_time_per_turn)
                {
                    m_play_time_curr_turn=0.0;

                    int player_ind_curr=m_curr_player_ind;
                    m_curr_player_ind++;
                    if(m_curr_player_ind>=(int)m_vec_player.size()) m_curr_player_ind=0;
                    int player_ind_next=m_curr_player_ind;

                    m_player_swap_mode=true;
                    m_player_swap_timer=0.0;

                    //find start and end pos for camera animation
                    float player_pos[8];
                    m_vec_player[player_ind_curr].get_pos(player_pos);
                    m_player_swap_cam_pos_start[0]=player_pos[0];
                    m_player_swap_cam_pos_start[1]=player_pos[1];
                    m_player_swap_cam_pos_start[2]=m_player_height;
                    m_player_swap_cam_pos_start[3]=player_pos[4];
                    m_player_swap_cam_pos_start[4]=player_pos[5];
                    //next player
                    m_vec_player[player_ind_next].get_pos(player_pos);
                    m_player_swap_cam_pos_end[0]=player_pos[0];
                    m_player_swap_cam_pos_end[1]=player_pos[1];
                    m_player_swap_cam_pos_end[2]=m_player_height;
                    m_player_swap_cam_pos_end[3]=player_pos[4];
                    m_player_swap_cam_pos_end[4]=player_pos[5];

                    //stop motor sounds
                    m_pSoundEngine->stopSound(m_sound_channel_motor);
                    m_pSoundEngine->stopSound(m_sound_channel_arm);
                    m_pSoundEngine->stopSound(m_sound_channel_head);

                    //play sound
                    m_pSoundEngine->playSimpleSound(wav_turn_end,1.0);
                    m_time_warning_sound_played=false;
                }
                else if(!m_time_warning_sound_played)//maybe play time warning sound
                {
                    if(m_play_time_per_turn-m_play_time_curr_turn<_time_warning_sound_time)
                    {
                        m_time_warning_sound_played=true;
                        //play sound
                        m_pSoundEngine->playSimpleSound(wav_end_turn_warning,1.0);
                    }
                }
            }

            if(m_player_swap_mode)//animate camera shift to next player
            {
                m_player_swap_timer+=m_time_diff_cycle;
                if(m_player_swap_timer>=m_player_swap_time)
                {
                    m_player_swap_timer=0.0;
                    m_player_swap_mode=false;
                    //play sound
                    m_pSoundEngine->playSimpleSound(wav_turn_start,1.0);
                }
            }
            else//allow player to move
            {
                float player_pos[8];
                m_vec_player[m_curr_player_ind].get_pos(player_pos);
                float player_pos_old[2]={player_pos[0],player_pos[1]};
                //cull rotations
                while(player_pos[2]<0.0)   player_pos[2]+=360.0;
                while(player_pos[2]>360.0) player_pos[2]-=360.0;
                while(player_pos[4]<0.0)   player_pos[4]+=360.0;
                while(player_pos[4]>360.0) player_pos[4]-=360.0;
                while(player_pos[6]<0.0)   player_pos[6]+=360.0;
                while(player_pos[6]>360.0) player_pos[6]-=360.0;
                //calc direction vector
                float dir_x=cosf( (player_pos[2]+90.0) *_deg2rand);
                float dir_y=sinf( (player_pos[2]+90.0) *_deg2rand);

                //Normal keybord input, temp

                /*//update rotations temp
                if(m_pKeys[37]) player_pos[2]+=100.0*m_time_diff_cycle;//arrow left
                if(m_pKeys[38]) player_pos[3]+=100.0*m_time_diff_cycle;//arrow up
                if(m_pKeys[39]) player_pos[2]-=100.0*m_time_diff_cycle;//arrow right
                if(m_pKeys[40]) player_pos[3]-=100.0*m_time_diff_cycle;//arrow down*/

                /*//update location temp
                float move_sens=5.0;
                if(m_pKeys[65])//a
                {
                    player_pos[0]-=move_sens*-dir_y*m_time_diff_cycle;
                    player_pos[1]+=move_sens*dir_x*m_time_diff_cycle;
                }
                if(m_pKeys[87])//w
                {
                    player_pos[0]-=move_sens*dir_x*m_time_diff_cycle;
                    player_pos[1]+=move_sens*dir_y*m_time_diff_cycle;
                }
                if(m_pKeys[68])//d
                {
                    player_pos[0]+=move_sens*-dir_y*m_time_diff_cycle;
                    player_pos[1]-=move_sens*dir_x*m_time_diff_cycle;
                }
                if(m_pKeys[83])//s
                {
                    player_pos[0]+=move_sens*dir_x*m_time_diff_cycle;
                    player_pos[1]-=move_sens*dir_y*m_time_diff_cycle;
                }*/

                /*//test for collision, without wallsliding
                if( !m_pWorld->test_for_collision_nofix( player_pos,player_pos_old ) )
                {
                    //collision, use old pos
                    player_pos[0]=player_pos_old[0];
                    player_pos[1]=player_pos_old[1];
                }*/
                /*//test for collision, with wallsliding (will not be false)
                if( !m_pWorld->test_for_collision( player_pos,player_pos_old ) )
                {
                    //collision, use old pos
                    player_pos[0]=player_pos_old[0];
                    player_pos[1]=player_pos_old[1];
                }*/

                /*//Track input keyboard temp
                if(m_pKeys[100])//np4 left track frw
                {
                    //translation
                    player_pos[0]-=0.30*dir_x*m_time_diff_cycle;
                    player_pos[1]+=0.30*dir_y*m_time_diff_cycle;
                    //rotation
                    player_pos[2]-=50.0*m_time_diff_cycle;
                    player_pos[4]-=50.0*m_time_diff_cycle;
                }
                if(m_pKeys[102])//np6 right track frw
                {
                    //translation
                    player_pos[0]-=0.30*dir_x*m_time_diff_cycle;
                    player_pos[1]+=0.30*dir_y*m_time_diff_cycle;
                    //rotation
                    player_pos[2]+=50.0*m_time_diff_cycle;
                    player_pos[4]+=50.0*m_time_diff_cycle;
                }*/

                //track input gamepad
                st_gamepad_data gamepad_data;
                if( m_gamepad[m_selected_gamepad_ind].IsConnected() )
                {
                    gamepad_data=m_gamepad[m_selected_gamepad_ind].GetState();
                }
                else//lost controller
                {
                    cout<<"Lost connection to controller: "<<m_selected_gamepad_ind+1<<endl;
                    //go to menu and wait for new selected controller
                    m_game_state=gs_menu;
                    //play sound
                    //m_pSoundEngine->playSimpleSound(wav_error,1.0);
                    break;//skip cycle
                }

                //get look inversion flags
                bool look_inversion[2];
                m_vec_player[m_curr_player_ind].get_look_inversion(look_inversion);

                //test for look inversion switch
                if(gamepad_data.button_left_thumbstick && m_look_inversion_toggle_delay<=0.0)
                {
                    m_look_inversion_toggle_delay=_look_inversion_toggle_delay_time;
                    look_inversion[0]=!look_inversion[0];
                    m_vec_player[m_curr_player_ind].set_look_inversion(look_inversion);
                }
                if(gamepad_data.button_right_thumbstick && m_look_inversion_toggle_delay<=0.0)
                {
                    m_look_inversion_toggle_delay=_look_inversion_toggle_delay_time;
                    look_inversion[1]=!look_inversion[1];
                    m_vec_player[m_curr_player_ind].set_look_inversion(look_inversion);
                }

                //motor speed sound
                float motor_pitch_body=0.0;
                float motor_pitch_arm=0.0;
                float motor_pitch_head=0.0;

                //body movement
                if(gamepad_data.button_left_trigger)
                {
                    float speed_sens=(float)gamepad_data.trigger_left/255.0;
                    //cout<<"trigger: "<<gamepad_data.trigger_left<<endl;
                    if(speed_sens>motor_pitch_body) motor_pitch_body=speed_sens*0.5;

                    //translation
                    player_pos[0]-=0.60*speed_sens*dir_x*m_time_diff_cycle;
                    player_pos[1]+=0.60*speed_sens*dir_y*m_time_diff_cycle;
                    //rotation
                    player_pos[2]-=80.0*speed_sens*m_time_diff_cycle;//body
                    player_pos[4]-=80.0*speed_sens*m_time_diff_cycle;//eye
                    player_pos[6]-=80.0*speed_sens*m_time_diff_cycle;//arm
                }
                if(gamepad_data.button_right_trigger)
                {
                    float speed_sens=(float)gamepad_data.trigger_right/255.0;
                    //if(speed_sens>motor_pitch_body) motor_pitch_body=speed_sens;
                    motor_pitch_body+=speed_sens*0.5;

                    //translation
                    player_pos[0]-=0.60*speed_sens*dir_x*m_time_diff_cycle;
                    player_pos[1]+=0.60*speed_sens*dir_y*m_time_diff_cycle;
                    //rotation
                    player_pos[2]+=80.0*speed_sens*m_time_diff_cycle;//body
                    player_pos[4]+=80.0*speed_sens*m_time_diff_cycle;//eye
                    player_pos[6]+=80.0*speed_sens*m_time_diff_cycle;//arm
                }
                if(gamepad_data.button_LB)//backwards
                {
                    float speed_sens=0.7;
                    //if(speed_sens>motor_pitch_body) motor_pitch_body=speed_sens;
                    motor_pitch_body+=speed_sens*0.5;
                    //translation
                    player_pos[0]+=0.60*speed_sens*dir_x*m_time_diff_cycle;
                    player_pos[1]-=0.60*speed_sens*dir_y*m_time_diff_cycle;
                    //rotation
                    player_pos[2]+=80.0*speed_sens*m_time_diff_cycle;//body
                    player_pos[4]+=80.0*speed_sens*m_time_diff_cycle;//eye
                    player_pos[6]+=80.0*speed_sens*m_time_diff_cycle;//arm
                }
                if(gamepad_data.button_RB)//backwards
                {
                    float speed_sens=0.7;
                    //if(speed_sens>motor_pitch_body) motor_pitch_body=speed_sens;
                    motor_pitch_body+=speed_sens*0.5;
                    //translation
                    player_pos[0]+=0.60*speed_sens*dir_x*m_time_diff_cycle;
                    player_pos[1]-=0.60*speed_sens*dir_y*m_time_diff_cycle;
                    //rotation
                    player_pos[2]-=80.0*speed_sens*m_time_diff_cycle;//body
                    player_pos[4]-=80.0*speed_sens*m_time_diff_cycle;//eye
                    player_pos[6]-=80.0*speed_sens*m_time_diff_cycle;//arm
                }
                //eye movement
                if(gamepad_data.thumbstick_right_x!=0)//32768 max
                {
                    float speed_sens=gamepad_data.thumbstick_right_x/32768.0;
                    if(fabs(speed_sens)>motor_pitch_head) motor_pitch_head=fabs(speed_sens);
                    player_pos[4]-=100.0*speed_sens*m_time_diff_cycle;
                }
                if(gamepad_data.thumbstick_right_y!=0)//32768 max
                {
                    float speed_sens=gamepad_data.thumbstick_right_y/32768.0;
                    if(fabs(speed_sens)>motor_pitch_head) motor_pitch_head=fabs(speed_sens);

                    if(look_inversion[1])
                     player_pos[5]-=100.0*speed_sens*m_time_diff_cycle;
                    else
                     player_pos[5]+=100.0*speed_sens*m_time_diff_cycle;

                    //cull to 40 deg
                    if(player_pos[5]<-40.0) player_pos[5]=-40.0;
                    if(player_pos[5]> 40.0) player_pos[5]=40.0;
                }
                //arm movement
                if(gamepad_data.thumbstick_left_x!=0)//32768 max
                {
                    float speed_sens=gamepad_data.thumbstick_left_x/32768.0;
                    if(fabs(speed_sens)>motor_pitch_arm) motor_pitch_arm=fabs(speed_sens);
                    player_pos[6]-=100.0*speed_sens*m_time_diff_cycle;
                }
                if(gamepad_data.thumbstick_left_y!=0)//32768 max
                {
                    float speed_sens=gamepad_data.thumbstick_left_y/32768.0;
                    if(fabs(speed_sens)>motor_pitch_arm) motor_pitch_arm=fabs(speed_sens);

                    if(look_inversion[0])
                     player_pos[7]-=100.0*speed_sens*m_time_diff_cycle;
                    else
                     player_pos[7]+=100.0*speed_sens*m_time_diff_cycle;

                    //cull to 40 deg
                    if(player_pos[7]<-40.0) player_pos[7]=-40.0;
                    if(player_pos[7]> 40.0) player_pos[7]=40.0;
                }

                //update motor sounds
                if(motor_pitch_body>0.1)
                {
                    float sound_data[]={0,0,0, 0,0,0, 0,0,-1, 0,1,0, 0,0,0, 0,0,0, motor_pitch_body,motor_pitch_body,1};
                    m_pSoundEngine->updateSound(m_sound_channel_motor,sound_data);
                }
                else//stop sound
                {
                    m_pSoundEngine->stopSound(m_sound_channel_motor);
                }
                if(motor_pitch_head>0.0)
                {
                    float sound_data[]={0,0,0, 0,0,0, 0,0,-1, 0,1,0, 0,0,0, 0,0,0, motor_pitch_head,motor_pitch_head,0};
                    m_pSoundEngine->updateSound(m_sound_channel_head,sound_data);
                }
                else//stop sound
                {
                    m_pSoundEngine->stopSound(m_sound_channel_head);
                }
                if(motor_pitch_arm>0.0)
                {
                    float sound_data[]={0,0,0, 0,0,0, 0,0,-1, 0,1,0, 0,0,0, 0,0,0, motor_pitch_arm,motor_pitch_arm,0};
                    m_pSoundEngine->updateSound(m_sound_channel_arm,sound_data);
                }
                else//stop sound
                {
                    m_pSoundEngine->stopSound(m_sound_channel_arm);
                }

                //test for wall collision, with wallsliding (will not be false)
                if( !m_pWorld->test_for_collision( player_pos,player_pos_old ) )
                {
                    //collision, use old pos
                    player_pos[0]=player_pos_old[0];
                    player_pos[1]=player_pos_old[1];
                }

                //test for player collision
                float move_vector[2];
                move_vector[0]=player_pos[0]-player_pos_old[0];
                move_vector[1]=player_pos[1]-player_pos_old[1];
                vector<int> vec_player_ind_should_move;
                vec_player_ind_should_move.push_back(m_curr_player_ind);
                bool new_pos_is_ok=true;
                for(int player_i=0;player_i<(int)m_vec_player.size();player_i++)
                {
                    if(player_i==m_curr_player_ind) continue;//dont test yourself
                    if( m_vec_player[player_i].point_inside_player_fast(player_pos[0],player_pos[1]) )
                    {
                        vec_player_ind_should_move.push_back(player_i);
                        //test if that player can be moved half of the move vector
                        move_vector[0]*=0.2;
                        move_vector[1]*=0.2;
                        //assume that the current player can move to this point (already approved of 2x vector)
                        //calc new pos for this player
                        float other_player_pos_old[8];
                        m_vec_player[player_i].get_pos(other_player_pos_old);
                        float other_player_pos[8];
                        other_player_pos[0]=other_player_pos_old[0]+move_vector[0];
                        other_player_pos[1]=other_player_pos_old[1]+move_vector[1];
                        //test if that player is close to a wall edge now
                        if(true)
                        {
                            int wall_node_x=int(other_player_pos[0]+0.5);
                            int wall_node_y=int(other_player_pos[1]+0.5);
                            if( m_pWorld->close_to_wall_edge(wall_node_x,wall_node_y) )
                            {
                                //calc distance to wall node
                                float diff_x=other_player_pos[0]-wall_node_x;
                                float diff_y=other_player_pos[1]-wall_node_y;
                                float dist_to_wall_node=sqrtf( diff_x*diff_x+diff_y*diff_y );
                                if( dist_to_wall_node<_wall_edge_push_limit )//player collides with a wall edge, abort move
                                {
                                    //new point not ok, dont allow the players to move
                                    vec_player_ind_should_move.clear();
                                    new_pos_is_ok=false;
                                    break;
                                }
                            }
                        }
                        //test new pos without wallsliding
                        if( !m_pWorld->test_for_collision_nofix( other_player_pos,other_player_pos_old ) )
                        {
                            //new point not ok, dont allow the players to move
                            vec_player_ind_should_move.clear();
                            new_pos_is_ok=false;
                            break;
                        }
                        else//continue testing
                        {
                            //test the new pos if inside another player
                            for(int player_i2=0;player_i2<(int)m_vec_player.size();player_i2++)
                            {
                                if(player_i2==player_i) continue;//dont test yourself
                                if( m_vec_player[player_i2].point_inside_player_fast(other_player_pos[0],other_player_pos[1]) )
                                {
                                    vec_player_ind_should_move.push_back(player_i2);
                                    //test if that player can be moved half of the move vector
                                    move_vector[0]*=0.5;
                                    move_vector[1]*=0.5;
                                    //assube that the current player can move to this point (already aproved of 2x vector)
                                    //calc new pos for this player
                                    float third_player_pos_old[8];
                                    m_vec_player[player_i2].get_pos(third_player_pos_old);
                                    float third_player_pos[8];
                                    third_player_pos[0]=third_player_pos_old[0]+move_vector[0];
                                    third_player_pos[1]=third_player_pos_old[1]+move_vector[1];
                                    //test if that player is close to a wall edge now
                                    if(true)
                                    {
                                        int wall_node_x=int(third_player_pos[0]+0.5);
                                        int wall_node_y=int(third_player_pos[1]+0.5);
                                        if( m_pWorld->close_to_wall_edge(wall_node_x,wall_node_y) )
                                        {
                                            //calc distance to wall node
                                            float diff_x=third_player_pos[0]-wall_node_x;
                                            float diff_y=third_player_pos[1]-wall_node_y;
                                            float dist_to_wall_node=sqrtf( diff_x*diff_x+diff_y*diff_y );
                                            if( dist_to_wall_node<_wall_edge_push_limit )//player collides with a wall edge, abort move
                                            {
                                                //new point not ok, dont allow the players to move
                                                vec_player_ind_should_move.clear();
                                                new_pos_is_ok=false;
                                                break;
                                            }
                                        }
                                    }
                                    //test new pos without wallsliding
                                    if( !m_pWorld->test_for_collision_nofix( third_player_pos,third_player_pos_old ) )
                                    {
                                        //new point not ok, dont allow the players to move
                                        vec_player_ind_should_move.clear();
                                        new_pos_is_ok=false;
                                        break;
                                    }
                                    else//final player test, then give up
                                    {
                                        //test the new pos if inside another player
                                        for(int player_i3=0;player_i3<(int)m_vec_player.size();player_i3++)
                                        {
                                            if(player_i3==player_i2 || player_i3==player_i) continue;//dont test yourself

                                            //test final player/player collision
                                            if( m_vec_player[player_i3].point_inside_player_fast(third_player_pos[0],third_player_pos[1]) )
                                            {
                                                //new point not ok, dont allow the players to move
                                                vec_player_ind_should_move.clear();
                                                new_pos_is_ok=false;
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                //update player(s) pos
                if(new_pos_is_ok)
                {
                    for(int player_ip=0;player_ip<(int)vec_player_ind_should_move.size();player_ip++)
                    {
                        //update pos
                        if(vec_player_ind_should_move[player_ip]==m_curr_player_ind)
                        {
                            //also update rotations
                            player_pos[0]=player_pos_old[0]+move_vector[0];
                            player_pos[1]=player_pos_old[1]+move_vector[1];
                            m_vec_player[ vec_player_ind_should_move[player_ip] ].set_pos(player_pos);
                        }
                        else
                        {
                            float temp_pos[8];
                            m_vec_player[ vec_player_ind_should_move[player_ip] ].get_pos(temp_pos);
                            temp_pos[0]+=move_vector[0];
                            temp_pos[1]+=move_vector[1];
                            m_vec_player[ vec_player_ind_should_move[player_ip] ].set_pos(temp_pos);

                            //if that player have any cubes. their pos should be updated
                            //if player is carrying a powcube, update that powcube's position and rotation
                            if( m_vec_player[ vec_player_ind_should_move[player_ip] ].get_arm_status()!=-1 )
                            {
                                //get powcube ind
                                int pow_cube_ind=m_vec_player[vec_player_ind_should_move[player_ip]].get_arm_status();
                                //get player's arm vector
                                float arm_dir_x=sinf( (temp_pos[7]+90.0) *_deg2rand) * cosf( (-temp_pos[6]+90.0) *_deg2rand);
                                float arm_dir_y=sinf( (temp_pos[7]+90.0) *_deg2rand) * sinf( (-temp_pos[6]+90.0) *_deg2rand);
                                float arm_dir_z=-cosf( (temp_pos[7]+90.0) *_deg2rand);
                                //get vector for translation movement to the right to fit hand pos
                                float hand_translate_x=cosf( (-temp_pos[6]+180.0) *_deg2rand);
                                float hand_translate_y=sinf( (-temp_pos[6]+180.0) *_deg2rand);
                                float hand_pos_shift=m_player_shoulder_length;
                                //calc x,y,z of powcube
                                float _x=temp_pos[0]+arm_dir_x*_arm_length_short*1.3+hand_translate_x*hand_pos_shift;
                                float _y=temp_pos[1]+arm_dir_y*_arm_length_short*1.3+hand_translate_y*hand_pos_shift;
                                float _z=m_player_arm_height+arm_dir_z*_arm_length_short*1.3;
                                float powcube_pos[5]={_x,_y,_z,temp_pos[6],temp_pos[7]};//temp_pos used from above
                                m_vec_powcube[pow_cube_ind].set_pos(powcube_pos);
                            }

                            //update battery cube pos
                            if( m_vec_player[ vec_player_ind_should_move[player_ip] ].get_battery_status()!=-1 )
                            {
                                //get powcube ind
                                int pow_cube_ind=m_vec_player[ vec_player_ind_should_move[player_ip] ].get_battery_status();
                                //get player's arm vector, -90 deg points to the left
                                float arm_dir_x=cosf( (-temp_pos[6]) *_deg2rand);
                                float arm_dir_y=sinf( (-temp_pos[6]) *_deg2rand);
                                //calc x,y,z of powcube
                                float distance_from_body=m_player_shoulder_length;
                                float _x=temp_pos[0]+arm_dir_x*distance_from_body;
                                float _y=temp_pos[1]+arm_dir_y*distance_from_body;
                                float powcube_pos[5]={_x,_y,m_player_arm_height,temp_pos[6],0};//temp_pos used from above
                                m_vec_powcube[pow_cube_ind].set_pos(powcube_pos);
                            }
                        }
                    }
                }
                else//reset current player pos
                {
                    //cout<<"no\n";
                    player_pos[0]=player_pos_old[0];
                    player_pos[1]=player_pos_old[1];
                }

                //push players away from eachother
                for(int player_i1=0;player_i1<(int)m_vec_player.size();player_i1++)
                {
                    //get first player's pos
                    float temp_pos1[2];
                    m_vec_player[player_i1].get_location(temp_pos1);
                    //col. test
                    for(int player_i2=0;player_i2<(int)m_vec_player.size();player_i2++)
                    {
                        if(player_i1==player_i2) continue;

                        //get other player's pos
                        float temp_pos2[2];
                        m_vec_player[player_i2].get_location(temp_pos2);
                        //size factor 2 due to radius of other player
                        if( m_vec_player[player_i1].point_inside_player_fast(temp_pos2[0],temp_pos2[1]) )
                        {
                            //move that player
                            float move_sens=_player_col_move_speed;
                            //calc move vector
                            float dist_to_player=sqrt( (temp_pos2[0]-temp_pos1[0])*(temp_pos2[0]-temp_pos1[0])+
                                                       (temp_pos2[1]-temp_pos1[1])*(temp_pos2[1]-temp_pos1[1]) );
                            if(dist_to_player!=0) move_sens=(1.0/dist_to_player)*_player_col_move_speed;
                            float move_dir_x=(temp_pos2[0]-temp_pos1[0])*move_sens;
                            float move_dir_y=(temp_pos2[1]-temp_pos1[1])*move_sens;
                            //update player's pos
                            m_vec_player[player_i2].set_location(temp_pos2[0]+move_dir_x,temp_pos2[1]+move_dir_y);
                        }
                    }
                }

                //push players away from wall edges, to avoid wall entering
                for(int player_i=0;player_i<(int)m_vec_player.size();player_i++)
                {
                    //get player pos
                    float temp_pos[8];
                    //if current player, use player_pos from above instead
                    if(player_i==m_curr_player_ind)
                    {
                        for(int val=0;val<8;val++)
                         temp_pos[val]=player_pos[val];
                    }
                    else m_vec_player[player_i].get_pos(temp_pos);
                    //find closest wall node
                    int wall_node_x=int(temp_pos[0]+0.5);
                    int wall_node_y=int(temp_pos[1]+0.5);
                    //is there a wall there
                    if( m_pWorld->close_to_wall_edge(wall_node_x,wall_node_y) )
                    {
                        //if player is too close to a wall edge, push that player away from that wall
                        //calc distance to wall node
                        float diff_x=temp_pos[0]-wall_node_x;
                        float diff_y=temp_pos[1]-wall_node_y;
                        float dist_to_wall_node=sqrtf( diff_x*diff_x+diff_y*diff_y );
                        if( dist_to_wall_node<_wall_edge_push_limit )//player should move
                        {
                            //calc move vector away from wall node
                            float move_scale_factor=1.0;
                            if(dist_to_wall_node!=0) move_scale_factor=_wall_edge_push_sens/dist_to_wall_node;
                            float move_dir_x=diff_x*move_scale_factor;
                            float move_dir_y=diff_y*move_scale_factor;

                            //update player pos
                            if(player_i==m_curr_player_ind)
                            {
                                player_pos[0]+=move_dir_x;
                                player_pos[1]+=move_dir_y;
                                m_vec_player[player_i].set_pos(player_pos);
                            }
                            else
                            {
                                temp_pos[0]+=move_dir_x;
                                temp_pos[1]+=move_dir_y;
                                m_vec_player[player_i].set_pos(temp_pos);
                            }
                        }
                    }
                }

                bool grab_button=false;
                if(gamepad_data.button_X||gamepad_data.button_Y||
                   gamepad_data.button_A||gamepad_data.button_B) grab_button=true;
                //arm grab update
                if(grab_button && m_vec_player[m_curr_player_ind].get_arm_status()==-1 &&
                   m_vec_player[m_curr_player_ind].get_battery_status()!=-1 && m_delay_timer_pickup<=0 )
                {
                    m_delay_timer_pickup=_delay_arm_pickup;//reset at trying?

                    //start grab animation
                    m_vec_player[m_curr_player_ind].anim_grab(pas_extend,1.0);

                    bool grabbed_cube=false;

                    //cout<<"Grab try:\n";
                    //try to grab

                    //get player arm vector from player_pos (used above and updated)
                    float arm_dir_x=sinf( (player_pos[7]+90.0) *_deg2rand) * cosf( (-player_pos[6]+90.0) *_deg2rand);
                    float arm_dir_y=sinf( (player_pos[7]+90.0) *_deg2rand) * sinf( (-player_pos[6]+90.0) *_deg2rand);
                    float arm_dir_z=-cosf( (player_pos[7]+90.0) *_deg2rand);
                    float hand_translate_x=cosf( (-player_pos[6]+180.0) *_deg2rand);
                    float hand_translate_y=sinf( (-player_pos[6]+180.0) *_deg2rand);
                    float hand_pos_shift=m_player_shoulder_length;//shoulder length
                    float test_point_x=player_pos[0]+hand_translate_x*hand_pos_shift;
                    float test_point_y=player_pos[1]+hand_translate_y*hand_pos_shift;
                    float test_point_z=m_player_arm_height;//arm height
                    //cout<<" Reached point: "<<test_point_x+arm_dir_x<<", "<<test_point_y+arm_dir_y<<", "<<test_point_z+arm_dir_z<<endl;

                    float numof_tests=20.0;
                    float curr_test=1.0;
                    while(curr_test<numof_tests)//loop to walk along arm vector, one length unit will be tested
                    {
                        //get new coordinate
                        test_point_x+=(arm_dir_x/numof_tests*_arm_length_reach);
                        test_point_y+=(arm_dir_y/numof_tests*_arm_length_reach);
                        test_point_z+=(arm_dir_z/numof_tests*_arm_length_reach);

                        //cout<<"Test point: "<<test_point_x<<", "<<test_point_y<<", "<<test_point_z<<endl;

                        //bullet hit test to find powcube
                        int max_tries=4;
                        for(int try_counter=1;try_counter<max_tries;try_counter++)
                        //vary the size factor for cubes to find more exact pos first, then more bigger cube size
                         for(int powcube_ind=0;powcube_ind<(int)m_vec_powcube.size();powcube_ind++)
                        {
                            float cube_size_factor=(float)try_counter;
                            if( m_vec_powcube[powcube_ind].point_inside_cube_fast(test_point_x,test_point_y,test_point_z,cube_size_factor) )
                            {//hit
                                cout<<" Powcube in range\n";
                                //test if wall in the way
                                float startpos[2]={player_pos[0],player_pos[1]};
                                float endpos[2]={test_point_x,test_point_y};
                                if( m_pWorld->wall_crossed(startpos,endpos) )
                                {
                                    //cout<<" Wall in the way\n";
                                    //crossed wall, end test
                                    curr_test=numof_tests+1;
                                    try_counter=max_tries;
                                    break;
                                }
                                else//no wall in the way
                                {
                                    grabbed_cube=true;

                                    m_delay_timer_pickup=_delay_arm_pickup;

                                    //cout<<" Grapped powcube\n";
                                    //if cube was in arm of another player, report this to that player
                                    switch( m_vec_powcube[powcube_ind].get_status() )
                                    {
                                        case pcs_in_station:
                                        {
                                            //tell station
                                            int station_id=m_vec_powcube[powcube_ind].get_station_id();
                                            if( !m_vec_station[station_id].remove_cube_from_station(powcube_ind) )
                                            {
                                                cout<<"ERROR: Cube Grab: Could not find that cube id in that station\n";
                                            }
                                        }break;

                                        case pcs_carried_in_arm:
                                        {
                                            //tell that player
                                            int other_player_ind=m_vec_powcube[powcube_ind].get_carrier_id();
                                            m_vec_player[other_player_ind].set_arm_status(-1);
                                        }break;

                                        case pcs_in_battery_slot:
                                        {
                                            //tell that player
                                            int other_player_ind=m_vec_powcube[powcube_ind].get_carrier_id();
                                            m_vec_player[other_player_ind].set_battery_status(-1);
                                        }break;
                                    }

                                    //report to cube
                                    m_vec_powcube[powcube_ind].set_status(pcs_carried_in_arm,m_curr_player_ind);
                                    //report pos for cube transfer animation
                                    /*float _x=player_pos[0]+arm_dir_x*_arm_length_short;
                                    float _y=player_pos[1]+arm_dir_y*_arm_length_short;
                                    float _z=m_player_height+arm_dir_z*_arm_length_short;*/
                                    float _x=player_pos[0]+arm_dir_x*_arm_length_short*1.3+hand_translate_x*hand_pos_shift;
                                    float _y=player_pos[1]+arm_dir_y*_arm_length_short*1.3+hand_translate_y*hand_pos_shift;
                                    float _z=m_player_arm_height+arm_dir_z*_arm_length_short*1.3;
                                    float arm_end_pos[5]={_x,_y,_z,player_pos[6],player_pos[7]};//player_pos used from above
                                    m_vec_powcube[powcube_ind].set_animation_from_curr_pos(arm_end_pos);
                                    //cout<<" Cube end pos: "<<_x<<", "<<_y<<", "<<_z<<endl,

                                    //report to player
                                    m_vec_player[m_curr_player_ind].set_arm_status(powcube_ind);
                                    //cout<<" Done with grab test\n";

                                    m_vec_player[m_curr_player_ind].anim_grab(pas_extend,float(curr_test)/float(numof_tests));

                                    //play sound
                                    float sound_data[]={0,0,0, 0,0,0, 0,0,-1, 0,1,0, 0,0,0, 0,0,0, (2.0-float(curr_test)/float(numof_tests)),1,0};
                                    m_pSoundEngine->playWAVE(wav_arm_pickup,sound_data);

                                    curr_test=numof_tests;
                                    try_counter=max_tries;
                                    break;
                                }
                            }
                        }
                        curr_test+=1.0;
                    }
                    if(!grabbed_cube)//did not grab anything
                    {
                        //play sound
                        m_pSoundEngine->playSimpleSound(wav_arm_pickup_nothing,1.0);
                    }
                }

                //arm drop update
                if(grab_button && m_vec_player[m_curr_player_ind].get_arm_status()!=-1 &&
                   m_vec_player[m_curr_player_ind].get_battery_status()!=-1 && m_delay_timer_pickup<=0 )
                {
                    m_delay_timer_pickup=_delay_arm_pickup;//reset at trying?

                    //test if on a station pos
                    for(int station_i=0;station_i<(int)m_vec_station.size();station_i++)
                    {
                        if( m_vec_station[station_i].in_station_pos( (int)player_pos[0],(int)player_pos[1] ) )
                        {
                            cout<<"player in station\n";
                            //hit-test if can hit that station
                            float arm_dir_x=sinf( (player_pos[7]+90.0) *_deg2rand) * cosf( (-player_pos[6]+90.0) *_deg2rand);
                            float arm_dir_y=sinf( (player_pos[7]+90.0) *_deg2rand) * sinf( (-player_pos[6]+90.0) *_deg2rand);
                            float arm_dir_z=-cosf( (player_pos[7]+90.0) *_deg2rand);
                            float hand_translate_x=cosf( (-player_pos[6]+180.0) *_deg2rand);
                            float hand_translate_y=sinf( (-player_pos[6]+180.0) *_deg2rand);
                            float hand_pos_shift=m_player_shoulder_length;
                            float test_point_x=player_pos[0]+hand_translate_x*hand_pos_shift;
                            float test_point_y=player_pos[1]+hand_translate_y*hand_pos_shift;
                            float test_point_z=m_player_height-0.2;
                            //cout<<" Reached point: "<<test_point_x+arm_dir_x<<", "<<test_point_y+arm_dir_y<<", "<<test_point_z+arm_dir_z<<endl;

                            float numof_tests=50;
                            float curr_test=1;
                            while(curr_test<numof_tests)//loop to walk along arm vector, one length unit will be tested
                            {
                                //get new coordinate
                                test_point_x+=arm_dir_x/numof_tests*_arm_length_reach;
                                test_point_y+=arm_dir_y/numof_tests*_arm_length_reach;
                                test_point_z+=arm_dir_z/numof_tests*_arm_length_reach;

                                //bullet hit test to find station
                                if( m_vec_station[station_i].point_inside_station(test_point_x,test_point_y,test_point_z) )
                                {//hit
                                    m_delay_timer_pickup=_delay_arm_pickup;

                                    cout<<"point inside station\n";
                                    float test_point_v[]={test_point_x,test_point_y,test_point_z};
                                    //report to cube
                                    int cube_ind=m_vec_player[m_curr_player_ind].get_arm_status();
                                    //report to station and get cube final pos, if slot avaliable
                                    float cube_pos_final[5];
                                    if( !m_vec_station[station_i].add_cube_to_station(cube_ind,cube_pos_final,test_point_v) )
                                    {
                                        //no slots avaliable, abort, stop testing
                                        cout<<"no free slots in station\n";
                                        curr_test=numof_tests;
                                        break;
                                    }
                                    //have cube final pos in station now "cube_pos_final"
                                    m_vec_powcube[cube_ind].set_status(pcs_in_station,-1);
                                    m_vec_powcube[cube_ind].set_station_id(station_i);

                                    //report pos for cube transfer animation, from arm to station
                                    float cube_pos_curr[5];
                                    m_vec_powcube[cube_ind].get_pos(cube_pos_curr);
                                    while (cube_pos_curr[3]>360) cube_pos_curr[3]-=360;
                                    while (cube_pos_curr[3]<0) cube_pos_curr[3]+=360;
                                    //find closest node, 0,90,180,270,360
                                    float target_angle=0;
                                    for(float target=0;target<361;target+=90)
                                    {
                                        if(cube_pos_curr[3]<target+45)
                                        {
                                            target_angle=target;
                                            break;
                                        }
                                    }
                                    cube_pos_final[3]=target_angle;
                                    //float cube_end_pos[5]={test_point_x,test_point_y,test_point_z,target_angle,0.0};
                                    m_vec_powcube[cube_ind].set_animation_from_curr_pos(cube_pos_final);

                                    //report to player
                                    m_vec_player[m_curr_player_ind].set_arm_status(-1);

                                    //start grab animation
                                    m_vec_player[m_curr_player_ind].anim_grab(pas_extend,float(curr_test)/float(numof_tests));

                                    //play sound
                                    float sound_data[]={0,0,0, 0,0,0, 0,0,-1, 0,1,0, 0,0,0, 0,0,0, (2.0-float(curr_test)/float(numof_tests)),1,0};
                                    m_pSoundEngine->playWAVE(wav_arm_pickup,sound_data);

                                    curr_test=numof_tests;//done
                                    station_i=(int)m_vec_station.size();
                                    break;
                                }
                                curr_test+=1;
                            }
                        }
                    }
                    //cout<<"test done\n";
                }

                //if player is carrying a powcube, update that powcube's position and rotation
                if( m_vec_player[m_curr_player_ind].get_arm_status()!=-1 )
                {
                    //get powcube ind
                    int pow_cube_ind=m_vec_player[m_curr_player_ind].get_arm_status();
                    //get player's arm vector
                    float arm_dir_x=sinf( (player_pos[7]+90.0) *_deg2rand) * cosf( (-player_pos[6]+90.0) *_deg2rand);
                    float arm_dir_y=sinf( (player_pos[7]+90.0) *_deg2rand) * sinf( (-player_pos[6]+90.0) *_deg2rand);
                    float arm_dir_z=-cosf( (player_pos[7]+90.0) *_deg2rand);
                    //get vector for translation movement to the right to fit hand pos

                    /*
                    float hand_dir_x=sinf( (player_pos[7]+90.0) *_deg2rand) * cosf( (-player_pos[6]+180.0) *_deg2rand);
                    float hand_dir_y=sinf( (player_pos[7]+90.0) *_deg2rand) * sinf( (-player_pos[6]+180.0) *_deg2rand);
                    float hand_dir_z=-cosf( (player_pos[7]+90.0) *_deg2rand);
                    */

                    float hand_translate_x=cosf( (-player_pos[6]+180.0) *_deg2rand);
                    float hand_translate_y=sinf( (-player_pos[6]+180.0) *_deg2rand);
                    float hand_pos_shift=m_player_shoulder_length;
                    //get vector down
                    //calc x,y,z of powcube
                    //float _x=player_pos[0]+hand_translate_x*hand_pos_shift;
                    //float _y=player_pos[1]+hand_translate_y*hand_pos_shift;
                    float _x=player_pos[0]+arm_dir_x*_arm_length_short*1.3+hand_translate_x*hand_pos_shift;
                    float _y=player_pos[1]+arm_dir_y*_arm_length_short*1.3+hand_translate_y*hand_pos_shift;
                    float _z=m_player_arm_height+arm_dir_z*_arm_length_short*1.3;

                    /*
                    float _x=player_pos[0]+arm_dir_x*_arm_length_short;//+hand_dir_x*hand_pos_shift;
                    float _y=player_pos[1]+arm_dir_y*_arm_length_short;//+hand_dir_y*hand_pos_shift;
                    float _z=m_player_height-0.2+arm_dir_z*_arm_length_short;//+hand_dir_z*hand_pos_shift;
                    */

                    float powcube_pos[5]={_x,_y,_z,player_pos[6],player_pos[7]};//player_pos used from above
                    m_vec_powcube[pow_cube_ind].set_pos(powcube_pos);
                }

                //test if player without battery and in his station for delivery
                if( m_vec_player[m_curr_player_ind].get_battery_status()==-1 )
                {
                    //test if in station
                    if( m_vec_station[m_curr_player_ind].in_station_pos((int)player_pos[0],(int)player_pos[1]) )
                    {
                        //give battery cube to player
                        m_vec_powcube.push_back( powcube() );
                        int cube_index=(int)m_vec_powcube.size()-1;
                        m_vec_powcube[cube_index].init(m_curr_player_ind);
                        m_vec_powcube[cube_index].set_status(pcs_in_battery_slot,m_curr_player_ind);
                        m_vec_player[m_curr_player_ind].set_battery_status(cube_index);

                        //place cube above player and start transfer animation
                        float powcube_pos[5]={player_pos[0],player_pos[1],_world_height,0,0};
                        m_vec_powcube[cube_index].set_pos( powcube_pos );
                        float powcube_pos_end[5]={player_pos[0],player_pos[1],m_player_arm_height,player_pos[2],0};
                        m_vec_powcube[cube_index].set_animation_from_curr_pos(powcube_pos_end);

                        //play sound
                        m_pSoundEngine->playSimpleSound(wav_cube_from_station,1.0);
                    }
                }

                //update battery cube pos
                if( m_vec_player[m_curr_player_ind].get_battery_status()!=-1 )
                {
                    //get powcube ind
                    int pow_cube_ind=m_vec_player[m_curr_player_ind].get_battery_status();
                    //get player's arm vector, -90 deg points to the left
                    float arm_dir_x=cosf( (-player_pos[6]) *_deg2rand);
                    float arm_dir_y=sinf( (-player_pos[6]) *_deg2rand);
                    /*float arm_dir_x=sinf( (player_pos[7]+90.0) *_deg2rand) * cosf( (-player_pos[6]+90.0) *_deg2rand);
                    float arm_dir_y=sinf( (player_pos[7]+90.0) *_deg2rand) * sinf( (-player_pos[6]+90.0) *_deg2rand);
                    float arm_dir_z=-cosf( (player_pos[7]+90.0) *_deg2rand);*/
                    //float body_dir_x=cosf( (player_pos[2]+90.0) *_deg2rand);
                    //float body_dir_y=sinf( (player_pos[2]+90.0) *_deg2rand);
                    //calc x,y,z of powcube
                    float distance_from_body=m_player_shoulder_length;
                    float _x=player_pos[0]+arm_dir_x*distance_from_body;
                    float _y=player_pos[1]+arm_dir_y*distance_from_body;
                    //float _z=m_player_height+arm_dir_z*distance_from_body;
                    //float powcube_pos[5]={player_pos[0],player_pos[1],0.5,player_pos[2],0};//player_pos used from above
                    float powcube_pos[5]={_x,_y,m_player_arm_height,player_pos[6],0};//player_pos used from above
                    m_vec_powcube[pow_cube_ind].set_pos(powcube_pos);
                }

                //update player height, temp
                //if(m_pKeys[82]) m_player_height+=1.0*m_time_diff_cycle;  //r
                //if(m_pKeys[70]) m_player_height-=1.0*m_time_diff_cycle;  //f

                //test if player wants to exit
                if(gamepad_data.button_back)
                {
                    m_ingame_exit_timer-=m_time_diff_cycle;
                    if(m_ingame_exit_timer<=0)
                    {
                        //go to menu
                        m_game_state=gs_menu;
                        //silence motor sounds
                        m_pSoundEngine->set_volume(m_sound_channel_motor,0.0);
                        m_pSoundEngine->set_volume(m_sound_channel_arm,0.0);
                        m_pSoundEngine->set_volume(m_sound_channel_head,0.0);
                        //turn up music vol to menu level
                        m_pSoundEngine->set_volume(m_sound_channel_music,_music_vol_menu);

                        return false;
                    }
                }
                else m_ingame_exit_timer=_ingame_exit_time;
            }

            m_pWorld->update();

            //game over test, dont test if only one player
            if(!m_game_over && (int)m_vec_player.size()>1 && !m_player_swap_mode)
            {
                int winner_ind=game_over_test();
                if(winner_ind!=-1)
                {
                    if(winner_ind==m_curr_player_ind)//you can only win if it is your turn
                    {
                        cout<<"GAME OVER: Winner is player "<<winner_ind+1<<endl;
                        m_game_over=true;
                        //vibrate control
                        m_gamepad[m_selected_gamepad_ind].Vibrate(30000,30000);

                        //start to fade music
                        m_music_fade_out=true;

                        //play sound
                        m_pSoundEngine->playSimpleSound(wav_win,10.0);
                    }
                }
            }
            if(m_game_over) m_game_over_timer-=m_time_diff_cycle;
            //test if game should end
            if(m_game_over_timer<=0.0)
            {
                //stop vibration
                m_gamepad[m_selected_gamepad_ind].Vibrate(0,0);
                //go to menu
                m_game_state=gs_menu;
                //stop engine sounds
                m_pSoundEngine->stopSound(m_sound_channel_motor);
                m_pSoundEngine->stopSound(m_sound_channel_arm);
                m_pSoundEngine->stopSound(m_sound_channel_head);
                //fade back music
                m_music_fade_in=true;
                //play sound
                //m_pSoundEngine->playSimpleSound(wav_beep1,1.0);

                return false;
            }

        }break;
    }


    return true;
}

bool game::draw(void)
{

    switch(m_game_state)
    {
        case gs_init:
        {
            //should not happen
        }break;

        case gs_menu:
        {
            m_pMenu->draw();

            /*static float rota=0.0;
            rota+=1.0;
            glTranslatef(0,0,-10);
            glRotatef(rota,0,1,0);
            m_pModel_test->draw();*/

        }break;

        case gs_ingame:
        {
            //glTranslatef(0,0,-100);
            //glRotatef(90,0.0,0.0,1.0);

            float cam_pos[5];//x,y,z,yaw,pitch
            //float player_head_offset=_player_head_offset_max;

            if(m_player_swap_mode)
            {
                //calc pos of camera
                float cam_progress=m_player_swap_timer/m_player_swap_time;
                cam_pos[0]=m_player_swap_cam_pos_start[0]+(m_player_swap_cam_pos_end[0]-m_player_swap_cam_pos_start[0])*cam_progress;
                cam_pos[1]=m_player_swap_cam_pos_start[1]+(m_player_swap_cam_pos_end[1]-m_player_swap_cam_pos_start[1])*cam_progress;
                cam_pos[2]=m_player_swap_cam_pos_start[2]+(m_player_swap_cam_pos_end[2]-m_player_swap_cam_pos_start[2])*cam_progress
                          +sinf( cam_progress*_pi )*_player_swap_height;//height arc
                cam_pos[3]=m_player_swap_cam_pos_start[3]+(m_player_swap_cam_pos_end[3]-m_player_swap_cam_pos_start[3])*cam_progress;
                //cam_pos[4]=m_player_swap_cam_pos_start[4]+(m_player_swap_cam_pos_end[4]-m_player_swap_cam_pos_start[4])*cam_progress;
                //cout<<cam_pos[4]<<endl;
                if(cam_progress<0.5)//transformation from start to -90
                 cam_pos[4]=m_player_swap_cam_pos_start[4]+(-90.0-m_player_swap_cam_pos_start[4])*cam_progress*2.0;
                else//transformation from -90 to end
                 cam_pos[4]=-90.0+( m_player_swap_cam_pos_end[4]+90.0 )*(cam_progress-0.5)*2.0;
            }
            else//use players pos
            {
                //get player pos
                float player_pos[8];//position of the camera, x,y,body yaw,body pitch,eye yaw,eye pitch,arm yaw,arm pitch
                m_vec_player[m_curr_player_ind].get_pos(player_pos);

                cam_pos[0]=player_pos[0];
                cam_pos[1]=player_pos[1];
                cam_pos[2]=m_player_height;
                cam_pos[3]=player_pos[4];
                cam_pos[4]=player_pos[5];

                /*//get vector to test how far back the neck could be placed
                float dir_x=cosf( (player_pos[2]+90.0) *_deg2rand);
                float dir_y=sinf( (player_pos[2]+90.0) *_deg2rand);
                for(int i=1;i<=10;i++)
                {
                    float pos_center[2]={player_pos[0],player_pos[1]};
                    float pos_backed[2]={player_pos[0]-dir_x*(float)i/10.0*_player_head_offset_max,
                                         player_pos[1]-dir_y*(float)i/10.0*_player_head_offset_max};
                    if(m_pWorld->test_for_collision_nofix( pos_backed,pos_center ) )
                    {
                        player_head_offset=_player_head_offset_max*(float)i/10.0;
                    }
                }*/
            }

            //temp
            //cout<<player_pos[0]<<", "<<player_pos[1]<<endl;

            /*glRotatef(player_pos[3],-1,0,0);//pitch rot
            glRotatef(player_pos[2],0,-1,0);//yaw rot
            //glTranslatef(-player_pos[0],-m_player_height,-player_pos[1]);//xpos,height,ypos
            glTranslatef(player_pos[0],-m_player_height,player_pos[1]);//xpos,height,ypos
            glRotatef(90,-1,0,0);
            //glRotatef(180,0,0,1);*/

            glPushMatrix();

            //glTranslatef(0,0,-player_head_offset);
            glRotatef(cam_pos[4],-1,0,0);//pitch rot
            glRotatef(cam_pos[3],0,-1,0);//yaw rot
            //glTranslatef(-player_pos[0],-m_player_height,-player_pos[1]);//xpos,height,ypos
            glTranslatef(cam_pos[0],-cam_pos[2],cam_pos[1]);//xpos,height,ypos
            glRotatef(90,-1,0,0);
            //glRotatef(180,0,0,1);

            //draw world
            m_pWorld->draw();

            //draw powcubes
            for(int powcube_ind=0;powcube_ind<(int)m_vec_powcube.size();powcube_ind++)
            {
                m_vec_powcube[powcube_ind].draw();
            }

            //draw stations
            for(int station_ind=0;station_ind<(int)m_vec_station.size();station_ind++)
            {
                m_vec_station[station_ind].draw();
            }

            //draw players
            //get draw order, back first
            vector<int> vec_draw_order;//give
            //calc distance to other players
            vector<float> vec_dist;
            for(int i=0;i<(int)m_vec_player.size();i++)
            {
                vec_draw_order.push_back(i);//temp order, wil be sorted
                if(i==m_curr_player_ind)//current player
                {
                    vec_dist.push_back(0);
                    continue;
                }
                //get player pos
                float pos[2];
                m_vec_player[i].get_location(pos);
                float dist=sqrtf( (pos[0]-cam_pos[0])*(pos[0]-cam_pos[0]) + (pos[1]-cam_pos[1])*(pos[1]-cam_pos[1]) );
                vec_dist.push_back(dist);
            }
            //sort, highest dist to the front (first drawn)
            while(true)
            {
                bool change=false;
                for(int i=0;i<(int)vec_draw_order.size()-1;i++)
                {
                    if( vec_dist[ vec_draw_order[i] ] < vec_dist[ vec_draw_order[i+1] ] )
                    {
                        int temp_int=vec_draw_order[i];
                        vec_draw_order[i]=vec_draw_order[i+1];
                        vec_draw_order[i+1]=temp_int;
                        change=true;
                    }
                }

                if(!change) break;//done
            }

            /*//temp print
            cout<<"ORDER\n";
            for (int i=0;i<(int)vec_draw_order.size();i++)
            {
                cout<< vec_dist[ vec_draw_order[i] ] <<endl;
            }
            cout<<endl;*/

            //draw in order
            for(int i=0;i<(int)m_vec_player.size();i++)
            {
                int player_ind=vec_draw_order[i];
                //if active player, draw fps view, draw arm aim or not
                //else draw outside view ?
                bool draw_active_player=false;
                if(player_ind==m_curr_player_ind && !m_player_swap_mode) draw_active_player=true;
                //draw curr player if in swapmode, but not in first frames
                //if() draw_active_player=false;
                m_vec_player[player_ind].draw(draw_active_player);
                //m_vec_player[player_ind].draw(draw_active_player,true);
            }

            glPopMatrix();

            //draw HUD
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_FOG);
            glEnable(GL_BLEND);

            //Player transition color bar
            if(m_player_swap_mode)
            {
                //intro shade
                if(m_intro_shade_timer>0)
                {
                    float shade_color=m_intro_shade_timer/_intro_shade_time;
                    glColor4f(0,0,0,shade_color);
                    glBegin(GL_QUADS);
                    glVertex3f(-10,-10,-1);
                    glVertex3f(10,-10,-1);
                    glVertex3f(10,10,-1);
                    glVertex3f(-10,10,-1);
                    glEnd();
                }
                else//draw player swap progress
                {
                    //get prev player ind, current is the next player
                    int prev_player_ind=m_curr_player_ind-1;
                    if(prev_player_ind<0) prev_player_ind=(int)m_vec_player.size()-1;

                    //calc swap progress
                    float progress_bar_pos=(m_player_swap_timer/_player_swap_time)*4.0;

                    glBegin(GL_QUADS);
                    glColor4f(_player_color[prev_player_ind][0],_player_color[prev_player_ind][1],_player_color[prev_player_ind][2],0.5);
                    glVertex3f(-2,-1.0,-5);
                    glVertex3f(-2,-2,-5);
                    glColor4f(_player_color[m_curr_player_ind][0],_player_color[m_curr_player_ind][1],_player_color[m_curr_player_ind][2],0.5);
                    glVertex3f(2,-2,-5);
                    glVertex3f(2,-1.0,-5);
                    glEnd();

                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    glColor4f(0.9,0.9,0.9,1.0);
                    glBegin(GL_QUADS);
                    glVertex3f(-2,-1.0,-5);
                    glVertex3f(-2,-2,-5);
                    glVertex3f(2,-2,-5);
                    glVertex3f(2,-1.0,-5);
                    glEnd();

                    //draw progress bar
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    glColor4f(0.1,0.1,0.1,1.0);
                    glBegin(GL_QUADS);
                    glVertex3f(-2+progress_bar_pos+0.1,-0.9,-5);
                    glVertex3f(-2+progress_bar_pos+0.1,-2.1,-5);
                    glVertex3f(-2+progress_bar_pos-0.1,-2.1,-5);
                    glVertex3f(-2+progress_bar_pos-0.1,-0.9,-5);
                    glEnd();

                    //draw edges
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    glColor4f(0.9,0.9,0.9,1.0);
                    glBegin(GL_QUADS);
                    glVertex3f(-2+progress_bar_pos+0.1,-0.9,-5);
                    glVertex3f(-2+progress_bar_pos+0.1,-2.1,-5);
                    glVertex3f(-2+progress_bar_pos-0.1,-2.1,-5);
                    glVertex3f(-2+progress_bar_pos-0.1,-0.9,-5);
                    glEnd();

                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }
            }
            else//normal HUD
            {
                glLineWidth(3);
                //get player rotations
                float pos[8];
                m_vec_player[m_curr_player_ind].get_pos(pos);

                //rotation wheels
                glPushMatrix();

                //high wheel (arm yaw)
                glTranslatef(0,1,-0.5);
                glRotatef(pos[6]-pos[4],0,1,0);
                float radius=2.0;
                int points_in_circle=30;
                float tick_height_max=0.3;
                glBegin(GL_LINES);
                for(int i=0;i<=points_in_circle;i++)
                {
                    float angle=(float)i*360.0/(float)points_in_circle;
                    float tick_height=sinf( (float)i/(float)points_in_circle*_pi)*tick_height_max;

                    //float tick_exp=tick_height*tick_height;

                    glColor3f(0.6,1.0-tick_height*5,1.0-tick_height*5);
                    if(angle==180)
                    {//center
                        tick_height*=1.5;
                        glColor3f(1,0,0);
                    }

                    glVertex3f( sinf(angle*_deg2rand)*(radius-tick_height),0,cosf(angle*_deg2rand)*(radius-tick_height) );
                    glVertex3f( sinf(angle*_deg2rand)*(radius+tick_height),0,cosf(angle*_deg2rand)*(radius+tick_height) );
                }
                glEnd();
                glPopMatrix();

                //side wheel (arm pitch)
                points_in_circle=36;
                radius=4.0;
                glPushMatrix();
                glTranslatef(-2.25*m_screen_aspect_ratio,0,-0.5);
                glRotatef(pos[7]-pos[5],1,0,0);
                glBegin(GL_LINES);
                for(int i=0;i<=points_in_circle*0.5;i++)
                {
                    float angle=(float)i*360.0/(float)points_in_circle+110;
                    float tick_height=sinf( (float)i/(float)points_in_circle*2.0*_pi)*tick_height_max;

                    //float tick_exp=tick_height*tick_height;

                    glColor3f(0.6,1.0-tick_height*5,1.0-tick_height*5);
                    if(angle==200)
                    {//center
                        tick_height*=1.5;
                        glColor3f(1,0,0);
                    }

                    glVertex3f( 0,sinf(angle*_deg2rand)*(radius-tick_height),cosf(angle*_deg2rand)*(radius-tick_height) );
                    glVertex3f( 0,sinf(angle*_deg2rand)*(radius+tick_height),cosf(angle*_deg2rand)*(radius+tick_height) );
                }
                glEnd();
                glPopMatrix();

                //low wheel (body yaw)
                points_in_circle=30;
                radius=2.0;
                glPushMatrix();
                glTranslatef(0,-1,-0.5);
                glRotatef(pos[2]-pos[4],0,1,0);
                /*glColor3f(0.9,0.3,0);
                glBegin(GL_LINE_STRIP);
                for(int i=0;i<=points_in_circle;i++)
                {
                    float angle=(float)i*360.0/(float)points_in_circle;
                    glVertex3f( sinf(angle*_deg2rand)*3,0,cosf(angle*_deg2rand)*3 );
                }
                glEnd();*/
                /*glBegin(GL_LINE_STRIP);
                for(int i=0;i<=points_in_circle;i++)
                {
                    float angle=(float)i*360.0/(float)points_in_circle;
                    glVertex3f( sinf(angle*_deg2rand)*15,0,cosf(angle*_deg2rand)*15 );
                }
                glEnd();*/

                glBegin(GL_LINES);
                for(int i=0;i<=points_in_circle;i++)
                {
                    float angle=(float)i*360.0/(float)points_in_circle;
                    float tick_height=sinf( (float)i/(float)points_in_circle*_pi)*tick_height_max;

                    //float tick_exp=tick_height*tick_height;

                    glColor3f(0.6,1.0-tick_height*5,1.0-tick_height*5);
                    if(angle==180)
                    {//center
                        tick_height*=1.5;
                        glColor3f(1,0,0);
                    }

                    glVertex3f( sinf(angle*_deg2rand)*(radius-tick_height),0,cosf(angle*_deg2rand)*(radius-tick_height) );
                    glVertex3f( sinf(angle*_deg2rand)*(radius+tick_height),0,cosf(angle*_deg2rand)*(radius+tick_height) );

                    /*glVertex3f( sinf(angle*_deg2rand)*radius,tick_height,cosf(angle*_deg2rand)*radius );
                    glVertex3f( sinf(angle*_deg2rand)*radius,-tick_height,cosf(angle*_deg2rand)*radius );*/
                }
                glEnd();
                glPopMatrix();

                //draw play timer (right side wheel)
                glPushMatrix();
                glTranslatef(2.25*m_screen_aspect_ratio,0,-0.5);
                radius=4.0;
                float start_angle=m_play_time_curr_turn/m_play_time_per_turn*50.0+155.0;
                int color_prog=int(m_play_time_curr_turn/m_play_time_per_turn*6.0);//set color
                switch(color_prog)
                {
                    case 0: glColor3f(0,0.7,0); break;
                    case 1: glColor3f(0.1,0.7,0); break;
                    case 2: glColor3f(0.2,0.7,0); break;
                    case 3: glColor3f(0.5,0.7,0); break;
                    case 4: glColor3f(0.7,0.5,0); break;
                    case 5: glColor3f(1,0,0); break;
                }
                glLineWidth(5);
                glBegin(GL_LINE_STRIP);
                for(float angle=start_angle;angle<205.0;angle+=1.0)
                {
                    glVertex3f( 0,sinf(angle*_deg2rand)*radius,cosf(angle*_deg2rand)*radius );
                }
                glVertex3f( 0,sinf(205.0*_deg2rand)*radius,cosf(205.0*_deg2rand)*radius );//final pos, to erase jitter
                glEnd();
                glLineWidth(1);
                glPopMatrix();

            }

            glEnable(GL_DEPTH_TEST);
            glEnable(GL_FOG);
            glDisable(GL_BLEND);

        }break;
    }

    return true;
}

//Private

int game::game_over_test(void)
{
    //test if any player have a full station
    for(int player_i=0;player_i<(int)m_vec_player.size();player_i++)//player ind is connected with it's station
    {
        vector<int> vec_cube_ind;//will contain ind of all cubes in that station
        m_vec_station[player_i].get_list_of_cube_in_station(vec_cube_ind);
        //check that there is one cube from each player
        bool owners[(int)m_vec_player.size()];
        for(int i=0;i<(int)m_vec_player.size();i++) owners[i]=false;
        for(int cube_i=0;cube_i<(int)vec_cube_ind.size();cube_i++)
        {
            int owner_ind=m_vec_powcube[ vec_cube_ind[cube_i] ].get_owner_id();
            //to be safe, limit test
            if(owner_ind<0 || owner_ind>=(int)m_vec_player.size())
            {
                cout<<"ERROR: Received bad owner id from cube during game over test\n";
                return -1;
            }
            owners[owner_ind]=true;
        }
        //check if all owners are represented
        bool have_all=true;
        for(int i=0;i<(int)m_vec_player.size();i++)
        {
            if(!owners[i])
            {
                have_all=false;
                break;
            }
        }

        if(have_all)//have a winner
        {
            return player_i;
        }
    }

    return -1;//no winner yet
}

bool game::load_textures(void)
{
    //load files and decode

    string s_decode=base64_decode( load_base64_file(file_texture_title) );
    unsigned char* texture_data=(unsigned char*)s_decode.c_str();
    //load texture from memory
    m_tex_title = SOIL_load_OGL_texture_from_memory
		(
		texture_data,//buffer
		(int)s_decode.length(),//Buffer length
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y
		);

    s_decode=base64_decode( load_base64_file(file_texture_help) );
    texture_data=(unsigned char*)s_decode.c_str();
    //load texture from memory
    m_tex_help = SOIL_load_OGL_texture_from_memory
		(
		texture_data,//buffer
		(int)s_decode.length(),//Buffer length
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y
		);

    s_decode=base64_decode( load_base64_file(file_texture_extratext) );
    texture_data=(unsigned char*)s_decode.c_str();
    //load texture from memory
    m_tex_extra_text = SOIL_load_OGL_texture_from_memory
		(
		texture_data,//buffer
		(int)s_decode.length(),//Buffer length
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y
		);

    /*OLD load
    m_tex_title=SOIL_load_OGL_texture
	(
		"title.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT
	);

	m_tex_help=SOIL_load_OGL_texture
	(
		"help.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT
	);

	m_tex_extra_text=SOIL_load_OGL_texture
	(
		"extratext.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT
	);*/

	if(m_tex_title==0||m_tex_help==0||m_tex_extra_text==0)
    {
        cout<<"ERROR: Could not load textures\n";
        return false;
    }

    return true;
}

bool game::load_sounds(void)
{
    //load files from text and decode
    m_pSoundEngine->load_WAVE_from_string(wav_arm_pickup, base64_decode( load_base64_file(file_sound_arm_pickup) ) );
    m_pSoundEngine->load_WAVE_from_string(wav_arm_pickup_nothing, base64_decode( load_base64_file(file_sound_arm_pickup_nothing) ) );
    m_pSoundEngine->load_WAVE_from_string(wav_cube_from_station, base64_decode( load_base64_file(file_sound_cube_from_station) ) );
    m_pSoundEngine->load_WAVE_from_string(wav_end_turn_warning, base64_decode( load_base64_file(file_sound_end_turn_warning) ) );
    m_pSoundEngine->load_WAVE_from_string(wav_engine_arm, base64_decode( load_base64_file(file_sound_engine_arm) ) );
    m_pSoundEngine->load_WAVE_from_string(wav_engine_body, base64_decode( load_base64_file(file_sound_engine_body) ) );
    m_pSoundEngine->load_WAVE_from_string(wav_engine_head, base64_decode( load_base64_file(file_sound_engine_head) ) );
    m_pSoundEngine->load_WAVE_from_string(wav_match_start, base64_decode( load_base64_file(file_sound_match_start) ) );
    m_pSoundEngine->load_WAVE_from_string(wav_menu_click, base64_decode( load_base64_file(file_sound_menu_click) ) );
    m_pSoundEngine->load_WAVE_from_string(wav_turn_end, base64_decode( load_base64_file(file_sound_turn_end) ) );
    m_pSoundEngine->load_WAVE_from_string(wav_turn_start, base64_decode( load_base64_file(file_sound_turn_start) ) );
    m_pSoundEngine->load_WAVE_from_string(wav_music_intro, base64_decode( load_base64_file(file_sound_music_intro) ) );
    m_pSoundEngine->load_WAVE_from_string(wav_music_loop, base64_decode( load_base64_file(file_sound_music_loop) ) );
    m_pSoundEngine->load_WAVE_from_string(wav_win, base64_decode( load_base64_file(file_sound_win) ) );

    int error_flag=m_pSoundEngine->get_error();
    if( error_flag!=0 )
    {
        cout<<"ERROR: Could not load sound: "<<error_flag<<endl;
        return false;
    }

    /*//OLD
    //if( !m_pSoundEngine->load_WAVE_from_file(wav_beep1,"sound\\beep1.wav") ) return false;
    if( !m_pSoundEngine->load_WAVE_from_file(wav_arm_pickup,"sound\\arm_pickup.wav") ) return false;
    //if( !m_pSoundEngine->load_WAVE_from_file(wav_arm_place,"sound\\arm_place.wav") ) return false;
    if( !m_pSoundEngine->load_WAVE_from_file(wav_end_turn_warning,"sound\\end_turn_warning.wav") ) return false;
    if( !m_pSoundEngine->load_WAVE_from_file(wav_turn_end,"sound\\turn_end.wav") ) return false;
    if( !m_pSoundEngine->load_WAVE_from_file(wav_turn_start,"sound\\turn_start.wav") ) return false;
    if( !m_pSoundEngine->load_WAVE_from_file(wav_win,"sound\\win.wav") ) return false;
    if( !m_pSoundEngine->load_WAVE_from_file(wav_engine_body,"sound\\engine_body.wav") ) return false;
    if( !m_pSoundEngine->load_WAVE_from_file(wav_engine_arm,"sound\\engine_arm.wav") ) return false;
    if( !m_pSoundEngine->load_WAVE_from_file(wav_engine_head,"sound\\engine_head.wav") ) return false;
    if( !m_pSoundEngine->load_WAVE_from_file(wav_cube_from_station,"sound\\cube_from_station.wav") ) return false;
    if( !m_pSoundEngine->load_WAVE_from_file(wav_menu_click,"sound\\menu_click.wav") ) return false;
    //if( !m_pSoundEngine->load_WAVE_from_file(wav_error,"sound\\error.wav") ) return false;
    if( !m_pSoundEngine->load_WAVE_from_file(wav_match_start,"sound\\match_start.wav") ) return false;
    if( !m_pSoundEngine->load_WAVE_from_file(wav_arm_pickup_nothing,"sound\\arm_pickup_nothing.wav") ) return false;
    if( !m_pSoundEngine->load_WAVE_from_file(wav_music_intro,"sound\\music_intro.wav") ) return false;
    if( !m_pSoundEngine->load_WAVE_from_file(wav_music_loop,"sound\\music_loop.wav") ) return false;
    //if( !m_pSoundEngine->load_WAVE_from_file(wav_,"sound\\.wav") ) return false;*/

    return true;
}

bool game::reset_game(int numof_players)
{
    cout<<"Resetting the game\n";
    //reset world
    delete m_pWorld;
    m_pWorld=new world();
    vector<st_pos_int> vec_station_pos;
    m_pWorld->init(numof_players,vec_station_pos);

    m_play_time_curr_turn=0.0;
    m_play_time_per_turn=numof_players*_play_time_per_turn_dynamic_coef+_play_time_per_turn_dynamic_const;
    m_curr_player_ind=0;
    m_player_swap_mode=true;
    m_player_swap_timer=0.0;
    m_game_over=false;
    m_look_inversion_toggle_delay=0.0;
    m_intro_shade_timer=_intro_shade_time;
    m_key_input_delay_timer=0.0;
    m_game_over_timer=_game_over_time;
    m_delay_timer_pickup=0.0;
    m_ingame_exit_timer=_ingame_exit_time;
    m_time_warning_sound_played=false;
    m_music_fade_out=false;

    cout<<"Number of Players: "<<numof_players<<endl;
    cout<<"Play time per turn: "<<m_play_time_per_turn<<" seconds\n";

    //reset players, stations and powercubes
    m_vec_player.clear();
    m_vec_station.clear();
    m_vec_powcube.clear();
    //add players and stations, share the same index
    model_obj* player_model_parts[]={m_pModel_player_body,
                                     m_pModel_player_shoulder,
                                     m_pModel_player_hand,
                                     m_pModel_player_head,
                                     m_pModel_player_throat};
    for(int player_i=0;player_i<numof_players;player_i++)
    {
        //the player
        m_vec_player.push_back( player(player_model_parts,player_i) );
        float rotation_body=0;
        switch(vec_station_pos[player_i].value)//direction
        {
            case 0: rotation_body=0; break;
            case 1: rotation_body=270; break;
            case 2: rotation_body=180; break;
            case 3: rotation_body=90; break;
        }
        float player_pos[8]={vec_station_pos[player_i].x+0.5,vec_station_pos[player_i].y+0.5,
                             rotation_body,0,rotation_body,0,rotation_body,0};
        m_vec_player[player_i].set_pos(player_pos);

        //the station
        m_vec_station.push_back( station( vec_station_pos[player_i].x,
                                          vec_station_pos[player_i].y,
                                          vec_station_pos[player_i].value,
                                          player_i,
                                          m_pModel_station ) );

        //add owners cube to that station
        m_vec_powcube.push_back( powcube() );
        m_vec_powcube.back().init(player_i);
        float final_cube_pos[5];
        float temp_test_point_v[3]={0,0,1};
        m_vec_station.back().add_cube_to_station( (int)m_vec_powcube.size()-1, final_cube_pos, temp_test_point_v );
        m_vec_powcube.back().set_pos(final_cube_pos);
        m_vec_powcube.back().set_status(pcs_in_station,-1);
        m_vec_powcube.back().set_station_id(player_i);
    }

    //set startpos for camera
    float player_pos[8];
    m_vec_player[m_curr_player_ind].get_pos(player_pos);
    m_player_swap_cam_pos_end[0]=player_pos[0];
    m_player_swap_cam_pos_end[1]=player_pos[1];
    m_player_swap_cam_pos_end[2]=m_player_height;
    m_player_swap_cam_pos_end[3]=player_pos[4];
    m_player_swap_cam_pos_end[4]=player_pos[5];
    m_player_swap_cam_pos_start[0]=_world_size*0.5;
    m_player_swap_cam_pos_start[1]=_world_size*0.5;
    m_player_swap_cam_pos_start[2]=5;
    m_player_swap_cam_pos_start[3]=361;
    m_player_swap_cam_pos_start[4]=-90;

    return true;
}

