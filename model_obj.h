#ifndef MODEL_OBJ_H
#define MODEL_OBJ_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <gl/gl.h>
#include <stdlib.h>
#include <cctype>
#include "base64.h"
#include "files_in_text.h"
#include "definitions.h"

//Will load vertices and faces from an Wavefront .obj file and store data for drawing

using namespace std;

struct st_point_xyz
{
    st_point_xyz(float _x,float _y,float _z)
    {
        x=_x; y=_y; z=_z;
    }

    float x,y,z;
};

struct st_face_quad
{
    st_face_quad(int _first,int _second,int _third,int _fourth)
    {
        first=_first;
        second=_second;
        third=_third;
        fourth=_fourth;
    }

    int first,second,third,fourth;
};

struct st_face_vec
{
    vector<int> vec_face_pointer;
};

class model_obj
{
    public:
        model_obj();
        model_obj(string filepath);
        model_obj(int model_id);

        bool draw_settings(int draw_type);
        bool draw(bool draw_to_zbuff=true);
        bool draw_to_zbuff(void);
        int  get_error(void);

    private:

        int  m_error;
        int  m_draw_type;

        vector<st_point_xyz> m_vec_vertex;
        vector<st_face_vec> m_vec_face;
};

#endif // MODEL_OBJ_H
