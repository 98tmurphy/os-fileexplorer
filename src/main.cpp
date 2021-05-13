#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string.h>
#include <vector>
#include <filesystem>
#include <dirent.h>
#include <algorithm>

#define WIDTH 800
#define HEIGHT 600

typedef struct AppData {
    TTF_Font *font;
    SDL_Texture *penguin;
    SDL_Texture *phrase;
    SDL_Rect penguin_rect;
    SDL_Rect phrase_rect;
    bool penguin_selected;
    bool phrase_selected;
    SDL_Point offset;
} AppData;


void initialize(SDL_Renderer *renderer); //, AppData *data_ptr
void render(SDL_Renderer *renderer); // , AppData *data_ptr
void renderDirectory(std::string directory);
void quit(AppData *data_ptr);

int main(int argc, char **argv)
{
    char *home = getenv("HOME");
    printf("HOME: %s\n", home);
    renderDirectory(home);

    // initializing SDL as Video
    SDL_Init(SDL_INIT_VIDEO);

    // create window and renderer
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);

    // initialize and perform rendering loop
    initialize(renderer);
    render(renderer);
    SDL_Event event;
    SDL_WaitEvent(&event);
    while (event.type != SDL_QUIT)
    {
        //render(renderer);
        SDL_WaitEvent(&event);
    }

    // clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void initialize(SDL_Renderer *renderer) // , AppData *data_ptr
{
    // set color of background when erasing frame
    SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);
}

void render(SDL_Renderer *renderer) // , AppData *data_ptr
{
    // erase renderer content
    SDL_RenderClear(renderer);
    
    // TODO: draw!

    // show rendered frame
    SDL_RenderPresent(renderer);
}

void renderDirectory(std::string directory) { // , AppData *data_ptr

    DIR* dir = opendir(directory.c_str());
    std::vector<std::string> fileList;
    struct dirent* fileEntry;
    while((fileEntry = readdir(dir)) != NULL) {
        fileList.push_back(fileEntry->d_name);
    }

    std::sort(fileList.begin(), fileList.end());

    for (int i = 0; i < fileList.size(); i++) {
        std::cout << fileList.at(i) << std::endl;
    }
    

    
}




