#include "powcube.h"

powcube::powcube()
{
    m_pos_x=m_pos_y=m_pos_z=0.0;
    m_rot_yaw=m_rot_pitch=0.0;
    m_owner_id=m_carried_id=m_at_station_id=-1;
    m_state=pcs_error;
    m_anim_progress=1.0;
}

bool powcube::init(int owner_id)
{
    m_owner_id=owner_id;

    return true;
}

bool powcube::update(float time_dif)
{
    if(m_anim_progress!=1.0)// 1 if done
    {
        //update travel pos
        m_pos_x=m_anim_start_pos_x+(m_anim_end_pos_x-m_anim_start_pos_x)*m_anim_progress;
        m_pos_y=m_anim_start_pos_y+(m_anim_end_pos_y-m_anim_start_pos_y)*m_anim_progress;
        m_pos_z=m_anim_start_pos_z+(m_anim_end_pos_z-m_anim_start_pos_z)*m_anim_progress;
        m_rot_yaw=m_anim_start_rot_yaw+(m_anim_end_rot_yaw-m_anim_start_rot_yaw)*m_anim_progress;
        m_rot_pitch=m_anim_start_rot_pitch+(m_anim_end_rot_pitch-m_anim_start_rot_pitch)*m_anim_progress;

        //update time
        m_anim_progress+=time_dif/_cube_transfer_time*m_anim_speed;
        if(m_anim_progress>=1.0) //done
        {
            m_anim_progress=1.0;
            //set final pos
            m_pos_x=m_anim_end_pos_x;
            m_pos_y=m_anim_end_pos_y;
            m_pos_z=m_anim_end_pos_z;
            m_rot_yaw=m_anim_end_rot_yaw;
            m_rot_pitch=m_anim_end_rot_pitch;
        }
    }

    return true;
}

