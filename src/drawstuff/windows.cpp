
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <ode/config.h>
#include <GL/gl.h>
#include <game\Game.h>

#include "drawstuff/internal.h"

#define CENTER 2.0f   //for Print

/// //////////////////////////////////////////////////////////////////////// ///
/// GLOBAL VARIABLES

extern SDL_Surface *screen;
TTF_Font *font_Courier;
int acabado = 0;
int cronometro = 0;
int jugando = 0;
Uint32 tempo = 0;
float record = 0;

bool done = false;          // variable global, finaliza el MAIN LOOP

GLuint glHUDTexture;
GLuint glHUDMask;

SDL_Color FontColor = {0,128,255,0};  // foreground color

/// //////////////////////////////////////////////////////////////////////// ///
/// PROTOTYPES

void Events(struct dsInterfaces *interfaces);
void EventsKeys(SDL_Event event, struct dsInterfaces *interfaces);
void EventsMouse(SDL_Event event);

GLuint SDL_GL_LoadTexture(SDL_Surface *surface, GLfloat *texcoord);
static int power_of_two(int input);
void SDL_GL_Enter2DMode();
void SDL_GL_Leave2DMode();
void GL_DeleteTextures();

/// //////////////////////////////////////////////////////////////////////// ///
///

extern "C" void dsPrint (char *msg, ...)
{
  va_list ap;
  va_start (ap,msg);
  vprintf (msg,ap);
}

extern "C" void dsError (char *msg, ...)
{
  va_list ap;
  va_start (ap,msg);
  vfprintf (stderr,msg,ap);
}

extern "C" void dsPanic (char *msg, ...)
{
  va_list ap;
  va_start (ap,msg);
  vfprintf (stderr,msg,ap);
  exit (1);
}


extern "C" void dsSetGLPrintColor(int r, int g, int b)
{
    FontColor.r = r;
    FontColor.g = g;
    FontColor.b = b;
}


extern "C" void dsGLPrint(float x, float y, char *msg, ...)
{
    // formatting the message string
    va_list ap;
    va_start (ap,msg);
    char text[1024];
    vsprintf (text,msg,ap);

    // printing the message
    SDL_Surface *sText = TTF_RenderText_Blended( font_Courier, text, FontColor );
    SDL_Rect rcDest = {0,0,0,255};
    SDL_BlitSurface( sText, NULL, screen, &rcDest );

    // printinf the mask (message in black)
    SDL_Color clrFgMask = {0,0,0,0};  // Black
    SDL_Surface *sMask = TTF_RenderText_Blended( font_Courier, text, clrFgMask );
    SDL_BlitSurface( sMask, NULL, screen, &rcDest );

    dsGLPrintSurface(sText,sMask,x,y);

    // free the surfaces
    SDL_FreeSurface( sText );
    SDL_FreeSurface( sMask );
}


extern "C" void dsGLPrintSurface(SDL_Surface *sTexture, SDL_Surface *sMask, float percent_x, float percent_y)
{
    int x,y;
    SDL_Surface *screen = SDL_GetVideoSurface();

    if (percent_x != CENTER)        x = (int) (screen->w * percent_x);
    else                            x = (int)((screen->w * 0.5) - (sTexture->w * 0.5));

    if (percent_y != CENTER)        y = (int) (screen->h * percent_y);
    else                            y = (int)((screen->h * 0.5) - (sTexture->h * 0.5));

    dsScreenPrint(sTexture, sMask, x, y);
}


extern "C" void dsScreenPrint(SDL_Surface *sTexture, SDL_Surface *sMask, int x, int y)
{
    // Free old message textures memory
    GL_DeleteTextures();

    // Convert the image into an OpenGL texture //
    GLfloat texcoord[4];
    glHUDTexture = SDL_GL_LoadTexture(sTexture, texcoord);
    glHUDMask = SDL_GL_LoadTexture(sMask, texcoord);

    // Make sure that the texture conversion is okay //
    if ( !glHUDTexture ) { return; }
    if ( !glHUDMask ) { return; }

    // Make texture coordinates easy to understand //
    GLfloat texMinX = texcoord[0];
    GLfloat texMinY = texcoord[1];
    GLfloat texMaxX = texcoord[2];
    GLfloat texMaxY = texcoord[3];

    // Show the image on the screen //

    int w = sTexture->w;
    int h = sTexture->h;

    SDL_Surface *screen = SDL_GetVideoSurface();

    SDL_GL_Enter2DMode();

      // MASK
      dsSetColor(1.0f, 1.0f, 1.0f);  // White BG
      glBlendFunc(GL_DST_COLOR,GL_ZERO);
      glBindTexture(GL_TEXTURE_2D, glHUDMask);
      glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(texMinX, texMinY); glVertex2i(x,   y  );
        glTexCoord2f(texMaxX, texMinY); glVertex2i(x+w, y  );
        glTexCoord2f(texMinX, texMaxY); glVertex2i(x,   y+h);
        glTexCoord2f(texMaxX, texMaxY); glVertex2i(x+w, y+h);
      glEnd();

      // TEXTURE
      dsSetColor(0.0f, 0.0f, 0.0f);  // Black BG
      glBlendFunc(GL_ONE,GL_ONE);
      glBindTexture(GL_TEXTURE_2D, glHUDTexture);
      glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(texMinX, texMinY); glVertex2i(x,   y  );
        glTexCoord2f(texMaxX, texMinY); glVertex2i(x+w, y  );
        glTexCoord2f(texMinX, texMaxY); glVertex2i(x,   y+h);
        glTexCoord2f(texMaxX, texMaxY); glVertex2i(x+w, y+h);
      glEnd();

    SDL_GL_Leave2DMode();
}

