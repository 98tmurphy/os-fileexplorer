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
    std::string nameString;
    SDL_Texture* fileName;
    SDL_Rect fileNameRect;

    SDL_Texture* fileSizeText;
    SDL_Rect fileSizeRect;

    std::string fileType;
    int offset;
} fileData;



typedef struct AppData {
    std::vector<fileData*> fileList;
    std::vector<fileData*> recFileList;
    TTF_Font *font;
    SDL_Texture* icons[6];
    SDL_Rect nextPageRect;
    SDL_Rect prevPageRect;
    SDL_Rect currPageRect;
    SDL_Rect recursiveRect;
    SDL_Texture* nextPageText;
    SDL_Texture* prevPageText;
    SDL_Texture* currPageText;
    SDL_Texture* recursiveText;
    int currPage = 0;
    std::string currDirectoryPath;
    bool recursive = false;
    bool initial = true;
} AppData;


void initialize(SDL_Renderer *renderer, AppData *data_ptr); 
void render(SDL_Renderer *renderer, AppData *data_ptr, std::string directory);
void renderDirectory(SDL_Renderer *renderer, std::string directory, AppData *data_ptr);
void renderRecDirectory(SDL_Renderer *renderer, std::string directory, AppData *data_ptr);
void renderButtons(SDL_Renderer* renderer, AppData* data_ptr);
void getDirectories(SDL_Renderer *renderer, std::string directory, AppData *data_ptr);
void getRecursiveDirectories(SDL_Renderer *renderer, std::string directory, AppData *data_ptr, int offset);
void quit(AppData *data_ptr);
std::string getFileType(std::string file);
char* getPermissions(char *file);

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
    render(renderer, &data, data.currDirectoryPath);
    data.initial = false;
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
                            
                            if(getFileType(data.fileList.at(i)->path) == "directory" && data.recursive == false){
                                // see if the mouse is clicking on a directory
                                SDL_DestroyRenderer(renderer);
                                SDL_DestroyWindow(window);
                                SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);
                                data.currPage = 0;
                                data.currDirectoryPath = data.fileList.at(i)->path.c_str();
                                getDirectories(renderer, data.currDirectoryPath, &data);
                                renderButtons(renderer, &data);
                                render(renderer, &data, data.currDirectoryPath);
                            }
                            else if (getFileType(data.fileList.at(i)->path) != "directory"){
                                int pid = fork();
                                if(pid == 0){
                                   //const char* arg = "xdg-open";
                                   //execl(data.fileList.at(i)->path.c_str(),arg);
                                   std::string command = "xdg-open '" + data.fileList.at(i)->path + "'";
                                   std::cout << system(command.c_str()) << std::endl;
                                   return 0;
                                }
                            }
                        }
                }
                if (event.button.button == SDL_BUTTON_LEFT &&
                    event.button.y >= data.nextPageRect.y &&
                    event.button.y <= data.nextPageRect.y + data.nextPageRect.h &&
                    event.button.x >= data.nextPageRect.x &&
                    event.button.x <= data.nextPageRect.x + data.nextPageRect.w){
                        if((data.currPage < data.fileList.size() / MAX_LINES && data.recursive == false) || 
                           (data.currPage < data.recFileList.size() / MAX_LINES && data.recursive == true)){
                        
                                data.currPage++;
                                SDL_DestroyRenderer(renderer);
                                SDL_DestroyWindow(window);
                                SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);
                                //renderDirectory(renderer, data.currDirectoryPath, &data);
                                renderButtons(renderer, &data);
                                render(renderer, &data, data.currDirectoryPath);
                           }

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
                        //renderDirectory(renderer, data.currDirectoryPath, &data);
                        renderButtons(renderer, &data);
                        render(renderer, &data, data.currDirectoryPath);

                }
                if (event.button.button == SDL_BUTTON_LEFT &&
                    event.button.y >= data.recursiveRect.y &&
                    event.button.y <= data.recursiveRect.y + data.recursiveRect.h &&
                    event.button.x >= data.recursiveRect.x &&
                    event.button.x <= data.recursiveRect.x + data.recursiveRect.w){

                        if(data.recursive == true){
                            data.recursive = false;
                        }else{
                            data.recursive = true;
                        }
                        data.currPage = 0;
                        SDL_DestroyRenderer(renderer);
                        SDL_DestroyWindow(window);
                        SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);
                        //renderDirectory(renderer, data.currDirectoryPath, &data);
                        renderButtons(renderer, &data);
                        render(renderer, &data, data.currDirectoryPath);

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
    std::string directory(getenv("HOME"));
    data_ptr->font = TTF_OpenFont("resrc/Anonymous.ttf", 16);
    std::cout << getenv("HOME") << std::endl;
    getRecursiveDirectories(renderer, directory, data_ptr, 0);
    getDirectories(renderer, directory, data_ptr);
    renderDirectory(renderer, getenv("HOME"), data_ptr);
    renderButtons(renderer, data_ptr);
}

