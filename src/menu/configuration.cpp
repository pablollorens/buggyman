#include"configuration.h"

Configuration::~Configuration()
{
    background = NULL;
    screen = NULL;
    update_required = false;
    loop_done = true;

    video_mode = 0;
    video_mode_flags = 0;
    video_mode_ready = 0;
}

int
Configuration::Run()
{
    if(!screen) return 0;
    if(!video_mode_ready) return 0;

    FPSmanager manager;
    SDL_initFramerate(&manager);
    SDL_setFramerate(&manager, 30);

    Set_Background("menu/backg_configuration.jpg");

    Button button_quit("Quit Configuration",376,496,
                    "menu/button_exit2_def.png","menu/button_exit2_press.png",
                    "menu/button_exit2_over.png","menu/button_exit2_dis.png",
                    Quit,(bool*)&loop_done);
    Add_Button(button_quit, SDLK_ESCAPE);







    resolutions_index = 0;
    fullscreen_index = 0;
    sky_index = 0;
    scenery_index = 0;
    draw_ratios_index = 0;

    resolutions_rect.Set_values(488,156,200,48);
    fullscreen_rect.Set_values(488,215,200,48);
    sky_rect.Set_values(488,276,200,48);
    scenery_rect.Set_values(488,337,200,48);
    draw_ratios_rect.Set_values(488,398,200,48);

    sub_Config *some;

    some = new sub_Config;
    some->def = image_collection("menu/sprite_resolution_320_def.png");
    some->press = image_collection("menu/sprite_resolution_320_press.png");
    some->x = 320;
    some->y = 240;
    resolutions.push_back(some);

    some = new sub_Config;
    some->def = image_collection("menu/sprite_resolution_512_def.png");
    some->press = image_collection("menu/sprite_resolution_512_press.png");
    some->x = 512;
    some->y = 384;
    resolutions.push_back(some);

    some = new sub_Config;
    some->def = image_collection("menu/sprite_resolution_640_def.png");
    some->press = image_collection("menu/sprite_resolution_640_press.png");
    some->x = 640;
    some->y = 480;
    resolutions.push_back(some);

    some = new sub_Config;
    some->def = image_collection("menu/sprite_resolution_800_def.png");
    some->press = image_collection("menu/sprite_resolution_800_press.png");
    some->x = 800;
    some->y = 600;
    resolutions.push_back(some);

    some = new sub_Config;
    some->def = image_collection("menu/sprite_resolution_1024_def.png");
    some->press = image_collection("menu/sprite_resolution_1024_press.png");
    some->x = 1024;
    some->y = 768;
    resolutions.push_back(some);

    some = new sub_Config;
    some->def = image_collection("menu/sprite_resolution_1280_def.png");
    some->press = image_collection("menu/sprite_resolution_1280_press.png");
    some->x = 1280;
    some->y = 1024;
    resolutions.push_back(some);

    some = new sub_Config;
    some->def = image_collection("menu/sprite_resolution_1600_def.png");
    some->press = image_collection("menu/sprite_resolution_1600_press.png");
    some->x = 1600;
    some->y = 1200;
    resolutions.push_back(some);








    some = new sub_Config;
    some->def = image_collection("menu/sprite_off_def.png");
    some->press = image_collection("menu/sprite_off_press.png");
    some->fullscreen = 0;
    fullscreen.push_back(some);

    some = new sub_Config;
    some->def = image_collection("menu/sprite_on_def.png");
    some->press = image_collection("menu/sprite_on_press.png");
    some->fullscreen = 1;
    fullscreen.push_back(some);






    some = new sub_Config;
    some->def = image_collection("menu/sprite_off_def.png");
    some->press = image_collection("menu/sprite_off_press.png");
    some->sky = 0;
    sky.push_back(some);

    some = new sub_Config;
    some->def = image_collection("menu/sprite_on_def.png");
    some->press = image_collection("menu/sprite_on_press.png");
    some->sky = 1;
    sky.push_back(some);






    some = new sub_Config;
    some->def = image_collection("menu/sprite_random_def.png");
    some->press = image_collection("menu/sprite_random_press.png");
    some->scenery = HOUR_RANDOM;
    scenery.push_back(some);

    some = new sub_Config;
    some->def = image_collection("menu/sprite_day_def.png");
    some->press = image_collection("menu/sprite_day_press.png");
    some->scenery = HOUR_DAY;
    scenery.push_back(some);

    some = new sub_Config;
    some->def = image_collection("menu/sprite_night_def.png");
    some->press = image_collection("menu/sprite_night_press.png");
    some->scenery = HOUR_NIGHT;
    scenery.push_back(some);






    some = new sub_Config;
    some->def = image_collection("menu/sprite_low_def.png");
    some->press = image_collection("menu/sprite_low_press.png");
    some->draw_ratio = DRAW_RATIO_LOW;
    draw_ratios.push_back(some);

    some = new sub_Config;
    some->def = image_collection("menu/sprite_medium_def.png");
    some->press = image_collection("menu/sprite_medium_press.png");
    some->draw_ratio = DRAW_RATIO_MEDIUM;
    draw_ratios.push_back(some);

    some = new sub_Config;
    some->def = image_collection("menu/sprite_high_def.png");
    some->press = image_collection("menu/sprite_high_press.png");
    some->draw_ratio = DRAW_RATIO_HIGH;
    draw_ratios.push_back(some);






    // program main loop
    cursor = image_collection("cursor/mouse1.gif");
    cursor_rect.Set_values(0,0,48,48);
    actual_button =NULL;
    loop_done = 0;
    SDL_Event event;
    while (!loop_done)
    {
        // message processing loop
        SDL_WaitEvent(&event);
        Manage_Events(event);
        if(update_required)
        {
            Draw();
            SDL_SetClipRect(screen, &cursor_rect);
            SDL_BlitSurface(cursor, 0, screen, &cursor_rect);
            SDL_SetClipRect(screen, NULL);

            SDL_Flip(screen);
            update_required = 0;
        }
        SDL_framerateDelay(&manager);
    } // end main loop

    Game_Config gc;
    gc.resolution.Set_Values(
        resolutions[resolutions_index]->x,
        resolutions[resolutions_index]->y,
        VIDEO_bpp);
    gc.full_screen = fullscreen[fullscreen_index]->fullscreen;
    gc.activate_sky = sky[sky_index]->sky;
    gc.hour_to_game = scenery[scenery_index]->scenery;
    gc.draw_ratio = draw_ratios[draw_ratios_index]->draw_ratio;
    gc.Save(GAME_CONFIG_NAME);
    return 1;
}

