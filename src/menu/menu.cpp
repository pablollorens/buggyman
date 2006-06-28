#include"menu.h"

Menu::Menu(char* some_name, SDL_Surface* some_screen)
{
    name = some_name;
    background = NULL;
    screen = some_screen;
    update_required = true;
    loop_done = false;
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

	return(*this);
}

Menu::~Menu()
{
    background = NULL;
    screen = NULL;
    update_required = false;
    loop_done = true;
}

int
Menu::Run()
{
    if(!screen) return 0;

    FPSmanager manager;
    SDL_initFramerate(&manager);
    SDL_setFramerate(&manager, 30);

    Rect2D position;
    SDL_Surface* cursor_default = image_collection("cursor/mouse1.gif");
    SDL_Surface* cursor = cursor_default;
    Rect2D cursor_rect(0,0,48,48);
    position = 0;
    Button* actual_button =NULL;

    // program main loop
    loop_done = 0;
    SDL_Event event;
    while (!loop_done)
    {
        // message processing loop
        SDL_WaitEvent(&event);
        Manage_Events(event, cursor_rect, actual_button);
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
Menu::Manage_Events(SDL_Event & event, Rect2D & cursor_rect, Button* actual_button)
{
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
            if(event.key.keysym.sym == SDLK_ESCAPE) loop_done = true;
            break;
        }
        case SDL_MOUSEMOTION:
        {
            update_required = 1;
            cursor_rect.x = event.motion.x;
            cursor_rect.y = event.motion.y;
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
                Init_Menu(800,600,32,SDL_HWSURFACE|SDL_DOUBLEBUF); //esti asi no eh???
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
    keys[(int)key] = &button;
}

void
Menu::Draw()
{
    SDL_BlitSurface(background, 0, screen, 0);
    fprintf(stderr,"la lista tiene %d elementos\n",button_list.size());

    for(list< Button* >::iterator itr=button_list.begin(), end=button_list.end();
        itr != end; ++itr)
    {
        Button * boton = (*itr);
        //SDL_SetClipRect(screen, &cursor_rect);
        (*boton).Draw(screen);
    }
}

void
Menu::Init_Menu(int x, int y, int bpp, Uint32 flags)
{
    if(SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        dsError( "Unable to init SDL: %s\n", SDL_GetError() );
        exit( 100 );
    }

    screen = SDL_SetVideoMode( x, y, bpp, flags);
    if ( !screen )
    {
        dsError("Unable to set video: %s\n", SDL_GetError());
        exit( 101 );
    }
    SDL_ShowCursor(SDL_DISABLE);
}

void
Menu::Quit_Menu()
{
    SDL_Quit();
}
