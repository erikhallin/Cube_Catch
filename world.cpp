#include "world.h"

world::world()
{
    //ctor
}

bool world::init(int numof_players,vector<st_pos_int>& vec_station_pos)
{
    m_world_size=_world_size;

    //gen walls
    srand(time(0));

    //temp
    /*for(int x=0;x<_world_size;x++)
    for(int y=0;y<_world_size;y++)
    {
        m_arr_wall[x][y]=0;
    }
    m_arr_wall[0][0]=3;
    m_arr_wall[2][0]=3;*/

    while(true)//rerun if to small connected area
    {
        //random walls
        for(int x=0;x<_world_size;x++)
        for(int y=0;y<_world_size;y++)
        {
            m_arr_wall[x][y]=rand()%4;
            if(m_arr_wall[x][y]!=0) m_arr_wall[x][y]=rand()%4;//extra space
            if(m_arr_wall[x][y]!=0) m_arr_wall[x][y]=rand()%4;//more extra space
        }

        /*//temp
        m_arr_wall[3][2]=0;
        m_arr_wall[3][4]=1;
        m_arr_wall[3][6]=2;
        m_arr_wall[3][8]=3;*/

        //close world edges near x=0 and y=0 with walls
        for(int x=1;x<_world_size;x++)
        {
            switch(m_arr_wall[x][0])
            {
                case 0: m_arr_wall[x][0]=1; break;
                //case 1: m_arr_wall[x][0]=1; break;
                case 2: m_arr_wall[x][0]=3; break;
                //case 3: m_arr_wall[x][0]=3; break;
            }
        }
        for(int y=1;y<_world_size;y++)
        {
            switch(m_arr_wall[0][y])
            {
                case 0: m_arr_wall[0][y]=2; break;
                case 1: m_arr_wall[0][y]=3; break;
                //case 2: m_arr_wall[0][y]=2; break;
                //case 3: m_arr_wall[0][y]=3; break;
            }
        }
        m_arr_wall[0][0]=3;

        //find connected area to find suitable player positions
        int connected_pos[_world_size][_world_size];//0=not connected, 1=connected and not tested, 2=connected and tested
        for(int x=0;x<_world_size;x++)
        for(int y=0;y<_world_size;y++)
        {
            connected_pos[x][y]=0;
        }
        //start at random pos
        int x_pos_curr=rand()%_world_size;
        int y_pos_curr=rand()%_world_size;
        connected_pos[x_pos_curr][y_pos_curr]=1;
        //take steps to test if wall in the way
        while(true)
        {
            bool no_more_tests=true;
            //go through all untested positions
            for(int x=0;x<_world_size;x++)
            for(int y=0;y<_world_size;y++)
            {
                if( connected_pos[x][y]==1 )//test expansions
                {
                    no_more_tests=false;

                    //x+
                    for(int x_add=1;x+x_add<_world_size;x_add++)
                    {
                        //break if wall
                        if( m_arr_wall[x+x_add][y]==2 || m_arr_wall[x+x_add][y]==3 ) break;//wall collision

                        if( connected_pos[x+x_add][y]==0 ) connected_pos[x+x_add][y]=1;
                    }

                    //x-
                    for(int x_add=-1;x+x_add>=0;x_add--)
                    {
                        //break if wall
                        if( m_arr_wall[x+x_add+1][y]==2 || m_arr_wall[x+x_add+1][y]==3 ) break;//wall collision

                        if( connected_pos[x+x_add][y]==0 ) connected_pos[x+x_add][y]=1;
                    }

                    //y+
                    for(int y_add=1;y+y_add<_world_size;y_add++)
                    {
                        //break if wall
                        if( m_arr_wall[x][y+y_add]==1 || m_arr_wall[x][y+y_add]==3 ) break;//wall collision

                        if( connected_pos[x][y+y_add]==0 ) connected_pos[x][y+y_add]=1;
                    }

                    //y-
                    for(int y_add=-1;y+y_add>=0;y_add--)
                    {
                        //break if wall
                        if( m_arr_wall[x][y+y_add+1]==1 || m_arr_wall[x][y+y_add+1]==3 ) break;//wall collision

                        if( connected_pos[x][y+y_add]==0 ) connected_pos[x][y+y_add]=1;
                    }

                    //set current pos to tested
                    connected_pos[x][y]=2;
                }
            }

            if(no_more_tests) break;
        }

        //test if done, count connected area size
        int area_counter=0;
        for(int x=0;x<_world_size;x++)
        for(int y=0;y<_world_size;y++)
        {
            if( connected_pos[x][y]==2 ) area_counter++;
        }
        //connected area have to be bigger than half of maximum area
        if( area_counter>_world_size*_world_size*0.5 )
        {
            //store connected area
            for(int x=0;x<_world_size;x++)
            for(int y=0;y<_world_size;y++)
            {
                m_arr_connected_pos[x][y]=connected_pos[x][y];
            }

            //place stations in connected area
            vector<st_pos_int> vec_station_pos_prel;
            int allowed_distance=2;
            if(numof_players==2) allowed_distance=7;
            if(numof_players==3) allowed_distance=5;
            bool new_walls_request=false;
            for(int player_i=0;player_i<numof_players;player_i++)
            {
                //get random pos inside connected area, with nearby wall
                int try_counter=0;
                int max_tries=100;

                while(true)//rerun if pos not ok
                {
                    if(try_counter>max_tries)
                    {
                        new_walls_request=true;
                        break;
                    }
                    try_counter++;

                    int x_pos=rand()%_world_size;
                    int y_pos=rand()%_world_size;

                    //distance to others test
                    bool to_close=false;
                    for(int station_i=0;station_i<(int)vec_station_pos_prel.size();station_i++)
                    {
                        if( fabs(vec_station_pos_prel[station_i].x-x_pos)<=allowed_distance &&
                            fabs(vec_station_pos_prel[station_i].y-y_pos)<=allowed_distance )
                        {
                            to_close=true;
                            break;
                        }
                    }
                    if(to_close) continue;//new values

                    //area test
                    if(m_arr_connected_pos[x_pos][y_pos]!=2) continue;//new values

                    //wall test
                    int station_direction=-1;
                    if(x_pos==0 && station_direction==-1)
                    {
                        station_direction=3;
                    }
                    if(y_pos==0 && station_direction==-1)
                    {
                        station_direction=0;
                    }
                    if(x_pos==_world_size-1 && station_direction==-1)
                    {
                        station_direction=1;
                    }
                    if(y_pos==_world_size-1 && station_direction==-1)
                    {
                        station_direction=2;
                    }
                    if(m_arr_wall[x_pos][y_pos]==1 && station_direction==-1)
                    {
                        station_direction=0;
                    }
                    if(m_arr_wall[x_pos][y_pos]==2 && station_direction==-1)
                    {
                        station_direction=3;
                    }
                    if(m_arr_wall[x_pos][y_pos]==3 && station_direction==-1)
                    {
                        int coin=rand()%2;
                        if(coin==0) station_direction=0;
                        else        station_direction=3;
                    }
                    if( (m_arr_wall[x_pos+1][y_pos]==2 || m_arr_wall[x_pos+1][y_pos]==3 ) && station_direction==-1)
                    {
                        station_direction=1;
                    }
                    if( (m_arr_wall[x_pos][y_pos+1]==1 || m_arr_wall[x_pos+1][y_pos]==3 ) && station_direction==-1)
                    {
                        station_direction=2;
                    }
                    if(station_direction==-1) continue;//new values

                    //pos ok, store
                    vec_station_pos_prel.push_back( st_pos_int(x_pos,y_pos,station_direction) );

                    break;//next player
                }

                if(new_walls_request) break;//stop trying to find station pos
            }

            if(new_walls_request) continue;//gen new walls

            //store station pos
            vec_station_pos=vec_station_pos_prel;

            break;
        }
    }

    /*//path gen, removes too much
    for(int x=0;x<_world_size;x++)
    for(int y=0;y<_world_size;y++)
    {
        m_arr_wall[x][y]=3;
    }
    //take random steps, removing walls until selected spots are connected

    //player positions that have to be connected
    vector<st_pos_int> vec_connected_pos;//temp, the four corners
    vec_connected_pos.push_back( st_pos_int(0,0) );
    vec_connected_pos.push_back( st_pos_int(_world_size-1,0) );
    vec_connected_pos.push_back( st_pos_int(_world_size-1,_world_size-1) );
    vec_connected_pos.push_back( st_pos_int(0,_world_size-1) );
    int x_pos_curr=vec_connected_pos[0].x;
    int y_pos_curr=vec_connected_pos[0].y;

    while(true)//generate path
    {
        //test if new pos reached
        for(int pos_ind=0;pos_ind<(int)vec_connected_pos.size();pos_ind++)
        {
            if( !vec_connected_pos[pos_ind].flag )
            {
                if( vec_connected_pos[pos_ind].x==x_pos_curr &&
                    vec_connected_pos[pos_ind].y==y_pos_curr )
                {
                    vec_connected_pos[pos_ind].flag=true;//position reached
                }
            }
        }

        //test if done
        bool done=true;
        for(int pos_ind=0;pos_ind<(int)vec_connected_pos.size();pos_ind++)
        {
            if( !vec_connected_pos[pos_ind].flag )
            {
                done=false;
                break;
            }
        }
        if(done) break;//done

        //make step
        int step_dir=rand()%4;//0=x+1, 1=y+1, 2=x-1, 3=y-1
        switch(step_dir)
        {
            case 0://x+1
            {
                //test limits
                if(x_pos_curr+1>=_world_size) break;//do not make step
                //else remove walls
                switch(m_arr_wall[x_pos_curr+1][y_pos_curr])
                {
                    case 0: m_arr_wall[x_pos_curr+1][y_pos_curr]=0; break;
                    case 1: m_arr_wall[x_pos_curr+1][y_pos_curr]=1; break;
                    case 2: m_arr_wall[x_pos_curr+1][y_pos_curr]=0; break;
                    case 3: m_arr_wall[x_pos_curr+1][y_pos_curr]=1; break;
                }
                //update pos
                x_pos_curr+=1;
            }break;

            case 1://y+1
            {
                //test limits
                if(y_pos_curr+1>=_world_size) break;//do not make step
                //else remove walls
                switch(m_arr_wall[x_pos_curr][y_pos_curr+1])
                {
                    case 0: m_arr_wall[x_pos_curr][y_pos_curr+1]=0; break;
                    case 1: m_arr_wall[x_pos_curr][y_pos_curr+1]=0; break;
                    case 2: m_arr_wall[x_pos_curr][y_pos_curr+1]=2; break;
                    case 3: m_arr_wall[x_pos_curr][y_pos_curr+1]=2; break;
                }
                //update pos
                y_pos_curr+=1;
            }break;

            case 2://x-1
            {
                //test limits
                if(x_pos_curr-1<0) break;//do not make step
                //else remove walls
                switch(m_arr_wall[x_pos_curr][y_pos_curr])
                {
                    case 0: m_arr_wall[x_pos_curr][y_pos_curr]=0; break;
                    case 1: m_arr_wall[x_pos_curr][y_pos_curr]=1; break;
                    case 2: m_arr_wall[x_pos_curr][y_pos_curr]=0; break;
                    case 3: m_arr_wall[x_pos_curr][y_pos_curr]=1; break;
                }
                //update pos
                x_pos_curr-=1;
            }break;

            case 3://y-1
            {
                //test limits
                if(y_pos_curr-1<0) break;//do not make step
                //else remove walls
                switch(m_arr_wall[x_pos_curr][y_pos_curr])
                {
                    case 0: m_arr_wall[x_pos_curr][y_pos_curr]=0; break;
                    case 1: m_arr_wall[x_pos_curr][y_pos_curr]=0; break;
                    case 2: m_arr_wall[x_pos_curr][y_pos_curr]=2; break;
                    case 3: m_arr_wall[x_pos_curr][y_pos_curr]=2; break;
                }
                //update pos
                y_pos_curr-=1;
            }break;
        }
    }*/

    return true;
}

