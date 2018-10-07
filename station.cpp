#include "station.h"

station::station()
{
    //ctor
}

station::station(float x_pos,float y_pos,int direction,int owner_id,model_obj* pModel)
{
    m_pos_x=x_pos;
    m_pos_y=y_pos;
    m_direction=direction;
    m_owner_id=owner_id;
    for(int i=0;i<_station_numof_slots;i++) m_slot_taken[i]=-1;
    m_pModel=pModel;
}

bool station::update(void)
{


    return true;
}

bool station::draw(void)
{
    glPushMatrix();
    glTranslatef(-m_pos_x,m_pos_y,0);

    //move into place
    switch(m_direction)
    {
        case 0://y-
        {
            glRotatef(90,0,0,1);
            glTranslatef(_world_wall_thickness_drawn-0.09,0.5,0.5);
            //glTranslatef(0.01,0.5,0.5);
        }break;

        case 1://x+
        {
            glTranslatef(-_world_wall_size+_world_wall_thickness_drawn-0.09,0.5,0.5);
            //glTranslatef(-0.99,0.5,0.5);
        }break;

        case 2://y+
        {
            glRotatef(-90,0,0,1);
            glTranslatef(-_world_wall_size+_world_wall_thickness_drawn-0.09,-0.5,0.5);
            //glTranslatef(-0.99,-0.5,0.5);
        }break;

        case 3://x-
        {
            glRotatef(180,0,0,1);
            glTranslatef(_world_wall_thickness_drawn-0.09,-0.5,0.5);
            //glTranslatef(0.01,-0.5,0.5);
        }break;
    }

    //draw model
    m_pModel->draw();
    //draw player colored stripe
    glEnable(GL_BLEND);
    glColor4f(_player_color[m_owner_id][0],_player_color[m_owner_id][1],_player_color[m_owner_id][2],0.6);
    glBegin(GL_QUADS);
    glVertex3f(0.137193, 0.170066, 0.249610);
    glVertex3f(0.162918, 0.151087, 0.230681);
    glVertex3f(0.162918, -0.150643, 0.230339);
    glVertex3f(0.137193, -0.169664, 0.249224);
    glEnd();
    glColor4f(0.1,0.1,0.1,0.9);
    glLineWidth(3);
    glBegin(GL_LINE_STRIP);
    glVertex3f(0.137193, 0.170066, 0.249610);
    glVertex3f(0.162918, 0.151087, 0.230681);
    glVertex3f(0.162918, -0.150643, 0.230339);
    glVertex3f(0.137193, -0.169664, 0.249224);
    glVertex3f(0.137193, 0.170066, 0.249610);
    glEnd();
    glDisable(GL_BLEND);

    glPopMatrix();

    /*//old drawing
    glColor3f(1,0.5,0.5);
    for(int i=0;i<2;i++)//draw first filled quads, then lines
    {
        switch(m_direction)
        {
            case 0://y-
            {
                glBegin(GL_QUADS);
                glVertex3f(-_station_spaceing,                 _world_wall_thickness_drawn+0.01,   _station_spaceing);
                glVertex3f(-_station_spaceing-_station_size,    _world_wall_thickness_drawn+0.01,   _station_spaceing);
                glVertex3f(-_station_spaceing-_station_size,    _world_wall_thickness_drawn+0.01,   _station_spaceing+_station_size);
                glVertex3f(-_station_spaceing,                 _world_wall_thickness_drawn+0.01,   _station_spaceing+_station_size);

                glVertex3f(-_station_spaceing,                 _world_wall_thickness_drawn+0.01+_station_thickness*0.5,   _station_spaceing);
                glVertex3f(-_station_spaceing-_station_size,    _world_wall_thickness_drawn+0.01+_station_thickness*0.5,   _station_spaceing);
                glVertex3f(-_station_spaceing-_station_size,    _world_wall_thickness_drawn+0.01+_station_thickness*0.5,   _station_spaceing+_station_size);
                glVertex3f(-_station_spaceing,                 _world_wall_thickness_drawn+0.01+_station_thickness*0.5,   _station_spaceing+_station_size);

                glVertex3f(-_station_spaceing,                 _world_wall_thickness_drawn+0.01,                          _station_spaceing);
                glVertex3f(-_station_spaceing,                 _world_wall_thickness_drawn+0.01+_station_thickness*0.5,   _station_spaceing);
                glVertex3f(-_station_spaceing,                 _world_wall_thickness_drawn+0.01+_station_thickness*0.5,   _station_spaceing+_station_size);
                glVertex3f(-_station_spaceing,                 _world_wall_thickness_drawn+0.01,                          _station_spaceing+_station_size);

                glVertex3f(-_station_spaceing-_station_size,   _world_wall_thickness_drawn+0.01,                          _station_spaceing);
                glVertex3f(-_station_spaceing-_station_size,   _world_wall_thickness_drawn+0.01+_station_thickness*0.5,   _station_spaceing);
                glVertex3f(-_station_spaceing-_station_size,   _world_wall_thickness_drawn+0.01+_station_thickness*0.5,   _station_spaceing+_station_size);
                glVertex3f(-_station_spaceing-_station_size,   _world_wall_thickness_drawn+0.01,                          _station_spaceing+_station_size);
                glEnd();
            }break;

            case 1://x+
            {
                glBegin(GL_QUADS);
                glVertex3f(-_world_wall_size+_world_wall_thickness_drawn+0.01,    _station_spaceing,                _station_spaceing);
                glVertex3f(-_world_wall_size+_world_wall_thickness_drawn+0.01,    _station_spaceing+_station_size,   _station_spaceing);
                glVertex3f(-_world_wall_size+_world_wall_thickness_drawn+0.01,    _station_spaceing+_station_size,   _station_spaceing+_station_size);
                glVertex3f(-_world_wall_size+_world_wall_thickness_drawn+0.01,    _station_spaceing,                _station_spaceing+_station_size);

                glVertex3f(-_world_wall_size+_world_wall_thickness_drawn+0.01+_station_thickness*0.5,    _station_spaceing,                _station_spaceing);
                glVertex3f(-_world_wall_size+_world_wall_thickness_drawn+0.01+_station_thickness*0.5,    _station_spaceing+_station_size,   _station_spaceing);
                glVertex3f(-_world_wall_size+_world_wall_thickness_drawn+0.01+_station_thickness*0.5,    _station_spaceing+_station_size,   _station_spaceing+_station_size);
                glVertex3f(-_world_wall_size+_world_wall_thickness_drawn+0.01+_station_thickness*0.5,    _station_spaceing,                _station_spaceing+_station_size);

                glVertex3f(-_world_wall_size+_world_wall_thickness_drawn+0.01,    _station_spaceing,                _station_spaceing);
                glVertex3f(-_world_wall_size+_world_wall_thickness_drawn+0.01+_station_thickness*0.5,    _station_spaceing,   _station_spaceing);
                glVertex3f(-_world_wall_size+_world_wall_thickness_drawn+0.01+_station_thickness*0.5,    _station_spaceing,   _station_spaceing+_station_size);
                glVertex3f(-_world_wall_size+_world_wall_thickness_drawn+0.01,    _station_spaceing,                _station_spaceing+_station_size);

                glVertex3f(-_world_wall_size+_world_wall_thickness_drawn+0.01,                           _station_spaceing+_station_size,                _station_spaceing);
                glVertex3f(-_world_wall_size+_world_wall_thickness_drawn+0.01+_station_thickness*0.5,    _station_spaceing+_station_size,   _station_spaceing);
                glVertex3f(-_world_wall_size+_world_wall_thickness_drawn+0.01+_station_thickness*0.5,    _station_spaceing+_station_size,   _station_spaceing+_station_size);
                glVertex3f(-_world_wall_size+_world_wall_thickness_drawn+0.01,                           _station_spaceing+_station_size,                _station_spaceing+_station_size);
                glEnd();
            }break;

            case 2://y+
            {
                glBegin(GL_QUADS);
                glVertex3f(-_station_spaceing,                 _world_wall_size-_world_wall_thickness_drawn-0.01,   _station_spaceing);
                glVertex3f(-_station_spaceing-_station_size,    _world_wall_size-_world_wall_thickness_drawn-0.01,   _station_spaceing);
                glVertex3f(-_station_spaceing-_station_size,    _world_wall_size-_world_wall_thickness_drawn-0.01,   _station_spaceing+_station_size);
                glVertex3f(-_station_spaceing,                 _world_wall_size-_world_wall_thickness_drawn-0.01,   _station_spaceing+_station_size);

                glVertex3f(-_station_spaceing,                 _world_wall_size-_world_wall_thickness_drawn-0.01-_station_thickness*0.5,   _station_spaceing);
                glVertex3f(-_station_spaceing-_station_size,    _world_wall_size-_world_wall_thickness_drawn-0.01-_station_thickness*0.5,   _station_spaceing);
                glVertex3f(-_station_spaceing-_station_size,    _world_wall_size-_world_wall_thickness_drawn-0.01-_station_thickness*0.5,   _station_spaceing+_station_size);
                glVertex3f(-_station_spaceing,                 _world_wall_size-_world_wall_thickness_drawn-0.01-_station_thickness*0.5,   _station_spaceing+_station_size);

                glVertex3f(-_station_spaceing,                 _world_wall_size-_world_wall_thickness_drawn-0.01,                          _station_spaceing);
                glVertex3f(-_station_spaceing,                 _world_wall_size-_world_wall_thickness_drawn-0.01-_station_thickness*0.5,   _station_spaceing);
                glVertex3f(-_station_spaceing,                 _world_wall_size-_world_wall_thickness_drawn-0.01-_station_thickness*0.5,   _station_spaceing+_station_size);
                glVertex3f(-_station_spaceing,                 _world_wall_size-_world_wall_thickness_drawn-0.01,                          _station_spaceing+_station_size);

                glVertex3f(-_station_spaceing-_station_size,   _world_wall_size-_world_wall_thickness_drawn-0.01,                          _station_spaceing);
                glVertex3f(-_station_spaceing-_station_size,   _world_wall_size-_world_wall_thickness_drawn-0.01-_station_thickness*0.5,   _station_spaceing);
                glVertex3f(-_station_spaceing-_station_size,   _world_wall_size-_world_wall_thickness_drawn-0.01-_station_thickness*0.5,   _station_spaceing+_station_size);
                glVertex3f(-_station_spaceing-_station_size,   _world_wall_size-_world_wall_thickness_drawn-0.01,                          _station_spaceing+_station_size);
                glEnd();
            }break;

            case 3://x-
            {
                glBegin(GL_QUADS);
                glVertex3f(-_world_wall_thickness_drawn-0.01,    _station_spaceing,                _station_spaceing);
                glVertex3f(-_world_wall_thickness_drawn-0.01,    _station_spaceing+_station_size,   _station_spaceing);
                glVertex3f(-_world_wall_thickness_drawn-0.01,    _station_spaceing+_station_size,   _station_spaceing+_station_size);
                glVertex3f(-_world_wall_thickness_drawn-0.01,    _station_spaceing,                _station_spaceing+_station_size);

                glVertex3f(-_world_wall_thickness_drawn-0.01-_station_thickness*0.5,    _station_spaceing,                _station_spaceing);
                glVertex3f(-_world_wall_thickness_drawn-0.01-_station_thickness*0.5,    _station_spaceing+_station_size,   _station_spaceing);
                glVertex3f(-_world_wall_thickness_drawn-0.01-_station_thickness*0.5,    _station_spaceing+_station_size,   _station_spaceing+_station_size);
                glVertex3f(-_world_wall_thickness_drawn-0.01-_station_thickness*0.5,    _station_spaceing,                _station_spaceing+_station_size);

                glVertex3f(-_world_wall_thickness_drawn-0.01,                           _station_spaceing,                _station_spaceing);
                glVertex3f(-_world_wall_thickness_drawn-0.01-_station_thickness*0.5,    _station_spaceing,                _station_spaceing);
                glVertex3f(-_world_wall_thickness_drawn-0.01-_station_thickness*0.5,    _station_spaceing,                _station_spaceing+_station_size);
                glVertex3f(-_world_wall_thickness_drawn-0.01,                           _station_spaceing,                _station_spaceing+_station_size);

                glVertex3f(-_world_wall_thickness_drawn-0.01,                           _station_spaceing+_station_size,  _station_spaceing);
                glVertex3f(-_world_wall_thickness_drawn-0.01-_station_thickness*0.5,    _station_spaceing+_station_size,  _station_spaceing);
                glVertex3f(-_world_wall_thickness_drawn-0.01-_station_thickness*0.5,    _station_spaceing+_station_size,  _station_spaceing+_station_size);
                glVertex3f(-_world_wall_thickness_drawn-0.01,                           _station_spaceing+_station_size,  _station_spaceing+_station_size);
                glEnd();
            }break;
        }
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glColor3f(0.9,0.9,0.9);
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glPopMatrix();*/

    return true;
}

