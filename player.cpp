#include "player.h"

player::player(model_obj* pModel_part[],int color_id)
{
    m_pos_x=0;
    m_pos_y=0;
    m_rot_yaw_body=0;
    m_rot_pitch_body=0;
    m_rot_yaw_eye=0;
    m_rot_pitch_eye=0;
    m_arm_carry_id=m_battery_cube_id=-1;
    m_arm_state=pas_idle;
    m_arm_grab_anim_timer=0.0;
    m_arm_length_curr=_arm_length_short;
    m_arm_length_req=1.0;
    m_inverse_look[0]=true; m_inverse_look[1]=true;
    m_color_id=color_id;

    m_pModel_body=pModel_part[0];
    m_pModel_shoulder=pModel_part[1];
    m_pModel_hand=pModel_part[2];
    m_pModel_head=pModel_part[3];
    m_pModel_throat=pModel_part[4];
}

bool player::set_location(float xpos,float ypos)
{
    m_pos_x=xpos;
    m_pos_y=ypos;

    return true;
}

bool player::get_location(float loc[2])
{
    loc[0]=m_pos_x;
    loc[1]=m_pos_y;

    return true;
}

bool player::set_pos(float pos[8])//x,y,yaw_body,pitch_body,yaw_eye,pitch_eye
{
    m_pos_x=pos[0];
    m_pos_y=pos[1];
    m_rot_yaw_body=pos[2];
    m_rot_pitch_body=pos[3];
    m_rot_yaw_eye=pos[4];
    m_rot_pitch_eye=pos[5];
    m_rot_yaw_arm=pos[6];
    m_rot_pitch_arm=pos[7];

    return true;
}

bool player::get_pos(float pos[8])//x,y,yaw_body,pitch_body,yaw_eye,pitch_eye
{
    pos[0]=m_pos_x;
    pos[1]=m_pos_y;
    pos[2]=m_rot_yaw_body;
    pos[3]=m_rot_pitch_body;
    pos[4]=m_rot_yaw_eye;
    pos[5]=m_rot_pitch_eye;
    pos[6]=m_rot_yaw_arm;
    pos[7]=m_rot_pitch_arm;

    return true;
}

bool player::update(float time_dif)
{
    switch(m_arm_state)
    {
        case pas_idle:
        {
            ;//nothing
        }break;

        case pas_extend:
        {
            //extend arm
            m_arm_grab_anim_timer+=time_dif;
            if(m_arm_grab_anim_timer>_arm_grab_anim_length*m_arm_length_req)//done
            {
                m_arm_state=pas_retract;
                //m_arm_grab_anim_timer=0.0;
                m_arm_length_curr=_arm_length_long*m_arm_length_req;
            }
            else
            {
                float progress=m_arm_grab_anim_timer/_arm_grab_anim_length;
                m_arm_length_curr=_arm_length_short+(_arm_length_long-_arm_length_short)*progress;
            }
        }break;

        case pas_retract:
        {
            //retract arm, count down timer
            m_arm_grab_anim_timer-=time_dif;
            if(m_arm_grab_anim_timer<0.0)//done
            {
                m_arm_state=pas_idle;
                m_arm_grab_anim_timer=0.0;
                m_arm_length_curr=_arm_length_short;
            }
            else
            {
                float progress=m_arm_grab_anim_timer/_arm_grab_anim_length;
                m_arm_length_curr=_arm_length_short+(_arm_length_long-_arm_length_short)*progress;
            }
        }break;
    }

    return true;
}