/// FUNCTIONS FOR THE EXTERN CONTROL
extern "C" void setAcabado(int var){acabado = var; }
extern "C" int getAcabado(){return( acabado ); }

extern "C" void setJugando(int var){jugando = var; }
extern "C" int getJugando(){return( jugando ); }

extern "C" void setCronometro(int var){cronometro = var; }
extern "C" int getCronometro(){return( cronometro ); }

extern "C" void resetTempo(){tempo = SDL_GetTicks(); }

extern "C" float getRecord(){ return( record ); }
extern "C" void resetRecord(){ record = 0; }

/// //////////////////////////////////////////////////////////////////////// ///
/// EVENTS MANAGEMENT

void Events(dsInterfaces *interfaces)
{
    SDL_Event event;

    // for keyboard state
    EventsKeys(event,interfaces);

    // for keyboard events
    while (SDL_PollEvent(&event))
    {
        // check for messages
        switch (event.type)
        {
          /// window is closed ///
          case SDL_QUIT:
          {
            done = true;
            break;
          }

          /// key ///
          case SDL_KEYDOWN:
          case SDL_KEYUP:
          {
            EventsKeys(event,interfaces);
            break;
          }

          /// mouse ///
          case SDL_MOUSEBUTTONDOWN:
          case SDL_MOUSEBUTTONUP:
          case SDL_MOUSEMOTION:
          {
              EventsMouse(event);
              break;
          }

        } // end switch
    }
}

void EventsKeys(SDL_Event event, dsInterfaces *interfaces)
{
    // CONST

    switch (event.key.keysym.sym)
    {
        case SDLK_ESCAPE:
        {
            done = true;
            break;
        }
        case SDLK_1:
        {
            interfaces->SetView(0);
            break;
        }
        case SDLK_2:
        {
            interfaces->SetView(1);
            break;
        }
        case SDLK_3:
        {
            interfaces->SetView(2);
            break;
        }
        case SDLK_4:
        {
            interfaces->SetView(3);
            break;
        }
        case SDLK_5:
            interfaces->SetView(4);
            break;
    }

    // NON CONST

    Uint8 *key_state = SDL_GetKeyState(0);

    // speed

    if (key_state[interfaces->Forward])
      interfaces->SpeedIncrease(1);
    else
    if (key_state[interfaces->Backward])
      interfaces->SpeedIncrease(-1);
    else
      interfaces->SpeedDecrease();

    // steer

    if (key_state[interfaces->Left])
      interfaces->SteerIncrease(1);
    else
    if (key_state[interfaces->Right])
      interfaces->SteerIncrease(-1);
    else
      interfaces->SteerDecrease();

    // actions

    if (event.key.keysym.sym == interfaces->Break)
      interfaces->DoAction(3);
    else
    if (event.key.keysym.sym == interfaces->Action
               && event.type == SDL_KEYDOWN)
        interfaces->DoAction(1);
    else
    if (event.key.keysym.sym == SDLK_r
                && event.type == SDL_KEYDOWN)
      interfaces->DoAction(2);
}

void EventsMouse(SDL_Event event)
{
// SDL_BUTTON_LEFT, SDL_BUTTON_MIDDLE, SDL_BUTTON_RIGHT, SDL_BUTTON_WHEELUP, SDL_BUTTON_WHEELDOWN

  static int button=0;

  /// DOWN
  if (event.button.type == 	SDL_MOUSEBUTTONDOWN)
  {
    switch (event.button.button)
    {
        case SDL_BUTTON_LEFT:
        {
            button |= 1;
            break;
        }
        case SDL_BUTTON_MIDDLE:
        {
            button |= 2;
            break;
        }
        case SDL_BUTTON_RIGHT:
        default:
        {
            button |= 4;
            break;
        }
    } // end switch
  }
  else
  /// UP
  if (event.button.type == SDL_MOUSEBUTTONUP)
  {
    switch (event.button.button)
    {
        case SDL_BUTTON_LEFT:
        {
            button &= ~1;
            break;
        }
        case SDL_BUTTON_MIDDLE:
        {
            button &= ~2;
            break;
        }
        case SDL_BUTTON_RIGHT:
        {
            button &= ~4;
            break;
        }
    } // end switch
  }

  /// MOTION
  if (event.type == SDL_MOUSEMOTION)
  {
    if (button)
      dsMotion (button, event.motion.xrel, event.motion.yrel);
  }

}