bool station::cube_place()
{

    return true;
}

bool station::in_station_pos(int x_pos,int y_pos)
{
    if( (int)m_pos_x==x_pos && (int)m_pos_y==y_pos ) return true;

    return false;
}

bool station::point_inside_station(float _x,float _y,float _z)
{
    switch(m_direction)
    {
        case 0://y-
        {
            float station_size_x=_station_size*0.6;
            float station_size_y=_station_thickness*0.5;
            float station_size_z=_station_size*0.6;
            float station_offset_x=_world_wall_size*0.5;
            float station_offset_y=_world_wall_thickness_drawn+_station_thickness*0.5;
            float station_offset_z=_world_wall_size*0.5;
            float pos_z=0.0;

            if( m_pos_x+station_offset_x+station_size_x > _x && m_pos_x+station_offset_x-station_size_x < _x &&
                m_pos_y+station_offset_y+station_size_y > _y && m_pos_y+station_offset_y-station_size_y < _y &&
                pos_z+station_offset_z+station_size_z > _z && pos_z+station_offset_z-station_size_z < _z )
            {
                return true;
            }
            else return false;

        }break;

        case 1://x+
        {
            float station_size_x=_station_thickness*0.5;
            float station_size_y=_station_size*0.6;
            float station_size_z=_station_size*0.6;
            float station_offset_x=_world_wall_size-_world_wall_thickness_drawn-_station_thickness*0.5;
            float station_offset_y=_world_wall_size*0.5;
            float station_offset_z=_world_wall_size*0.5;
            float pos_z=0.0;

            if( m_pos_x+station_offset_x+station_size_x > _x && m_pos_x+station_offset_x-station_size_x < _x &&
                m_pos_y+station_offset_y+station_size_y > _y && m_pos_y+station_offset_y-station_size_y < _y &&
                pos_z+station_offset_z+station_size_z > _z && pos_z+station_offset_z-station_size_z < _z )
            {
                return true;
            }
            else return false;

        }break;

        case 2://y+
        {
            float station_size_x=_station_size*0.6;
            float station_size_y=_station_thickness*0.5;
            float station_size_z=_station_size*0.6;
            float station_offset_x=_world_wall_size*0.5;
            float station_offset_y=_world_wall_size-_world_wall_thickness_drawn-_station_thickness*0.5;
            float station_offset_z=_world_wall_size*0.5;
            float pos_z=0.0;

            if( m_pos_x+station_offset_x+station_size_x > _x && m_pos_x+station_offset_x-station_size_x < _x &&
                m_pos_y+station_offset_y+station_size_y > _y && m_pos_y+station_offset_y-station_size_y < _y &&
                pos_z+station_offset_z+station_size_z > _z && pos_z+station_offset_z-station_size_z < _z )
            {
                return true;
            }
            else return false;

        }break;

        case 3://x-
        {
            float station_size_x=_station_thickness*0.5;
            float station_size_y=_station_size*0.6;
            float station_size_z=_station_size*0.6;
            float station_offset_x=_world_wall_thickness_drawn+_station_thickness*0.5;
            float station_offset_y=_world_wall_size*0.5;
            float station_offset_z=_world_wall_size*0.5;
            float pos_z=0.0;

            if( m_pos_x+station_offset_x+station_size_x > _x && m_pos_x+station_offset_x-station_size_x < _x &&
                m_pos_y+station_offset_y+station_size_y > _y && m_pos_y+station_offset_y-station_size_y < _y &&
                pos_z+station_offset_z+station_size_z > _z && pos_z+station_offset_z-station_size_z < _z )
            {
                return true;
            }
            else return false;

        }break;
    }

    return false;
}

