#include "editor.h"

Editor::~Editor()
{
    background = NULL;
    screen = NULL;
    update_required = false;
    loop_done = true;

    video_mode = 0;
    video_mode_flags = 0;
    video_mode_ready = 0;
}

void
Editor::Load_Tracks(Grid3D & grid)
{
    char fcadena[300];
    string ruta = getcwd(fcadena,300);
    int j=0;

    list< string > directorios = Get_Directories(DIR_TRACKS);
    for (list< string >::iterator itr = directorios.begin(), end = directorios.end();
         itr != end ; ++itr)
    {
        fprintf(stderr,"==================================\n");
        fprintf(stderr,"cargando pista \"%s\"\n",(char*)(*itr).c_str());
        string ruta_nueva = ruta + "\\";
               ruta_nueva += DIR_TRACKS;
               ruta_nueva += "\\";
               ruta_nueva += *itr;
        chdir(ruta_nueva.c_str());

        FILE *fich = fopen("config.cfg","r");
        if(fich == NULL) continue;
        else fclose(fich);

        string con = "config.cfg";
        Track * track = new Track(con);
        (*track).Set_Name(*itr);
        grid.Add_Track(track);
        tracks_map[*itr] = track;
    }
    chdir(ruta.c_str());
}


int
Editor::Run()
{
    if(!screen) return 0;
//    if(!video_mode_ready) return 0;

    FPSmanager manager;
    SDL_initFramerate(&manager);
    SDL_setFramerate(&manager, 30);

    //event flags
    int status = EDITOR_STATUS_NONE;
    bool mode_displacement_on;
    bool move_grid = 0, done;

//    printf("Setting window icon... \0");
//    SDL_WM_SetIcon(IMG_Load("textures/icon.gif"), NULL);

    //point3d para detectar error
    Error_Track_List circuit_error;

    // ///////////////////////////////////////////////////////////////////////
    printf("Loading background and cursors... \n");
    background = image_collection(EDITOR_background);
    SDL_Surface* cursor_default = image_collection(CURSOR_DEFAULT);
    SDL_Surface* cursor_hand_open = image_collection(CURSOR_HAND_OPEN);
    SDL_Surface* cursor_hand_close = image_collection(CURSOR_HAND_CLOSE);
    cursor = cursor_default;
    cursor_rect.Set_values(0,0,48,48);
    Rect2D dstrect;
    printf("Loading background and cursors... ok\n");

    // ///////////////////////////////////////////////////////////////////////
    SDL_Surface* void_icon  = image_collection(TRACK_icon_void);
    SDL_Surface* void_icon3d  = image_collection(TRACK_icon3d_void);
    Track null_track("Void",void_icon,void_icon3d);

    printf("Creating tracks grid... \0");
    Grid3D tools("Tools",TOOLS_X,TOOLS_Y,TOOLS_Z,
        EDITOR_TRACKS_WINDOW_X, EDITOR_TRACKS_WINDOW_Y,
        EDITOR_TRACKS_WINDOW_W, EDITOR_TRACKS_WINDOW_H);
    tools.Set_BackGround(GRID_tools_backg);
    Load_Tracks(tools);
    printf("ok\n");
    // ///////////////////////////////////////////////////////////////////////

    printf("Creatring circuit grid... \0");
    Grid3D world("Mundo",GRID_X,GRID_Y,GRID_Z,
        EDITOR_CIRCUIT_WINDOW_X, EDITOR_CIRCUIT_WINDOW_Y,
        EDITOR_CIRCUIT_WINDOW_W, EDITOR_CIRCUIT_WINDOW_H);
    printf("ok\n");
    world.Set_BackGround(GRID_world_backg);
    world.Set_Minimap_Window(
        EDITOR_CIRCUIT_MINIMAP_WINDOW_X, EDITOR_CIRCUIT_MINIMAP_WINDOW_Y,
        EDITOR_CIRCUIT_MINIMAP_WINDOW_W, EDITOR_CIRCUIT_MINIMAP_WINDOW_H);

    world.Load("terreno.cfg",tracks_map);



    // ///////////////////////////////////////////////////////////////////////
    // Buttons
    // ///////////////////////////////////////////////////////////////////////
    Button button_new_circuit("New Circuit",4,174,
                    "menu/button_new_def.png","menu/button_new_press.png",
                    "menu/button_new_over.png","menu/button_new_dis.png",
                    New_Circuit,(Grid3D*)&world);
    Add_Button(button_new_circuit, SDLK_n);

    Button button_load_circuit("Load Circuit",4,226,
                    "menu/button_load_def.png","menu/button_load_press.png",
                    "menu/button_load_over.png","menu/button_load_dis.png",
                    NULL,NULL,BUTTON_STATUS_DISABLED);
    Add_Button(button_load_circuit, SDLK_l);

    Button button_save_circuit("Save Circuit",4,278,
                    "menu/button_save_def.png","menu/button_save_press.png",
                    "menu/button_save_over.png","menu/button_save_dis.png",
                    NULL,NULL,BUTTON_STATUS_DISABLED);
    Add_Button(button_save_circuit, SDLK_s);

    Button button_save_as_circuit("Save Circuit As...",4,330,
                    "menu/button_save_as_def.png","menu/button_save_as_press.png",
                    "menu/button_save_as_over.png","menu/button_save_as_dis.png",
                    NULL,NULL,BUTTON_STATUS_DISABLED);
    Add_Button(button_save_as_circuit, SDLK_a);

    Button button_quit_editor("Quit Editor",4,383,
                    "menu/button_exit2_def.png","menu/button_exit2_press.png",
                    "menu/button_exit2_over.png","menu/button_exit2_dis.png",
                    Quit_Editor,(bool*)&done);
    Add_Button(button_quit_editor, SDLK_ESCAPE);


    pair<Grid3D*, Error_Track_List * > par;
    par.first = &world;
    par.second = &circuit_error;
    void* data = (pair<Grid3D*, Error_Track_List * >*)&par;

    Button button_check_circuit("Check Circuit",4,436,
                    "menu/button_check_def.png","menu/button_check_press.png",
                    "menu/button_check_over.png","menu/button_check_dis.png",
                    Check_Circuit,data);
    Add_Button(button_check_circuit, SDLK_c);

    //bool error_circuit = false;


    Track* floating = NULL;

    //coordenadas del raton
    int ratonx = 0;
    int ratony = 0;


    // program main loop
    printf("Main Loop...\n");
    SDL_Event event;
    for(done = false; !done; update_required=false)
    {
        // message processing loop
        //printf("\tLectura de eventos...\n");
        while (SDL_PollEvent(&event))
        {
            // check for messages
            switch (event.type)
            {
                // exit if the window is closed
                case SDL_QUIT:
                done = true;
                break;

                // check for keypresses
                case SDL_KEYDOWN:
                {
                    Button * button = keys[(int)event.key.keysym.sym];
                    if(button){ (*button).Click(); /*done=true;*/}

                    if (event.key.keysym.sym == SDLK_ESCAPE)
                        done = true;
                    if (event.key.keysym.sym == SDLK_LSHIFT) mode_displacement_on = true;
                    if (event.key.keysym.sym == SDLK_UP)    world.Incr_Offset(0,-CELL_Y,0);
                    if (event.key.keysym.sym == SDLK_DOWN)  world.Incr_Offset(0,CELL_Y,0);
                    if (event.key.keysym.sym == SDLK_LEFT)  world.Incr_Offset(-CELL_X,0,0);
                    if (event.key.keysym.sym == SDLK_RIGHT) world.Incr_Offset(CELL_X,0,0);
                    update_required=true;
                    if (event.key.keysym.sym == SDLK_c)
                    {
                        if (!world.Check_Circuit(circuit_error))
                        {
                            //fprintf(stderr,"CIRCUITO ERRONEO. CASILLA(%d,%d.%d)", circuit_error.x,circuit_error.y,circuit_error.z);
                            //world.Activate_Tracks_Error(circuit_error.x,circuit_error.y,circuit_error.z);
                            if (circuit_error.error_circuit)
                            {
                                world.Deactivate_All_Tracks();
                                for (list< Point3D<int> >::iterator itr = circuit_error.track_error.begin(), end = circuit_error.track_error.end(); itr != end ; ++itr)
                                {
                                    world.Activate_Tracks_Error((*itr).x, (*itr).y, (*itr).z);
                                }
                                if (circuit_error.start_error.size() != 1)
                                {
                                    for (list< Point3D<int> >::iterator itr = circuit_error.start_error.begin(), end = circuit_error.start_error.end(); itr != end ; ++itr)
                                    {
                                        world.Activate_Tracks_Error((*itr).x, (*itr).y, (*itr).z);
                                    }
                                }
                                circuit_error.track_error.clear();
                                circuit_error.start_error.clear();
                                circuit_error.error_circuit = false;
                            }
                        }
                    }
                    if (event.key.keysym.sym == SDLK_m)
                    {
                        if(move_grid == 0)
                        {
                            move_grid = 1;
                            mode_displacement_on = 0;
                            status = EDITOR_STATUS_NONE;
                            if(floating)
                            {
                                delete floating;
                                floating = NULL;
                            }
                            cursor = cursor_hand_open;
                            world.Deactivate_All_Tracks();
                        }
                        else
                        {
                            move_grid = 0;
                            cursor = cursor_default;
                        }
                    }
                    break;
                }
                case SDL_KEYUP:
                {
                    if (event.key.keysym.sym == SDLK_LSHIFT) mode_displacement_on = false;
                    break;
                }

                case SDL_MOUSEBUTTONDOWN:
                {
                    update_required=true;
                    if( event.button.button == (SDL_BUTTON_LEFT | !SDL_BUTTON_RIGHT | !SDL_BUTTON_MIDDLE))
                    {
                        mode_displacement_on = true;
                        cursor = cursor_default;
                        if(move_grid)
                        {
                            if( world.Mouse_Over(event.button.x,event.button.y) ||
                                tools.Mouse_Over(event.button.x,event.button.y))
                                {
                                    cursor = cursor_hand_close;
                                }
                        }
                        else if(floating)
                        {
                            if(world.Set_Track(event.button.x,event.button.y,*floating))
                            {
                                if      (status == EDITOR_STATUS_ADD)
                                {
                                    //*some_track = *floating;
                                }
                                else if (status == EDITOR_STATUS_MOVE)
                                {
                                    //*some_track = *floating;
                                    delete floating;
                                    floating = NULL;
                                }
                            }
                            else
                            {
                                delete floating;
                                floating = NULL;
                                status = EDITOR_STATUS_NONE;
                            }
                        }
                        else
                        {
                            if(floating = tools.Get_Track(event.button.x,event.button.y))
                            {
                                status = EDITOR_STATUS_ADD;
                            }
                            else
                            {
                                if(floating = world.Get_Track(event.button.x,event.button.y))
                                {
                                    status = EDITOR_STATUS_MOVE;
                                    world.Delete_Track(event.button.x,event.button.y);
                                }
                            }
                        }
                    }
                    else if(event.button.button == SDL_BUTTON_RIGHT)
                    {
                        if(floating) (*floating).Rotate_Counter_Clockwise();
                    }

                    pair<int,int> coord(event.button.x,event.button.y);
                    Button* boton = buttons[coord];
                    if(boton)
                    {
                        (*boton).Set_Status(BUTTON_STATUS_PRESSED);
                    }
                    break;
                }
                case SDL_MOUSEBUTTONUP:
                {
                    if (event.button.button == SDL_BUTTON_LEFT)
                    {
                        mode_displacement_on = false;
                        if(move_grid) cursor = cursor_hand_open;
                    }
                    pair<int,int> coord(event.button.x,event.button.y);
                    Button* boton = buttons[coord];
                    if(boton)
                    {
                        //Quit_Menu();
                        (*boton).Set_Status(BUTTON_STATUS_OVER);
                        (*boton).Click();
                        //Set_Video_Mode();
                    }
                    break;
                }
                case SDL_MOUSEMOTION:
                {
                    if(move_grid && cursor == cursor_hand_close)
                    {
                        if( world.Mouse_Over(event.motion.x,event.motion.y))
                            world.Incr_Offset(-event.motion.xrel,-event.motion.yrel,0);
                        if( tools.Mouse_Over(event.motion.x,event.motion.y))
                            tools.Incr_Offset(-event.motion.xrel,-event.motion.yrel,0);
                    }
                    update_required=true;
                    ratonx = event.motion.x;
                    ratony = event.motion.y;
                    // Implementation of drawn by mouse displacement
                    if (floating)
                    {
                        if(!( tools.Mouse_Over(event.motion.x,event.motion.y)))
                        {
                            world.Activate_Tracks_Under(event.button.x,event.button.y,floating);
                            if (mode_displacement_on &&
                                *floating != *world.Get_Track(event.button.x,event.button.y) &&
                                !(*floating == *world.Get_Track(event.button.x+((*floating).Get_DimX()-1)*CELL_X+1,event.button.y) ||
                                  *floating == *world.Get_Track(event.button.x,event.button.y+((*floating).Get_DimY()-1)*CELL_Y+1)))
                            {
                                if(world.Set_Track(event.button.x,event.button.y,*floating))
                                {
                                    if      (status == EDITOR_STATUS_ADD)
                                    {
                                    }
                                    else if (status == EDITOR_STATUS_MOVE)
                                    {
                                        delete floating;
                                        floating = NULL;
                                    }
                                }
                                else
                                {
                                    delete floating;
                                    floating = NULL;
                                    status = EDITOR_STATUS_NONE;
                                }
                            }
                        }
                    }
                    //gestion de botones
                    cursor_rect.Set_XY(event.motion.x,event.motion.y);
                    pair<int,int> coord(event.motion.x,event.motion.y);
                    Button* boton = buttons[coord];
                    if(boton)
                    {
                        (*boton).Set_Status(BUTTON_STATUS_OVER);
                        if(actual_button)
                        {
                            if(actual_button != boton)
                            {
                                (*actual_button).Set_Status(BUTTON_STATUS_DEFAULT);
                                actual_button = boton;
                            }
                        }
                        else actual_button = boton;
                    }
                    else if(actual_button)
                    {
                        (*actual_button).Set_Status(BUTTON_STATUS_DEFAULT);
                        actual_button = NULL;
                    }
                    // End of implementation of drawn by mouse displacement
                    break;
                }
            } // end switch
        } // end of message processing

        //printf("\tRenderizado...\n");
        if(update_required)
        {
            // DRAWING STARTS HERE
            // clear screen
            //SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));

            // draw bitmap
            dstrect.x = 0;
            dstrect.y = 0;
            SDL_BlitSurface(background, 0, screen, &dstrect);
            //blit_list.push_back(New_Blit_Task(background,dstrect,screen);

            world.Draw(screen);
            tools.Draw(screen);
            Draw();

            if(floating)
            {
                dstrect.x = ratonx - CELL_X/2;
                dstrect.y = ratony - CELL_Y/2;
                (*floating).Draw_Icon3D(screen,EDITOR_ICON3D_X,EDITOR_ICON3D_Y);
                (*floating).Draw(screen,dstrect);
            }

            cursor_rect.x = ratonx;
            cursor_rect.y = ratony;
            cursor_rect.w = 48;
            cursor_rect.h = 48;
            SDL_SetClipRect(screen, &cursor_rect);
            SDL_BlitSurface(cursor, 0, screen, &cursor_rect);
            SDL_SetClipRect(screen, NULL);

            SDL_GL_SwapBuffers();
            SDL_Flip(screen);
        }
        SDL_framerateDelay(&manager);
        update_required=0;
    } // end main loop

//    if(path_to_save_circuit)
//    {
//        printf("Saving Circuit... \0");
//        if(world.Save(path_to_save_circuit))    printf("ok\n");
//        else                                    printf("FAILED\n");
//    }
    world.Save("terreno.cfg");

    // all is well ;)
    printf("Editor Terminated cleanly.\n");
    //SDL_Quit();
    return 0;
}

void
Editor::Draw()
{
    for(list< Button* >::iterator itr=button_list.begin(), end=button_list.end();
        itr != end; ++itr)
        (*(*itr)).Draw(screen);
}

int
Editor::New_Circuit(void* data)
{
    (*((Grid3D*)data)).Clear_Circuit();
    return 0;
}

int
Editor::Quit_Editor(void* data)
{
    *((bool*)data) = true;
    return 0;
}

int
Editor::Check_Circuit(void* data)
{
    pair<Grid3D*, Error_Track_List * > par;
    par = *((pair<Grid3D*, Error_Track_List * >*)data);
    int a = (*par.first).Check_Circuit(*par.second);
    return a;
}