/// //////////////////////////////////////////////////////////////////////// ///
/// MAIN SDL PROGRAM

void dsPlatformSimLoop (int window_width, int window_height, bool fullscreen, dsFunctions *fn, dsInterfaces *in, int initial_pause)
{
    unsigned int video_mode = 0;

    video_mode = SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_OPENGLBLIT;

    if (fullscreen) video_mode |= SDL_FULLSCREEN;


    dsPrint("# Initializing SDL Video ... ");

    // initialize SDL video

    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
        dsPanic( "Unable to init SDL: %s\n", SDL_GetError() );

    // TTF Fonts
    TTF_Init();
    font_Courier = TTF_OpenFont( "Fonts\\lucida.ttf", 26 );

    // create a new window
    screen = SDL_SetVideoMode(window_width, window_height, 32, video_mode);
    if ( !screen )
        dsPanic("Unable to set %dx%d video: %s\n", window_width, window_height, SDL_GetError());
    dsPrint("OK\n");

    // Setup OpenGL
    dsStartGraphics (window_width, window_height, fn);
    // Load Textures
    if (fn->start) fn->start();

    dsPrint(".. Main Loop\n");

    /// PROGRAM MAIN LOOP ///

    bool terminado = 0;
    Uint32 starting = 5;
    // tick's control
    Uint32 start_time = SDL_GetTicks();
    Uint32 this_time = 0;
	int frames = 0;
	// %events, %2D, %3D
	Uint32 ticks_percent_ini;
	Uint32 ticks_percent_events = 0;
    Uint32 ticks_percent_2D = 0;
	Uint32 ticks_percent_3D = 0;

	int countdown = 0;

    while (!done)
    {
        // Message events
        countdown = (int)(starting-((SDL_GetTicks()-tempo)/1000.0));
        if (!jugando && (countdown<=0))
        {
            jugando=1;
            Game::crono.Start();
        }
        if (jugando)
        {
            ticks_percent_ini = SDL_GetTicks();
            Events(in);
            ticks_percent_events += (SDL_GetTicks() - ticks_percent_ini);
        }

        /// DRAW STARTS ///

        // clear screen 2D
        SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));
        // clear screen 3D
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        /// 3D
        ticks_percent_ini = SDL_GetTicks();
        dsDrawFrame (fn, initial_pause);
        ticks_percent_3D += SDL_GetTicks() - ticks_percent_ini;

        /// 2D over 3D

        /// CRONOMETRO Y CONTROLADOR DE CARRERA
        //si estamos jugando
        dsSetGLPrintColor(255,0,0);
        if (jugando)
        {
            if (acabado) // si hemos acabado la carrera
            {
                if (!cronometro) // variable para parar el cronometro (solo una vez)
                {
                    Game::crono.Stop();
                    cronometro = 1;
                }
                dsGLPrint(CENTER,0.4,"HAS TERMINADO!!");
                dsGLPrint(CENTER,0.6,"Tiempo: %2.f sg",(float)Game::crono.getFinal()/1000.0);
            }
            else{
                record = (float)(SDL_GetTicks()-Game::crono.getInicial())/1000.0;
                dsGLPrint(0.6,0.001,"Tiempo %2.f sg",record);
                if (record <= 1.0 && countdown == 0) dsGLPrint(CENTER,0.55,"AHORA!!");
            }
        }
        else
        {   //Cuenta atras inicial
            dsGLPrint(CENTER,CENTER,"%d",countdown);
            acabado = 0;
        }

        ticks_percent_ini = SDL_GetTicks();
        dsSetGLPrintColor(0,128,255);
        dsGLPrint(0.001,0.001,"%2.f FPS",((float)frames/(SDL_GetTicks()-start_time))*1000.0);
        ticks_percent_2D += SDL_GetTicks() - ticks_percent_ini;

        /// DRAW ENDS ///

		// Check for error conditions. //

		GLenum gl_error = glGetError( );
		if( gl_error != GL_NO_ERROR ) {
			dsError("OpenGL error: %d\n", gl_error );
		}

		char* sdl_error = SDL_GetError( );
		if( sdl_error[0] != '\0' ) {
			dsError("SDL error: '%s'\n", sdl_error);
			SDL_ClearError();
		}

		++frames;

		/// RENDER ///

        // finally, update the screen :)
        SDL_GL_SwapBuffers();

        SDL_Flip(screen);
    } // end main loop

    /// END ///

    dsPrint(".. End\n");

	// Print out the frames per second //
	this_time = SDL_GetTicks();
	if ( this_time != start_time ) {
		printf("%2.2f FPS\n",
			((float)frames/(this_time-start_time))*1000.0);
	}
    // Print debug info
    printf("Events: %2.3f %c\n",100*(float)ticks_percent_events/(this_time-start_time),'%');
    printf("  2D  : %2.3f %c\n",100*(float)ticks_percent_2D/(this_time-start_time),'%');
    printf("  3D  : %2.3f %c\n",100*(float)ticks_percent_3D/(this_time-start_time),'%');

    TTF_CloseFont( font_Courier );

    // free texture memory
    GL_DeleteTextures();

    if (fn->stop) fn->stop();
    dsStopGraphics();

    // Close TTF
    TTF_Quit();
    // Close SDL
    SDL_Quit();
}