bool station::add_cube_to_station(int cube_id,float cube_pos_final[5],float test_point_v[3])
{
    //cout<<"added cube id: "<<cube_id<<endl;

    //calc slot ind that is closest to the test point
    int prefered_slot_ind=0;
    float limit_low=0.4375;
    float limit_high=0.5625;
    //make test_point_v relative to station pos
    test_point_v[0]-=m_pos_x;
    test_point_v[1]-=m_pos_y;
    switch(m_direction)
    {
        case 0://y-
        {
            int prefered_x=1;
            //int prefered_y=1;
            int prefered_z=1;
            //z is same for all
            if(test_point_v[2]<limit_low) prefered_z=2;
            else if(test_point_v[2]>limit_high) prefered_z=0;
            //x
            if(test_point_v[0]<limit_low) prefered_x=0;
            else if(test_point_v[0]>limit_high) prefered_x=2;

            prefered_slot_ind=prefered_z*3+prefered_x;
        }break;

        case 1://x+
        {
            //int prefered_x=1;
            int prefered_y=1;
            int prefered_z=1;
            //z is same for all
            if(test_point_v[2]<limit_low) prefered_z=2;
            else if(test_point_v[2]>limit_high) prefered_z=0;
            //y
            if(test_point_v[1]<limit_low) prefered_y=0;
            else if(test_point_v[1]>limit_high) prefered_y=2;

            prefered_slot_ind=prefered_z*3+prefered_y;
        }break;

        case 2://y+
        {
            int prefered_x=1;
            //int prefered_y=1;
            int prefered_z=1;
            //z is same for all
            if(test_point_v[2]<limit_low) prefered_z=2;
            else if(test_point_v[2]>limit_high) prefered_z=0;
            //x
            if(test_point_v[0]>limit_high) prefered_x=0;
            else if(test_point_v[0]<limit_low) prefered_x=2;

            prefered_slot_ind=prefered_z*3+prefered_x;
        }break;

        case 3://x-
        {
            //int prefered_x=1;
            int prefered_y=1;
            int prefered_z=1;
            //z is same for all
            if(test_point_v[2]<limit_low) prefered_z=2;
            else if(test_point_v[2]>limit_high) prefered_z=0;
            //y
            if(test_point_v[1]>limit_high) prefered_y=0;
            else if(test_point_v[1]<limit_low) prefered_y=2;

            prefered_slot_ind=prefered_z*3+prefered_y;
        }break;
    }
    //cout<<"prefered station slot is: "<<prefered_slot_ind<<endl;

    //test if prefered slot is free
    int free_slot_ind=-1;
    if(m_slot_taken[prefered_slot_ind]!=-1)
    {
        //cout<<"but that slot was taken, finding another slot\n";
        //find first empty slot instead
        for(int slot_i=0;slot_i<_station_numof_slots;slot_i++)
        {
            if(m_slot_taken[slot_i]==-1)
            {
                free_slot_ind=slot_i;
                break;
            }
        }
        if(free_slot_ind==-1) return false;//no free slots
    }
    else free_slot_ind=prefered_slot_ind;

    //cout<<"free slot no: "<<free_slot_ind<<endl;

    //calc pos for free slot
    cube_pos_final[3]=0.0;//yaw
    cube_pos_final[4]=0.0;//pitch
    float center_pos[3];//x,y,z
    switch(m_direction)
    {
        case 0://y-
        {
            center_pos[0]=m_pos_x+_station_spaceing+_station_size*0.5;
            center_pos[1]=m_pos_y+_world_wall_thickness_drawn+_station_thickness;
            center_pos[2]=_station_spaceing+_station_size*0.5;
            //slot pos adjustment
            switch(free_slot_ind)
            {
                case 0://tl
                {
                    cube_pos_final[0]=center_pos[0]-_cube_size*2.0-_cube_spacing;
                    cube_pos_final[1]=center_pos[1];
                    cube_pos_final[2]=center_pos[2]+_cube_size*2.0+_cube_spacing;
                }break;

                case 1://tm
                {
                    cube_pos_final[0]=center_pos[0];
                    cube_pos_final[1]=center_pos[1];
                    cube_pos_final[2]=center_pos[2]+_cube_size*2.0+_cube_spacing;
                }break;

                case 2://tr
                {
                    cube_pos_final[0]=center_pos[0]+_cube_size*2.0+_cube_spacing;
                    cube_pos_final[1]=center_pos[1];
                    cube_pos_final[2]=center_pos[2]+_cube_size*2.0+_cube_spacing;
                }break;

                case 3://ml
                {
                    cube_pos_final[0]=center_pos[0]-_cube_size*2.0-_cube_spacing;
                    cube_pos_final[1]=center_pos[1];
                    cube_pos_final[2]=center_pos[2];
                }break;

                case 4://mm
                {
                    cube_pos_final[0]=center_pos[0];
                    cube_pos_final[1]=center_pos[1];
                    cube_pos_final[2]=center_pos[2];
                }break;

                case 5://mr
                {
                    cube_pos_final[0]=center_pos[0]+_cube_size*2.0+_cube_spacing;
                    cube_pos_final[1]=center_pos[1];
                    cube_pos_final[2]=center_pos[2];
                }break;

                case 6://dl
                {
                    cube_pos_final[0]=center_pos[0]-_cube_size*2.0-_cube_spacing;
                    cube_pos_final[1]=center_pos[1];
                    cube_pos_final[2]=center_pos[2]-_cube_size*2.0-_cube_spacing;
                }break;

                case 7://dm
                {
                    cube_pos_final[0]=center_pos[0];
                    cube_pos_final[1]=center_pos[1];
                    cube_pos_final[2]=center_pos[2]-_cube_size*2.0-_cube_spacing;
                }break;

                case 8://dr
                {
                    cube_pos_final[0]=center_pos[0]+_cube_size*2.0+_cube_spacing;
                    cube_pos_final[1]=center_pos[1];
                    cube_pos_final[2]=center_pos[2]-_cube_size*2.0-_cube_spacing;
                }break;
            }
        }break;

        case 1://x+
        {
            center_pos[0]=m_pos_x+_world_wall_size-_world_wall_thickness_drawn-_station_thickness;
            center_pos[1]=m_pos_y+_station_spaceing+_station_size*0.5;
            center_pos[2]=_station_spaceing+_station_size*0.5;
            //slot pos adjustment
            switch(free_slot_ind)
            {
                case 0://tl
                {
                    cube_pos_final[0]=center_pos[0];
                    cube_pos_final[1]=center_pos[1]-_cube_size*2.0-_cube_spacing;
                    cube_pos_final[2]=center_pos[2]+_cube_size*2.0+_cube_spacing;
                }break;

                case 1://tm
                {
                    cube_pos_final[0]=center_pos[0];
                    cube_pos_final[1]=center_pos[1];
                    cube_pos_final[2]=center_pos[2]+_cube_size*2.0+_cube_spacing;
                }break;

                case 2://tr
                {
                    cube_pos_final[0]=center_pos[0];
                    cube_pos_final[1]=center_pos[1]+_cube_size*2.0+_cube_spacing;
                    cube_pos_final[2]=center_pos[2]+_cube_size*2.0+_cube_spacing;
                }break;

                case 3://ml
                {
                    cube_pos_final[0]=center_pos[0];
                    cube_pos_final[1]=center_pos[1]-_cube_size*2.0-_cube_spacing;
                    cube_pos_final[2]=center_pos[2];
                }break;

                case 4://mm
                {
                    cube_pos_final[0]=center_pos[0];
                    cube_pos_final[1]=center_pos[1];
                    cube_pos_final[2]=center_pos[2];
                }break;

                case 5://mr
                {
                    cube_pos_final[0]=center_pos[0];
                    cube_pos_final[1]=center_pos[1]+_cube_size*2.0+_cube_spacing;
                    cube_pos_final[2]=center_pos[2];
                }break;

                case 6://dl
                {
                    cube_pos_final[0]=center_pos[0];
                    cube_pos_final[1]=center_pos[1]-_cube_size*2.0-_cube_spacing;
                    cube_pos_final[2]=center_pos[2]-_cube_size*2.0-_cube_spacing;
                }break;

                case 7://dm
                {
                    cube_pos_final[0]=center_pos[0];
                    cube_pos_final[1]=center_pos[1];
                    cube_pos_final[2]=center_pos[2]-_cube_size*2.0-_cube_spacing;
                }break;

                case 8://dr
                {
                    cube_pos_final[0]=center_pos[0];
                    cube_pos_final[1]=center_pos[1]+_cube_size*2.0+_cube_spacing;
                    cube_pos_final[2]=center_pos[2]-_cube_size*2.0-_cube_spacing;
                }break;
            }
        }break;

        case 2://y+
        {
            center_pos[0]=m_pos_x+_station_spaceing+_station_size*0.5;
            center_pos[1]=m_pos_y+_world_wall_size-_world_wall_thickness_drawn-_station_thickness;
            center_pos[2]=_station_spaceing+_station_size*0.5;
            //slot pos adjustment
            switch(free_slot_ind)
            {
                case 0://tl
                {
                    cube_pos_final[0]=center_pos[0]+_cube_size*2.0+_cube_spacing;
                    cube_pos_final[1]=center_pos[1];
                    cube_pos_final[2]=center_pos[2]+_cube_size*2.0+_cube_spacing;
                }break;

                case 1://tm
                {
                    cube_pos_final[0]=center_pos[0];
                    cube_pos_final[1]=center_pos[1];
                    cube_pos_final[2]=center_pos[2]+_cube_size*2.0+_cube_spacing;
                }break;

                case 2://tr
                {
                    cube_pos_final[0]=center_pos[0]-_cube_size*2.0-_cube_spacing;
                    cube_pos_final[1]=center_pos[1];
                    cube_pos_final[2]=center_pos[2]+_cube_size*2.0+_cube_spacing;
                }break;

                case 3://ml
                {
                    cube_pos_final[0]=center_pos[0]+_cube_size*2.0+_cube_spacing;
                    cube_pos_final[1]=center_pos[1];
                    cube_pos_final[2]=center_pos[2];
                }break;

                case 4://mm
                {
                    cube_pos_final[0]=center_pos[0];
                    cube_pos_final[1]=center_pos[1];
                    cube_pos_final[2]=center_pos[2];
                }break;

                case 5://mr
                {
                    cube_pos_final[0]=center_pos[0]-_cube_size*2.0-_cube_spacing;
                    cube_pos_final[1]=center_pos[1];
                    cube_pos_final[2]=center_pos[2];
                }break;

                case 6://dl
                {
                    cube_pos_final[0]=center_pos[0]+_cube_size*2.0+_cube_spacing;
                    cube_pos_final[1]=center_pos[1];
                    cube_pos_final[2]=center_pos[2]-_cube_size*2.0-_cube_spacing;
                }break;

                case 7://dm
                {
                    cube_pos_final[0]=center_pos[0];
                    cube_pos_final[1]=center_pos[1];
                    cube_pos_final[2]=center_pos[2]-_cube_size*2.0-_cube_spacing;
                }break;

                case 8://dr
                {
                    cube_pos_final[0]=center_pos[0]-_cube_size*2.0-_cube_spacing;
                    cube_pos_final[1]=center_pos[1];
                    cube_pos_final[2]=center_pos[2]-_cube_size*2.0-_cube_spacing;
                }break;
            }
        }break;

        case 3://x-
        {
            center_pos[0]=m_pos_x+_world_wall_thickness_drawn+_station_thickness;
            center_pos[1]=m_pos_y+_station_spaceing+_station_size*0.5;
            center_pos[2]=_station_spaceing+_station_size*0.5;
            //slot pos adjustment
            switch(free_slot_ind)
            {
                case 0://tl
                {
                    cube_pos_final[0]=center_pos[0];
                    cube_pos_final[1]=center_pos[1]+_cube_size*2.0+_cube_spacing;
                    cube_pos_final[2]=center_pos[2]+_cube_size*2.0+_cube_spacing;
                }break;

                case 1://tm
                {
                    cube_pos_final[0]=center_pos[0];
                    cube_pos_final[1]=center_pos[1];
                    cube_pos_final[2]=center_pos[2]+_cube_size*2.0+_cube_spacing;
                }break;

                case 2://tr
                {
                    cube_pos_final[0]=center_pos[0];
                    cube_pos_final[1]=center_pos[1]-_cube_size*2.0-_cube_spacing;
                    cube_pos_final[2]=center_pos[2]+_cube_size*2.0+_cube_spacing;
                }break;

                case 3://ml
                {
                    cube_pos_final[0]=center_pos[0];
                    cube_pos_final[1]=center_pos[1]+_cube_size*2.0+_cube_spacing;
                    cube_pos_final[2]=center_pos[2];
                }break;

                case 4://mm
                {
                    cube_pos_final[0]=center_pos[0];
                    cube_pos_final[1]=center_pos[1];
                    cube_pos_final[2]=center_pos[2];
                }break;

                case 5://mr
                {
                    cube_pos_final[0]=center_pos[0];
                    cube_pos_final[1]=center_pos[1]-_cube_size*2.0-_cube_spacing;
                    cube_pos_final[2]=center_pos[2];
                }break;

                case 6://dl
                {
                    cube_pos_final[0]=center_pos[0];
                    cube_pos_final[1]=center_pos[1]+_cube_size*2.0+_cube_spacing;
                    cube_pos_final[2]=center_pos[2]-_cube_size*2.0-_cube_spacing;
                }break;

                case 7://dm
                {
                    cube_pos_final[0]=center_pos[0];
                    cube_pos_final[1]=center_pos[1];
                    cube_pos_final[2]=center_pos[2]-_cube_size*2.0-_cube_spacing;
                }break;

                case 8://dr
                {
                    cube_pos_final[0]=center_pos[0];
                    cube_pos_final[1]=center_pos[1]-_cube_size*2.0-_cube_spacing;
                    cube_pos_final[2]=center_pos[2]-_cube_size*2.0-_cube_spacing;
                }break;
            }
        }break;
    }

    //store id
    m_vec_contained_powcubes_id.push_back(cube_id);
    //reserve slot
    m_slot_taken[free_slot_ind]=cube_id;

    //cout<<"Final pos: "<<cube_pos_final[0]<<", "<<cube_pos_final[1]<<", "<<cube_pos_final[2]<<endl;

    return true;
}

