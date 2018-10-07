#include "menu.h"

menu::menu()
{
    m_player_counter=1;
}

bool menu::init(float fov,float ratio,int textures[3],model_obj* pModel_robot,model_obj* pModel_station)
{
    m_tex_title=textures[0];
    m_tex_help=textures[1];
    m_tex_extra_text=textures[2];

    m_screen_fov=fov;
    m_screen_aspect_ratio_real=m_screen_aspect_ratio=ratio;
    //if(ratio>16.0/9.0) m_screen_aspect_ratio=16.0/9.0;//cull to 16:9

    m_pModel_robot=pModel_robot;
    m_pModel_station=pModel_station;

    m_player_counter=1;
    m_intro_zoom_timer=_intro_menu_zoom_time;

    m_show_help=false;

    return true;
}

bool menu::update(float m_time_diff_cycle)
{
    if(m_intro_zoom_timer>=0.0) m_intro_zoom_timer-=m_time_diff_cycle;

    return true;
}

bool menu::draw(void)
{
    glPushMatrix();

    //intro zoom
    if(m_intro_zoom_timer>0.0)
    {
        glTranslatef(0,0,-200.0*m_intro_zoom_timer/_intro_menu_zoom_time);
    }

    glDepthMask(false);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_FOG);
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_ONE,GL_ONE);


    //show help
    if(m_show_help)
    {
        glBindTexture(GL_TEXTURE_2D, m_tex_help);
        static float timer=0.0; timer+=0.01;
        float distance_to_screen=7.80*( 1.0/m_screen_aspect_ratio )/0.5625 +0.05*sinf(timer*0.5);
        float brightness=0.8+0.2*sinf(timer*0.2);
        glColor3f(brightness,brightness,brightness);
        float pos_x_min=-8;
        float pos_x_max=8;
        float pos_y_min=-4.5;
        float pos_y_max=4.5;
        glBegin(GL_QUADS);
        glTexCoord2f(0.0,0.0);
        glVertex3f(pos_x_min,pos_y_min,-distance_to_screen);
        glTexCoord2f(1.0,0.0);
        glVertex3f(pos_x_max,pos_y_min,-distance_to_screen);
        glTexCoord2f(1.0,1.0);
        glVertex3f(pos_x_max,pos_y_max,-distance_to_screen);
        glTexCoord2f(0.0,1.0);
        glVertex3f(pos_x_min,pos_y_max,-distance_to_screen);
        glEnd();

        glDepthMask(true);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_FOG);
        glDisable(GL_TEXTURE_2D);

        return true;//nothing else hould be drawn
    }

    //draw title
    glBindTexture(GL_TEXTURE_2D, m_tex_title);
    static float timer=0.0; timer+=0.01;
    float distance_to_screen=7.80*( 1.0/m_screen_aspect_ratio )/0.5625 +0.05*sinf(timer*0.5);
    float brightness=0.8+0.2*sinf(timer*0.2);
    glColor3f(brightness,brightness,brightness);
    float pos_x_min=-8;
    float pos_x_max=8;
    float pos_y_min=-4.5;
    float pos_y_max=4.5;
    glBegin(GL_QUADS);
    glTexCoord2f(0.0,0.0);
    glVertex3f(pos_x_min,pos_y_min,-distance_to_screen);
    glTexCoord2f(1.0,0.0);
    glVertex3f(pos_x_max,pos_y_min,-distance_to_screen);
    glTexCoord2f(1.0,1.0);
    glVertex3f(pos_x_max,pos_y_max,-distance_to_screen);
    glTexCoord2f(0.0,1.0);
    glVertex3f(pos_x_min,pos_y_max,-distance_to_screen);
    glEnd();

    //draw start to start
    glBindTexture(GL_TEXTURE_2D, m_tex_extra_text);
    distance_to_screen=7.80*( 1.0/m_screen_aspect_ratio )/0.5625 +0.3*sinf(timer*0.3);
    brightness=0.8+0.2*sinf(timer*0.4);
    glColor3f(brightness,brightness,brightness);
    pos_x_min=-8*(513.0/1920.0)*0.5-2.0;
    pos_x_max=8*(513.0/1920.0)*0.5-2.0;
    pos_y_min=-4.5*(62.0/1080.0)*0.5+0.05;
    pos_y_max=4.5*(62.0/1080.0)*0.5+0.05;
    float tex_x_min=0.0;
    float tex_x_max=513.0/1920.0;
    float tex_y_max=1.0-0.0;
    float tex_y_min=1.0-62.0/1080.0;
    glBegin(GL_QUADS);
    glTexCoord2f(tex_x_min,tex_y_min);
    glVertex3f(pos_x_min,pos_y_min,-distance_to_screen);
    glTexCoord2f(tex_x_max,tex_y_min);
    glVertex3f(pos_x_max,pos_y_min,-distance_to_screen);
    glTexCoord2f(tex_x_max,tex_y_max);
    glVertex3f(pos_x_max,pos_y_max,-distance_to_screen);
    glTexCoord2f(tex_x_min,tex_y_max);
    glVertex3f(pos_x_min,pos_y_max,-distance_to_screen);
    glEnd();

    //draw back for help
    distance_to_screen=7.80*( 1.0/m_screen_aspect_ratio )/0.5625 +0.3*sinf(timer*0.1);
    brightness=0.8+0.2*sinf(timer*0.8);
    glColor3f(brightness,brightness,brightness);
    pos_x_min=-8*(505.0/1920.0)*0.5+2.0;
    pos_x_max=8*(505.0/1920.0)*0.5+2.0;
    pos_y_min=-4.5*((128.0-63.0)/1080.0)*0.5+0.05;
    pos_y_max=4.5*((128.0-63.0)/1080.0)*0.5+0.05;
    tex_x_min=0.0;
    tex_x_max=505.0/1920.0;
    tex_y_max=1.0-63.0/1080.0;
    tex_y_min=1.0-128.0/1080.0;
    glBegin(GL_QUADS);
    glTexCoord2f(tex_x_min,tex_y_min);
    glVertex3f(pos_x_min,pos_y_min,-distance_to_screen);
    glTexCoord2f(tex_x_max,tex_y_min);
    glVertex3f(pos_x_max,pos_y_min,-distance_to_screen);
    glTexCoord2f(tex_x_max,tex_y_max);
    glVertex3f(pos_x_max,pos_y_max,-distance_to_screen);
    glTexCoord2f(tex_x_min,tex_y_max);
    glVertex3f(pos_x_min,pos_y_max,-distance_to_screen);
    glEnd();

    //draw #players
    distance_to_screen=7.80*( 1.0/m_screen_aspect_ratio )/0.5625 +0.01*sinf(timer*0.6);
    brightness=0.8+0.2*sinf(timer*0.1);
    glColor3f(brightness,brightness,brightness);
    pos_x_min=-8*(319.0/1920.0)*0.5+6.3;
    pos_x_max=8*(319.0/1920.0)*0.5+6.3;
    pos_y_min=-4.5*((191.0-129.0)/1080.0)*0.5+3.5;
    pos_y_max=4.5*((191.0-129.0)/1080.0)*0.5+3.5;
    tex_x_min=0.0;
    tex_x_max=319.0/1920.0;
    tex_y_max=1.0-129.0/1080.0;
    tex_y_min=1.0-191.0/1080.0;
    glBegin(GL_QUADS);
    glTexCoord2f(tex_x_min,tex_y_min);
    glVertex3f(pos_x_min,pos_y_min,-distance_to_screen);
    glTexCoord2f(tex_x_max,tex_y_min);
    glVertex3f(pos_x_max,pos_y_min,-distance_to_screen);
    glTexCoord2f(tex_x_max,tex_y_max);
    glVertex3f(pos_x_max,pos_y_max,-distance_to_screen);
    glTexCoord2f(tex_x_min,tex_y_max);
    glVertex3f(pos_x_min,pos_y_max,-distance_to_screen);
    glEnd();

    //draw cred
    distance_to_screen=7.80*( 1.0/m_screen_aspect_ratio )/0.5625 +0.1*sinf(timer*0.1);
    brightness=0.8+0.2*sinf(timer*0.8);
    glColor3f(brightness,brightness,brightness);
    pos_x_min=-8*(1124.0/1920.0)*0.5+5.5;
    pos_x_max=8*(1124.0/1920.0)*0.5+5.5;
    pos_y_min=-4.5*((259.0-192.0)/1080.0)*0.5-4.3;
    pos_y_max=4.5*((259.0-192.0)/1080.0)*0.5-4.3;
    tex_x_min=0.0;
    tex_x_max=1124.0/1920.0;
    tex_y_max=1.0-192.0/1080.0;
    tex_y_min=1.0-259.0/1080.0;
    glBegin(GL_QUADS);
    glTexCoord2f(tex_x_min,tex_y_min);
    glVertex3f(pos_x_min,pos_y_min,-distance_to_screen);
    glTexCoord2f(tex_x_max,tex_y_min);
    glVertex3f(pos_x_max,pos_y_min,-distance_to_screen);
    glTexCoord2f(tex_x_max,tex_y_max);
    glVertex3f(pos_x_max,pos_y_max,-distance_to_screen);
    glTexCoord2f(tex_x_min,tex_y_max);
    glVertex3f(pos_x_min,pos_y_max,-distance_to_screen);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    if(m_intro_zoom_timer<=0.0)
    {
        //draw player model in 2D
        glPopMatrix();
        glPushMatrix();
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glOrtho(-10.0*m_screen_aspect_ratio*( 1.0/m_screen_aspect_ratio )/0.5625 ,
                10.0*m_screen_aspect_ratio*( 1.0/m_screen_aspect_ratio )/0.5625 ,
                -10*( 1.0/m_screen_aspect_ratio )/0.5625 ,
                10*( 1.0/m_screen_aspect_ratio )/0.5625  ,
                0.001f, 100.0f);
        static float rota=0.0;
        rota+=0.05;
        glScalef(1.7,1.7,1.7);
        glTranslatef(-7,3.2,-10);
        glRotatef(rota,0,1,0);
        m_pModel_robot->draw();
        glTranslatef(0,-5.0,0);
        glRotatef(rota*0.1+90,0,1,0);
        m_pModel_robot->draw();
        gluPerspective(m_screen_fov, m_screen_aspect_ratio , 0.001f, 100.0f);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        //draw station model
        glPushMatrix();
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glOrtho(-10.0*m_screen_aspect_ratio*( 1.0/m_screen_aspect_ratio )/0.5625 ,
                10.0*m_screen_aspect_ratio*( 1.0/m_screen_aspect_ratio )/0.5625 ,
                -10*( 1.0/m_screen_aspect_ratio )/0.5625 ,
                10*( 1.0/m_screen_aspect_ratio )/0.5625  ,
                0.001f, 100.0f);

        glTranslatef(14,4.5,0);
        glRotatef(90,0,1,0);
        float scale=8.0;
        glScalef(scale,scale,scale);
        m_pModel_station->draw();

        gluPerspective(m_screen_fov, m_screen_aspect_ratio , 0.001f, 100.0f);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);

        glPopMatrix();

        //draw number of players in station
        glPushMatrix();
        glTranslatef(34.53,14.6,-46.0*( 1.0/m_screen_aspect_ratio )/0.5625);

        float val=2.65;

        for(int player_ind=0;player_ind<m_player_counter&&player_ind<9;player_ind++)
        {
            glPushMatrix();
            switch(player_ind)
            {
                case 0: glTranslatef(0,0,0); break;
                case 1: glTranslatef(val,0,0); break;
                case 2: glTranslatef(val*2,0,0); break;
                case 3: glTranslatef(0,-val,0); break;
                case 4: glTranslatef(val,-val,0); break;
                case 5: glTranslatef(val*2,-val,0); break;
                case 6: glTranslatef(0,-val*2,0); break;
                case 7: glTranslatef(val,-val*2,0); break;
                case 8: glTranslatef(val*2,-val*2,0); break;
            }

            //draw
            glColor3fv(_player_color[player_ind]);
            glBegin(GL_QUADS);
            glVertex3f(-1,-1,0);
            glVertex3f(1,-1,0);
            glVertex3f(1,1,0);
            glVertex3f(-1,1,0);
            glEnd();
            glPopMatrix();
        }
    }

    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FOG);
    glDepthMask(true);


    return true;
}

bool menu::inc_player_counter(void)
{
    m_player_counter++;
    if(m_player_counter>9)
    {
        m_player_counter=9;
        return false;//no increase
    }

    return true;
}

bool menu::dec_player_counter(void)
{
    m_player_counter--;
    if(m_player_counter<1)
    {
        m_player_counter=1;
        return false;//no decrease
    }
    return true;
}

int menu::get_numof_players(void)
{
    return m_player_counter;
}

bool menu::show_help(bool flag)
{
    m_show_help=flag;
    return true;
}