bool world::update(void)
{


    return true;
}

bool world::draw(void)
{
    //draw floor TEMP
    glBegin(GL_QUADS);
    glColor3f(0.3,0.3,0.3);
    glVertex3f(0,0,-0.01);
    //glColor3f(1,0,0);
    glVertex3f(0,m_world_size,-0.01);
    //glColor3f(0,1,0);
    glVertex3f(-m_world_size,m_world_size,-0.01);
    //glColor3f(0,0,1);
    glVertex3f(-m_world_size,0,-0.01);
    glEnd();

    //draw extra walls for the edge
    //float color_low[3]={0.4,0.4,0.4};
    //float color_high[3]={0.7,0.7,0.7};
    float color_low[3]={0.2,0.2,0.2};
    float color_high[3]={0.4,0.4,0.4};
    glBegin(GL_QUADS);
    for(int i=0;i<_world_size;i++)
    {
        glColor3fv(color_high);
        glVertex3f(-i,                  m_world_size-_world_wall_thickness_drawn,   _world_height);
        glColor3fv(color_low);
        glVertex3f(-i,                  m_world_size-_world_wall_thickness_drawn,   0);
        glVertex3f(-i-_world_wall_size, m_world_size-_world_wall_thickness_drawn,   0);
        glColor3fv(color_high);
        glVertex3f(-i-_world_wall_size, m_world_size-_world_wall_thickness_drawn,   _world_height);

        glColor3fv(color_high);
        glVertex3f(-m_world_size+_world_wall_thickness_drawn, i+_world_wall_size,   _world_height);
        glColor3fv(color_low);
        glVertex3f(-m_world_size+_world_wall_thickness_drawn, i+_world_wall_size,   0);
        glVertex3f(-m_world_size+_world_wall_thickness_drawn, i,   0);
        glColor3fv(color_high);
        glVertex3f(-m_world_size+_world_wall_thickness_drawn, i,   _world_height);
    }
    glEnd();
    //wall lines for the edge
    float extra_dist=0.001;
    glLineWidth(3);
    glColor3f(0.9,0.9,0.9);
    for(int i=0;i<_world_size;i++)
    {
        glBegin(GL_LINE_STRIP);
        glVertex3f(-i,                  m_world_size-_world_wall_thickness_drawn-extra_dist,   _world_height-extra_dist);
        glVertex3f(-i,                  m_world_size-_world_wall_thickness_drawn-extra_dist,   extra_dist);
        glVertex3f(-i-_world_wall_size, m_world_size-_world_wall_thickness_drawn-extra_dist,   extra_dist);
        glVertex3f(-i-_world_wall_size, m_world_size-_world_wall_thickness_drawn-extra_dist,   _world_height-extra_dist);
        glVertex3f(-i,                  m_world_size-_world_wall_thickness_drawn-extra_dist,   _world_height-extra_dist);
        glEnd();

        glBegin(GL_LINE_STRIP);
        glVertex3f(-m_world_size+_world_wall_thickness_drawn+extra_dist, i+_world_wall_size,   _world_height-extra_dist);
        glVertex3f(-m_world_size+_world_wall_thickness_drawn+extra_dist, i+_world_wall_size,   extra_dist);
        glVertex3f(-m_world_size+_world_wall_thickness_drawn+extra_dist, i,   extra_dist);
        glVertex3f(-m_world_size+_world_wall_thickness_drawn+extra_dist, i,   _world_height-extra_dist);
        glVertex3f(-m_world_size+_world_wall_thickness_drawn+extra_dist, i+_world_wall_size,   _world_height-extra_dist);
        glEnd();
    }
    glLineWidth(1);

    /*//draw world edge xmax,ymax
    glColor3f(1,1,1);
    glBegin(GL_QUADS);
    glVertex3f(-m_world_size,m_world_size,_world_height);
    glVertex3f(-m_world_size,m_world_size,0);
    glVertex3f(0,m_world_size,0);
    glVertex3f(0,m_world_size,_world_height);

    glVertex3f(-m_world_size,m_world_size,_world_height);
    glVertex3f(-m_world_size,m_world_size,0);
    glVertex3f(-m_world_size,0,0);
    glVertex3f(-m_world_size,0,_world_height);
    glEnd();*/

    //draw walls
    glBegin(GL_QUADS);
    for(int x=0;x<_world_size;x++)
    for(int y=0;y<_world_size;y++)
    {
        switch(m_arr_wall[x][y])
        {
            case 0://no walls
            {
                //nothing
            }break;

            case 1://x wall
            {
                glColor3fv(color_high);
                glVertex3f(-x,y-_world_wall_thickness_drawn,_world_height);
                glColor3fv(color_low);
                glVertex3f(-x,y-_world_wall_thickness_drawn,0);
                glVertex3f(-x-_world_wall_size,y-_world_wall_thickness_drawn,0);
                glColor3fv(color_high);
                glVertex3f(-x-_world_wall_size,y-_world_wall_thickness_drawn,_world_height);

                glVertex3f(-x,y+_world_wall_thickness_drawn,_world_height);
                glColor3fv(color_low);
                glVertex3f(-x,y+_world_wall_thickness_drawn,0);
                glVertex3f(-x-_world_wall_size,y+_world_wall_thickness_drawn,0);
                glColor3fv(color_high);
                glVertex3f(-x-_world_wall_size,y+_world_wall_thickness_drawn,_world_height);

                glVertex3f(-x,y+_world_wall_thickness_drawn,_world_height);
                glColor3fv(color_low);
                glVertex3f(-x,y+_world_wall_thickness_drawn,0);
                glVertex3f(-x,y-_world_wall_thickness_drawn,0);
                glColor3fv(color_high);
                glVertex3f(-x,y-_world_wall_thickness_drawn,_world_height);

                glVertex3f(-x-_world_wall_size,y+_world_wall_thickness_drawn,_world_height);
                glColor3fv(color_low);
                glVertex3f(-x-_world_wall_size,y+_world_wall_thickness_drawn,0);
                glVertex3f(-x-_world_wall_size,y-_world_wall_thickness_drawn,0);
                glColor3fv(color_high);
                glVertex3f(-x-_world_wall_size,y-_world_wall_thickness_drawn,_world_height);

            }break;

            case 2://y wall
            {
                glColor3fv(color_high);
                glVertex3f(-x-_world_wall_thickness_drawn,y,_world_height);
                glColor3fv(color_low);
                glVertex3f(-x-_world_wall_thickness_drawn,y,0);
                glVertex3f(-x-_world_wall_thickness_drawn,y+_world_wall_size,0);
                glColor3fv(color_high);
                glVertex3f(-x-_world_wall_thickness_drawn,y+_world_wall_size,_world_height);

                glVertex3f(-x+_world_wall_thickness_drawn,y,_world_height);
                glColor3fv(color_low);
                glVertex3f(-x+_world_wall_thickness_drawn,y,0);
                glVertex3f(-x+_world_wall_thickness_drawn,y+_world_wall_size,0);
                glColor3fv(color_high);
                glVertex3f(-x+_world_wall_thickness_drawn,y+_world_wall_size,_world_height);

                glVertex3f(-x+_world_wall_thickness_drawn,y,_world_height);
                glColor3fv(color_low);
                glVertex3f(-x+_world_wall_thickness_drawn,y,0);
                glVertex3f(-x-_world_wall_thickness_drawn,y,0);
                glColor3fv(color_high);
                glVertex3f(-x-_world_wall_thickness_drawn,y,_world_height);

                glVertex3f(-x+_world_wall_thickness_drawn,y+_world_wall_size,_world_height);
                glColor3fv(color_low);
                glVertex3f(-x+_world_wall_thickness_drawn,y+_world_wall_size,0);
                glVertex3f(-x-_world_wall_thickness_drawn,y+_world_wall_size,0);
                glColor3fv(color_high);
                glVertex3f(-x-_world_wall_thickness_drawn,y+_world_wall_size,_world_height);

            }break;

            case 3://x and y wall
            {
                glColor3fv(color_high);
                glVertex3f(-x,y-_world_wall_thickness_drawn,_world_height);
                glColor3fv(color_low);
                glVertex3f(-x,y-_world_wall_thickness_drawn,0);
                glVertex3f(-x-_world_wall_size,y-_world_wall_thickness_drawn,0);
                glColor3fv(color_high);
                glVertex3f(-x-_world_wall_size,y-_world_wall_thickness_drawn,_world_height);

                glVertex3f(-x,y+_world_wall_thickness_drawn,_world_height);
                glColor3fv(color_low);
                glVertex3f(-x,y+_world_wall_thickness_drawn,0);
                glVertex3f(-x-_world_wall_size,y+_world_wall_thickness_drawn,0);
                glColor3fv(color_high);
                glVertex3f(-x-_world_wall_size,y+_world_wall_thickness_drawn,_world_height);

                glVertex3f(-x-_world_wall_thickness_drawn,y,_world_height);
                glColor3fv(color_low);
                glVertex3f(-x-_world_wall_thickness_drawn,y,0);
                glVertex3f(-x-_world_wall_thickness_drawn,y+_world_wall_size,0);
                glColor3fv(color_high);
                glVertex3f(-x-_world_wall_thickness_drawn,y+_world_wall_size,_world_height);

                glVertex3f(-x+_world_wall_thickness_drawn,y,_world_height);
                glColor3fv(color_low);
                glVertex3f(-x+_world_wall_thickness_drawn,y,0);
                glVertex3f(-x+_world_wall_thickness_drawn,y+_world_wall_size,0);
                glColor3fv(color_high);
                glVertex3f(-x+_world_wall_thickness_drawn,y+_world_wall_size,_world_height);

                glVertex3f(-x,y+_world_wall_thickness_drawn,_world_height);
                glColor3fv(color_low);
                glVertex3f(-x,y+_world_wall_thickness_drawn,0);
                glVertex3f(-x,y-_world_wall_thickness_drawn,0);
                glColor3fv(color_high);
                glVertex3f(-x,y-_world_wall_thickness_drawn,_world_height);

                glVertex3f(-x-_world_wall_size,y+_world_wall_thickness_drawn,_world_height);
                glColor3fv(color_low);
                glVertex3f(-x-_world_wall_size,y+_world_wall_thickness_drawn,0);
                glVertex3f(-x-_world_wall_size,y-_world_wall_thickness_drawn,0);
                glColor3fv(color_high);
                glVertex3f(-x-_world_wall_size,y-_world_wall_thickness_drawn,_world_height);

                glVertex3f(-x+_world_wall_thickness_drawn,y,_world_height);
                glColor3fv(color_low);
                glVertex3f(-x+_world_wall_thickness_drawn,y,0);
                glVertex3f(-x-_world_wall_thickness_drawn,y,0);
                glColor3fv(color_high);
                glVertex3f(-x-_world_wall_thickness_drawn,y,_world_height);

                glVertex3f(-x+_world_wall_thickness_drawn,y+_world_wall_size,_world_height);
                glColor3fv(color_low);
                glVertex3f(-x+_world_wall_thickness_drawn,y+_world_wall_size,0);
                glVertex3f(-x-_world_wall_thickness_drawn,y+_world_wall_size,0);
                glColor3fv(color_high);
                glVertex3f(-x-_world_wall_thickness_drawn,y+_world_wall_size,_world_height);

            }break;
        }
    }
    glEnd();

    //draw wall outline
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glColor4f(0.9,0.9,0.9,0.5);
    glColor3f(0.9,0.9,0.9);
    //glDepthMask(false);
    glLineWidth(3);
    for(int x=0;x<_world_size;x++)
    for(int y=0;y<_world_size;y++)
    {
        glBegin(GL_LINE_STRIP);
        switch(m_arr_wall[x][y])
        {
            case 0://no walls
            {
                //nothing
            }break;

            case 1://x wall
            {
                glVertex3f(-x+extra_dist,  y-_world_wall_thickness_drawn-extra_dist,_world_height-extra_dist);
                glVertex3f(-x+extra_dist,  y-_world_wall_thickness_drawn-extra_dist,extra_dist);
                glVertex3f(-x-_world_wall_size-extra_dist,y-_world_wall_thickness_drawn-extra_dist,extra_dist);
                glVertex3f(-x-_world_wall_size-extra_dist,y-_world_wall_thickness_drawn-extra_dist,_world_height-extra_dist);
                glVertex3f(-x+extra_dist,  y-_world_wall_thickness_drawn-extra_dist,_world_height-extra_dist);

                glVertex3f(-x+extra_dist,  y+_world_wall_thickness_drawn+extra_dist,_world_height-extra_dist);
                glVertex3f(-x+extra_dist,  y+_world_wall_thickness_drawn+extra_dist,extra_dist);
                glVertex3f(-x-_world_wall_size-extra_dist,y+_world_wall_thickness_drawn+extra_dist,extra_dist);
                glVertex3f(-x-_world_wall_size-extra_dist,y+_world_wall_thickness_drawn+extra_dist,_world_height-extra_dist);
                glVertex3f(-x+extra_dist,  y+_world_wall_thickness_drawn+extra_dist,_world_height-extra_dist);

                glVertex3f(-x+extra_dist,y+_world_wall_thickness_drawn+extra_dist,_world_height-extra_dist);
                glVertex3f(-x+extra_dist,y+_world_wall_thickness_drawn+extra_dist,extra_dist);
                glVertex3f(-x+extra_dist,y-_world_wall_thickness_drawn-extra_dist,extra_dist);
                glVertex3f(-x+extra_dist,y-_world_wall_thickness_drawn-extra_dist,_world_height-extra_dist);
                glVertex3f(-x+extra_dist,y+_world_wall_thickness_drawn+extra_dist,_world_height-extra_dist);

                glVertex3f(-x-_world_wall_size-extra_dist,y+_world_wall_thickness_drawn+extra_dist,_world_height-extra_dist);
                glVertex3f(-x-_world_wall_size-extra_dist,y+_world_wall_thickness_drawn+extra_dist,extra_dist);
                glVertex3f(-x-_world_wall_size-extra_dist,y-_world_wall_thickness_drawn-extra_dist,extra_dist);
                glVertex3f(-x-_world_wall_size-extra_dist,y-_world_wall_thickness_drawn-extra_dist,_world_height-extra_dist);
                glVertex3f(-x-_world_wall_size-extra_dist,y+_world_wall_thickness_drawn+extra_dist,_world_height-extra_dist);
            }break;

            case 2://y wall
            {
                glVertex3f(-x-_world_wall_thickness_drawn-extra_dist,y-extra_dist,  _world_height-extra_dist);
                glVertex3f(-x-_world_wall_thickness_drawn-extra_dist,y-extra_dist,  extra_dist);
                glVertex3f(-x-_world_wall_thickness_drawn-extra_dist,y+_world_wall_size+extra_dist,extra_dist);
                glVertex3f(-x-_world_wall_thickness_drawn-extra_dist,y+_world_wall_size+extra_dist,_world_height-extra_dist);
                glVertex3f(-x-_world_wall_thickness_drawn-extra_dist,y-extra_dist,  _world_height-extra_dist);

                glVertex3f(-x+_world_wall_thickness_drawn+extra_dist,y-extra_dist,_world_height-extra_dist);
                glVertex3f(-x+_world_wall_thickness_drawn+extra_dist,y-extra_dist,extra_dist);
                glVertex3f(-x+_world_wall_thickness_drawn+extra_dist,y+_world_wall_size+extra_dist,extra_dist);
                glVertex3f(-x+_world_wall_thickness_drawn+extra_dist,y+_world_wall_size+extra_dist,_world_height-extra_dist);
                glVertex3f(-x+_world_wall_thickness_drawn+extra_dist,y-extra_dist,_world_height-extra_dist);

                glVertex3f(-x+_world_wall_thickness_drawn+extra_dist,y-extra_dist,_world_height-extra_dist);
                glVertex3f(-x+_world_wall_thickness_drawn+extra_dist,y-extra_dist,extra_dist);
                glVertex3f(-x-_world_wall_thickness_drawn-extra_dist,y-extra_dist,extra_dist);
                glVertex3f(-x-_world_wall_thickness_drawn-extra_dist,y-extra_dist,_world_height-extra_dist);
                glVertex3f(-x+_world_wall_thickness_drawn+extra_dist,y-extra_dist,_world_height-extra_dist);

                glVertex3f(-x+_world_wall_thickness_drawn+extra_dist,y+_world_wall_size+extra_dist,_world_height-extra_dist);
                glVertex3f(-x+_world_wall_thickness_drawn+extra_dist,y+_world_wall_size+extra_dist,extra_dist);
                glVertex3f(-x-_world_wall_thickness_drawn-extra_dist,y+_world_wall_size+extra_dist,extra_dist);
                glVertex3f(-x-_world_wall_thickness_drawn-extra_dist,y+_world_wall_size+extra_dist,_world_height-extra_dist);
                glVertex3f(-x+_world_wall_thickness_drawn+extra_dist,y+_world_wall_size+extra_dist,_world_height-extra_dist);
            }break;

            case 3://x and y wall
            {
                glVertex3f(-x+extra_dist,  y-_world_wall_thickness_drawn-extra_dist,_world_height-extra_dist);
                glVertex3f(-x+extra_dist,  y-_world_wall_thickness_drawn-extra_dist,extra_dist);
                glVertex3f(-x-_world_wall_size-extra_dist,y-_world_wall_thickness_drawn-extra_dist,extra_dist);
                glVertex3f(-x-_world_wall_size-extra_dist,y-_world_wall_thickness_drawn-extra_dist,_world_height-extra_dist);
                glVertex3f(-x+extra_dist,  y-_world_wall_thickness_drawn-extra_dist,_world_height-extra_dist);

                glVertex3f(-x+extra_dist,  y+_world_wall_thickness_drawn+extra_dist,_world_height-extra_dist);
                glVertex3f(-x+extra_dist,  y+_world_wall_thickness_drawn+extra_dist,extra_dist);
                glVertex3f(-x-_world_wall_size-extra_dist,y+_world_wall_thickness_drawn+extra_dist,extra_dist);
                glVertex3f(-x-_world_wall_size-extra_dist,y+_world_wall_thickness_drawn+extra_dist,_world_height-extra_dist);
                glVertex3f(-x+extra_dist,  y+_world_wall_thickness_drawn+extra_dist,_world_height-extra_dist);

                glVertex3f(-x+extra_dist,y+_world_wall_thickness_drawn+extra_dist,_world_height-extra_dist);
                glVertex3f(-x+extra_dist,y+_world_wall_thickness_drawn+extra_dist,extra_dist);
                glVertex3f(-x+extra_dist,y-_world_wall_thickness_drawn-extra_dist,extra_dist);
                glVertex3f(-x+extra_dist,y-_world_wall_thickness_drawn-extra_dist,_world_height-extra_dist);
                glVertex3f(-x+extra_dist,y+_world_wall_thickness_drawn+extra_dist,_world_height-extra_dist);

                glVertex3f(-x-_world_wall_size-extra_dist,y+_world_wall_thickness_drawn+extra_dist,_world_height-extra_dist);
                glVertex3f(-x-_world_wall_size-extra_dist,y+_world_wall_thickness_drawn+extra_dist,extra_dist);
                glVertex3f(-x-_world_wall_size-extra_dist,y-_world_wall_thickness_drawn-extra_dist,extra_dist);
                glVertex3f(-x-_world_wall_size-extra_dist,y-_world_wall_thickness_drawn-extra_dist,_world_height-extra_dist);
                glVertex3f(-x-_world_wall_size-extra_dist,y+_world_wall_thickness_drawn+extra_dist,_world_height-extra_dist);

                glVertex3f(-x-_world_wall_thickness_drawn-extra_dist,y-extra_dist,  _world_height-extra_dist);
                glVertex3f(-x-_world_wall_thickness_drawn-extra_dist,y-extra_dist,  extra_dist);
                glVertex3f(-x-_world_wall_thickness_drawn-extra_dist,y+_world_wall_size+extra_dist,extra_dist);
                glVertex3f(-x-_world_wall_thickness_drawn-extra_dist,y+_world_wall_size+extra_dist,_world_height-extra_dist);
                glVertex3f(-x-_world_wall_thickness_drawn-extra_dist,y-extra_dist,  _world_height-extra_dist);

                glVertex3f(-x+_world_wall_thickness_drawn+extra_dist,y-extra_dist,_world_height-extra_dist);
                glVertex3f(-x+_world_wall_thickness_drawn+extra_dist,y-extra_dist,extra_dist);
                glVertex3f(-x+_world_wall_thickness_drawn+extra_dist,y+_world_wall_size+extra_dist,extra_dist);
                glVertex3f(-x+_world_wall_thickness_drawn+extra_dist,y+_world_wall_size+extra_dist,_world_height-extra_dist);
                glVertex3f(-x+_world_wall_thickness_drawn+extra_dist,y-extra_dist,_world_height-extra_dist);

                glVertex3f(-x+_world_wall_thickness_drawn+extra_dist,y-extra_dist,_world_height-extra_dist);
                glVertex3f(-x+_world_wall_thickness_drawn+extra_dist,y-extra_dist,extra_dist);
                glVertex3f(-x-_world_wall_thickness_drawn-extra_dist,y-extra_dist,extra_dist);
                glVertex3f(-x-_world_wall_thickness_drawn-extra_dist,y-extra_dist,_world_height-extra_dist);
                glVertex3f(-x+_world_wall_thickness_drawn+extra_dist,y-extra_dist,_world_height-extra_dist);

                glVertex3f(-x+_world_wall_thickness_drawn+extra_dist,y+_world_wall_size+extra_dist,_world_height-extra_dist);
                glVertex3f(-x+_world_wall_thickness_drawn+extra_dist,y+_world_wall_size+extra_dist,extra_dist);
                glVertex3f(-x-_world_wall_thickness_drawn-extra_dist,y+_world_wall_size+extra_dist,extra_dist);
                glVertex3f(-x-_world_wall_thickness_drawn-extra_dist,y+_world_wall_size+extra_dist,_world_height-extra_dist);
                glVertex3f(-x+_world_wall_thickness_drawn+extra_dist,y+_world_wall_size+extra_dist,_world_height-extra_dist);
            }break;
        }
        glEnd();
    }
    glLineWidth(1);
    /*//outline 2
    glColor3f(0.9,0.9,0.9);
    glLineWidth(2);
    for(int x=0;x<_world_size;x++)
    for(int y=0;y<_world_size;y++)
    {
        glBegin(GL_LINE_STRIP);
        switch(m_arr_wall[x][y])
        {
            case 0://no walls
            {
                //nothing
            }break;

            case 1://x wall
            {
                glVertex3f(-x,y-_world_wall_thickness_drawn,_world_height);
                glVertex3f(-x,y-_world_wall_thickness_drawn,0);
                glVertex3f(-x-_world_wall_size,y-_world_wall_thickness_drawn,0);
                glVertex3f(-x-_world_wall_size,y-_world_wall_thickness_drawn,_world_height);

                glVertex3f(-x,y+_world_wall_thickness_drawn,_world_height);
                glVertex3f(-x,y+_world_wall_thickness_drawn,0);
                glVertex3f(-x-_world_wall_size,y+_world_wall_thickness_drawn,0);
                glVertex3f(-x-_world_wall_size,y+_world_wall_thickness_drawn,_world_height);

                glVertex3f(-x,y+_world_wall_thickness_drawn,_world_height);
                glVertex3f(-x,y+_world_wall_thickness_drawn,0);
                glVertex3f(-x,y-_world_wall_thickness_drawn,0);
                glVertex3f(-x,y-_world_wall_thickness_drawn,_world_height);

                glVertex3f(-x-_world_wall_size,y+_world_wall_thickness_drawn,_world_height);
                glVertex3f(-x-_world_wall_size,y+_world_wall_thickness_drawn,0);
                glVertex3f(-x-_world_wall_size,y-_world_wall_thickness_drawn,0);
                glVertex3f(-x-_world_wall_size,y-_world_wall_thickness_drawn,_world_height);
            }break;

            case 2://y wall
            {
                glVertex3f(-x-_world_wall_thickness_drawn,y,_world_height);
                glVertex3f(-x-_world_wall_thickness_drawn,y,0);
                glVertex3f(-x-_world_wall_thickness_drawn,y+_world_wall_size,0);
                glVertex3f(-x-_world_wall_thickness_drawn,y+_world_wall_size,_world_height);

                glVertex3f(-x+_world_wall_thickness_drawn,y,_world_height);
                glVertex3f(-x+_world_wall_thickness_drawn,y,0);
                glVertex3f(-x+_world_wall_thickness_drawn,y+_world_wall_size,0);
                glVertex3f(-x+_world_wall_thickness_drawn,y+_world_wall_size,_world_height);

                glVertex3f(-x+_world_wall_thickness_drawn,y,_world_height);
                glVertex3f(-x+_world_wall_thickness_drawn,y,0);
                glVertex3f(-x-_world_wall_thickness_drawn,y,0);
                glVertex3f(-x-_world_wall_thickness_drawn,y,_world_height);

                glVertex3f(-x+_world_wall_thickness_drawn,y+_world_wall_size,_world_height);
                glVertex3f(-x+_world_wall_thickness_drawn,y+_world_wall_size,0);
                glVertex3f(-x-_world_wall_thickness_drawn,y+_world_wall_size,0);
                glVertex3f(-x-_world_wall_thickness_drawn,y+_world_wall_size,_world_height);
            }break;

            case 3://x and y wall
            {
                glVertex3f(-x,y-_world_wall_thickness_drawn,_world_height);
                glVertex3f(-x,y-_world_wall_thickness_drawn,0);
                glVertex3f(-x-_world_wall_size,y-_world_wall_thickness_drawn,0);
                glVertex3f(-x-_world_wall_size,y-_world_wall_thickness_drawn,_world_height);

                glVertex3f(-x,y+_world_wall_thickness_drawn,_world_height);
                glVertex3f(-x,y+_world_wall_thickness_drawn,0);
                glVertex3f(-x-_world_wall_size,y+_world_wall_thickness_drawn,0);
                glVertex3f(-x-_world_wall_size,y+_world_wall_thickness_drawn,_world_height);

                glVertex3f(-x-_world_wall_thickness_drawn,y,_world_height);
                glVertex3f(-x-_world_wall_thickness_drawn,y,0);
                glVertex3f(-x-_world_wall_thickness_drawn,y+_world_wall_size,0);
                glVertex3f(-x-_world_wall_thickness_drawn,y+_world_wall_size,_world_height);

                glVertex3f(-x+_world_wall_thickness_drawn,y,_world_height);
                glVertex3f(-x+_world_wall_thickness_drawn,y,0);
                glVertex3f(-x+_world_wall_thickness_drawn,y+_world_wall_size,0);
                glVertex3f(-x+_world_wall_thickness_drawn,y+_world_wall_size,_world_height);

                glVertex3f(-x,y+_world_wall_thickness_drawn,_world_height);
                glVertex3f(-x,y+_world_wall_thickness_drawn,0);
                glVertex3f(-x,y-_world_wall_thickness_drawn,0);
                glVertex3f(-x,y-_world_wall_thickness_drawn,_world_height);

                glVertex3f(-x-_world_wall_size,y+_world_wall_thickness_drawn,_world_height);
                glVertex3f(-x-_world_wall_size,y+_world_wall_thickness_drawn,0);
                glVertex3f(-x-_world_wall_size,y-_world_wall_thickness_drawn,0);
                glVertex3f(-x-_world_wall_size,y-_world_wall_thickness_drawn,_world_height);

                glVertex3f(-x+_world_wall_thickness_drawn,y,_world_height);
                glVertex3f(-x+_world_wall_thickness_drawn,y,0);
                glVertex3f(-x-_world_wall_thickness_drawn,y,0);
                glVertex3f(-x-_world_wall_thickness_drawn,y,_world_height);

                glVertex3f(-x+_world_wall_thickness_drawn,y+_world_wall_size,_world_height);
                glVertex3f(-x+_world_wall_thickness_drawn,y+_world_wall_size,0);
                glVertex3f(-x-_world_wall_thickness_drawn,y+_world_wall_size,0);
                glVertex3f(-x-_world_wall_thickness_drawn,y+_world_wall_size,_world_height);
            }break;
        }
        glEnd();
    }
    glLineWidth(1);*/
    //glDepthMask(true);
    //glDisable(GL_BLEND);

    //draw topsides of walls
    glColor3f(0.9,0.9,0.9);
    glBegin(GL_QUADS);
    for(int x=0;x<_world_size;x++)
    for(int y=0;y<_world_size;y++)
    {
        switch(m_arr_wall[x][y])
        {
            case 0://no walls
            {
                //nothing
            }break;

            case 1://x wall
            {
                glVertex3f(-x,y+_world_wall_thickness_drawn,_world_height);
                glVertex3f(-x-_world_wall_size,y+_world_wall_thickness_drawn,_world_height);
                glVertex3f(-x-_world_wall_size,y-_world_wall_thickness_drawn,_world_height);
                glVertex3f(-x,y-_world_wall_thickness_drawn,_world_height);
            }break;

            case 2://y wall
            {
                glVertex3f(-x+_world_wall_thickness_drawn,y,_world_height);
                glVertex3f(-x+_world_wall_thickness_drawn,y+_world_wall_size,_world_height);
                glVertex3f(-x-_world_wall_thickness_drawn,y+_world_wall_size,_world_height);
                glVertex3f(-x-_world_wall_thickness_drawn,y,_world_height);
            }break;

            case 3://x+y walls
            {
                glVertex3f(-x+_world_wall_thickness_drawn,y,_world_height);
                glVertex3f(-x+_world_wall_thickness_drawn,y+_world_wall_size,_world_height);
                glVertex3f(-x-_world_wall_thickness_drawn,y+_world_wall_size,_world_height);
                glVertex3f(-x-_world_wall_thickness_drawn,y,_world_height);

                glVertex3f(-x,y+_world_wall_thickness_drawn,_world_height);
                glVertex3f(-x-_world_wall_size,y+_world_wall_thickness_drawn,_world_height);
                glVertex3f(-x-_world_wall_size,y-_world_wall_thickness_drawn,_world_height);
                glVertex3f(-x,y-_world_wall_thickness_drawn,_world_height);
            }break;
        }
    }
    glEnd();

    //draw topside of edge wall
    glColor3f(0.9,0.9,0.9);
    glBegin(GL_QUADS);
    glVertex3f(-m_world_size-_world_wall_thickness_drawn    ,   -_world_wall_thickness_drawn,   _world_height);
    glVertex3f(-m_world_size+_world_wall_thickness_drawn    ,   -_world_wall_thickness_drawn,   _world_height);
    glVertex3f(-m_world_size+_world_wall_thickness_drawn    ,   m_world_size+_world_wall_thickness_drawn,   _world_height);
    glVertex3f(-m_world_size-_world_wall_thickness_drawn    ,   m_world_size+_world_wall_thickness_drawn,   _world_height);

    glVertex3f(-m_world_size-_world_wall_thickness_drawn    ,   m_world_size-_world_wall_thickness_drawn,   _world_height);
    glVertex3f(-m_world_size-_world_wall_thickness_drawn    ,   m_world_size+_world_wall_thickness_drawn,   _world_height);
    glVertex3f(_world_wall_thickness_drawn    ,   m_world_size+_world_wall_thickness_drawn,   _world_height);
    glVertex3f(_world_wall_thickness_drawn    ,   m_world_size-_world_wall_thickness_drawn,   _world_height);
    glEnd();

    /*//draw connected area
    glColor3f(0.2,0.2,1);
    glBegin(GL_QUADS);
    for(int x=0;x<_world_size;x++)
    for(int y=0;y<_world_size;y++)
    {
        if( m_arr_connected_pos[x][y]!=0 )
        {
            switch(m_arr_connected_pos[x][y])
            {
                case 1: glColor3f(1.0,0.2,0.2);break;
                case 2: glColor3f(0.2,0.2,1);break;
            }

            glVertex3f(-x-_world_wall_size*0.8,y+_world_wall_size*0.2,_world_height);
            glVertex3f(-x-_world_wall_size*0.2,y+_world_wall_size*0.2,_world_height);
            glVertex3f(-x-_world_wall_size*0.2,y+_world_wall_size*0.8,_world_height);
            glVertex3f(-x-_world_wall_size*0.8,y+_world_wall_size*0.8,_world_height);
        }
    }
    glEnd();*/

    return true;
}

