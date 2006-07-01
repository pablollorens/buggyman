#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#include <SDL.h>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <list>
#include <vector>
#include <iostream>

using namespace std;

list<string> & Get_Directories(char*);
vector<string> & Get_MusicFiles(char*);

#endif //__FUNCTIONS_H__