bool
Configuration::Manage_Events(SDL_Event & event)
{
    update_required = 0;
    switch (event.type)
    {
        // exit if the window is closed
        case SDL_QUIT:
            loop_done = true;
            break;
        case SDL_KEYDOWN:
        {
            Button * button = keys[(int)event.key.keysym.sym];
            if(button){ (*button).Click();}
            //if(event.key.keysym.sym == SDLK_ESCAPE) loop_done = true;
            break;
        }
        case SDL_MOUSEMOTION:
        {
            update_required = 1;

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
            break;
        }
        case SDL_MOUSEBUTTONDOWN:
        {
            pair<int,int> coord(event.button.x,event.button.y);
            Button* boton = buttons[coord];
            if(boton)
            {
                (*boton).Set_Status(BUTTON_STATUS_PRESSED);
            }
            update_required = 1;
            break;
        }
        case SDL_MOUSEBUTTONUP:
        {
            pair<int,int> coord(event.button.x,event.button.y);
            Button* boton = buttons[coord];
            if      (boton)
            {
                //Quit_Menu();
                (*boton).Set_Status(BUTTON_STATUS_OVER);
                (*boton).Click();
                //Set_Video_Mode();
            }
            else if (resolutions_rect.IsOver(event.button.x,event.button.y))
                resolutions_index = (resolutions_index+1)%resolutions.size();
            else if (fullscreen_rect.IsOver(event.button.x,event.button.y))
                fullscreen_index = (fullscreen_index+1)%fullscreen.size();
            else if (sky_rect.IsOver(event.button.x,event.button.y))
                sky_index = (sky_index+1)%sky.size();
            else if (scenery_rect.IsOver(event.button.x,event.button.y))
                scenery_index = (scenery_index+1)%scenery.size();
            else if (draw_ratios_rect.IsOver(event.button.x,event.button.y))
                draw_ratios_index = (draw_ratios_index+1)%draw_ratios.size();
            update_required = 1;
            break;
        }
    } // end switch
    return update_required;
}

void
Configuration::Draw()
{
    SDL_BlitSurface(background, 0, screen, 0);
    for(list< Button* >::iterator itr=button_list.begin(), end=button_list.end();
        itr != end; ++itr)
        (*(*itr)).Draw(screen);

    SDL_BlitSurface(resolutions[resolutions_index]->def, 0, screen, &resolutions_rect);
    SDL_BlitSurface(fullscreen[fullscreen_index]->def, 0, screen, &fullscreen_rect);
    SDL_BlitSurface(sky[sky_index]->def, 0, screen, &sky_rect);
    SDL_BlitSurface(scenery[scenery_index]->def, 0, screen, &scenery_rect);
    SDL_BlitSurface(draw_ratios[draw_ratios_index]->def, 0, screen, &draw_ratios_rect);
}

int
Configuration::Quit(void* data)
{
    *((bool*)data) = true;
    return 0;
}