bool world::test_for_collision_nofix(float pos_new[2],float pos_old[2])
{
    //cull float pos to int, this pos can only collide with walls from (x,y) (x+1,y) (x,y+1)
    int x_pos_int=int(pos_new[0]);
    int y_pos_int=int(pos_new[1]);
    //test if within limits
    if( x_pos_int<0 || y_pos_int<0 || x_pos_int>=_world_size || y_pos_int>=_world_size )
    {
        return false;
    }
    //test wall collision
    //world edge test x+, y+
    if( x_pos_int==_world_size-1 )//close to edge
    {
        if( pos_new[0]-(float)x_pos_int>_world_wall_size-_world_wall_thickness )
        {
            return false;
        }
    }
    if( y_pos_int==_world_size-1 )//close to edge
    {
        if( pos_new[1]-(float)y_pos_int>_world_wall_size-_world_wall_thickness )
        {
            return false;
        }
    }
    //other walls
    //x,y x+
    if( m_arr_wall[x_pos_int][y_pos_int]==1 || m_arr_wall[x_pos_int][y_pos_int]==3 )
    {
        if( pos_new[1]-(float)y_pos_int<_world_wall_thickness )
        {
            return false;
        }
    }
    //x,y y+
    if( m_arr_wall[x_pos_int][y_pos_int]==2 || m_arr_wall[x_pos_int][y_pos_int]==3 )
    {
        if( pos_new[0]-(float)x_pos_int<_world_wall_thickness )
        {
            return false;
        }
    }
    //x+1,y y+
    if( x_pos_int!=_world_size-1 )//wall will be world edge, already been tested
    {
        if( m_arr_wall[x_pos_int+1][y_pos_int]==2 || m_arr_wall[x_pos_int+1][y_pos_int]==3 )
        {
            if( pos_new[0]-(float)x_pos_int>_world_wall_size-_world_wall_thickness )
            {
                return false;
            }
        }
    }
    //x,y+1 x+
    if( y_pos_int!=_world_size-1 )//wall will be world edge, already been tested
    {
        if( m_arr_wall[x_pos_int][y_pos_int+1]==1 || m_arr_wall[x_pos_int][y_pos_int+1]==3 )
        {
            if( pos_new[1]-(float)y_pos_int>_world_wall_size-_world_wall_thickness )
            {
                return false;
            }
        }
    }

    return true;//no collision
}

