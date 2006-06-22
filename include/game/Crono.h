#ifndef CLASS_CRONO
#define CLASS_CRONO

#include <SDL/SDL.h>

class Crono {

  public:

    inline void Start(){ time_initial = SDL_GetTicks(); };
    inline void Reset(){ time_initial = SDL_GetTicks(); };
    inline void Stop(){  time_final = SDL_GetTicks(); };

    inline Uint32 getTime(){ return( SDL_GetTicks() ); };
    inline Uint32 getFinal(){ return( time_final-time_initial ); };


  private:
    Uint32 time_initial;
    Uint32 time_final;

};

#endif