bool player::draw(bool draw_active_player)
{
    //draw body
    glPushMatrix();
    glTranslatef(-m_pos_x,m_pos_y,0);
    glRotatef(m_rot_yaw_body+90,0,0,1);
    glRotatef(90,1,0,0);
    glTranslatef(0,0.58,0);
    glScalef(0.3,0.3,0.3);
    m_pModel_body->draw(false);
    glPopMatrix();

    //draw shoulder (first if other player)
    if(!draw_active_player)
    {
        glPushMatrix();
        glTranslatef(-m_pos_x,m_pos_y,0);
        glRotatef(m_rot_yaw_arm+90,0,0,1);
        glRotatef(90,1,0,0);
        glTranslatef(0,0.58,0);
        glScalef(0.3,0.3,0.3);
        m_pModel_shoulder->draw(false);
        glPopMatrix();
    }

    //draw arm (if active player, draw from other direction)
    float arm_progress=(m_arm_length_curr-_arm_length_short)/(_arm_length_long-_arm_length_short);//0-1
    if(draw_active_player)
    {
        glDisable(GL_DEPTH_TEST);

        glPushMatrix();
        glTranslatef(-m_pos_x,m_pos_y,0.38);
        glRotatef(m_rot_yaw_arm+90,0,0,1);
        glTranslatef(0.0,-0.28,0.0);
        glRotatef(m_rot_pitch_arm,0,-1,0);
        glRotatef(90,1,0,0);

        //draw hand
        glTranslatef(0.03,0,0);//1
        glTranslatef(0.09+0.06*arm_progress,0,0);//2
        glTranslatef(0.02+0.12*arm_progress,0,0);//3
        glTranslatef(0.01+0.06*arm_progress,0,0);//4
        glScalef(0.3,0.3,0.3);
        m_pModel_hand->draw(false);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-m_pos_x,m_pos_y,0.38);
        glRotatef(m_rot_yaw_arm+90,0,0,1);
        glTranslatef(0.0,-0.28,0.0);
        glRotatef(m_rot_pitch_arm,0,-1,0);
        glRotatef(90,1,0,0);
        //draw third arm
        if(true)
        {
        glDepthMask(false);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glTranslatef(0.03,0,0);//1
        glTranslatef(0.09+0.06*arm_progress,0,0);//2
        glTranslatef(0.02+0.12*arm_progress,0,0);//3
        float arm_size_x=0.01+0.06*arm_progress;
        float arm_size_y=0.015;
        float arm_size_z=0.015;
        glColor4f(0,0.5,0,0.5);
        glBegin(GL_QUADS);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);

        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);

        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);

        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);

        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);

        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glEnd();
        //outline
        glLineWidth(4);
        glColor4f(0,0.8,0,0.4);
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glEnd();
        //oultine 2
        glLineWidth(2);
        glColor4f(0.2,1,0.2,0.5);
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glEnd();
        glLineWidth(1);
        glPopMatrix();
        }

        //draw second arm
        if(true)
        {
        glPushMatrix();
        glTranslatef(-m_pos_x,m_pos_y,0.38);
        glRotatef(m_rot_yaw_arm+90,0,0,1);
        glTranslatef(0.0,-0.28,0.0);
        glRotatef(m_rot_pitch_arm,0,-1,0);
        glRotatef(90,1,0,0);

        glTranslatef(0.03,0,0);//1
        glTranslatef(0.09+0.06*arm_progress,0,0);//2
        float arm_size_x=0.01+0.06*arm_progress;
        float arm_size_y=0.025;
        float arm_size_z=0.025;
        glColor4f(0,0.5,0,0.5);
        glBegin(GL_QUADS);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);

        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);

        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);

        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);

        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);

        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glEnd();
        //outline
        glLineWidth(4);
        glColor4f(0,0.8,0,0.4);
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glEnd();
        //oultine 2
        glLineWidth(2);
        glColor4f(0.2,1,0.2,0.5);
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glEnd();
        glLineWidth(1);
        glPopMatrix();
        }

        //draw first arm
        if(true)
        {
        glPushMatrix();
        glTranslatef(-m_pos_x,m_pos_y,0.38);
        glRotatef(m_rot_yaw_arm+90,0,0,1);
        glTranslatef(0.0,-0.28,0.0);
        glRotatef(m_rot_pitch_arm,0,-1,0);
        glRotatef(90,1,0,0);

        glTranslatef(0.03,0,0);
        float arm_size_x=0.08;
        float arm_size_y=0.04;
        float arm_size_z=0.04;
        glColor4f(0,0.5,0,0.5);
        glBegin(GL_QUADS);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);

        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);

        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);

        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);

        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);

        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glEnd();
        //outline
        glLineWidth(4);
        glColor4f(0,0.8,0,0.4);
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glEnd();
        //oultine 2
        glLineWidth(2);
        glColor4f(0.2,1,0.2,0.5);
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glEnd();
        glLineWidth(1);

        glDisable(GL_BLEND);
        glDepthMask(true);
        glPopMatrix();
        }

        glEnable(GL_DEPTH_TEST);
    }
    else//large part to small
    {
        //draw first arm
        {
        glPushMatrix();
        glTranslatef(-m_pos_x,m_pos_y,0.38);
        glRotatef(m_rot_yaw_arm+90,0,0,1);
        glTranslatef(0.0,-0.28,0.0);
        glRotatef(m_rot_pitch_arm,0,-1,0);
        glRotatef(90,1,0,0);

        glDepthMask(false);
        glTranslatef(0.03,0,0);
        float arm_size_x=0.08;
        float arm_size_y=0.04;
        float arm_size_z=0.04;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0,0.5,0,0.5);
        glBegin(GL_QUADS);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);

        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);

        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);

        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);

        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);

        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glEnd();
        //outline
        glLineWidth(4);
        glColor4f(0,0.8,0,0.4);
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glEnd();
        //oultine 2
        glLineWidth(2);
        glColor4f(0.2,1,0.2,0.5);
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glEnd();
        glLineWidth(1);
        }
        //draw second arm
        {
        glTranslatef(0.09+0.06*arm_progress,0,0);
        float arm_size_x=0.01+0.06*arm_progress;
        float arm_size_y=0.025;
        float arm_size_z=0.025;
        glColor4f(0,0.5,0,0.5);
        glBegin(GL_QUADS);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);

        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);

        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);

        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);

        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);

        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glEnd();
        //outline
        glLineWidth(4);
        glColor4f(0,0.8,0,0.4);
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glEnd();
        //oultine 2
        glLineWidth(2);
        glColor4f(0.2,1,0.2,0.5);
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glEnd();
        glLineWidth(1);
        }
        //draw third arm
        {
        glTranslatef(0.02+0.12*arm_progress,0,0);
        float arm_size_x=0.01+0.06*arm_progress;
        float arm_size_y=0.015;
        float arm_size_z=0.015;
        glColor4f(0,0.5,0,0.5);
        glBegin(GL_QUADS);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);

        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);

        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);

        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);

        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);

        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glEnd();
        //outline
        glLineWidth(4);
        glColor4f(0,0.8,0,0.4);
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glEnd();
        //oultine 2
        glLineWidth(2);
        glColor4f(0.2,1,0.2,0.5);
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
        glEnd();
        glBegin(GL_LINE_STRIP);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,arm_size_z);
        glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
        glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
        glEnd();
        glLineWidth(1);
        }
        //draw hand
        glDisable(GL_BLEND);
        glDepthMask(true);
        glTranslatef(0.01+0.06*arm_progress,0,0);
        glScalef(0.3,0.3,0.3);
        m_pModel_hand->draw(false);
        glPopMatrix();
    }

    //draw shoulder (after if active player)
    if(draw_active_player)
    {
        glPushMatrix();
        glTranslatef(-m_pos_x,m_pos_y,0);
        glRotatef(m_rot_yaw_arm+90,0,0,1);
        glRotatef(90,1,0,0);
        glTranslatef(0,0.58,0);
        glScalef(0.3,0.3,0.3);
        m_pModel_shoulder->draw(false);
        glPopMatrix();
    }

    //draw throat
    if(!draw_active_player)
    {
        glPushMatrix();
        glTranslatef(-m_pos_x,m_pos_y,0);
        glRotatef(m_rot_yaw_body+90,0,0,1);
        glRotatef(90,1,0,0);
        glTranslatef(0,0.58,0);
        glScalef(0.3,0.3,0.3);
        m_pModel_throat->draw(false);
        glPopMatrix();
    }

    //draw head
    if(!draw_active_player)
    {
        //draw eye
        glPushMatrix();
        glTranslatef(-m_pos_x,m_pos_y,0);
        glRotatef(m_rot_yaw_eye+90,0,0,1);
        glRotatef(90,1,0,0);
        glTranslatef(0.16,0.72,0.07);
        glEnable(GL_BLEND);
        glColor4f(_player_color[m_color_id][0],_player_color[m_color_id][1],_player_color[m_color_id][2],0.7);
        float eye_size_x=0.003;
        float eye_size_y=0.01;
        float eye_size_z=0.01;
        glBegin(GL_QUADS);
        glVertex3f(-eye_size_x,-eye_size_y,-eye_size_z);
        glVertex3f(eye_size_x,-eye_size_y,-eye_size_z);
        glVertex3f(eye_size_x,eye_size_y,-eye_size_z);
        glVertex3f(-eye_size_x,eye_size_y,-eye_size_z);

        glVertex3f(-eye_size_x,-eye_size_y,eye_size_z);
        glVertex3f(eye_size_x,-eye_size_y,eye_size_z);
        glVertex3f(eye_size_x,eye_size_y,eye_size_z);
        glVertex3f(-eye_size_x,eye_size_y,eye_size_z);

        /*glVertex3f(-eye_size_x,-eye_size_y,-eye_size_z);
        glVertex3f(-eye_size_x,-eye_size_y,eye_size_z);
        glVertex3f(-eye_size_x,eye_size_y,eye_size_z);
        glVertex3f(-eye_size_x,eye_size_y,-eye_size_z);*/

        glVertex3f(eye_size_x,-eye_size_y,-eye_size_z);
        glVertex3f(eye_size_x,-eye_size_y,eye_size_z);
        glVertex3f(eye_size_x,eye_size_y,eye_size_z);
        glVertex3f(eye_size_x,eye_size_y,-eye_size_z);

        glVertex3f(-eye_size_x,-eye_size_y,-eye_size_z);
        glVertex3f(-eye_size_x,-eye_size_y,eye_size_z);
        glVertex3f(eye_size_x,-eye_size_y,eye_size_z);
        glVertex3f(eye_size_x,-eye_size_y,-eye_size_z);

        glVertex3f(-eye_size_x,eye_size_y,-eye_size_z);
        glVertex3f(-eye_size_x,eye_size_y,eye_size_z);
        glVertex3f(eye_size_x,eye_size_y,eye_size_z);
        glVertex3f(eye_size_x,eye_size_y,-eye_size_z);
        glEnd();
        //outline
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(2);
        glColor4f(0.1,0.1,0.1,0.9);
        glBegin(GL_QUADS);
        /*glVertex3f(-eye_size_x,-eye_size_y,-eye_size_z);
        glVertex3f(eye_size_x,-eye_size_y,-eye_size_z);
        glVertex3f(eye_size_x,eye_size_y,-eye_size_z);
        glVertex3f(-eye_size_x,eye_size_y,-eye_size_z);

        glVertex3f(-eye_size_x,-eye_size_y,eye_size_z);
        glVertex3f(eye_size_x,-eye_size_y,eye_size_z);
        glVertex3f(eye_size_x,eye_size_y,eye_size_z);
        glVertex3f(-eye_size_x,eye_size_y,eye_size_z);*/

        glVertex3f(-eye_size_x,-eye_size_y,-eye_size_z);
        glVertex3f(-eye_size_x,-eye_size_y,eye_size_z);
        glVertex3f(-eye_size_x,eye_size_y,eye_size_z);
        glVertex3f(-eye_size_x,eye_size_y,-eye_size_z);

        /*glVertex3f(eye_size_x,-eye_size_y,-eye_size_z);
        glVertex3f(eye_size_x,-eye_size_y,eye_size_z);
        glVertex3f(eye_size_x,eye_size_y,eye_size_z);
        glVertex3f(eye_size_x,eye_size_y,-eye_size_z);

        glVertex3f(-eye_size_x,-eye_size_y,-eye_size_z);
        glVertex3f(-eye_size_x,-eye_size_y,eye_size_z);
        glVertex3f(eye_size_x,-eye_size_y,eye_size_z);
        glVertex3f(eye_size_x,-eye_size_y,-eye_size_z);

        glVertex3f(-eye_size_x,eye_size_y,-eye_size_z);
        glVertex3f(-eye_size_x,eye_size_y,eye_size_z);
        glVertex3f(eye_size_x,eye_size_y,eye_size_z);
        glVertex3f(eye_size_x,eye_size_y,-eye_size_z);*/
        glEnd();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable(GL_BLEND);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(-m_pos_x,m_pos_y,0);
        glRotatef(m_rot_yaw_eye+90,0,0,1);
        glRotatef(90,1,0,0);
        glTranslatef(0,0.58,0);
        //glRotatef(m_rot_pitch_eye,0,1,0);
        glScalef(0.3,0.3,0.3);
        m_pModel_head->draw(false);
        glPopMatrix();
    }



    //REPEAT to draw to z-buff
    glPushMatrix();
    glTranslatef(-m_pos_x,m_pos_y,0);
    glRotatef(m_rot_yaw_body+90,0,0,1);
    glRotatef(90,1,0,0);
    glTranslatef(0,0.58,0);
    glScalef(0.3,0.3,0.3);
    //draw body
    m_pModel_body->draw_to_zbuff();
    //draw throat
    if(!draw_active_player) m_pModel_throat->draw_to_zbuff();
    //draw shoulder
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-m_pos_x,m_pos_y,0);
    glRotatef(m_rot_yaw_arm+90,0,0,1);
    glRotatef(90,1,0,0);
    glTranslatef(0,0.58,0);
    glScalef(0.3,0.3,0.3);
    m_pModel_shoulder->draw_to_zbuff();
    glPopMatrix();
    //draw arm
    glColorMask(false,false,false,false);
    glPushMatrix();
    glTranslatef(-m_pos_x,m_pos_y,0.38);
    glRotatef(m_rot_yaw_arm+90,0,0,1);
    glTranslatef(0.0,-0.28,0.0);
    glRotatef(m_rot_pitch_arm,0,-1,0);
    glRotatef(90,1,0,0);
    //draw first arm
    glTranslatef(0.03,0,0);
    float arm_size_x=0.08;
    float arm_size_y=0.04;
    float arm_size_z=0.04;
    //glColor4f(0,0.5,0,0.5);
    glBegin(GL_QUADS);
    glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
    glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
    glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
    glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);

    glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
    glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
    glVertex3f(arm_size_x,arm_size_y,arm_size_z);
    glVertex3f(-arm_size_x,arm_size_y,arm_size_z);

    glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
    glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
    glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
    glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);

    glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
    glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
    glVertex3f(arm_size_x,arm_size_y,arm_size_z);
    glVertex3f(arm_size_x,arm_size_y,-arm_size_z);

    glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
    glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
    glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
    glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);

    glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
    glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
    glVertex3f(arm_size_x,arm_size_y,arm_size_z);
    glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
    glEnd();
    //draw second arm
    glTranslatef(0.09+0.06*arm_progress,0,0);
    arm_size_x=0.01+0.06*arm_progress;
    arm_size_y=0.025;
    arm_size_z=0.025;
    //glColor4f(0,0.5,0,0.5);
    glBegin(GL_QUADS);
    glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
    glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
    glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
    glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);

    glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
    glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
    glVertex3f(arm_size_x,arm_size_y,arm_size_z);
    glVertex3f(-arm_size_x,arm_size_y,arm_size_z);

    glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
    glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
    glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
    glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);

    glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
    glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
    glVertex3f(arm_size_x,arm_size_y,arm_size_z);
    glVertex3f(arm_size_x,arm_size_y,-arm_size_z);

    glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
    glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
    glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
    glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);

    glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
    glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
    glVertex3f(arm_size_x,arm_size_y,arm_size_z);
    glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
    glEnd();
    //draw third arm
    glTranslatef(0.02+0.12*arm_progress,0,0);
    arm_size_x=0.01+0.06*arm_progress;
    arm_size_y=0.015;
    arm_size_z=0.015;
    //glColor4f(0,0.5,0,0.5);
    glBegin(GL_QUADS);
    glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
    glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
    glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
    glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);

    glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
    glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
    glVertex3f(arm_size_x,arm_size_y,arm_size_z);
    glVertex3f(-arm_size_x,arm_size_y,arm_size_z);

    glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
    glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
    glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
    glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);

    glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);
    glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
    glVertex3f(arm_size_x,arm_size_y,arm_size_z);
    glVertex3f(arm_size_x,arm_size_y,-arm_size_z);

    glVertex3f(-arm_size_x,-arm_size_y,-arm_size_z);
    glVertex3f(-arm_size_x,-arm_size_y,arm_size_z);
    glVertex3f(arm_size_x,-arm_size_y,arm_size_z);
    glVertex3f(arm_size_x,-arm_size_y,-arm_size_z);

    glVertex3f(-arm_size_x,arm_size_y,-arm_size_z);
    glVertex3f(-arm_size_x,arm_size_y,arm_size_z);
    glVertex3f(arm_size_x,arm_size_y,arm_size_z);
    glVertex3f(arm_size_x,arm_size_y,-arm_size_z);
    glEnd();
    glColorMask(true,true,true,true);
    //draw hand
    glTranslatef(0.01+0.06*arm_progress,0,0);
    glScalef(0.3,0.3,0.3);
    m_pModel_hand->draw_to_zbuff();
    glPopMatrix();

    /*glPushMatrix();
    glTranslatef(-m_pos_x,m_pos_y,0.39);
    glRotatef(m_rot_yaw_arm+90,0,0,1);
    glTranslatef(0.0,-0.27,0.0);
    glRotatef(m_rot_pitch_arm,0,-1,0);
    glRotatef(90,1,0,0);
    glScalef(0.3,0.3,0.3);
    m_pModel_hand->draw_to_zbuff();
    glPopMatrix();*/
    //draw head
    if(!draw_active_player)
    {
        glPushMatrix();
        glTranslatef(-m_pos_x,m_pos_y,0);
        glRotatef(m_rot_yaw_eye+90,0,0,1);
        glRotatef(90,1,0,0);
        glTranslatef(0,0.58,0);
        //glRotatef(m_rot_pitch_eye,0,1,0);
        glScalef(0.3,0.3,0.3);
        m_pModel_head->draw_to_zbuff();
        glPopMatrix();
    }

    /*//test
    glPushMatrix();

    glTranslatef(-m_pos_x,m_pos_y,0);
    glRotatef(m_rot_yaw_arm,0,0,1);

    glColor3f(1,0,0);
    glBegin(GL_QUADS);
    //left track
    glVertex3f(-0.3,0.3,0.38);
    glVertex3f(-0.3,-0.3,0.38);
    glVertex3f(0.3,-0.3,0.38);
    glVertex3f(0.3,0.3,0.38);
    glEnd();

    glPopMatrix();
    //xxxxx*/

    return true;

    /*//Old draw
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glPushMatrix();

    glTranslatef(-m_pos_x,m_pos_y,0);
    glRotatef(m_rot_yaw_body,0,0,1);
    //draw tracks
    if(draw_edges)  glColor3f(0.9,0.9,0.9);
    else            glColor3f(0.0,0.2,0.5);
    glBegin(GL_QUADS);
    //left track
    glVertex3f(-0.3,0.3,0.2);
    glVertex3f(-0.3,-0.3,0.2);
    glVertex3f(-0.10,-0.3,0.2);
    glVertex3f(-0.10,0.3,0.2);

    glVertex3f(-0.3,0.3,0.01);
    glVertex3f(-0.3,-0.3,0.01);
    glVertex3f(-0.10,-0.3,0.01);
    glVertex3f(-0.10,0.3,0.01);

    glVertex3f(-0.3,-0.3,0.01);
    glVertex3f(-0.3,-0.3,0.2);
    glVertex3f(-0.10,-0.3,0.2);
    glVertex3f(-0.10,-0.3,0.01);

    glVertex3f(-0.3,0.3,0.01);
    glVertex3f(-0.3,0.3,0.2);
    glVertex3f(-0.10,0.3,0.2);
    glVertex3f(-0.10,0.3,0.01);

    glVertex3f(-0.3,0.3,0.01);
    glVertex3f(-0.3,0.3,0.2);
    glVertex3f(-0.3,-0.3,0.2);
    glVertex3f(-0.3,-0.3,0.01);

    glVertex3f(-0.10,0.3,0.01);
    glVertex3f(-0.10,0.3,0.2);
    glVertex3f(-0.10,-0.3,0.2);
    glVertex3f(-0.10,-0.3,0.01);

    //right track
    glVertex3f(0.10,0.3,0.2);
    glVertex3f(0.10,-0.3,0.2);
    glVertex3f(0.3,-0.3,0.2);
    glVertex3f(0.3,0.3,0.2);

    glVertex3f(0.10,0.3,0.01);
    glVertex3f(0.10,-0.3,0.01);
    glVertex3f(0.3,-0.3,0.01);
    glVertex3f(0.3,0.3,0.01);

    glVertex3f(0.10,-0.3,0.01);
    glVertex3f(0.10,-0.3,0.2);
    glVertex3f(0.3,-0.3,0.2);
    glVertex3f(0.3,-0.3,0.01);

    glVertex3f(0.10,0.3,0.01);
    glVertex3f(0.10,0.3,0.2);
    glVertex3f(0.3,0.3,0.2);
    glVertex3f(0.3,0.3,0.01);

    glVertex3f(0.10,0.3,0.01);
    glVertex3f(0.10,0.3,0.2);
    glVertex3f(0.10,-0.3,0.2);
    glVertex3f(0.10,-0.3,0.01);

    glVertex3f(0.3,0.3,0.01);
    glVertex3f(0.3,0.3,0.2);
    glVertex3f(0.3,-0.3,0.2);
    glVertex3f(0.3,-0.3,0.01);

    if(!draw_active_player)
    {
        //draw center tower
        if(draw_edges)  glColor3f(0.9,0.9,0.9);
        else            glColor3f(0.2,0.2,0.4);
        glVertex3f(-0.12,-0.12,0.05);
        glVertex3f(-0.12,-0.12,0.7);
        glVertex3f(0.12,-0.12,0.7);
        glVertex3f(0.12,-0.12,0.05);

        glVertex3f(-0.12,0.12,0.05);
        glVertex3f(-0.12,0.12,0.7);
        glVertex3f(0.12,0.12,0.7);
        glVertex3f(0.12,0.12,0.05);

        glVertex3f(-0.12,-0.12,0.05);
        glVertex3f(-0.12,-0.12,0.7);
        glVertex3f(-0.12,0.12,0.7);
        glVertex3f(-0.12,0.12,0.05);

        glVertex3f(0.12,-0.12,0.05);
        glVertex3f(0.12,-0.12,0.7);
        glVertex3f(0.12,0.12,0.7);
        glVertex3f(0.12,0.12,0.05);
    }

    glEnd();

    //draw head
    glRotatef(-m_rot_yaw_body,0,0,1);//reset
    glRotatef(m_rot_yaw_eye,0,0,1);
    glTranslatef(0,0,0.7);
    glRotatef(m_rot_pitch_eye*0.3,1,0,0);//lower pitch angle
    //glTranslatef(0,0,-0.7);
    if(!draw_active_player)
    {
        if(draw_edges)  glColor3f(0.9,0.9,0.9);
        else            glColor3f(0.3,0.2,0.2);
        glBegin(GL_QUADS);

        glVertex3f(-0.16,0.16,-0.1);
        glVertex3f(-0.16,-0.16,-0.1);
        glVertex3f(0.16,-0.16,-0.1);
        glVertex3f(0.16,0.16,-0.1);

        glVertex3f(-0.16,0.16,0.1);
        glVertex3f(-0.16,-0.16,0.1);
        glVertex3f(0.16,-0.16,0.1);
        glVertex3f(0.16,0.16,0.1);

        glVertex3f(-0.16,-0.16,-0.1);
        glVertex3f(-0.16,-0.16,0.1);
        glVertex3f(0.16,-0.16,0.1);
        glVertex3f(0.16,-0.16,-0.1);

        glVertex3f(-0.16,0.16,-0.1);
        glVertex3f(-0.16,0.16,0.1);
        glVertex3f(0.16,0.16,0.1);
        glVertex3f(0.16,0.16,-0.1);

        glVertex3f(-0.16,-0.16,-0.1);
        glVertex3f(-0.16,-0.16,0.1);
        glVertex3f(-0.16,0.16,0.1);
        glVertex3f(-0.16,0.16,-0.1);

        glVertex3f(0.16,-0.16,-0.1);
        glVertex3f(0.16,-0.16,0.1);
        glVertex3f(0.16,0.16,0.1);
        glVertex3f(0.16,0.16,-0.1);

        glEnd();

        //lines
    }

    //draw arm
    glRotatef(-m_rot_pitch_eye*0.3,1,0,0);//reset
    //glTranslatef(0,0,-0.7);
    glRotatef(-m_rot_yaw_eye,0,0,1);//reset
    //glTranslatef(0,0,-0.2);
    glRotatef(m_rot_yaw_arm,0,0,1);
    //glTranslatef(0.18,0,-0.2);
    glTranslatef(0,0,-0.2);
    glRotatef(m_rot_pitch_arm,1,0,0);
    //glTranslatef(0,0,-0.2);
    glTranslatef(0.18,0,0);
    if(draw_edges)  glColor3f(0.9,0.9,0.9);
    else            glColor3f(0.5,0.4,0.4);
    float arm_length=m_arm_length_curr;
    glBegin(GL_QUADS);

    glVertex3f(-0.05,arm_length,0.05);
    glVertex3f(-0.05,-0.1,0.05);
    glVertex3f(0.05,-0.1,0.05);
    glVertex3f(0.05,arm_length,0.05);

    glVertex3f(-0.05,arm_length,-0.05);
    glVertex3f(-0.05,-0.1,-0.05);
    glVertex3f(0.05,-0.1,-0.05);
    glVertex3f(0.05,arm_length,-0.05);

    glVertex3f(-0.05,-0.1,-0.05);
    glVertex3f(-0.05,-0.1,0.05);
    glVertex3f(0.05,-0.1,0.05);
    glVertex3f(0.05,-0.1,-0.05);

    if(draw_edges)  glColor3f(0.9,0.9,0.9);
    else            glColor3f(0.7,0.4,0.4);
    glVertex3f(-0.05,arm_length,-0.05);
    glVertex3f(-0.05,arm_length,0.05);
    glVertex3f(0.05,arm_length,0.05);
    glVertex3f(0.05,arm_length,-0.05);

    if(draw_edges)  glColor3f(0.9,0.9,0.9);
    else            glColor3f(0.5,0.4,0.4);
    glVertex3f(-0.05,-0.1,-0.05);
    glVertex3f(-0.05,-0.1,0.05);
    glVertex3f(-0.05,arm_length,0.05);
    glVertex3f(-0.05,arm_length,-0.05);

    glVertex3f(0.05,-0.1,-0.05);
    glVertex3f(0.05,-0.1,0.05);
    glVertex3f(0.05,arm_length,0.05);
    glVertex3f(0.05,arm_length,-0.05);

    glEnd();

    glPopMatrix();

    if(draw_edges) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    return true;*/
}