bool station::remove_cube_from_station(int cube_id)
{
    bool cube_found=false;
    //remove from vector
    for(int cube_i=0;cube_i<(int)m_vec_contained_powcubes_id.size();cube_i++)
    {
        if(m_vec_contained_powcubes_id[cube_i]==cube_id)
        {
            m_vec_contained_powcubes_id.erase(m_vec_contained_powcubes_id.begin()+cube_i);
            cube_found=true;
        }
    }

    if(!cube_found)
    {
        cout<<"ERROR: Could not find cube: "<<cube_id<<" in this station vector\n";
    }

    bool cube_found2=false;
    //remove from grid
    for(int slot_i=0;slot_i<_station_numof_slots;slot_i++)
    {
        if(m_slot_taken[slot_i]==cube_id)
        {
            m_slot_taken[slot_i]=-1;
            cube_found2=true;
        }
    }

    if(!cube_found2)
    {
        cout<<"ERROR: Could not find cube: "<<cube_id<<" in this station grid\n";
    }

    if(!cube_found || !cube_found2) return false;//could not find id

    return true;
}

bool station::get_list_of_cube_in_station(vector<int>& vec_cube_list)
{
    if(m_vec_contained_powcubes_id.empty()) return false;//contains nothing

    vec_cube_list=m_vec_contained_powcubes_id;

    return true;
}

