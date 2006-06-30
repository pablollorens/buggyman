#include "functions.h"

list<string> & Get_Directories(char* dir_work)
{
    char dirchain[300];
    list<string> *list_directories = new list<string>;

    string path_work = getcwd(dirchain,300);
    string full_path = path_work + "\\";
           full_path += dir_work;

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
        string  path_completly = full_path + "\\";;
                path_completly += name;

        if (stat(path_completly.c_str(), &dataFile) == -1)
            printf ("not exist directory\n");
        else
        {
            if (S_ISDIR(dataFile.st_mode) && name != "." && name != ".." && name != "CVS")
                (*list_directories).push_back(pent->d_name);
                //printf("%s\n", pent->d_name); // Variable a almacenar -- path_completly --
        }
    }

    if (errno)
    {
        printf ("readdir() failure; terminating");
        exit(1);
    }
    closedir(pdir);

    return *list_directories;
}

list<string> & Get_MusicFiles(char* dir_work)
{
    char dirchain[300];
    list<string> *list_directories = new list<string>;

    string path_work = getcwd(dirchain,300);
    string full_path = path_work + "\\";
           full_path += dir_work;

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
        string  path_completly = full_path + "\\";;
                path_completly += name;

        if (stat(path_completly.c_str(), &dataFile) == -1)
            printf ("not exist directory\n");
        else
        {
            if (!S_ISDIR(dataFile.st_mode) && name != "." && name != ".." && name != "CVS")
            {
                int name_length = name.length();
                string cad_aux = "";
                int position = 0;

                for (int itr = 0; itr < name_length; itr++)
                {
                    cad_aux = name[itr];
                    if (cad_aux == ".") position = itr;
                }

                cad_aux = "";
                for (int itr = position+1; itr < name_length; itr++)
                    cad_aux = cad_aux + name[itr];

                if (cad_aux == "mp3" || cad_aux == "ogg" || cad_aux == "wav" || cad_aux == "wma")
                    (*list_directories).push_back(pent->d_name);
            }
        }
    }

    if (errno)
    {
        printf ("readdir() failure; terminating");
        exit(1);
    }
    closedir(pdir);

    return *list_directories;
}