int player::get_arm_status(void)
{
    return m_arm_carry_id;
}

bool player::set_arm_status(int status)
{
    //if status==-1 dropped powcube, else grabbed powcube with id=status
    m_arm_carry_id=status;

    return true;
}

int player::get_battery_status(void)
{
    return m_battery_cube_id;
}

bool player::set_battery_status(int status)
{
    //if status==-1 dropped powcube, else grabbed powcube with id=status
    m_battery_cube_id=status;

    return true;
}

bool player::anim_grab(int arm_state,float arm_length_req)
{
    m_arm_state=arm_state;
    m_arm_length_req=arm_length_req;

    //m_arm_length_req=1.0;

    //cout<<m_arm_length_req<<endl;
    //m_arm_length_req*=3.0;
    //if(m_arm_length_req>1.0) m_arm_length_req=1.0;

    return true;
}

bool player::point_inside_player_fast(float _x,float _y,float size_factor)
{
    //test only axis locked square around x,y pos
    float player_size=_player_size*size_factor;
    if( m_pos_x+player_size > _x && m_pos_x-player_size < _x &&
        m_pos_y+player_size > _y && m_pos_y-player_size < _y )
    {
        return true;
    }

    return false;

    return false;
}

bool player::get_look_inversion(bool inverse_flag[2])
{
    inverse_flag[0]=m_inverse_look[0];
    inverse_flag[1]=m_inverse_look[1];

    return true;
}

bool player::set_look_inversion(bool inverse_flag[2])
{
    m_inverse_look[0]=inverse_flag[0];
    m_inverse_look[1]=inverse_flag[1];

    return true;
}

//Private