bool powcube::draw(void)
{
    glPushMatrix();

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(false);

    glTranslatef(-m_pos_x,m_pos_y,m_pos_z);
    glRotatef(m_rot_yaw,0,0,1);//yaw
    glRotatef(m_rot_pitch,1,0,0);//pitch
    glColor4f(_player_color[m_owner_id][0],_player_color[m_owner_id][1],_player_color[m_owner_id][2],0.6);
    float cube_size=_cube_size*0.9;
    glBegin(GL_QUADS);
    glVertex3f(-cube_size,cube_size,-cube_size);
    glVertex3f(-cube_size,-cube_size,-cube_size);
    glVertex3f(cube_size,-cube_size,-cube_size);
    glVertex3f(cube_size,cube_size,-cube_size);

    glVertex3f(-cube_size,cube_size,cube_size);
    glVertex3f(-cube_size,-cube_size,cube_size);
    glVertex3f(cube_size,-cube_size,cube_size);
    glVertex3f(cube_size,cube_size,cube_size);


    glVertex3f(-cube_size,-cube_size,cube_size);
    glVertex3f(-cube_size,-cube_size,-cube_size);
    glVertex3f(cube_size,-cube_size,-cube_size);
    glVertex3f(cube_size,-cube_size,cube_size);

    glVertex3f(-cube_size,cube_size,cube_size);
    glVertex3f(-cube_size,cube_size,-cube_size);
    glVertex3f(cube_size,cube_size,-cube_size);
    glVertex3f(cube_size,cube_size,cube_size);


    glVertex3f(-cube_size,-cube_size,cube_size);
    glVertex3f(-cube_size,-cube_size,-cube_size);
    glVertex3f(-cube_size,cube_size,-cube_size);
    glVertex3f(-cube_size,cube_size,cube_size);

    glVertex3f(cube_size,-cube_size,cube_size);
    glVertex3f(cube_size,-cube_size,-cube_size);
    glVertex3f(cube_size,cube_size,-cube_size);
    glVertex3f(cube_size,cube_size,cube_size);
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(3);
    glColor4f(0.9,0.9,0.9,0.5);
    glBegin(GL_QUADS);
    glVertex3f(-cube_size,cube_size,-cube_size);
    glVertex3f(-cube_size,-cube_size,-cube_size);
    glVertex3f(cube_size,-cube_size,-cube_size);
    glVertex3f(cube_size,cube_size,-cube_size);

    glVertex3f(-cube_size,cube_size,cube_size);
    glVertex3f(-cube_size,-cube_size,cube_size);
    glVertex3f(cube_size,-cube_size,cube_size);
    glVertex3f(cube_size,cube_size,cube_size);


    glVertex3f(-cube_size,-cube_size,cube_size);
    glVertex3f(-cube_size,-cube_size,-cube_size);
    glVertex3f(cube_size,-cube_size,-cube_size);
    glVertex3f(cube_size,-cube_size,cube_size);

    glVertex3f(-cube_size,cube_size,cube_size);
    glVertex3f(-cube_size,cube_size,-cube_size);
    glVertex3f(cube_size,cube_size,-cube_size);
    glVertex3f(cube_size,cube_size,cube_size);


    glVertex3f(-cube_size,-cube_size,cube_size);
    glVertex3f(-cube_size,-cube_size,-cube_size);
    glVertex3f(-cube_size,cube_size,-cube_size);
    glVertex3f(-cube_size,cube_size,cube_size);

    glVertex3f(cube_size,-cube_size,cube_size);
    glVertex3f(cube_size,-cube_size,-cube_size);
    glVertex3f(cube_size,cube_size,-cube_size);
    glVertex3f(cube_size,cube_size,cube_size);
    glEnd();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDisable(GL_BLEND);
    glDepthMask(true);

    //repeat to zbuff
    glColorMask(false,false,false,false);
    glBegin(GL_QUADS);
    glVertex3f(-cube_size,cube_size,-cube_size);
    glVertex3f(-cube_size,-cube_size,-cube_size);
    glVertex3f(cube_size,-cube_size,-cube_size);
    glVertex3f(cube_size,cube_size,-cube_size);

    glVertex3f(-cube_size,cube_size,cube_size);
    glVertex3f(-cube_size,-cube_size,cube_size);
    glVertex3f(cube_size,-cube_size,cube_size);
    glVertex3f(cube_size,cube_size,cube_size);

    glVertex3f(-cube_size,-cube_size,cube_size);
    glVertex3f(-cube_size,-cube_size,-cube_size);
    glVertex3f(cube_size,-cube_size,-cube_size);
    glVertex3f(cube_size,-cube_size,cube_size);

    glVertex3f(-cube_size,cube_size,cube_size);
    glVertex3f(-cube_size,cube_size,-cube_size);
    glVertex3f(cube_size,cube_size,-cube_size);
    glVertex3f(cube_size,cube_size,cube_size);

    glVertex3f(-cube_size,-cube_size,cube_size);
    glVertex3f(-cube_size,-cube_size,-cube_size);
    glVertex3f(-cube_size,cube_size,-cube_size);
    glVertex3f(-cube_size,cube_size,cube_size);

    glVertex3f(cube_size,-cube_size,cube_size);
    glVertex3f(cube_size,-cube_size,-cube_size);
    glVertex3f(cube_size,cube_size,-cube_size);
    glVertex3f(cube_size,cube_size,cube_size);
    glEnd();

    glColorMask(true,true,true,true);

    glPopMatrix();

    return true;
}

bool powcube::set_pos(float pos[5])//x,y,z,yaw,pitch
{
    //if transfer animation in progress, update end pos instead of current pos
    if(m_anim_progress!=1.0)
    {
        m_anim_end_pos_x=pos[0];
        m_anim_end_pos_y=pos[1];
        m_anim_end_pos_z=pos[2];
        m_anim_end_rot_yaw=pos[3];
        m_anim_end_rot_pitch=pos[4];

        while(m_anim_end_rot_yaw>360)  m_anim_end_rot_yaw-=360;
        while(m_anim_end_rot_yaw<0) m_anim_end_rot_yaw+=360;
    }
    else//update pos as normal
    {
        m_pos_x=pos[0];
        m_pos_y=pos[1];
        m_pos_z=pos[2];
        m_rot_yaw=pos[3];
        m_rot_pitch=pos[4];
    }

    return true;
}

bool powcube::get_pos(float pos[5])//x,y,z,yaw,pitch
{
    pos[0]=m_pos_x;
    pos[1]=m_pos_y;
    pos[2]=m_pos_z;
    pos[3]=m_rot_yaw;
    pos[4]=m_rot_pitch;

    return true;
}

