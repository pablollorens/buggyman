#include <img_collection.h>

IMG_Collection::IMG_Collection()
{
}

IMG_Collection::IMG_Collection(const IMG_Collection& some)
{
    (*this)=some;
}

IMG_Collection&
IMG_Collection::operator=(const IMG_Collection& some)
{
	if(this != &some)
	{
        surfaces = some.surfaces;
	}
    return(*this);
}

IMG_Collection::~IMG_Collection()
{
    Clear();
}

SDL_Surface*
IMG_Collection::operator()(char* file)
{
    SDL_Surface* tmp = surfaces[file];
    if(!tmp)
    {
        printf("Loading bitmap: \"%s\"\n",file);
        if(!(tmp = surfaces[file] = IMG_Load(file)))
        {
            fprintf(stderr,"Unable to load bitmap: %s\n", SDL_GetError());
            exit(1);
        }
    }
    return tmp;
}

void
IMG_Collection::Clear()
{
    for(itr = surfaces.begin(); itr != surfaces.end(); itr++)
        SDL_FreeSurface((*itr).second);
    surfaces.clear();
    itr = surfaces.begin();
}

