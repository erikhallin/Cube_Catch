#include "model_obj.h"

model_obj::model_obj()
{
    m_error=1;//no model
    m_draw_type=0;
}

model_obj::model_obj(string filepath)
{
    cout<<"Loading file: "<<filepath<<endl;
    m_error=0;

    //load file
    ifstream file( filepath.c_str() );
    if(file==0)
    {
        cout<<"ERROR: Model Loader: Could not find file\n";
        m_error=2;//no file
        return;
    }

    string line,word;
    while( getline(file,line) )
    {
        switch(line[0])
        {
            case 'v'://vertex
            {
                //store vertex
                stringstream ss(line);
                ss>>word;//skip first letter 'v'
                ss>>word;//x
                float x=atof( word.c_str() );
                ss>>word;//y
                float y=atof( word.c_str() );
                ss>>word;//z
                float z=atof( word.c_str() );
                m_vec_vertex.push_back( st_point_xyz(x,y,z) );
            }break;

            case 'f'://face
            {
                //store face triangle fan, can have more than 4 corners
                stringstream ss(line);
                ss>>word;//skip first letter 'f'
                st_face_vec face;
                while(ss>>word)
                {
                    face.vec_face_pointer.push_back( atoi( word.c_str() )-1 );
                }
                m_vec_face.push_back( face );

                /*//store face as quad
                stringstream ss(line);
                ss>>word;//skip first letter 'f'
                ss>>word;//first
                float first=atoi( word.c_str() )-1;
                ss>>word;//second
                float second=atoi( word.c_str() )-1;
                ss>>word;//third
                float third=atoi( word.c_str() )-1;
                ss>>word;//fourth
                float fourth=atoi( word.c_str() )-1;
                //cout<<word<<endl;
                m_vec_face.push_back( st_face_quad(first,second,third,fourth) );*/

            }break;
        }
    }

    if( m_vec_vertex.empty() || m_vec_face.empty() )
    {
        cout<<"ERROR: Model Loader: No data found\n";
        m_error=4;
    }
    else cout<<"Model loaded\n";

    file.close();

    m_draw_type=0;
}

model_obj::model_obj(int model_id)
{
    cout<<"Loading model id: "<<model_id<<endl;
    m_error=0;

    //load data from string
    stringstream ss( load_base64_file(model_id) );//not base64

    string line,word;
    while( getline(ss,line) )
    {
        switch(line[0])
        {
            case 'v'://vertex
            {
                //store vertex
                stringstream ss(line);
                ss>>word;//skip first letter 'v'
                ss>>word;//x
                float x=atof( word.c_str() );
                ss>>word;//y
                float y=atof( word.c_str() );
                ss>>word;//z
                float z=atof( word.c_str() );
                m_vec_vertex.push_back( st_point_xyz(x,y,z) );
            }break;

            case 'f'://face
            {
                //store face triangle fan, can have more than 4 corners
                stringstream ss(line);
                ss>>word;//skip first letter 'f'
                st_face_vec face;
                while(ss>>word)
                {
                    face.vec_face_pointer.push_back( atoi( word.c_str() )-1 );
                }
                m_vec_face.push_back( face );

                /*//store face as quad
                stringstream ss(line);
                ss>>word;//skip first letter 'f'
                ss>>word;//first
                float first=atoi( word.c_str() )-1;
                ss>>word;//second
                float second=atoi( word.c_str() )-1;
                ss>>word;//third
                float third=atoi( word.c_str() )-1;
                ss>>word;//fourth
                float fourth=atoi( word.c_str() )-1;
                //cout<<word<<endl;
                m_vec_face.push_back( st_face_quad(first,second,third,fourth) );*/

            }break;
        }
    }

    if( m_vec_vertex.empty() || m_vec_face.empty() )
    {
        cout<<"ERROR: Model Loader: No data found\n";
        m_error=4;
    }
    else cout<<"Model loaded\n";

    m_draw_type=0;
}

bool model_obj::draw_settings(int draw_type)
{
    m_draw_type=draw_type;

    return true;
}