bool powcube::point_inside_cube_fast(float _x,float _y,float _z,float size_factor)
{
    //test only axis locked cube around x,y,z pos
    float cube_size=_cube_size*size_factor;
    if( m_pos_x+cube_size > _x && m_pos_x-cube_size < _x &&
        m_pos_y+cube_size > _y && m_pos_y-cube_size < _y &&
        m_pos_z+cube_size > _z && m_pos_z-cube_size < _z )
    {
        //cout<<"inside at: "<<m_pos_x<<", "<<m_pos_y<<", "<<m_pos_z<<" - size: "<<cube_size<<endl;
        return true;
    }

    return false;
}

int powcube::get_status(void)
{
    return m_state;
}

bool powcube::set_status(int status,int id)
{
    m_state=status;

    m_carried_id=id;

    return true;
}

int powcube::get_carrier_id(void)
{
    return m_carried_id;
}

bool powcube::set_station_id(int station_id)
{
    m_at_station_id=station_id;

    return true;
}

int powcube::get_station_id(void)
{
    return m_at_station_id;
}

bool powcube::set_animation(float start_pos[5],float end_pos[5])
{
    m_anim_progress=0.0;

    m_anim_start_pos_x=start_pos[0];
    m_anim_start_pos_y=start_pos[1];
    m_anim_start_pos_z=start_pos[2];
    m_anim_start_rot_yaw=start_pos[3];
    m_anim_start_rot_pitch=start_pos[4];

    m_anim_end_pos_x=end_pos[0];
    m_anim_end_pos_y=end_pos[1];
    m_anim_end_pos_z=end_pos[2];
    m_anim_end_rot_yaw=end_pos[3];
    m_anim_end_rot_pitch=end_pos[4];

    //calc distance to travel
    float dist=sqrtf( (m_anim_end_pos_x-m_anim_start_pos_x) * (m_anim_end_pos_x-m_anim_start_pos_x) +
                      (m_anim_end_pos_y-m_anim_start_pos_y) * (m_anim_end_pos_y-m_anim_start_pos_y) +
                      (m_anim_end_pos_z-m_anim_start_pos_z) * (m_anim_end_pos_z-m_anim_start_pos_z) );
    if(dist==0) m_anim_speed=1.0;
    else        m_anim_speed=0.3/dist;
    cout<<m_anim_speed<<endl;

    return true;
}

bool powcube::set_animation_from_curr_pos(float end_pos[5])
{
    m_anim_progress=0.0;

    m_anim_start_pos_x=m_pos_x;
    m_anim_start_pos_y=m_pos_y;
    m_anim_start_pos_z=m_pos_z;
    m_anim_start_rot_yaw=m_rot_yaw;
    m_anim_start_rot_pitch=m_rot_pitch;

    m_anim_end_pos_x=end_pos[0];
    m_anim_end_pos_y=end_pos[1];
    m_anim_end_pos_z=end_pos[2];
    m_anim_end_rot_yaw=end_pos[3];
    m_anim_end_rot_pitch=end_pos[4];

    //calc distance to travel
    float dist=sqrtf( (m_anim_end_pos_x-m_anim_start_pos_x) * (m_anim_end_pos_x-m_anim_start_pos_x) +
                      (m_anim_end_pos_y-m_anim_start_pos_y) * (m_anim_end_pos_y-m_anim_start_pos_y) +
                      (m_anim_end_pos_z-m_anim_start_pos_z) * (m_anim_end_pos_z-m_anim_start_pos_z) );
    if(dist==0) m_anim_speed=1.0;
    else        m_anim_speed=0.3/dist;

    //minimise yaw rotations
    while(m_anim_start_rot_yaw>360)  m_anim_start_rot_yaw-=360;
    while(m_anim_start_rot_yaw<0) m_anim_start_rot_yaw+=360;
    while(m_anim_end_rot_yaw>360)  m_anim_end_rot_yaw-=360;
    while(m_anim_end_rot_yaw<0) m_anim_end_rot_yaw+=360;

    //due to 90deg symmetry add n*90 to start yaw, to match end yaw
    float shortest_diff=360;
    float best_adjustment=0;
    for(float adjustment=-270;adjustment<=270;adjustment+=90)
    {
        float diff=fabs(m_anim_end_rot_yaw-m_anim_start_rot_yaw-adjustment);
        if( diff<shortest_diff)
        {
            shortest_diff=diff;
            best_adjustment=adjustment;
        }
    }

    m_anim_start_rot_yaw+=best_adjustment;

    return true;
}

int powcube::get_owner_id(void)
{
    return m_owner_id;
}
