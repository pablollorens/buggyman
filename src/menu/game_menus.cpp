#include "game_menus.h"

int Run_Parametrized_Game(int argc, char** argv)
{
    if(argc != 3) return argc;

    Game_Config gc("game.cfg");
    Game::SetResolution(gc.resolution.x,gc.resolution.y);
    fprintf(stderr,"fullscreen = %d\n",gc.full_screen);
    Game::SetFullScreen(gc.full_screen);
    string circuit = argv[2];
    string car = argv[1];
    Game::Run(circuit,car);//circuit and car
    exit(0);
}

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

int Run_Editor(void* data)
{
    dsPrint("\t### RUN EDITOR ###\n");

    Editor ed("Editor",screen);
    ed.Set_Video_Mode_CFG(VIDEO_x,VIDEO_y,VIDEO_bpp,SDL_HWSURFACE|SDL_DOUBLEBUF);
    ed.Set_Video_Mode();

    vector< string > songs = Get_MusicFiles("music/editor");
    if (songs.size())
    {
        srand(SDL_GetTicks());
        int cont = rand()%songs.size();
        string ruta = "music/editor/" + songs[cont];
        musica_editor = Mix_LoadMUS(ruta.c_str());
    }
    if(musica_editor) Mix_PlayMusic(musica_editor, -1);

    ed.Run();

    Mix_HaltMusic();
    if(musica_menu) Mix_PlayMusic(musica_menu, -1);

    dsPrint("\t### END EDITOR ###\n");

    return 0;
}

int Run_Configuration(void* data)
{
    dsPrint("\t### RUN Configuration ###\n");

    Configuration conf("Configuration",screen);
    conf.Set_Video_Mode_CFG(VIDEO_x,VIDEO_y,VIDEO_bpp,SDL_HWSURFACE|SDL_DOUBLEBUF);
    conf.Set_Video_Mode();
    conf.Run();

    dsPrint("\t### END Configuration ###\n");

    return 0;
}

int Run_Main_Menu(void* data)
{
    Menu main_menu("Main",screen);
    main_menu.Init_Menu();
    main_menu.Set_Background("menu/backg_main.png");

    Button select_car( "Play",367,199,
                    "menu/button_play_def.png","menu/button_play_press.png",
                    "menu/button_play_over.png","menu/button_play_dis.png",
                    Select_Car,NULL);
    main_menu.Add_Button(select_car, SDLK_p);

    Button runeditor("Run Editor",284,266,
                    "menu/button_editor_def.png","menu/button_editor_press.png",
                    "menu/button_editor_over.png","menu/button_editor_dis.png",
                    Run_Editor,NULL);
    main_menu.Add_Button(runeditor, SDLK_e);

    Button configuration("Configuration",273,335,
                    "menu/button_configuration_def.png","menu/button_configuration_press.png",
                    "menu/button_configuration_over.png","menu/button_configuration_dis.png",
                    Run_Configuration,NULL);
    main_menu.Add_Button(configuration, SDLK_c);

    Button quit_game("Quit Game",369,406,
                    "menu/button_exit_def.png","menu/button_exit_press.png",
                    "menu/button_exit_over.png","menu/button_exit_dis.png",
                    Quit_Game,(bool*)main_menu.Get_Loop_Boolean());
    main_menu.Add_Button(quit_game, SDLK_ESCAPE);

    main_menu.Set_Video_Mode_CFG(800,600,32,SDL_HWSURFACE|SDL_DOUBLEBUF);
    main_menu.Set_Video_Mode();

    // Musica del juego
    vector< string > songs = Get_MusicFiles("music/menu");
    if (songs.size())
    {
        srand(SDL_GetTicks());
        int cont = rand()%songs.size();
        string ruta = "music/menu/" + songs[cont];
        musica_menu = Mix_LoadMUS(ruta.c_str());
    }
    if(musica_menu) Mix_PlayMusic(musica_menu, -1);

    main_menu.Run();

    // Paramos la música
    Mix_HaltMusic();
    // liberamos recursos
    Mix_FreeMusic(musica_menu);
    Mix_FreeMusic(musica_juego);
    Mix_FreeMusic(musica_editor);

    main_menu.Quit_Menu();
    return 0;
}
