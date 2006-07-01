#ifndef __GAME_CONFIG_H__
#define __GAME_CONFIG_H__


#include <SDL.h>
#include <SDL_config.h>
#include "point3d.h"
#include "utils.h"
#include <cstdlib>
#include <iostream>
#include<fstream>
#include <unistd.h>

//#define methods

#define HOUR_RANDOM 0
#define HOUR_DAY 1
#define HOUR_NIGHT 2


class Game_Config
{
    public:
        Game_Config(Point3D<int> some_resolution, bool some_full_screen, int some_ratio_track_draw,
                    bool some_activate_sky, int some_hour_to_game/*,
                    SDLKey some_key_accelerate, SDLKey some_key_brake, SDLKey some_key_right,
                    SDLKey some_key_left, SDLKey some_key_jump, SDLKey some_key_return*/);

        Game_Config(const Game_Config &);
        Game_Config & operator=(const Game_Config &);

        void Load_GameConfig();
        void Save_GameConfig();



    private:
		Point3D <int> resolution;
		bool full_screen;
		int ratio_track_draw;
		bool activate_sky;
		int hour_to_game;


		// Teclas
//		SDLKey key_accelerate;
//		SDLKey key_brake;
//		SDLKey key_right;
//		SDLKey key_left;
//		SDLKey key_jump;
//		SDLKey key_return;


};

#endif