void render(SDL_Renderer *renderer, AppData *data_ptr,std::string directory) // , AppData *data_ptr
{
    // erase renderer content
    SDL_SetRenderDrawColor(renderer, 170, 160, 144, 255);
    SDL_RenderClear(renderer);
    
    // TODO: draw!
    if(data_ptr->recursive == false){
        renderDirectory(renderer, directory, data_ptr);
        for (int i = 0; i < data_ptr->fileList.size(); i++) {
            SDL_SetRenderDrawColor(renderer, 235, 0, 255, 255);
            //SDL_RenderFillRect(renderer, &(data_ptr->fileList.at(i)->iconRect));
            SDL_RenderCopy(renderer, data_ptr->fileList.at(i)->iconTexture, NULL, &(data_ptr->fileList.at(i)->iconRect));
            SDL_RenderCopy(renderer, data_ptr->fileList.at(i)->fileName, NULL, &(data_ptr->fileList.at(i)->fileNameRect));
            SDL_RenderCopy(renderer, data_ptr->fileList.at(i)->fileSizeText, NULL, &(data_ptr->fileList.at(i)->fileSizeRect));
            
        }
    }else{
        renderRecDirectory(renderer, directory, data_ptr);
        for (int i = 0; i < data_ptr->recFileList.size(); i++) {
            SDL_SetRenderDrawColor(renderer, 235, 0, 255, 255);
            //SDL_RenderFillRect(renderer, &(data_ptr->fileList.at(i)->iconRect));
            SDL_RenderCopy(renderer, data_ptr->recFileList.at(i)->iconTexture, NULL, &(data_ptr->recFileList.at(i)->iconRect));
            SDL_RenderCopy(renderer, data_ptr->recFileList.at(i)->fileName, NULL, &(data_ptr->recFileList.at(i)->fileNameRect));
            SDL_RenderCopy(renderer, data_ptr->recFileList.at(i)->fileSizeText, NULL, &(data_ptr->recFileList.at(i)->fileSizeRect));
            
        }
    }
    
    SDL_RenderCopy(renderer, data_ptr->nextPageText, NULL, &(data_ptr->nextPageRect));
    SDL_RenderCopy(renderer, data_ptr->prevPageText, NULL, &(data_ptr->prevPageRect));
    SDL_RenderCopy(renderer, data_ptr->recursiveText, NULL, &(data_ptr->recursiveRect));

    // show rendered frame
    SDL_RenderPresent(renderer);
}

