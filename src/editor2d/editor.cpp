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
    bool move_grid = 0;

//    printf("Setting window icon... \0");
//    SDL_WM_SetIcon(IMG_Load("textures/icon.gif"), NULL);


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

//    int New_Circuit(void* data)
//    {
//        //(*((Grid3D*)data)).
//        return 0;
//    };

    // ///////////////////////////////////////////////////////////////////////
    // Buttons
    // ///////////////////////////////////////////////////////////////////////
//    Button new_circuit("New Circuit",0,194,
//                    "menu/button_new__def.png","menu/button_new__press.png",
//                    "menu/button_new__over.png","menu/button_new__dis.png",
//                    New_Circuit,(Grid3D*)world);
//    Add_Button(new_circuit, SDLK_n);









    Track* floating = NULL;

    //coordenadas del raton
    int ratonx = 0;
    int ratony = 0;

    // program main loop
    printf("Main Loop...\n");
    SDL_Event event;
    for(bool done = false; !done; update_required=false)
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
                    // exit if ESCAPE is pressed
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                    done = true;
                    if (event.key.keysym.sym == SDLK_LSHIFT) mode_displacement_on = true;
                    if (event.key.keysym.sym == SDLK_UP)    world.Incr_Offset(0,-CELL_Y,0);
                    if (event.key.keysym.sym == SDLK_DOWN)  world.Incr_Offset(0,CELL_Y,0);
                    if (event.key.keysym.sym == SDLK_LEFT)  world.Incr_Offset(-CELL_X,0,0);
                    if (event.key.keysym.sym == SDLK_RIGHT) world.Incr_Offset(CELL_X,0,0);
                    update_required=true;
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

                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDLK_LSHIFT) mode_displacement_on = false;
                    break;

                case SDL_MOUSEBUTTONDOWN:
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
                    break;

                case SDL_MOUSEBUTTONUP:
                    if (event.button.button == SDL_BUTTON_LEFT)
                    {
                        mode_displacement_on = false;
                        if(move_grid) cursor = cursor_hand_open;
                    }
                    break;

                case SDL_MOUSEMOTION:
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
                    // End of implementation of drawn by mouse displacement
                    break;
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

