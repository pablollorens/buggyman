#include"functions.h"

Blit_Task* New_Blit_Task(SDL_Surface * some_src, SDL_Rect &some_window, SDL_Surface * some_dst)
{
    Blit_Task* bt = new Blit_Task;
    bt->src = some_src;
    bt->window.x = some_window.x;
    bt->window.y = some_window.y;
    bt->window.w = some_window.w;
    bt->window.h = some_window.h;
    bt->dst = some_dst;
    return bt;
}

list<string> Get_Directories(char* dir_work)
{
    char dirchain[300];
    list<string> list_directories;

    string path_work = getcwd(dirchain,300);
    string full_path = path_work + "\\" + dir_work;

    DIR *pdir;
    struct dirent *pent;
    struct stat dataFile;

    pdir=opendir(full_path.c_str());
    if (!pdir)
    {
        printf ("opendir() failure; terminating");
        exit(1);
    }
    errno=0;
    while ((pent=readdir(pdir)))
    {
        string name = pent->d_name;
        string path_completly = full_path + "\\" + name;

        if (stat(path_completly.c_str(), &dataFile) == -1)
            printf ("not exist directory\n");
        else
        {
            if (S_ISDIR(dataFile.st_mode) && name != "." && name != ".." && name != "CVS")
                list_directories.push_back(path_completly);
                //printf("%s\n", pent->d_name); // Variable a almacenar -- path_completly --
        }
    }

    if (errno)
    {
        printf ("readdir() failure; terminating");
        exit(1);
    }
    closedir(pdir);

    return list_directories;
}