bool world::test_for_collision(float pos_new[2],float pos_old[2])
{
    /*//calc test position, will be a vector with known lenght
    float dx=pos_new[0]-pos_old[0];
    float dy=pos_new[1]-pos_old[1];
    //scale vector
    float pos_test[2];// point that will be tested
    float vector_length=sqrt( dx*dx+dy*dy );
    if(vector_length==0)//error (same pos)
    {
        pos_test[0]=pos_new[0];
        pos_test[1]=pos_new[1];
    }
    else
    {
        float target_vector_length=0.3;
        float scale_factor=target_vector_length/vector_length;
        pos_test[0]=pos_old[0]+dx*scale_factor;
        pos_test[1]=pos_old[1]+dy*scale_factor;
    }*/

    //test wall collision
    while(true)
    {
        //cull float pos to int, this pos can only collide with walls from (x,y) (x+1,y) (x,y+1)
        int x_pos_int=int(pos_new[0]);
        int y_pos_int=int(pos_new[1]);
        //test if within limits
        if( x_pos_int<0 || y_pos_int<0 || x_pos_int>=_world_size || y_pos_int>=_world_size )
        {
            return false;
        }
        //test if step changed gridpos
        bool grid_step=false;
        if( (int)pos_old[0]!=(int)pos_new[0] || (int)pos_old[1]!=(int)pos_new[1] ) grid_step=true;

        //world edge test x+, y+
        if( x_pos_int==_world_size-1 )//close to edge
        {
            if( pos_new[0]-(float)x_pos_int>_world_wall_size-_world_wall_thickness )
            {
                //move to closest pos to the wall
                pos_new[0]=(float)x_pos_int+(_world_wall_size-_world_wall_thickness)*0.99;
                continue;
            }
        }
        if( y_pos_int==_world_size-1 )//close to edge
        {
            if( pos_new[1]-(float)y_pos_int>_world_wall_size-_world_wall_thickness )
            {
                //move to closest pos to the wall
                pos_new[1]=(float)y_pos_int+(_world_wall_size-_world_wall_thickness)*0.99;
                continue;
            }
        }
        //other walls
        //x,y x+
        if( m_arr_wall[x_pos_int][y_pos_int]==1 || m_arr_wall[x_pos_int][y_pos_int]==3 )
        {
            if( pos_new[1]-(float)y_pos_int<_world_wall_thickness )
            {
                //move to closest pos to the wall
                if(grid_step)
                 pos_new[0]=pos_old[0];
                else
                 pos_new[1]=(float)y_pos_int+_world_wall_thickness*1.01;
                continue;
            }
        }
        //x,y y+
        if( m_arr_wall[x_pos_int][y_pos_int]==2 || m_arr_wall[x_pos_int][y_pos_int]==3 )
        {
            if( pos_new[0]-(float)x_pos_int<_world_wall_thickness )
            {
                //move to closest pos to the wall
                if(grid_step)
                 pos_new[1]=pos_old[1];
                else
                 pos_new[0]=(float)x_pos_int+_world_wall_thickness*1.01;
                continue;
            }
        }
        //x+1,y y+
        if( x_pos_int!=_world_size-1 )//wall will be world edge, already been tested
        {
            if( m_arr_wall[x_pos_int+1][y_pos_int]==2 || m_arr_wall[x_pos_int+1][y_pos_int]==3 )
            {
                if( pos_new[0]-(float)x_pos_int>_world_wall_size-_world_wall_thickness )
                {
                    //move to closest pos to the wall
                    if(grid_step)
                     pos_new[1]=pos_old[1];
                    else
                     pos_new[0]=(float)x_pos_int+(_world_wall_size-_world_wall_thickness)*0.99;
                    continue;
                }
            }
        }
        //x,y+1 x+
        if( y_pos_int!=_world_size-1 )//wall will be world edge, already been tested
        {
            if( m_arr_wall[x_pos_int][y_pos_int+1]==1 || m_arr_wall[x_pos_int][y_pos_int+1]==3 )
            {
                if( pos_new[1]-(float)y_pos_int>_world_wall_size-_world_wall_thickness )
                {
                    //move to closest pos to the wall
                    if(grid_step)
                     pos_new[0]=pos_old[0];
                    else
                     pos_new[1]=(float)y_pos_int+(_world_wall_size-_world_wall_thickness)*0.99;
                    continue;
                }
            }
        }

        break;
    }

    return true;//no collision
}

