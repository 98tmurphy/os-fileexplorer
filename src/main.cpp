#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string.h>
#include <vector>
#include <filesystem>
#include <dirent.h>
#include <algorithm>
#include <string>
#include <cstring>
#include <sys/stat.h>


#define WIDTH 800
#define HEIGHT 600


typedef struct fileData {
    int iconIndex;
    SDL_Rect iconRect;
    SDL_Texture* fileName;
    SDL_Rect fileNameRect;
} fileData;



typedef struct AppData {
    std::vector<fileData*> fileList;
    TTF_Font *font;
    SDL_Texture* icons[6];




} AppData;


void initialize(SDL_Renderer *renderer, AppData *data_ptr); //, AppData *data_ptr
void render(SDL_Renderer *renderer, AppData *data_ptr); // , AppData *data_ptr
void renderDirectory(SDL_Renderer *renderer, std::string directory, AppData *data_ptr);
void quit(AppData *data_ptr);

int main(int argc, char **argv)
{
    char *home = getenv("HOME");
    printf("HOME: %s\n", home);
    //renderDirectory(home);

    // initializing SDL as Video
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    // create window and renderer
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);

    // initialize and perform rendering loop
    AppData data;
    initialize(renderer, &data);
    render(renderer, &data);
    SDL_Event event;
    SDL_WaitEvent(&event);
    while (event.type != SDL_QUIT)
    {
        //render(renderer);
        SDL_WaitEvent(&event);
        switch(event.type) 
        {

        }
    }

    // clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void initialize(SDL_Renderer *renderer, AppData *data_ptr) // , AppData *data_ptr
{
    // set color of background when erasing frame
    data_ptr->font = TTF_OpenFont("resrc/OpenSans-Regular.ttf", 12);
    std::cout << getenv("HOME") << std::endl;
    renderDirectory(renderer, getenv("HOME"), data_ptr);


    
}

void render(SDL_Renderer *renderer, AppData *data_ptr) // , AppData *data_ptr
{
    // erase renderer content
    SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);
    SDL_RenderClear(renderer);
    
    // TODO: draw!

    for (int i = 0; i < data_ptr->fileList.size(); i++) {
        SDL_SetRenderDrawColor(renderer, 235, 0, 0, 255);
        SDL_RenderFillRect(renderer, &(data_ptr->fileList.at(i)->iconRect));
        SDL_RenderCopy(renderer, data_ptr->fileList.at(i)->fileName, NULL, &(data_ptr->fileList.at(i)->fileNameRect));
 
    }
   


    // show rendered frame
    SDL_RenderPresent(renderer);
}

void renderDirectory(SDL_Renderer *renderer, std::string directory, AppData *data_ptr) { // , AppData *data_ptr

    DIR* dir = opendir(directory.c_str());
    std::vector<std::string> fileList;
    struct dirent* fileEntry;
    while((fileEntry = readdir(dir)) != NULL) {
        fileList.push_back(fileEntry->d_name);
    }

    std::sort(fileList.begin(), fileList.end());

    for (int i = 0; i < fileList.size(); i++) {
        fileData *file = new fileData();
        file->iconIndex = 0;
        file->iconRect.x = 25;
        file->iconRect.y = 25 + (i * 25);
        file->iconRect.w = 16;
        file->iconRect.h = 16;
        SDL_Color color = { 0, 0, 0 };
        SDL_Surface *phrase_surf = TTF_RenderText_Solid(data_ptr->font, fileList.at(i).c_str(), color);
        file->fileName = SDL_CreateTextureFromSurface(renderer, phrase_surf);
        SDL_FreeSurface(phrase_surf);
        file->fileNameRect.x = 100;
        file->fileNameRect.y = 25 + (i * 25);
        SDL_QueryTexture(file->fileName, NULL, NULL, &(file->fileNameRect.w), &(file->fileNameRect.h));
        data_ptr->fileList.push_back(file);
    }
    

    
}

void quit(AppData *data_ptr) {

    //SDL_DestroyTexture(data_ptr->)
    //SDL_DestroyTexture(data_ptr->)
    //TTF_CloseFont(data_ptr->font);

}