bool model_obj::draw(bool draw_to_zbuff)
{
    if(m_error!=0 || m_draw_type==0) return false;

    switch(m_draw_type)
    {
        case dt_error://draw test cube
        {
            glColor3f(0.5,0.5,0.5);
            glBegin(GL_QUADS);
            glVertex3f(-1,-1,-1);
            glVertex3f(1,-1,-1);
            glVertex3f(1,1,-1);
            glVertex3f(-1,1,-1);

            glVertex3f(-1,-1,1);
            glVertex3f(1,-1,1);
            glVertex3f(1,1,1);
            glVertex3f(-1,1,1);

            glVertex3f(-1,-1,-1);
            glVertex3f(-1,-1,1);
            glVertex3f(-1,1,1);
            glVertex3f(-1,1,-1);

            glVertex3f(1,-1,-1);
            glVertex3f(1,-1,1);
            glVertex3f(1,1,1);
            glVertex3f(1,1,-1);

            glVertex3f(-1,-1,-1);
            glVertex3f(-1,-1,1);
            glVertex3f(1,-1,1);
            glVertex3f(1,-1,-1);

            glVertex3f(-1,1,-1);
            glVertex3f(-1,1,1);
            glVertex3f(1,1,1);
            glVertex3f(1,1,-1);
            glEnd();
        }break;

        case dt_player://transparant faces with outline
        {
            glDepthMask(false);//will continue to draw this models polygons ignoring the depthtest for this models polygons whitout ignoring other drawn objects
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glColor4f(0,0.5,0,0.5);
            for(int face=0;face<(int)m_vec_face.size();face++)
            {
                glBegin(GL_TRIANGLE_FAN);//as quads, faces with 4 corners
                for(int vertex=0;vertex<(int)m_vec_face[face].vec_face_pointer.size();vertex++ )
                 glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].x,
                             m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].y,
                             m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].z );
                glEnd();
            }

            //outline
            glLineWidth(4);
            glColor4f(0,0.8,0,0.4);
            glBegin(GL_LINES);
            for(int face=0;face<(int)m_vec_face.size();face++)
            {
                for(int vertex=0;vertex<(int)m_vec_face[face].vec_face_pointer.size()-1;vertex++ )
                {
                    glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].x,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].y,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].z );
                    glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex+1] ].x,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex+1] ].y,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex+1] ].z );
                }
                //last to start
                glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer.back() ].x,
                            m_vec_vertex[ m_vec_face[face].vec_face_pointer.back() ].y,
                            m_vec_vertex[ m_vec_face[face].vec_face_pointer.back() ].z );
                glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer.front() ].x,
                            m_vec_vertex[ m_vec_face[face].vec_face_pointer.front() ].y,
                            m_vec_vertex[ m_vec_face[face].vec_face_pointer.front() ].z );
            }
            glEnd();
            glLineWidth(1);
            //oultine 2
            glLineWidth(2);
            glColor4f(0.2,1,0.2,0.5);
            glBegin(GL_LINES);
            for(int face=0;face<(int)m_vec_face.size();face++)
            {
                for(int vertex=0;vertex<(int)m_vec_face[face].vec_face_pointer.size()-1;vertex++ )
                {
                    glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].x,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].y,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].z );
                    glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex+1] ].x,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex+1] ].y,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex+1] ].z );
                }
                //last to start
                glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer.back() ].x,
                            m_vec_vertex[ m_vec_face[face].vec_face_pointer.back() ].y,
                            m_vec_vertex[ m_vec_face[face].vec_face_pointer.back() ].z );
                glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer.front() ].x,
                            m_vec_vertex[ m_vec_face[face].vec_face_pointer.front() ].y,
                            m_vec_vertex[ m_vec_face[face].vec_face_pointer.front() ].z );
            }
            glEnd();
            glLineWidth(1);

            glDisable(GL_BLEND);

            glDepthMask(true);


            if(draw_to_zbuff)
            {
                //draw again to fill depthbuffer
                glColorMask(false,false,false,false);
                for(int face=0;face<(int)m_vec_face.size();face++)
                {
                    glBegin(GL_TRIANGLE_FAN);//as quads, faces with 4 corners
                    for(int vertex=0;vertex<(int)m_vec_face[face].vec_face_pointer.size();vertex++ )
                     glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].x,
                                 m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].y,
                                 m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].z );
                    glEnd();
                }
                glColorMask(true,true,true,true);
            }

        }break;

        case dt_station://solid faces with outline
        {
            glEnable(GL_BLEND);
            glDepthMask(false);

            //draw faces
            //glColor3f(0.5,0.5,0.5);
            glColor4f(0.5,0.5,0.5,0.5);
            for(int face=0;face<(int)m_vec_face.size();face++)
            {
                glBegin(GL_TRIANGLE_FAN);//as quads, faces with 4 corners
                for(int vertex=0;vertex<(int)m_vec_face[face].vec_face_pointer.size();vertex++ )
                 glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].x,
                             m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].y,
                             m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].z );
                glEnd();
            }

            //outline
            glLineWidth(4);
            //glColor3f(0.7,0.7,0.7);
            glColor4f(0.7,0.7,0.7,0.5);
            glBegin(GL_LINES);
            for(int face=0;face<(int)m_vec_face.size();face++)
            {
                for(int vertex=0;vertex<(int)m_vec_face[face].vec_face_pointer.size()-1;vertex++ )
                {
                    glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].x,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].y,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].z );
                    glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex+1] ].x,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex+1] ].y,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex+1] ].z );
                }
            }
            glEnd();
            glLineWidth(1);
            //oultine 2
            glLineWidth(2);
            //glColor3f(0.9,0.9,0.9);
            glColor4f(0.9,0.9,0.9,0.5);
            glBegin(GL_LINES);
            for(int face=0;face<(int)m_vec_face.size();face++)
            {
                for(int vertex=0;vertex<(int)m_vec_face[face].vec_face_pointer.size()-1;vertex++ )
                {
                    glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].x,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].y,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].z );
                    glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex+1] ].x,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex+1] ].y,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex+1] ].z );
                }
            }
            glEnd();
            glLineWidth(1);

            glDisable(GL_BLEND);
            glDepthMask(true);

            //draw to zbuff
            glColorMask(false,false,false,false);
            for(int face=0;face<(int)m_vec_face.size();face++)
            {
                glBegin(GL_TRIANGLE_FAN);//as quads, faces with 4 corners
                for(int vertex=0;vertex<(int)m_vec_face[face].vec_face_pointer.size();vertex++ )
                 glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].x,
                             m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].y,
                             m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].z );
                glEnd();
            }
            glColorMask(true,true,true,true);
        }break;

        case dt_menu_robot://transparant faces with outline, weaker colors
        {
            glDepthMask(false);//will continue to draw this models polygons ignoring the depthtest for this models polygons whitout ignoring other drawn objects
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glColor4f(0,0.4,0,0.3);
            for(int face=0;face<(int)m_vec_face.size();face++)
            {
                glBegin(GL_TRIANGLE_FAN);//as quads, faces with 4 corners
                for(int vertex=0;vertex<(int)m_vec_face[face].vec_face_pointer.size();vertex++ )
                 glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].x,
                             m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].y,
                             m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].z );
                glEnd();
            }

            //outline
            glLineWidth(2);
            glColor4f(0,0.5,0,0.4);
            glBegin(GL_LINES);
            for(int face=0;face<(int)m_vec_face.size();face++)
            {
                for(int vertex=0;vertex<(int)m_vec_face[face].vec_face_pointer.size()-1;vertex++ )
                {
                    glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].x,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].y,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].z );
                    glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex+1] ].x,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex+1] ].y,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex+1] ].z );
                }
                //last to start
                glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer.back() ].x,
                            m_vec_vertex[ m_vec_face[face].vec_face_pointer.back() ].y,
                            m_vec_vertex[ m_vec_face[face].vec_face_pointer.back() ].z );
                glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer.front() ].x,
                            m_vec_vertex[ m_vec_face[face].vec_face_pointer.front() ].y,
                            m_vec_vertex[ m_vec_face[face].vec_face_pointer.front() ].z );
            }
            glEnd();
            glLineWidth(1);
            //oultine 2
            glLineWidth(1);
            glColor4f(0.1,0.7,0.1,0.5);
            glBegin(GL_LINES);
            for(int face=0;face<(int)m_vec_face.size();face++)
            {
                for(int vertex=0;vertex<(int)m_vec_face[face].vec_face_pointer.size()-1;vertex++ )
                {
                    glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].x,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].y,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].z );
                    glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex+1] ].x,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex+1] ].y,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex+1] ].z );
                }
                //last to start
                glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer.back() ].x,
                            m_vec_vertex[ m_vec_face[face].vec_face_pointer.back() ].y,
                            m_vec_vertex[ m_vec_face[face].vec_face_pointer.back() ].z );
                glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer.front() ].x,
                            m_vec_vertex[ m_vec_face[face].vec_face_pointer.front() ].y,
                            m_vec_vertex[ m_vec_face[face].vec_face_pointer.front() ].z );
            }
            glEnd();
            glLineWidth(1);

            glDisable(GL_BLEND);

            glDepthMask(true);


            if(draw_to_zbuff)
            {
                //draw again to fill depthbuffer
                glColorMask(false,false,false,false);
                for(int face=0;face<(int)m_vec_face.size();face++)
                {
                    glBegin(GL_TRIANGLE_FAN);//as quads, faces with 4 corners
                    for(int vertex=0;vertex<(int)m_vec_face[face].vec_face_pointer.size();vertex++ )
                     glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].x,
                                 m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].y,
                                 m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].z );
                    glEnd();
                }
                glColorMask(true,true,true,true);
            }

        }break;

        case dt_menu_station://solid faces with outline, weaker
        {
            glEnable(GL_BLEND);
            glDepthMask(false);

            //draw faces
            //glColor3f(0.5,0.5,0.5);
            glColor4f(0.5,0.5,0.5,0.5);
            for(int face=0;face<(int)m_vec_face.size();face++)
            {
                glBegin(GL_TRIANGLE_FAN);//as quads, faces with 4 corners
                for(int vertex=0;vertex<(int)m_vec_face[face].vec_face_pointer.size();vertex++ )
                 glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].x,
                             m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].y,
                             m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].z );
                glEnd();
            }

            //outline
            glLineWidth(3);
            //glColor3f(0.7,0.7,0.7);
            glColor4f(0.5,0.5,0.5,0.3);
            glBegin(GL_LINES);
            for(int face=0;face<(int)m_vec_face.size();face++)
            {
                for(int vertex=0;vertex<(int)m_vec_face[face].vec_face_pointer.size()-1;vertex++ )
                {
                    glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].x,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].y,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].z );
                    glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex+1] ].x,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex+1] ].y,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex+1] ].z );
                }
            }
            glEnd();
            glLineWidth(1);
            //oultine 2
            glLineWidth(1);
            //glColor3f(0.9,0.9,0.9);
            glColor4f(0.7,0.7,0.7,0.5);
            glBegin(GL_LINES);
            for(int face=0;face<(int)m_vec_face.size();face++)
            {
                for(int vertex=0;vertex<(int)m_vec_face[face].vec_face_pointer.size()-1;vertex++ )
                {
                    glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].x,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].y,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].z );
                    glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex+1] ].x,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex+1] ].y,
                                m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex+1] ].z );
                }
            }
            glEnd();
            glLineWidth(1);

            glDisable(GL_BLEND);
            glDepthMask(true);

            //draw to zbuff
            glColorMask(false,false,false,false);
            for(int face=0;face<(int)m_vec_face.size();face++)
            {
                glBegin(GL_TRIANGLE_FAN);//as quads, faces with 4 corners
                for(int vertex=0;vertex<(int)m_vec_face[face].vec_face_pointer.size();vertex++ )
                 glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].x,
                             m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].y,
                             m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].z );
                glEnd();
            }
            glColorMask(true,true,true,true);
        }break;
    }




    /*glBegin(GL_QUADS);//as quads, faces with 4 corners
    for(int quad=0;quad<(int)m_vec_face.size();quad++)
    {
        glVertex3f( m_vec_vertex[ m_vec_face[quad].first ].x,
                    m_vec_vertex[ m_vec_face[quad].first ].y,
                    m_vec_vertex[ m_vec_face[quad].first ].z );
        glVertex3f( m_vec_vertex[ m_vec_face[quad].second ].x,
                    m_vec_vertex[ m_vec_face[quad].second ].y,
                    m_vec_vertex[ m_vec_face[quad].second ].z );
        glVertex3f( m_vec_vertex[ m_vec_face[quad].third ].x,
                    m_vec_vertex[ m_vec_face[quad].third ].y,
                    m_vec_vertex[ m_vec_face[quad].third ].z );
        glVertex3f( m_vec_vertex[ m_vec_face[quad].fourth ].x,
                    m_vec_vertex[ m_vec_face[quad].fourth ].y,
                    m_vec_vertex[ m_vec_face[quad].fourth ].z );
    }
    glEnd();*/

    /*cout<<"vert\n";
    for(int i=0;i<(int)m_vec_vertex.size();i++)
    {
        cout<<m_vec_vertex[i].x<<", "<<m_vec_vertex[i].y<<", "<<m_vec_vertex[i].z<<endl;
    }
    cout<<"face\n";
    for(int i=0;i<(int)m_vec_face.size();i++)
    {
        cout<<m_vec_face[i].first<<", "<<m_vec_face[i].second<<", "<<m_vec_face[i].third<<", "<<m_vec_face[i].fourth<<endl;
    }*/

    return true;
}

bool model_obj::draw_to_zbuff(void)
{
    glColorMask(false,false,false,false);
    for(int face=0;face<(int)m_vec_face.size();face++)
    {
        glBegin(GL_TRIANGLE_FAN);//as quads, faces with 4 corners
        for(int vertex=0;vertex<(int)m_vec_face[face].vec_face_pointer.size();vertex++ )
         glVertex3f( m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].x,
                     m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].y,
                     m_vec_vertex[ m_vec_face[face].vec_face_pointer[vertex] ].z );
        glEnd();
    }
    glColorMask(true,true,true,true);

    return true;
}

int model_obj::get_error(void)
{
    return m_error;
}
