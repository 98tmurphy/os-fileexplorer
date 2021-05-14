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
    std::string path;
    std::string fileNameString;
    int iconIndex;
    SDL_Rect iconRect;
    SDL_Texture* fileName;
    SDL_Rect fileNameRect;
    std::string fileType;
    SDL_Texture* iconTexture;
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
std::string getFileType(std::string file);

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
            case SDL_MOUSEBUTTONDOWN:
            //check to see if the mouse is clicking on a file
                for(int i = 0; i < data.fileList.size(); i++){
                    if (event.button.button == SDL_BUTTON_LEFT &&
                        event.button.y >= data.fileList.at(i)->fileNameRect.y &&
                        event.button.y <= data.fileList.at(i)->fileNameRect.y + data.fileList.at(i)->fileNameRect.h){
                            
                            if(getFileType(data.fileList.at(i)->path) == "directory"){
                                // see if the mouse is clicking on a directory
                                SDL_DestroyRenderer(renderer);
                                SDL_DestroyWindow(window);
                                SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);
                                if(data.fileList.at(i)->fileNameString == ".."){
                                    //checks if the directory is the parent directory
                                    std::cout << "FOUND THE FUCKER" << std::endl;
                                    data.fileList.at(i)->path = data.fileList.at(i)->path + "/..";
                                    std::cout << data.fileList.at(i)->path << std::endl;
                                }
                                
                                
                                renderDirectory(renderer, data.fileList.at(i)->path.c_str(), &data);
                                render(renderer, &data);
                                break;
                            }
                        }
                }
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
        SDL_SetRenderDrawColor(renderer, 235, 0, 255, 255);
        //SDL_RenderFillRect(renderer, &(data_ptr->fileList.at(i)->iconRect));
        SDL_RenderCopy(renderer, data_ptr->fileList.at(i)->iconTexture, NULL, &(data_ptr->fileList.at(i)->iconRect));
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
    fileList.erase(fileList.begin());


    for (int i = 0; i < fileList.size(); i++) {
        fileData *file = new fileData();
        std::string filePath = directory + "/" + fileList.at(i);
        std::cout << filePath << std::endl;
        file->path = filePath;
        file->fileNameString = fileList.at(i);

        file->fileType = getFileType(filePath);
        file->iconIndex = 0;
        file->iconRect.x = 10;
        file->iconRect.y = 25 + (i * 25);
        file->iconRect.w = 25;
        file->iconRect.h = 25;
        SDL_Color color = { 0, 0, 0 };

        SDL_Surface *phrase_surf = TTF_RenderText_Solid(data_ptr->font, fileList.at(i).c_str(), color);
        file->fileName = SDL_CreateTextureFromSurface(renderer, phrase_surf);
        SDL_FreeSurface(phrase_surf);
        file->fileNameRect.x = 50;
        file->fileNameRect.y = 25 + (i * 25);
        file->fileNameRect.h = 50;
        SDL_QueryTexture(file->fileName, NULL, NULL, &(file->fileNameRect.w), &(file->fileNameRect.h));


        SDL_Surface *img_surf;
        //img_surf = IMG_Load("resrc/exe-extension.png");


        if (file->fileType == "executable") {
            ///std::cout << "exe" << std::endl;
            img_surf = IMG_Load("resrc/exe-extension.png");
        } else if (file->fileType == "directory") {
            //std::cout << "dir" << std::endl;
            img_surf = IMG_Load("resrc/directory-extension.webp");
        } else if (file->fileType == "image") {
            //std::cout << "img" << std::endl;
            img_surf = IMG_Load("resrc/img-extension.png");
        } else if (file->fileType == "video") {
            //std::cout << "vid" << std::endl;
            img_surf = IMG_Load("resrc/video-extension.png");
        } else if (file->fileType == "other") {
            //std::cout << "other" << std::endl;
            img_surf = IMG_Load("resrc/other-extension.png");
        } else if (file->fileType == "code") {
            //std::cout << "code" << std::endl;
            img_surf = IMG_Load("resrc/code-extension.png");
        }

        file->iconTexture = SDL_CreateTextureFromSurface(renderer, img_surf);
        SDL_FreeSurface(img_surf);
        data_ptr->fileList.push_back(file);

    }


    
}

void quit(AppData *data_ptr) {

    //SDL_DestroyTexture(data_ptr->)
    //SDL_DestroyTexture(data_ptr->)
    //TTF_CloseFont(data_ptr->font);

}

// returns the file's type in the form of a string

std::string getFileType(std::string file) {
    std::vector<std::string> imgExtensions = {".jpg", ",jpeg", ".png", ".tif", ".tiff", ".gif", ".webp"};
    std::vector<std::string> videoExtensions = {".mp4", ".mov", ".mkv", ".avi", ".webm"};
    std::vector<std::string> codeExtensions = {".h", ".c", ".cpp", ".py", ".java", ".js"};
    const std::filesystem::path path(file);


    if (std::filesystem::is_directory(path)) {
        return "directory";
    }

    for (int i = 0; i < imgExtensions.size(); i++) {
        std::size_t match = file.find(imgExtensions[i]);
        if (match != std::string::npos) {
            return "image";
        }
    }

    for (int i = 0; i < videoExtensions.size(); i++) {
        std::size_t match = file.find(videoExtensions[i]);
        if (match != std::string::npos) {
            return "video";
        }
    }

    for (int i = 0; i < codeExtensions.size(); i++) {
        std::size_t match = file.find(codeExtensions[i]);
        if (match != std::string::npos) {
            return "code";
        }
    }

    return "other";

    



}