void renderDirectory(SDL_Renderer *renderer, std::string directory, AppData *data_ptr) { // , AppData *data_ptr

    int currTopIndex = data_ptr->currPage * MAX_LINES;
    int currBottomIndex = currTopIndex + MAX_LINES;
    for (int i = currTopIndex; i < currBottomIndex && i < data_ptr->fileList.size(); i++) {
        //std::cout << data_ptr->fileList.at(i)->nameString << std::endl;
        
        SDL_Color color = { 0, 0, 0 };
        data_ptr->fileList.at(i)->iconRect.x = 10 +(50 * data_ptr->fileList.at(i)->offset);
        data_ptr->fileList.at(i)->iconRect.y = 15 + ((i - currTopIndex + 1) * 25);
        data_ptr->fileList.at(i)->iconRect.w = 25;
        data_ptr->fileList.at(i)->iconRect.h = 25;

        SDL_Surface *img_surf;
        if (data_ptr->fileList.at(i)->fileType == "executable") {
            ///std::cout << "exe" << std::endl;
            img_surf = IMG_Load("resrc/exe-extension.png");
        } else if (data_ptr->fileList.at(i)->fileType == "directory") {
            //std::cout << "dir" << std::endl;
            img_surf = IMG_Load("resrc/directory-extension.webp");
        } else if (data_ptr->fileList.at(i)->fileType == "image") {
            //std::cout << "img" << std::endl;
            img_surf = IMG_Load("resrc/img-extension.png");
        } else if (data_ptr->fileList.at(i)->fileType == "video") {
            //std::cout << "vid" << std::endl;
            img_surf = IMG_Load("resrc/video-extension.png");
        } else if (data_ptr->fileList.at(i)->fileType == "other") {
            //std::cout << "other" << std::endl;
            img_surf = IMG_Load("resrc/other-extension.png");
        } else if (data_ptr->fileList.at(i)->fileType == "code") {
            //std::cout << "code" << std::endl;
            img_surf = IMG_Load("resrc/code-extension.png");
        }

        data_ptr->fileList.at(i)->iconTexture = SDL_CreateTextureFromSurface(renderer, img_surf);
        SDL_FreeSurface(img_surf);

        if(data_ptr->fileList.at(i)->fileType != "directory" && data_ptr->fileList.at(i)->offset == 0){
            int spacesToAdd = 45 - data_ptr->fileList.at(i)->nameString.length();
            for(int j = 0; j < spacesToAdd; j++){
                data_ptr->fileList.at(i)->nameString.append(" ");
            }

            double fileSizeBits = (double)std::filesystem::file_size(data_ptr->fileList.at(i)->path);
            std::string adjustedFileSize;

            if(fileSizeBits < 1024){
                data_ptr->fileList.at(i)->nameString.append(std::to_string(fileSizeBits) + " B");
            }else if (fileSizeBits >= 1024 && fileSizeBits < 1048576){
                data_ptr->fileList.at(i)->nameString.append(std::to_string(fileSizeBits/1024) + " KiB");
            }else if( fileSizeBits >= 1048576 && fileSizeBits < 1073741824){
                data_ptr->fileList.at(i)->nameString.append(std::to_string(fileSizeBits/1048576) + " MiB");
            }else{
                data_ptr->fileList.at(i)->nameString.append(std::to_string(fileSizeBits/1073741824) + " GiB");
            }

            spacesToAdd = 59 - data_ptr->fileList.at(i)->nameString.length();
            for(int j = 0; j < spacesToAdd; j++){
                data_ptr->fileList.at(i)->nameString.append(" ");
            }
            data_ptr->fileList.at(i)->nameString.append(getPermissions((char*)data_ptr->fileList.at(i)->path.c_str()));
           
        }
             
        SDL_Surface *phrase_surf = TTF_RenderText_Solid(data_ptr->font, data_ptr->fileList.at(i)->nameString.c_str(), color);
        data_ptr->fileList.at(i)->fileName = SDL_CreateTextureFromSurface(renderer, phrase_surf);
        SDL_FreeSurface(phrase_surf);
        data_ptr->fileList.at(i)->fileNameRect.x = 50 + (50 * data_ptr->fileList.at(i)->offset);
        data_ptr->fileList.at(i)->fileNameRect.y = 23 + ((i - currTopIndex + 1) * 25);
        data_ptr->fileList.at(i)->fileNameRect.h = 25;
        SDL_QueryTexture(data_ptr->fileList.at(i)->fileName, NULL, NULL, &(data_ptr->fileList.at(i)->fileNameRect.w), &(data_ptr->fileList.at(i)->fileNameRect.h));

    }
    

}

