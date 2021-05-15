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
#include <stdio.h>
#include <unistd.h>
#include <thread>
#include <filesystem>


#define WIDTH 800
#define HEIGHT 600
#define MAX_LINES 21

typedef struct fileData {
    SDL_Rect iconRect;
    SDL_Texture* iconTexture;

    std::string path;
    SDL_Texture* fileName;
    SDL_Rect fileNameRect;

    SDL_Texture* fileSizeText;
    SDL_Rect fileSizeRect;

    std::string fileType;
} fileData;



typedef struct AppData {
    std::vector<fileData*> fileList;
    TTF_Font *font;
    SDL_Texture* icons[6];
    SDL_Rect nextPageRect;
    SDL_Rect prevPageRect;
    SDL_Rect currPageRect;
    SDL_Texture* nextPageText;
    SDL_Texture* prevPageText;
    SDL_Texture* currPageText;
    int currPage = 0;
    std::string currDirectoryPath;
} AppData;


void initialize(SDL_Renderer *renderer, AppData *data_ptr); //, AppData *data_ptr 
void render(SDL_Renderer *renderer, AppData *data_ptr); // , AppData *data_ptr
void renderDirectory(SDL_Renderer *renderer, std::string directory, AppData *data_ptr);
void renderButtons(SDL_Renderer* renderer, AppData* data_ptr);
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
    data.currDirectoryPath = home;
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
                                data.currPage = 0;
                                data.currDirectoryPath = data.fileList.at(i)->path.c_str();
                                renderDirectory(renderer, data.currDirectoryPath, &data);
                                renderButtons(renderer, &data);
                                render(renderer, &data);
                            }
                            else{
                                int pid = fork();
                                if(pid == 0){
                                   //const char* arg = "xdg-open";
                                   //execl(data.fileList.at(i)->path.c_str(),arg);
                                   std::string command = "xdg-open '" + data.fileList.at(i)->path + "'";
                                   std::cout << system(command.c_str()) << std::endl;
                                   std::terminate();
                                }
                            }
                        }
                }
                if (event.button.button == SDL_BUTTON_LEFT &&
                    event.button.y >= data.nextPageRect.y &&
                    event.button.y <= data.nextPageRect.y + data.nextPageRect.h &&
                    event.button.x >= data.nextPageRect.x &&
                    event.button.x <= data.nextPageRect.x + data.nextPageRect.w &&
                    data.currPage < data.fileList.size() / MAX_LINES){
                        
                        data.currPage++;
                        SDL_DestroyRenderer(renderer);
                        SDL_DestroyWindow(window);
                        SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);
                        renderDirectory(renderer, data.currDirectoryPath, &data);
                        renderButtons(renderer, &data);
                        render(renderer, &data);

                }
                if (event.button.button == SDL_BUTTON_LEFT &&
                    event.button.y >= data.prevPageRect.y &&
                    event.button.y <= data.prevPageRect.y + data.prevPageRect.h &&
                    event.button.x >= data.prevPageRect.x &&
                    event.button.x <= data.prevPageRect.x + data.prevPageRect.w &&
                    data.currPage >= 1){
                        
                        data.currPage--;
                        SDL_DestroyRenderer(renderer);
                        SDL_DestroyWindow(window);
                        SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);
                        renderDirectory(renderer, data.currDirectoryPath, &data);
                        renderButtons(renderer, &data);
                        render(renderer, &data);

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
    data_ptr->font = TTF_OpenFont("resrc/OpenSans-Regular.ttf", 16);
    std::cout << getenv("HOME") << std::endl;
    renderDirectory(renderer, getenv("HOME"), data_ptr);
    renderButtons(renderer, data_ptr);


    
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
        SDL_RenderCopy(renderer, data_ptr->fileList.at(i)->fileSizeText, NULL, &(data_ptr->fileList.at(i)->fileSizeRect));
        
 
    }
    
    SDL_RenderCopy(renderer, data_ptr->nextPageText, NULL, &(data_ptr->nextPageRect));
    SDL_RenderCopy(renderer, data_ptr->prevPageText, NULL, &(data_ptr->prevPageRect));

    // show rendered frame
    SDL_RenderPresent(renderer);
}

