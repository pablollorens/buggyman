
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <ode/config.h>
#include <GL/gl.h>

#include "drawstuff/internal.h"

/// //////////////////////////////////////////////////////////////////////// ///
/// GLOBAL VARIABLES

SDL_Surface *screen;
TTF_Font *font_Courier;

bool done = false;          // variable global, finaliza el MAIN LOOP

GLuint HUD_GLTexture;

/// //////////////////////////////////////////////////////////////////////// ///
/// PROTOTYPES

void Events(struct dsInterfaces *interfaces);
void EventsKeys(SDL_Event event, struct dsInterfaces *interfaces);
void EventsMouse(SDL_Event event);

GLuint SDL_GL_LoadTexture(SDL_Surface *surface, GLfloat *texcoord);
static int power_of_two(int input);
void SDL_GL_Enter2DMode();
void SDL_GL_Leave2DMode();

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


extern "C" void dsGLPrint(int x,int y,char *msg, ...)
{
    if ( HUD_GLTexture ) {
      glDeleteTextures( 1, &HUD_GLTexture );
      HUD_GLTexture = 0;
    }

  va_list ap;
  va_start (ap,msg);
  char text[1024];
  vsprintf (text,msg,ap);


  SDL_Color clrFg = {0,0,255,0};  // Blue ("Fg" is foreground)
  SDL_Surface *sText = TTF_RenderText_Solid( font_Courier, text, clrFg );
  SDL_Rect rcDest = {0,0,0,250};
  SDL_BlitSurface( sText,NULL, screen,&rcDest );

    GLuint printf_texture = 0;
    GLfloat texcoord[4];

    int w = 20 * strlen(msg);
    int h = 40;

    // Convert the image into an OpenGL texture //
    printf_texture = SDL_GL_LoadTexture(sText, texcoord);

    // Make texture coordinates easy to understand //
    GLfloat texMinX = texcoord[0];
    GLfloat texMinY = texcoord[1];
    GLfloat texMaxX = texcoord[2];
    GLfloat texMaxY = texcoord[3];

    // We don't need the original image anymore //
    SDL_FreeSurface( sText );

    // Make sure that the texture conversion is okay //
    if ( ! printf_texture ) { return; }

    // Show the image on the screen //
    SDL_GL_Enter2DMode();
    glBindTexture(GL_TEXTURE_2D, printf_texture);
    glBegin(GL_TRIANGLE_STRIP);
      glTexCoord2f(texMinX, texMinY); glVertex2i(x,   y  );
      glTexCoord2f(texMaxX, texMinY); glVertex2i(x+w, y  );
      glTexCoord2f(texMinX, texMaxY); glVertex2i(x,   y+h);
      glTexCoord2f(texMaxX, texMaxY); glVertex2i(x+w, y+h);
    glEnd();
    SDL_GL_Leave2DMode();
}

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

    // make sure SDL cleans up before exit
    atexit(SDL_Quit);

    // TTF Fonts
    TTF_Init();
    atexit(TTF_Quit);
    font_Courier = TTF_OpenFont( "Fonts\\cour.ttf", 26 );

    // create a new window
    screen = SDL_SetVideoMode(window_width, window_height, 32, video_mode);
    if ( !screen )
        dsPanic("Unable to set %dx%d video: %s\n", window_width, window_height, SDL_GetError());

    dsPrint("OK\n");

    dsStartGraphics (window_width, window_height, fn);
    if (fn->start) fn->start();

    dsPrint(".. Main Loop\n");

    /// PROGRAM MAIN LOOP ///

    Uint32 start_time = SDL_GetTicks();
    Uint32 this_time = 0;
	int frames = 0;

    GLenum gl_error;
	char* sdl_error;

    while (!done)
    {
        // Message events
        Events(in);

        /// DRAWING STARTS HERE ///

        // clear screen 2D
        SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));

        // clear screen 3D
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        // 3D
        dsDrawFrame (window_width, window_height, fn, initial_pause);

        // 2Do3D
        dsGLPrint(2,2,"%2.2f FPS",((float)frames/(SDL_GetTicks()-start_time))*1000.0);

        /// DRAWING ENDS HERE ///

		// Check for error conditions. //

		gl_error = glGetError( );
		if( gl_error != GL_NO_ERROR ) {
			dsError("OpenGL error: %d\n", gl_error );
		}

		sdl_error = SDL_GetError( );
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

    TTF_CloseFont( font_Courier );

     if ( HUD_GLTexture ) {
      glDeleteTextures( 1, &HUD_GLTexture );
      HUD_GLTexture = 0;
    }

    if (fn->stop) fn->stop();
    dsStopGraphics();
}

GLuint SDL_GL_LoadTexture(SDL_Surface *surface, GLfloat *texcoord)
{
//	GLuint texture;
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

	if ( image == NULL ) {
		return 0;
	}

	// Save the alpha blending attributes //
	saved_flags = surface->flags&(SDL_SRCALPHA|SDL_RLEACCELOK);
	saved_alpha = surface->format->alpha;
	if ( (saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA ) {
		SDL_SetAlpha(surface, 0, 0);
	}

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
	glGenTextures(1, &HUD_GLTexture);

	glBindTexture(GL_TEXTURE_2D, HUD_GLTexture);
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

	return HUD_GLTexture;
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

	/* Note, there may be other things you need to change,
	   depending on how you have your OpenGL state set up.
	*/
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);

	/* This allows alpha blending of 2D textures with the scene */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glViewport(0, 0, screen->w, screen->h);

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

	glPopAttrib();
}
