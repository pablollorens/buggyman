#ifndef __IMG_COLLECTION_H__
#define __IMG_COLLECTION_H__


#include <string>
#include <SDL_Image.h>
#include <map>
using namespace std;

class IMG_Collection
{
    public:
        IMG_Collection();
        IMG_Collection(const IMG_Collection&);
        IMG_Collection& operator=(const IMG_Collection&);
        ~IMG_Collection();
        SDL_Surface* operator()(char* file);
        void Clear();
    private:
        map< string , SDL_Surface* > surfaces;
        map< string, SDL_Surface* >::iterator itr;
};

#endif // __IMG_COLLECTION_H__