void renderDirectory(SDL_Renderer *renderer, std::string directory, AppData *data_ptr) { // , AppData *data_ptr

    DIR* dir = opendir(directory.c_str());
    std::vector<std::string> fileList;
    struct dirent* fileEntry;
    data_ptr->fileList.clear();
    while((fileEntry = readdir(dir)) != NULL) {
        fileList.push_back(fileEntry->d_name);
    }


    std::sort(fileList.begin(), fileList.end());
    fileList.erase(fileList.begin());

    int currTopIndex = data_ptr->currPage * MAX_LINES;
    int currBottomIndex = currTopIndex + MAX_LINES;
    for (int i = currTopIndex; i < currBottomIndex && i < fileList.size(); i++) {
        fileData *file = new fileData();
        std::string filePath = directory + "/" + fileList.at(i);
        //std::cout << filePath << std::endl;
        file->path = filePath;

        file->fileType = getFileType(filePath);
        file->iconRect.x = 10;
        file->iconRect.y = 15 + ((i-currTopIndex + 1) * 25);
        file->iconRect.w = 25;
        file->iconRect.h = 25;
        SDL_Color color = { 0, 0, 0 };

        SDL_Surface *phrase_surf = TTF_RenderText_Solid(data_ptr->font, fileList.at(i).c_str(), color);
        file->fileName = SDL_CreateTextureFromSurface(renderer, phrase_surf);
        SDL_FreeSurface(phrase_surf);
        file->fileNameRect.x = 50;
        file->fileNameRect.y = 15 + ((i-currTopIndex + 1) * 25);
        file->fileNameRect.h = 25;
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

        if(file->fileType != "directory"){
            double fileSizeBits = (double)std::filesystem::file_size(filePath);
            std::string adjustedFileSize;
            if(fileSizeBits < 1024){
                adjustedFileSize = std::to_string(fileSizeBits) + " B";
            }else if (fileSizeBits >= 1024 && fileSizeBits < 1048576){
                adjustedFileSize = std::to_string(fileSizeBits/1024) + " KiB";
            }else if( fileSizeBits >= 1048576 && fileSizeBits < 1073741824){
                adjustedFileSize = std::to_string(fileSizeBits/1048576) + " MiB";
            }else{
                adjustedFileSize = std::to_string(fileSizeBits/1073741824) + " GiB";
            }

            SDL_Surface *sizeSurf = TTF_RenderText_Solid(data_ptr->font, adjustedFileSize.c_str(), color);
            file->fileSizeText = SDL_CreateTextureFromSurface(renderer, sizeSurf);
            SDL_FreeSurface(sizeSurf);
            file->fileSizeRect.x = 300;
            file->fileSizeRect.y = 15 + ((i-currTopIndex + 1) * 25);
            file->fileSizeRect.w = 100;
            file->fileSizeRect.h = 24;
            SDL_QueryTexture(file->fileName, NULL, NULL, &(file->fileSizeRect.w), &(file->fileSizeRect.h));


        }

        file->iconTexture = SDL_CreateTextureFromSurface(renderer, img_surf);
        SDL_FreeSurface(img_surf);
        data_ptr->fileList.push_back(file);

    }

}

void renderButtons(SDL_Renderer* renderer, AppData* data_ptr){
    SDL_Color nextColor;
    if(data_ptr->currPage >= data_ptr->fileList.size()/MAX_LINES){
        nextColor = {150, 150, 150};
    }else{
        nextColor = {255, 0, 0};
    } 
    SDL_Surface *nextSurf = TTF_RenderText_Solid(data_ptr->font, "NEXT", nextColor);
    data_ptr->nextPageText = SDL_CreateTextureFromSurface(renderer, nextSurf);
    SDL_FreeSurface(nextSurf);
    data_ptr->nextPageRect.x = WIDTH - 40;
    data_ptr->nextPageRect.y = HEIGHT - 25;

    SDL_QueryTexture(data_ptr->nextPageText, NULL, NULL, &(data_ptr->nextPageRect.w), &(data_ptr->nextPageRect.h));

    SDL_Color prevColor;
    if(data_ptr->currPage == 0){
        prevColor = {150, 150, 150};
    }else{
        prevColor = {255, 0, 0};
    }
    SDL_Surface *prevSurf = TTF_RenderText_Solid(data_ptr->font, "PREV", prevColor);
    data_ptr->prevPageText = SDL_CreateTextureFromSurface(renderer, prevSurf);
    SDL_FreeSurface(prevSurf);
    data_ptr->prevPageRect.x = 10;
    data_ptr->prevPageRect.y = HEIGHT - 25;
    data_ptr->prevPageRect.w = 400;
    data_ptr->prevPageRect.h = 25;
    SDL_QueryTexture(data_ptr->prevPageText, NULL, NULL, &(data_ptr->prevPageRect.w), &(data_ptr->prevPageRect.h));


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