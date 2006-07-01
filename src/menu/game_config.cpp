#include"game_config.h"

Game_Config::Game_Config()
{
    resolution.Set_Values(VIDEO_x,VIDEO_y,VIDEO_bpp);
    full_screen = VIDEO_full_screen;
    draw_ratio = DRAW_RATIO;
    activate_sky = DRAW_SKY;
    hour_to_game = HOUR_RANDOM;
}

Game_Config::Game_Config(Uint16 x, Uint16 y, Uint8 bpp, bool some_full_screen, int some_draw_ratio,
                    bool some_activate_sky, int some_hour_to_game/*,
                    SDLKey some_key_accelerate, SDLKey some_key_brake, SDLKey some_key_right,
                    SDLKey some_key_left, SDLKey some_key_jump, SDLKey some_key_return*/)
{
    resolution.Set_Values(x,y,bpp);
    full_screen = some_full_screen;
    draw_ratio = some_draw_ratio;
    activate_sky = some_activate_sky;
    hour_to_game = some_hour_to_game;
}

Game_Config::Game_Config(Point3D<int> some_resolution, bool some_full_screen, int some_draw_ratio,
                    bool some_activate_sky, int some_hour_to_game/*,
                    SDLKey some_key_accelerate, SDLKey some_key_brake, SDLKey some_key_right,
                    SDLKey some_key_left, SDLKey some_key_jump, SDLKey some_key_return*/)
{
    resolution = some_resolution;
    full_screen = some_full_screen;
    draw_ratio = some_draw_ratio;
    activate_sky = some_activate_sky;
    hour_to_game = some_hour_to_game;
//    key_accelerate = some_key_accelerate;
//    key_brake = some_key_brake;
//    key_right = some_key_right;
//    key_left = some_key_left;
//    key_jump = some_key_jump;
//    key_return = some_key_return;
}

Game_Config::Game_Config(char* path)
{
    Load(path);
}

Game_Config::Game_Config(const Game_Config &some)
{
    (*this)=some;
}

Game_Config &
Game_Config::operator=(const Game_Config & some)
{
	if(this == &some) return(*this);

    resolution = some.resolution;
    full_screen = some.full_screen;
    draw_ratio = some.draw_ratio;
    activate_sky = some.activate_sky;
    hour_to_game = some.hour_to_game;
//    key_accelerate = some.key_accelerate;
//    key_brake = some.key_brake;
//    key_right = some.key_right;
//    key_left = some.key_left;
//    key_jump = some.key_jump;
//    key_return = some.key_return;

    return(*this);
}

Game_Config &
Game_Config::operator=(char* path)
{
	Load(path);
    return(*this);
}

Game_Config::~Game_Config()
{
}

void
Game_Config::Load(char* path)
{
    CFG_File config;

    int result = CFG_OpenFile(path, &config );

    if ( result == CFG_ERROR || result == CFG_CRITICAL_ERROR )
    {
        fprintf(stderr,"Unable to load file: %s\n", SDL_GetError());
        exit(1);
    }

    if( CFG_OK == CFG_SelectGroup("configuration", 0))
    {
        resolution.x = CFG_ReadInt("resolution_width", VIDEO_x);
        resolution.y = CFG_ReadInt("resolution_height", VIDEO_y);
        resolution.z = CFG_ReadInt("resolution_bpp", VIDEO_bpp);
        full_screen = CFG_ReadBool("full_screen", VIDEO_full_screen);
        draw_ratio = CFG_ReadInt("draw_ratio", DRAW_RATIO);
        activate_sky = CFG_ReadBool("show_sky", DRAW_SKY);
        hour_to_game = CFG_ReadInt("hour_to_game", HOUR_RANDOM);
    }
    else
    {
        Game_Config some;
        (*this) = some;
    }
    CFG_CloseFile(0);
}

void
Game_Config::Save(char* path)
{
    fstream o(GAME_CONFIG_NAME,ios::out);
    o<<"[configuration]\n";
    o<<"resolution_width = "<<resolution.x<<endl;
    o<<"resolution_height = "<<resolution.y<<endl;
    o<<"resolution_bpp = "<<resolution.z<<endl;
    o<<"full_screen = "<<full_screen<<endl;
    o<<"draw_ratio = "<<draw_ratio<<endl;
    o<<"show_sky = "<<activate_sky<<endl;
    o<<"hour_to_game = "<<hour_to_game<<endl<<endl;
    o.close();

//    CFG_File config;
//
//    int result = CFG_OpenFile(NULL, &config );
//
//    if ( result == CFG_ERROR || result == CFG_CRITICAL_ERROR )
//    {
//        fprintf(stderr,"Unable to open file: %s\n", SDL_GetError());
//        exit(1);
//        //fstream o(CONFIG_GAME,ios::out);
//    }
//
//    if( CFG_OK == CFG_SelectGroup("configuration", 1))
//    {
//        CFG_WriteInt("resolution_width", resolution.x);
//        CFG_WriteInt("resolution_height", resolution.y);
//        CFG_WriteInt("resolution_bpp", resolution.z);
//        CFG_WriteBool("full_screen", full_screen);
//        CFG_WriteInt("draw_ratio", draw_ratio);
//        CFG_WriteBool("show_sky", activate_sky);
//        CFG_WriteInt("hour_to_game", hour_to_game);
//    }
//    CFG_SaveFile(GAME_CONFIG_NAME,CFG_SORT_ORIGINAL);
//    CFG_CloseFile(0);
}