// load the texture into "*texture" param
GLuint SDL_GL_LoadTexture(SDL_Surface *surface, GLfloat *texcoord)
{
	GLuint texture;
	int w, h;
	SDL_Surface *image;
	SDL_Rect area;
	Uint32 saved_flags;
	Uint8  saved_alpha;

	// Use the surface width and height expanded to powers of 2 //
	w = power_of_two(surface->w);
	h = power_of_two(surface->h);
	texcoord[0] = 0.0f;			// Min X //
	texcoord[1] = 0.0f;			// Min Y //
	texcoord[2] = (GLfloat)surface->w / w;	// Max X //
	texcoord[3] = (GLfloat)surface->h / h;	// Max Y //

	image = SDL_CreateRGBSurface(
			SDL_SWSURFACE,
			w, h,
			32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN // OpenGL RGBA masks //
			0x000000FF,
			0x0000FF00,
			0x00FF0000,
			0xFF000000
#else
			0xFF000000,
			0x00FF0000,
			0x0000FF00,
			0x000000FF
#endif
		       );

	if ( image == NULL ) {  return 0;  }

	// Save the alpha blending attributes //
	saved_flags = surface->flags&(SDL_SRCALPHA|SDL_RLEACCELOK);
	saved_alpha = surface->format->alpha;
	if ( (saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA )
		SDL_SetAlpha(surface, 0, 0);

	// Copy the surface into the GL texture image //
	area.x = 0;
	area.y = 0;
	area.w = surface->w;
	area.h = surface->h;
	SDL_BlitSurface(surface, &area, image, &area);

	// Restore the alpha blending attributes //
	if ( (saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA ) {
		SDL_SetAlpha(surface, saved_flags, saved_alpha);
	}

	// Create an OpenGL texture for the image //
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D,
		     0,
		     GL_RGBA,
		     w, h,
		     0,
		     GL_RGBA,
		     GL_UNSIGNED_BYTE,
		     image->pixels);

	SDL_FreeSurface(image); // No longer needed //

	return texture;
}

/* Quick utility function for texture creation */
static int power_of_two(int input)
{
	int value = 1;

	while ( value < input ) {
		value <<= 1;
	}
	return value;
}

void SDL_GL_Enter2DMode()
{
	SDL_Surface *screen = SDL_GetVideoSurface();

	glPushAttrib(GL_LIGHTING);
	glDisable(GL_LIGHTING);

	glPushAttrib(GL_ENABLE_BIT);

	glPushAttrib(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);

	glPushAttrib(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);

	glPushAttrib(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_2D);

    glPushAttrib(GL_BLEND);
	glEnable(GL_BLEND);

	glViewport(0, 0, screen->w, screen->h);

	glPushAttrib(GL_MATRIX_MODE);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glOrtho(0.0, (GLdouble)screen->w, (GLdouble)screen->h, 0.0, 0.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
}

void SDL_GL_Leave2DMode()
{
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

    glMatrixMode(GL_PROJECTION);
	glPopMatrix();

    glPopAttrib();  // GL_MATRIX_MODE
	glPopAttrib();  // GL_BLEND
	glPopAttrib();  // GL_TEXTURE_2D
	glPopAttrib();  // GL_CULL_FACE
	glPopAttrib();  // GL_DEPTH_TEST
	glPopAttrib();  // GL_ENABLE_BIT
	glPopAttrib();  // GL_LIGHTING
}

void GL_DeleteTextures()
{
    // free HUD texture memory
    if ( glHUDTexture ) {
      glDeleteTextures( 1, &glHUDTexture );
      glHUDTexture = 0;
    }
    if ( glHUDMask ) {
      glDeleteTextures( 1, &glHUDMask );
      glHUDMask = 0;
    }
}
