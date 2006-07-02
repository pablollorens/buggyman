#include "game_menus.h"

int Quit_Game(void* data)
{
    *((bool*)data) = true;
    return 0;
}


int Run_Game(void* data)
{
    dsPrint("\t### RUN GAME ###\n");

    vector< string > songs = Get_MusicFiles("music/game");
    if (songs.size())
    {
        srand(SDL_GetTicks());
        int cont = rand()%songs.size();
        string ruta = "music/game/" + songs[cont];
        musica_juego = Mix_LoadMUS(ruta.c_str());
    }
    if(musica_juego) Mix_PlayMusic(musica_juego, -1);

    int retorno = system((char*)data);

    Mix_HaltMusic();
    if(musica_menu) Mix_PlayMusic(musica_menu, -1);

    dsPrint("\t### END GAME ###\n");
    return retorno;
}

int Select_Car(void* data)
{
    Menu menu("Select Car",screen);
    menu.Init_Menu();
    menu.Set_Background("menu/backg_select_car.jpg");

    Button play_desert( "Play Desert AP",84,95,
                    "menu/button_car_desert_mini.jpg","menu/button_car_desert_mini.jpg",
                    "menu/button_car_desert_mini.jpg","menu/button_car_desert_mini.jpg",
                    Run_Game,(char*)"Buggy-Man.exe desert.car terreno.cfg\n");
    menu.Add_Button(play_desert, SDLK_1);

    Button play_golgotha( "Golgotha",466,95,
                    "menu/button_car_golgotha_mini.jpg","menu/button_car_golgotha_mini.jpg",
                    "menu/button_car_golgotha_mini.jpg","menu/button_car_golgotha_mini.jpg",
                    Run_Game,(char*)"Buggy-Man.exe golgotha.car terreno.cfg\n");
    menu.Add_Button(play_golgotha, SDLK_2);

    Button play_manhunter( "Man-Hunter",84,343,
                    "menu/button_car_manhunter_mini.jpg","menu/button_car_manhunter_mini.jpg",
                    "menu/button_car_manhunter_mini.jpg","menu/button_car_manhunter_mini.jpg",
                    Run_Game,(char*)"Buggy-Man.exe manhunter.car terreno.cfg\n");
    menu.Add_Button(play_manhunter, SDLK_3);

    Button play_panda( "Seat Panda",466,343,
                    "menu/button_car_panda_mini.jpg","menu/button_car_panda_mini.jpg",
                    "menu/button_car_panda_mini.jpg","menu/button_car_panda_mini.jpg",
                    Run_Game,(char*)"Buggy-Man.exe panda.car terreno.cfg\n");
    menu.Add_Button(play_panda, SDLK_4);

    Button quit_game("Return",376,510,
                    "menu/button_exit2_def.png","menu/button_exit2_press.png",
                    "menu/button_exit2_over.png","menu/button_exit2_dis.png",
                    Quit_Game,(bool*)menu.Get_Loop_Boolean());
    menu.Add_Button(quit_game, SDLK_ESCAPE);

    menu.Set_Video_Mode_CFG(VIDEO_x,VIDEO_y,VIDEO_bpp,SDL_HWSURFACE|SDL_DOUBLEBUF);
    menu.Set_Video_Mode();

    return menu.Run();
}
