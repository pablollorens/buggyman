
#include <cstdlib>
#include <windows.h>
#include <editor.h>
#include <vector>

using namespace std;
IMG_Collection image_collection;

//LRESULT CALLBACK WndProc (HWND hWnd, UINT message,WPARAM wParam, LPARAM lParam);
int main ( int argc, char** argv )
{
    Editor ed;
    if(argc == 2) return (ed.Run(argv[1]));
    return (ed.Run("terreno.cfg"));
}