bool world::wall_crossed(float pos_new[2],float pos_old[2])
{
    //test if int has changed
    if( (int)pos_old[0]!=(int)pos_new[0] && (int)pos_old[1]==(int)pos_new[1] )
    {
        int y_val=(int)pos_new[1];
        int hi_x=(int)pos_old[0];
        int lo_x=(int)pos_new[0];
        if(hi_x<lo_x)//swap
        {
            lo_x=(int)pos_old[0];
            hi_x=(int)pos_new[0];
        }
        //test close by walls
        //x,y y+
        if( m_arr_wall[hi_x][y_val]==2 || m_arr_wall[hi_x][y_val]==3 )
        {
            return true;
        }
    }
    if( (int)pos_old[1]!=(int)pos_new[1] && (int)pos_old[0]==(int)pos_new[0] )
    {
        int x_val=(int)pos_new[0];
        int hi_y=(int)pos_old[1];
        int lo_y=(int)pos_new[1];
        if(hi_y<lo_y)//swap
        {
            lo_y=(int)pos_old[1];
            hi_y=(int)pos_new[1];
        }
        //test close by walls
        //x,y x+
        if( m_arr_wall[x_val][hi_y]==1 || m_arr_wall[x_val][hi_y]==3 )
        {
            return true;
        }
    }
    //diagonal test
    if( (int)pos_old[0]!=(int)pos_new[0] && (int)pos_old[1]!=(int)pos_new[1] )
    {
        int hi_x=(int)pos_old[0];
        int lo_x=(int)pos_new[0];
        if(hi_x<lo_x)//swap
        {
            lo_x=(int)pos_old[0];
            hi_x=(int)pos_new[0];
        }
        int hi_y=(int)pos_old[1];
        int lo_y=(int)pos_new[1];
        if(hi_y<lo_y)//swap
        {
            lo_y=(int)pos_old[1];
            hi_y=(int)pos_new[1];
        }
        //test close by walls
        if( m_arr_wall[hi_x][hi_y]!=0 ||
            m_arr_wall[hi_x][lo_y]==3 || m_arr_wall[hi_x][lo_y]==2 ||
            m_arr_wall[lo_x][hi_y]==3 || m_arr_wall[lo_x][hi_y]==1 )
        {
            return true;
        }
    }

    return false;
}

bool world::close_to_wall_edge(int x_node,int y_node)
{
    //check limits
    if(x_node<0 || y_node<0 || x_node>_world_size || y_node>_world_size)  return false;//error, outside world
    //check edges
    if(x_node==0 || y_node==0 || x_node==_world_size || y_node==_world_size)  return true;//always a wall
    //check center node
    if( m_arr_wall[x_node][y_node]!=0 )
    {
        return true;
    }
    //check close nodes
    if( m_arr_wall[x_node-1][y_node]==1 || m_arr_wall[x_node-1][y_node]==3 )
    {
        return true;
    }
    if( m_arr_wall[x_node][y_node-1]==2 || m_arr_wall[x_node][y_node-1]==3 )
    {
        return true;
    }

    return false;//no wall there
}

//Private
