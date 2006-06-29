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
Editor::Add_Tracks(Grid3D & grid)
{
    // Carga de iconos de los tramos. esto ira en capsulado y automatizado
    printf("Creatring tracks... \n");

//    (*bank_a.Get_Cell(0,0,0)).Set_West(CONN_PLANE);
//    (*recta.Get_Cell(0,0,0)).Set_South(CONN_PLANE);

    SDL_Surface* icon = NULL;
    SDL_Surface* icon3d = NULL;

    icon = image_collection("tracks/bank_a/icon.png");
    icon3d = image_collection("tracks/bank_a/icon3d.png");
    Track bank_a("bank_a",icon,icon3d,1,1,1,"Bank A");

    icon = image_collection("tracks/bank_b/icon.png");
    icon3d = image_collection("tracks/bank_b/icon3d.png");
    Track bank_b("bank_b",icon,icon3d,1,1,1,"Bank B");

    icon = image_collection("tracks/banked_standard_curve/icon.png");
    icon3d = image_collection("tracks/banked_standard_curve/icon3d.png");
    Track banked_standard_curve("banked_standard_curve",icon,icon3d,2,2,1,"banked_standard_curve");
    banked_standard_curve.Set_I_CheckPoint(true);

    icon = image_collection("tracks/banked_straight/icon.png");
    icon3d = image_collection("tracks/banked_straight/icon3d.png");
    Track banked_straight("banked_straight",icon,icon3d,1,1,1,"banked_straight");

    icon = image_collection("tracks/building1/icon.png");
    icon3d = image_collection("tracks/building1/icon3d.png");
    Track building1("building1",icon,icon3d,1,1,1,"building1");

    icon = image_collection("tracks/straight/icon.png");
    icon3d = image_collection("tracks/straight/icon3d.png");
    Track straight("straight",icon,icon3d,1,1,1,"STRAIGHT");

    icon = image_collection("tracks/start/icon.png");
    icon3d = image_collection("tracks/start/icon3d.png");
    Track start("start",icon,icon3d,1,1,1,"START");
    start.Set_Start(true);

    icon = image_collection("tracks/inner_curve/icon.png");
    icon3d = image_collection("tracks/inner_curve/icon3d.png");
    Track inner_curve("inner_curve",icon,icon3d,1,1,1,"INNER_CURVE");
    inner_curve.Set_I_CheckPoint(true);

    icon = image_collection("tracks/standard_curve/icon.png");
    icon3d = image_collection("tracks/standard_curve/icon3d.png");
    Track standard_curve("standard_curve",icon,icon3d,2,2,1,"standard_curve");
    standard_curve.Set_I_CheckPoint(true);

    icon = image_collection("tracks/cross/icon.png");
    icon3d = image_collection("tracks/cross/icon3d.png");
    Track cross("cross",icon,icon3d,1,1,1,"cross");

    icon = image_collection("tracks/ramp_a/icon.png");
    icon3d = image_collection("tracks/ramp_a/icon3d.png");
    Track ramp_a("ramp_a",icon,icon3d,2,1,1,"ramp_a");

    icon = image_collection("tracks/ramp_b/icon.png");
    icon3d = image_collection("tracks/ramp_b/icon3d.png");
    Track ramp_b("ramp_b",icon,icon3d,2,1,1,"ramp_b");

    icon = image_collection("tracks/loop/icon.png");
    icon3d = image_collection("tracks/loop/icon3d.png");
    Track loop("loop",icon,icon3d,2,2,1,"loop");

    icon = image_collection("tracks/tree/icon.png");
    icon3d = image_collection("tracks/tree/icon3d.png");
    Track tree("tree",icon,icon3d,1,1,1,"tree");

    printf("Creatring tracks... ok\n");

    printf("Pushing tracks into tracks grid... \0");
    grid.Set_Track(0,0,0,straight);
    grid.Set_Track(1,0,0,inner_curve);
    grid.Set_Track(2,0,0,cross);
    grid.Set_Track(3,0,0,building1);
    grid.Set_Track(0,6,0,start);

    grid.Set_Track(0,1,0,bank_a);
    grid.Set_Track(1,1,0,banked_straight);
    grid.Set_Track(2,1,0,bank_b);
    grid.Set_Track(3,1,0,tree);

    grid.Set_Track(0,2,0,standard_curve);
    grid.Set_Track(2,2,0,banked_standard_curve);

    grid.Set_Track(0,4,0,loop);
    grid.Set_Track(2,4,0,ramp_a);
    grid.Set_Track(2,5,0,ramp_b);
    printf("ok\n");
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
    bool level = 0;
    int status = EDITOR_STATUS_NONE;
    bool mode_displacement_on; //Variable to implementation of drawn by mouse displacement
    bool move_grid = 0;


//    printf("Setting window icon... \0");
//    SDL_WM_SetIcon(IMG_Load("textures/icon.gif"), NULL);
//    printf("ok\n");

    // create a new window
//    printf("Setting VideoMode... \0");
//    /*SDL_Surface* */screen = SDL_SetVideoMode(
//        VIDEO_width, VIDEO_height, VIDEO_depth,
//        SDL_HWSURFACE|SDL_DOUBLEBUF|FULLSCREEN_FLAG);
////        SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_FULLSCREEN|SDL_OPENGLBLIT);
//    if ( !screen ) { printf("Unable to set %dx%d video: %s\n", VIDEO_width, VIDEO_height, SDL_GetError()); return 1; }
//    printf("ok\n");

    //SDL_ShowCursor(SDL_DISABLE);

    // load an image
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
    SDL_Surface* bbox_icon  = image_collection("textures/track_box.gif");
    SDL_Surface* void_icon  = image_collection(EDITOR_VOID_ICON);
    SDL_Surface* void_icon3d  = image_collection(EDITOR_VOID_ICON3D);
    Track null_track("Void",void_icon,void_icon3d);

    printf("Creating tracks grid... \0");
    Grid3D tools("Tools",4,7,1,
        EDITOR_TRACKS_WINDOW_X, EDITOR_TRACKS_WINDOW_Y,
        EDITOR_TRACKS_WINDOW_W, EDITOR_TRACKS_WINDOW_H);
    printf("ok\n");

    Add_Tracks(tools);
    fprintf(stderr,"Pistas anyadidas a Tools\n");
    fprintf(stderr,"------------------------\n");
    // ///////////////////////////////////////////////////////////////////////

    printf("Creatring circuit grid... \0");
    Grid3D world("Mundo",GRID_X,GRID_Y,2,//GRID_Z,
        EDITOR_CIRCUIT_WINDOW_X, EDITOR_CIRCUIT_WINDOW_Y,
        EDITOR_CIRCUIT_WINDOW_W, EDITOR_CIRCUIT_WINDOW_H);
    printf("ok\n");

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
//                    if (event.key.keysym.sym == SDLK_UP)    world.Decr_Offset_Y();
//                    if (event.key.keysym.sym == SDLK_DOWN)  world.Incr_Offset_Y();
//                    if (event.key.keysym.sym == SDLK_LEFT)  world.Decr_Offset_X();
//                    if (event.key.keysym.sym == SDLK_RIGHT) world.Incr_Offset_X();
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
                            if( event.button.x >= EDITOR_CIRCUIT_WINDOW_X &&
                                event.button.x< EDITOR_CIRCUIT_WINDOW_X + EDITOR_CIRCUIT_WINDOW_W &&
                                event.button.y >= EDITOR_CIRCUIT_WINDOW_Y &&
                                event.button.y< EDITOR_CIRCUIT_WINDOW_Y + EDITOR_CIRCUIT_WINDOW_H)
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
//                                if(!( event.button.x >= EDITOR_TRACKS_WINDOW_X &&
//                                    event.button.x< EDITOR_TRACKS_WINDOW_X + EDITOR_TRACKS_WINDOW_W &&
//                                    event.button.y >= EDITOR_TRACKS_WINDOW_Y &&
//                                    event.button.y< EDITOR_TRACKS_WINDOW_Y + EDITOR_TRACKS_WINDOW_H))
                                {
                                    delete floating;
                                    floating = NULL;
                                    status = EDITOR_STATUS_NONE;
                                }
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
                        world.Incr_Offset(-event.motion.xrel,-event.motion.yrel,0);
                    }
                    update_required=true;
                    ratonx = event.motion.x;
                    ratony = event.motion.y;
                    // Implementation of drawn by mouse displacement
                    if (floating)
                    {
                        if(!( event.button.x >= EDITOR_TRACKS_WINDOW_X &&
                            event.button.x< EDITOR_TRACKS_WINDOW_X + EDITOR_TRACKS_WINDOW_W &&
                            event.button.y >= EDITOR_TRACKS_WINDOW_Y &&
                            event.button.y< EDITOR_TRACKS_WINDOW_Y + EDITOR_TRACKS_WINDOW_H))
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
            SDL_BlitSurface(cursor, 0, screen, &cursor_rect);

            SDL_GL_SwapBuffers();
            SDL_Flip(screen);
        }
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

