#include"menu.h"

Menu::Menu(char* some_name, SDL_Surface* some_screen)
{
    name = some_name;
    background = NULL;
    screen = some_screen;
    update_required = true;
    loop_done = false;

    video_mode = 0;
    video_mode_flags = 0;
    video_mode_ready = 0;
}

Menu::Menu(const Menu& some)
{
    (*this)=some;
}

Menu&
Menu::operator=(const Menu& some)
{
	if(this == &some) return(*this);

    name = some.name;
    background = some.background;
    screen = some.screen;
    update_required = some.update_required;
    loop_done = some.update_required;
    button_list = some.button_list;
    buttons = some.buttons;
    keys = some.keys;

    video_mode       = some.video_mode ;
    video_mode_flags = some.video_mode_flags ;
    video_mode_ready = some.video_mode_ready ;
	return(*this);
}

Menu::~Menu()
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
Menu::Run()
{
    if(!screen) return 0;
    if(!video_mode_ready) return 0;

    FPSmanager manager;
    SDL_initFramerate(&manager);
    SDL_setFramerate(&manager, 30);

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
    return 1;
}

bool
Menu::Manage_Events(SDL_Event & event)
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
            if(button){ (*button).Click(); loop_done=true;}
            //if(event.key.keysym.sym == SDLK_ESCAPE) loop_done = true;
            break;
        }
        case SDL_MOUSEMOTION:
        {
            update_required = 1;

            cursor_rect.Set_XY(event.motion.x,event.motion.y);
//            cursor_rect.x = event.motion.x;
//            cursor_rect.y = event.motion.y;
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
            if(boton)
            {
                //Quit_Menu();
                (*boton).Set_Status(BUTTON_STATUS_OVER);
                (*boton).Click();
                Set_Video_Mode();
            }
            update_required = 1;
            break;
        }
    } // end switch
    return update_required;
}

void
Menu::Add_Button(Button & button, SDLKey key)
{
    for(int i = button.Get_Position().x, n = button.Get_Position().x + button.Get_Position().w; i<n; i++)
        for(int j = button.Get_Position().y, m = button.Get_Position().y + button.Get_Position().h; j<m; j++)
        {
            pair<int,int> aux(i,j);
            buttons[aux] = &button;
        }
    //keys[(int)key] button.Get_Click_Function();
    button_list.push_back(&button);
    if(key != SDLK_UNKNOWN)
        keys[(int)key] = &button;
}

void
Menu::Add_Button(Button & button)
{
    Add_Button(button, SDLK_UNKNOWN);
}

void
Menu::Draw()
{
    SDL_BlitSurface(background, 0, screen, 0);
    for(list< Button* >::iterator itr=button_list.begin(), end=button_list.end();
        itr != end; ++itr)
    {
        //Button * boton = (*itr);
        //SDL_SetClipRect(screen, &cursor_rect);
        //(*boton).Draw(screen);
        (*(*itr)).Draw(screen);
    }
}

void
Menu::Init_Menu()
{
    if(SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
    {
        dsError( "Unable to init SDL: %s\n", SDL_GetError() );
        exit( 100 );
    }
}

void
Menu::Set_Video_Mode_CFG(int x, int y, int bpp, Uint32 flags)
{
    video_mode.x = x;
    video_mode.y = y;
    video_mode.z = bpp;
    video_mode_flags = flags;
    video_mode_ready = 1;
}

void
Menu::Set_Video_Mode()
{
    if(!video_mode_ready) return;
    screen = SDL_SetVideoMode( video_mode.x, video_mode.y, video_mode.z, video_mode_flags);
    if ( !screen )
    {
        dsError("Unable to set video: %s\n", SDL_GetError());
        exit( 101 );
    }
    SDL_ShowCursor(SDL_DISABLE);

    if(Mix_OpenAudio(22050, AUDIO_S16, 2, 4096)){
        printf("No se puede inicializar SDL_mixer %s\n",Mix_GetError());
        exit(0);
    }
    atexit(Mix_CloseAudio);
}

void
Menu::Quit_Menu()
{
    SDL_Quit();
}