void renderRecDirectory(SDL_Renderer *renderer, std::string directory, AppData *data_ptr) { // , AppData *data_ptr
    int currTopIndex = data_ptr->currPage * MAX_LINES;
    int currBottomIndex = currTopIndex + MAX_LINES;
    for (int i = currTopIndex; i < currBottomIndex && i < data_ptr->recFileList.size(); i++) {
        //std::cout << data_ptr->fileList.at(i)->nameString << std::endl;
        
        SDL_Color color = { 0, 0, 0 };
        data_ptr->recFileList.at(i)->iconRect.x = 10 +(50 * data_ptr->recFileList.at(i)->offset);
        data_ptr->recFileList.at(i)->iconRect.y = 15 + ((i - currTopIndex + 1) * 25);
        data_ptr->recFileList.at(i)->iconRect.w = 25;
        data_ptr->recFileList.at(i)->iconRect.h = 25;

        SDL_Surface *img_surf;
        if (data_ptr->recFileList.at(i)->fileType == "executable") {
            ///std::cout << "exe" << std::endl;
            img_surf = IMG_Load("resrc/exe-extension.png");
        } else if (data_ptr->recFileList.at(i)->fileType == "directory") {
            //std::cout << "dir" << std::endl;
            img_surf = IMG_Load("resrc/directory-extension.webp");
        } else if (data_ptr->recFileList.at(i)->fileType == "image") {
            //std::cout << "img" << std::endl;
            img_surf = IMG_Load("resrc/img-extension.png");
        } else if (data_ptr->recFileList.at(i)->fileType == "video") {
            //std::cout << "vid" << std::endl;
            img_surf = IMG_Load("resrc/video-extension.png");
        } else if (data_ptr->recFileList.at(i)->fileType == "other") {
            //std::cout << "other" << std::endl;
            img_surf = IMG_Load("resrc/other-extension.png");
        } else if (data_ptr->recFileList.at(i)->fileType == "code") {
            //std::cout << "code" << std::endl;
            img_surf = IMG_Load("resrc/code-extension.png");
        }

        data_ptr->recFileList.at(i)->iconTexture = SDL_CreateTextureFromSurface(renderer, img_surf);
        SDL_FreeSurface(img_surf);

        if(data_ptr->recFileList.at(i)->fileType != "directory" && data_ptr->recFileList.at(i)->offset == 0){
            int spacesToAdd = 45 - data_ptr->recFileList.at(i)->nameString.length();
            for(int j = 0; j < spacesToAdd; j++){
                data_ptr->recFileList.at(i)->nameString.append(" ");
            }

            double fileSizeBits = (double)std::filesystem::file_size(data_ptr->recFileList.at(i)->path);
            std::string adjustedFileSize;

            if(fileSizeBits < 1024){
                data_ptr->recFileList.at(i)->nameString.append(std::to_string(fileSizeBits) + " B");
            }else if (fileSizeBits >= 1024 && fileSizeBits < 1048576){
                data_ptr->recFileList.at(i)->nameString.append(std::to_string(fileSizeBits/1024) + " KiB");
            }else if( fileSizeBits >= 1048576 && fileSizeBits < 1073741824){
                data_ptr->recFileList.at(i)->nameString.append(std::to_string(fileSizeBits/1048576) + " MiB");
            }else{
                data_ptr->recFileList.at(i)->nameString.append(std::to_string(fileSizeBits/1073741824) + " GiB");
            }

            spacesToAdd = 59 - data_ptr->recFileList.at(i)->nameString.length();
            for(int j = 0; j < spacesToAdd; j++){
                data_ptr->recFileList.at(i)->nameString.append(" ");
            }
            data_ptr->recFileList.at(i)->nameString.append(getPermissions((char*)data_ptr->recFileList.at(i)->path.c_str()));
           
        }
             
        SDL_Surface *phrase_surf = TTF_RenderText_Solid(data_ptr->font, data_ptr->recFileList.at(i)->nameString.c_str(), color);
        data_ptr->recFileList.at(i)->fileName = SDL_CreateTextureFromSurface(renderer, phrase_surf);
        SDL_FreeSurface(phrase_surf);
        data_ptr->recFileList.at(i)->fileNameRect.x = 50 + (50 * data_ptr->recFileList.at(i)->offset);
        data_ptr->recFileList.at(i)->fileNameRect.y = 23 + ((i - currTopIndex + 1) * 25);
        data_ptr->recFileList.at(i)->fileNameRect.h = 25;
        SDL_QueryTexture(data_ptr->recFileList.at(i)->fileName, NULL, NULL, &(data_ptr->recFileList.at(i)->fileNameRect.w), &(data_ptr->recFileList.at(i)->fileNameRect.h));

    }
    

}

