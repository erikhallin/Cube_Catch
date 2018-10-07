#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <fcntl.h> //for console
#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>

#include "resource.h"
#include "game.h"

//4:3
 //int   g_wind_w=1024;
 //int   g_wind_h=768;
//16:9
 int   g_wind_w=800;
 int   g_wind_h=450;
float g_fov=60.0;
bool* g_pKeys;

game* Game;

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*,bool vsync);
void DisableOpenGL(HWND, HDC, HGLRC);


int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    string command_line=lpCmdLine;
    bool debug_mode=false;
    bool vsync=true;
    int  numof_players=1;//temp
    float time_per_turn=_play_time_per_turn_static;//temp
    float time_per_turn_static=-1.0;//temp, -1 to see if default in use
    float time_per_turn_dynamic_coef=_play_time_per_turn_dynamic_coef;//temp
    float time_per_turn_dynamic_const=_play_time_per_turn_dynamic_const;//temp
    float player_swap_time=_player_swap_time;//temp
    //get commands
    vector<string> vec_commands;
    string word;
    stringstream ss(command_line);
    while(ss>>word)
    {
        vec_commands.push_back(word);
    }
    //interpret commands, "command=value"
    for(int com=0;com<(int)vec_commands.size();com++)
    {
        string command,s_value;
        //int i_value=0;
        //float f_value=0.0;
        //cut after '='
        for(int char_i=0;char_i<(int)vec_commands[com].size();char_i++)
        {
            if(vec_commands[com][char_i]=='=')
            {
                command=string(vec_commands[com],0,char_i);
                s_value=string(vec_commands[com],char_i+1);
                break;
            }
        }
        //interpret
        if(vec_commands[com]=="debug")
        {
            debug_mode=true;
        }
        if(command=="numof_players")
        {
            numof_players=atoi(s_value.c_str());
            if(numof_players<1) numof_players=1;
            if(numof_players>9) numof_players=9;
        }
        if(command=="time_per_turn_static")
        {
            time_per_turn_static=atof(s_value.c_str());
            if(time_per_turn_static<1.0) time_per_turn_static=1.0;
        }
        if(command=="time_per_turn_dynamic_coef")
        {
            time_per_turn_dynamic_coef=atof(s_value.c_str());
            //if(time_per_turn_dynamic_coef<1.0) time_per_turn_dynamic_coef=1.0;
        }
        if(command=="time_per_turn_dynamic_const")
        {
            time_per_turn_dynamic_const=atof(s_value.c_str());
            //if(time_per_turn_dynamic_const<1.0) time_per_turn_dynamic_const=1.0;
        }
        if(command=="player_swap_time")
        {
            player_swap_time=atof(s_value.c_str());
            if(player_swap_time<0.0) player_swap_time=1.0;
        }
        if(command=="fov")
        {
            g_fov=atof(s_value.c_str());
            if(g_fov<0.0) g_fov=60.0;
        }
        if(vec_commands[com]=="vsync_off")
        {
            vsync=false;
        }
    }
    //test if to use default values
    if(time_per_turn_static==-1.0)
    {
        //use dynamic timeing
        time_per_turn=numof_players*time_per_turn_dynamic_coef+time_per_turn_dynamic_const;
        //cull value
        if(numof_players==1) time_per_turn=_play_time_per_turn_static;//why swap?
        if(numof_players==2) time_per_turn=10;
    }

    //if debug mode start console
    if(debug_mode)
    {
        //Open a console window
        AllocConsole();
        //Connect console output
        HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
        int hCrt          = _open_osfhandle((long) handle_out, _O_TEXT);
        FILE* hf_out      = _fdopen(hCrt, "w");
        setvbuf(hf_out, NULL, _IONBF, 1);
        *stdout = *hf_out;
        //Connect console input
        HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
        hCrt             = _open_osfhandle((long) handle_in, _O_TEXT);
        FILE* hf_in      = _fdopen(hCrt, "r");
        setvbuf(hf_in, NULL, _IONBF, 128);
        *stdin = *hf_in;
        //Set console title
        SetConsoleTitle("Debug Console");

        cout<<"Input Command: ";
        for(int com_i=0;com_i<(int)vec_commands.size();com_i++)
        {
            cout<<vec_commands[com_i]<<" ";
        }
        cout<<"\n\n";
    }
    else
    {
        //set fullscreen
        //Detect screen resolution
        RECT desktop;
        // Get a handle to the desktop window
        const HWND hDesktop = GetDesktopWindow();
        // Get the size of screen to the variable desktop
        GetWindowRect(hDesktop, &desktop);
        // The top left corner will have coordinates (0,0)
        // and the bottom right corner will have coordinates
        // (horizontal, vertical)
        g_wind_w = desktop.right;
        g_wind_h = desktop.bottom;

        //hide cursor
        ShowCursor(FALSE);
    }

    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    //wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(IDI_CUB));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "CubeCatch";
    //wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hIconSm = LoadIcon(NULL, NULL);


    if (!RegisterClassEx(&wcex))
        return 0;

    //create main window
    if(debug_mode)
     hwnd = CreateWindowEx(0,
                           "CubeCatch",
                           "CubeCatch",
                           WS_OVERLAPPEDWINDOW,//normal window
                           CW_USEDEFAULT,
                           CW_USEDEFAULT,
                           g_wind_w,
                           g_wind_h,
                           NULL,
                           NULL,
                           hInstance,
                           NULL);
    else
     hwnd = CreateWindowEx(0,
                           "CubeCatch",
                           "CubeCatch",
                           WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,//no borders
                           CW_USEDEFAULT,
                           CW_USEDEFAULT,
                           g_wind_w,
                           g_wind_h,
                           NULL,
                           NULL,
                           hInstance,
                           NULL);

    ShowWindow(hwnd, nCmdShow);

    EnableOpenGL(hwnd, &hDC, &hRC, vsync);

    //reset keys
    g_pKeys=new bool[256];
    for(int i=0;i<256;i++) g_pKeys[i]=false;
    //init game
    Game=new game();
    if ( !Game->init(g_pKeys,numof_players,time_per_turn,player_swap_time,g_fov,(float)g_wind_w/(float)g_wind_h) ) return false;

    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            //game cycle

            Game->update();

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            glPushMatrix();
            Game->draw();
            glPopMatrix();

            //glFlush();
            //glFinish();

            SwapBuffers(hDC);
        }
    }

    DisableOpenGL(hwnd, hDC, hRC);

    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
        {
            g_pKeys[wParam]=true;

            //temp
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                break;
            }
        }break;

        case WM_KEYUP:
        {
            g_pKeys[wParam]=false;
        }break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC, bool vsync)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    *hDC = GetDC(hwnd);

    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);

    gluPerspective(g_fov, (float)g_wind_w/(float)g_wind_h , 0.001f, 100.0f);
    //glOrtho(-10,10,-10,10 , 0.001f, 100.0f);

    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    //fog
    //int filter;                      // Which Filter To Use
    int fogMode[]= { GL_EXP, GL_EXP2, GL_LINEAR };   // Storage For Three Types Of Fog
    int fogfilter= 0;                    // Which Fog To Use
    float fogColor[4]= {0.0f, 0.0f, 0.0f, 1.0f};      // Fog Color
    glFogi(GL_FOG_MODE, fogMode[fogfilter]);        // Fog Mode
    glFogfv(GL_FOG_COLOR, fogColor);            // Set Fog Color
    glFogf(GL_FOG_DENSITY, 0.15f);              // How Dense Will The Fog Be
    glHint(GL_FOG_HINT, GL_DONT_CARE);          // Fog Hint Value
    glFogf(GL_FOG_START, 3.0f);             // Fog Start Depth
    glFogf(GL_FOG_END, 20.0f);               // Fog End Depth
    glEnable(GL_FOG);                   // Enables GL_FOG

    glEnable( GL_LINE_SMOOTH );
    glEnable( GL_POLYGON_SMOOTH );

    //vsync
    if(vsync)
    {
        typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALPROC)( int );
        PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT = 0;
        const char *extensions = (char*)glGetString( GL_EXTENSIONS );
        if( strstr( extensions, "WGL_EXT_swap_control" ) == 0 )
        {
            cout<<"ERROR: No Vsync support\n";
        }
        else
        {
            wglSwapIntervalEXT = (PFNWGLSWAPINTERVALPROC)wglGetProcAddress( "wglSwapIntervalEXT" );

            if( wglSwapIntervalEXT )
                wglSwapIntervalEXT(true);
        }
    }
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}

