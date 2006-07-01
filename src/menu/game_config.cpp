#include"game_config.h"

Game_Config::Game_Config(Point3D<int> some_resolution, bool some_full_screen, int some_ratio_track_draw,
                    bool some_activate_sky, int some_hour_to_game/*,
                    SDLKey some_key_accelerate, SDLKey some_key_brake, SDLKey some_key_right,
                    SDLKey some_key_left, SDLKey some_key_jump, SDLKey some_key_return*/)
{
    resolution = some_resolution;
    full_screen = some_full_screen;
    ratio_track_draw = some_ratio_track_draw;
    activate_sky = some_activate_sky;
    hour_to_game = some_hour_to_game;
//    key_accelerate = some_key_accelerate;
//    key_brake = some_key_brake;
//    key_right = some_key_right;
//    key_left = some_key_left;
//    key_jump = some_key_jump;
//    key_return = some_key_return;
}

Game_Config::Game_Config(const Game_Config &some)
{
    (*this)=some;
}

Game_Config &
Game_Config::operator=(const Game_Config & some)
{
	if(this != &some)
	{
	    resolution = some.resolution;
        full_screen = some.full_screen;
        ratio_track_draw = some.ratio_track_draw;
        activate_sky = some.activate_sky;
        hour_to_game = some.hour_to_game;
//        key_accelerate = some.key_accelerate;
//        key_brake = some.key_brake;
//        key_right = some.key_right;
//        key_left = some.key_left;
//        key_jump = some.key_jump;
//        key_return = some.key_return;
	}
    return(*this);
}

void
Game_Config::Load_GameConfig()
{
    CFG_File config;

    int result = CFG_OpenFile(CONFIG_GAME, &config );

    if ( result == CFG_ERROR || result == CFG_CRITICAL_ERROR )
    {
        fprintf(stderr,"Unable to load file: %s\n", SDL_GetError());
        exit(1);
    }

    if( CFG_OK == CFG_SelectGroup("configuration", 0))
    {
        int width = CFG_ReadInt("resolution_width", VIDEO_width);
        int height = CFG_ReadInt("resolution_height", VIDEO_height);
        int bpp = CFG_ReadInt("resolution_bpp", VIDEO_depth);

        Point3D<int> *point = new Point3D<int>;
        (*point).Set_Values(width,height,bpp);

        resolution = (*point);
        full_screen = CFG_ReadBool("full_screen", false);
        ratio_track_draw = CFG_ReadInt("ratio_track_draw", 2);
        activate_sky = CFG_ReadBool("show_sky", false);
        hour_to_game = CFG_ReadInt("hour_to_game", 0);

//        key_accelerate = some.key_accelerate;
//        key_brake = some.key_brake;
//        key_right = some.key_right;
//        key_left = some.key_left;
//        key_jump = some.key_jump;
//        key_return = some.key_return;
    }
    else
    {
        resolution.Set_Values(VIDEO_width,VIDEO_height,VIDEO_depth);
        full_screen = false;
        ratio_track_draw = 2;
        activate_sky = false;
        hour_to_game = 0;
    }

    CFG_CloseFile(0);

}

void
Game_Config::Save_GameConfig()
{
    CFG_File config;

    int result = CFG_OpenFile(NULL, &config );

    if ( result == CFG_ERROR || result == CFG_CRITICAL_ERROR )
    {
        fprintf(stderr,"Unable to load file: %s\n", SDL_GetError());
        exit(1);
        //fstream o(CONFIG_GAME,ios::out);
    }

    if( CFG_OK == CFG_SelectGroup("configuration", 1))
    {
        CFG_WriteInt("resolution_width", resolution.x);
        CFG_WriteInt("resolution_height", resolution.y);
        CFG_WriteInt("resolution_bpp", resolution.z);
        CFG_WriteBool("full_screen", full_screen);
        CFG_WriteInt("ratio_track_draw", ratio_track_draw);
        CFG_WriteBool("show_sky", activate_sky);
        CFG_WriteInt("hour_to_game", hour_to_game);
    }
    CFG_SaveFile(CONFIG_GAME,CFG_SORT_ORIGINAL);

}