void renderButtons(SDL_Renderer* renderer, AppData* data_ptr){
    SDL_Color nextColor;
    if(data_ptr->recursive == false){
        if(data_ptr->currPage >= data_ptr->fileList.size()/MAX_LINES){
            nextColor = {150, 150, 150};
        }else{
            nextColor = {255, 0, 0};
        }
    }else{
        if(data_ptr->currPage >= data_ptr->recFileList.size()/MAX_LINES){
            nextColor = {150, 150, 150};
        }else{
            nextColor = {255, 0, 0};
        }
    }
    SDL_Surface *nextSurf = TTF_RenderText_Solid(data_ptr->font, "NEXT", nextColor);
    data_ptr->nextPageText = SDL_CreateTextureFromSurface(renderer, nextSurf);
    SDL_FreeSurface(nextSurf);
    data_ptr->nextPageRect.x = WIDTH - 45;
    data_ptr->nextPageRect.y = HEIGHT - 20;

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
    data_ptr->prevPageRect.h = 20;
    SDL_QueryTexture(data_ptr->prevPageText, NULL, NULL, &(data_ptr->prevPageRect.w), &(data_ptr->prevPageRect.h));

    SDL_Color recColor;
    if(data_ptr->recursive == true ){
        recColor = {150, 0, 150};
    }else{
        recColor = {150, 150, 150};
    }
    SDL_Surface *recSurface = TTF_RenderText_Solid(data_ptr->font, "RECURSIVE", recColor);
    data_ptr->recursiveText = SDL_CreateTextureFromSurface(renderer, recSurface);
    SDL_FreeSurface(recSurface);
    data_ptr->recursiveRect.x = 350;
    data_ptr->recursiveRect.y = 2;
    data_ptr->recursiveRect.h = 20;
    data_ptr->recursiveRect.w = 400;
    SDL_QueryTexture(data_ptr->recursiveText, NULL, NULL, &(data_ptr->recursiveRect.w), &(data_ptr->recursiveRect.h));

}

void getRecursiveDirectories(SDL_Renderer *renderer, std::string directory, AppData *data_ptr, int offset){
    DIR* dir = opendir(directory.c_str());
    std::vector<std::string> fileList;
    struct dirent* fileEntry;
    
    while((fileEntry = readdir(dir)) != NULL) {
        fileList.push_back(fileEntry->d_name);
    }

    std::sort(fileList.begin(), fileList.end());
    fileList.erase(fileList.begin());
    
    for(int i = 0; i < fileList.size(); i++){
        fileData *file = new fileData();
        std::string filePath = directory + "/" + fileList.at(i);
        file->path = filePath;
        file->nameString = fileList.at(i);
        file->fileType = getFileType(filePath);
        file->offset = offset;
        data_ptr->recFileList.push_back(file);
        if(file->fileType == "directory" && i >= 1){
            getRecursiveDirectories(renderer, filePath, data_ptr, offset + 1);
        }
    }
    return;
}

void getDirectories(SDL_Renderer *renderer, std::string directory, AppData *data_ptr){
    data_ptr->fileList.clear();
    DIR* dir = opendir(directory.c_str());
    std::vector<std::string> fileList;
    struct dirent* fileEntry;
    
    while((fileEntry = readdir(dir)) != NULL) {
        fileList.push_back(fileEntry->d_name);
    }
    
    std::sort(fileList.begin(), fileList.end());
    fileList.erase(fileList.begin());
    
    for(int i = 0; i < fileList.size(); i++){    
        fileData *file = new fileData();
        std::string filePath = directory + "/" + fileList.at(i);
        file->path = filePath;
        file->nameString = fileList.at(i);
        file->fileType = getFileType(filePath);
        data_ptr->fileList.push_back(file);

    }
    
    return;
}

void quit(AppData *data_ptr) {

    //SDL_DestroyTexture(data_ptr->)
    //SDL_DestroyTexture(data_ptr->)
    //TTF_CloseFont(data_ptr->font);

}

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

char* getPermissions(char *file){
    struct stat st;
    char *modeval;
    modeval = (char*)malloc(sizeof(char) * 9 + 1);
    if(stat(file, &st) == 0){
        mode_t perm = st.st_mode;
        modeval[0] = (perm & S_IRUSR) ? 'r' : '-';
        modeval[1] = (perm & S_IWUSR) ? 'w' : '-';
        modeval[2] = (perm & S_IXUSR) ? 'x' : '-';
        modeval[3] = (perm & S_IRGRP) ? 'r' : '-';
        modeval[4] = (perm & S_IWGRP) ? 'w' : '-';
        modeval[5] = (perm & S_IXGRP) ? 'x' : '-';
        modeval[6] = (perm & S_IROTH) ? 'r' : '-';
        modeval[7] = (perm & S_IWOTH) ? 'w' : '-';
        modeval[8] = (perm & S_IXOTH) ? 'x' : '-';
        modeval[9] = '\0';
        return modeval;     
    }
    else{
        return strerror(errno);
    }   
}