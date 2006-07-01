
#include <cstdlib>
#include <windows.h>
#include <editor.h>
#include <button.h>
#include <menu.h>
#include <vector>
#include <map>
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <game/Game.h>
#include <drawstuff/drawstuff.h>

#include <img_collection.h>
IMG_Collection image_collection;
SDL_Surface *screen;
Mix_Music *musica_fondo;
Mix_Music *coche_arranque;
using namespace std;

int Run_Game_with_System_call(void* data)
{
    return system("Buggy-Man.exe game\n");
}

int Run_Game(void* data)
{
    dsPrint("\t### RUN GAME ###\n");

    Game::SetResolution(((int*)data)[0],((int*)data)[1]);
    Game::SetFullScreen(false);

    if(musica_fondo) Mix_PlayMusic(musica_fondo, -1);
//    Mix_PlayMusic(coche_arranque, 1);
    Game::Run();

    dsPrint("\t### END GAME ###\n");

    return 0;
}

int Run_Editor(void* data)
{
    dsPrint("\t### RUN EDITOR ###\n");

    Editor ed("Editor",screen);
    ed.Set_Video_Mode_CFG(800,600,32,SDL_HWSURFACE|SDL_DOUBLEBUF);
    ed.Set_Video_Mode();
    ed.Run();

    dsPrint("\t### END EDITOR ###\n");

    return 0;
}

int Quit_Game(void* data)
{
    *((bool*)data) = true;
    return 0;
}

int main ( int argc, char** argv )
{
    dsPrint("### MAIN ###\n");

    if(argc == 2 && !strcmp(argv[1],"game"))
    {
        void* data = (int*)new int(2);
        ((int*)data)[0]=320;
        ((int*)data)[1]=240;
        return(Run_Game(data));
    }


    Menu main_menu("Main",screen);
    main_menu.Init_Menu();
    main_menu.Set_Background("menu/backg_main.png");

    Button playlow( "Play LowRes",242,208,
                    "menu/button_play320_def.png","menu/button_play320_press.png",
                    "menu/button_play320_over.png","menu/button_play320_dis.png",
                    Run_Game_with_System_call,NULL);
//    ((int*)playlow.Get_Click_Data())[0]=320;
//    ((int*)playlow.Get_Click_Data())[1]=240;
    main_menu.Add_Button(playlow, SDLK_l);

    Button playhigh("Play HighRes",241,268,
                    "menu/button_play640_def.png","menu/button_play640_press.png",
                    "menu/button_play640_over.png","menu/button_play640_dis.png",
                    Run_Game,(int*)new int(2));
    ((int*)playhigh.Get_Click_Data())[0]=800;
    ((int*)playhigh.Get_Click_Data())[1]=600;
    main_menu.Add_Button(playhigh, SDLK_h);

    Button runeditor("Run Editor",277,324,
                    "menu/button_editor_def.png","menu/button_editor_press.png",
                    "menu/button_editor_over.png","menu/button_editor_dis.png",
                    Run_Editor,NULL);
    main_menu.Add_Button(runeditor, SDLK_e);

    Button configuration("Configuration",270,383,
                    "menu/button_configuration_def.png","menu/button_configuration_press.png",
                    "menu/button_configuration_over.png","menu/button_configuration_dis.png",
                    NULL,NULL,BUTTON_STATUS_DISABLED);
    main_menu.Add_Button(configuration, SDLK_c);

    Button quit_game("Quit Game",362,437,
                    "menu/button_exit_def.png","menu/button_exit_press.png",
                    "menu/button_exit_over.png","menu/button_exit_dis.png",
                    Quit_Game,(bool*)main_menu.Get_Loop_Boolean());
    main_menu.Add_Button(quit_game, SDLK_ESCAPE);

    main_menu.Set_Video_Mode_CFG(800,600,32,SDL_HWSURFACE|SDL_DOUBLEBUF);
    main_menu.Set_Video_Mode();

    /// Musica del juego
    //coche_arranque = Mix_LoadMUS("music/arranque.mp3");
    musica_fondo   = Mix_LoadMUS("music/song1.mp3");
//    if ( !musica_fondo || !coche_arranque ) {
//        printf("No pude cargar musica: %s\n", Mix_GetError());
//        exit(1);
//    }

    main_menu.Run();

    // Paramos la música
    Mix_HaltMusic();
    // liberamos recursos
    Mix_FreeMusic(musica_fondo);
    Mix_FreeMusic(coche_arranque);
    main_menu.Quit_Menu();

    dsPrint("### THE END ###\n");
    return 0;
}
