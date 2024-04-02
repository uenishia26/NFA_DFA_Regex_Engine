//Libaries
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //For getopt()
#include <getopt.h> 
#include "string.h"
#include <dirent.h> // Header for directory traversal 
#include <sys/stat.h> //For File stats 
#include <sys/param.h> //For PATH_MAX
#include "traverseDirectory.h" //Include the functions in the static librbary 
#include "stdbool.h"

//Checking if a file has the particual extension (iF YES, return true else False)
bool hasExtension(const char *fileName, const char *fileExtension)
{
    char *result = strrchr(fileName, '.'); 
    if(result != NULL && (strcmp(fileExtension, result + 1) == 0)) //If a match with fileEXNTEION RETURN TRUE
        return true;  
    else 
        return false;
}

void traverseDirectory(const char *path, char *string, const char *fileExtension, int symLinks)
{
    struct stat statbuf; //Stores information about currentFile (instance of struct stat)
    DIR *dirP = opendir(path); //Directory pointer (opens direcotry w/h pathName)
    struct dirent *currentFile; //Type struct dirent points to currentFile (doesn't have much info about the file lik stat does though)
    char fullPath[PATH_MAX]; 
    char symLinkPath[PATH_MAX]; //buffer for the filename symlink points to

    if(dirP == NULL) //Error handling Couldn't open the directory 
        printf("Error");
    while((currentFile = readdir(dirP))!= NULL) //As long as there are still files in the dirP we keep looping
    {
        //Ignore the . and .. to avoid infinite recursion (If currentFile equals the . or .. we skip the while loop)
        if(strcmp(currentFile->d_name, ".")==0 || strcmp(currentFile->d_name, "..")==0)
            continue; //Skip the rest of the function 

        fullPath[0] = '\0'; //Resets the fullPath first character to the null terminating sequence 
        //Creating the fullPath
        strcpy(fullPath,path); 
        strcat(fullPath, "/");
        strcat(fullPath, currentFile->d_name); 

        lstat(fullPath, &statbuf);//lstat takes in full pathName and place to store the information about that specifc file is statBUf
        if(S_ISDIR(statbuf.st_mode)!=0){ //If currentFile is a directory
            traverseDirectory(fullPath, string, fileExtension, symLinks); //Recursively call function for subdirectories 
        }
        else
        {  
    /*If commands -l and -f is used (aka.. Look at regular files and symbolic link files with .extension )
    This if statment will check if both commands were used and if they were, we need to open all symbolic link
    files and Regular files.(AND CHECK THEIR EXTENSION IF NECESSARY)
    */
            if(symLinks == 1)
            {
                if(S_ISLNK(statbuf.st_mode) != 0) //Check if the file is a symbolic link file 
                {
                    stat(fullPath, &statbuf); //Store infomration about the symLink in the statBuffer
                    if(S_ISREG(statbuf.st_mode)!=0) //If the file pointed to by the symlink file is a regular file
                    {
                        //Code to store the pathName of the file the symbolic link points too
                        int len; 
                        symLinkPath[0] = '\0'; //Reset it 
                        len = readlink(fullPath, symLinkPath, sizeof(symLinkPath)-1); 
                        symLinkPath[len] = '\0'; //Manually write the term sequence cause readLinke doesnt do that 
                        if(len == 0) //Error opening readLin     
                            printf("error\n");   
                        else 
                        {
                            /*We are passing the fullPath cause this will save the pathName to the 
                            Symbolc Link which is what we want to ouput (Not the pathName of the actual file 
                            Symlink file is pointing too)
                            */
                            //If the fileExtension was specifed for the Symbolic Link File 
                            if(fileExtension!=NULL) 
                            {    
                                if(hasExtension(symLinkPath, fileExtension))
                                    searchFile(symLinkPath, string, fullPath);
                            }//If the fileExtnesion was NOT specified for the Symbolic Link File 
                            else
                                searchFile(symLinkPath, string, fullPath); 
                                
                        }
                    }
                }
                else //If not a symbolic link we just want to check regular file (W/h or W/o Extension) 
                {//With extension 
                    if(fileExtension!= NULL){
                        if(S_ISREG(statbuf.st_mode)!=0){ //If the file is a regular file 
                            if(hasExtension(fullPath, fileExtension)) //Only search into files that match the extension from terminal
                                searchFile(fullPath, string, NULL);  //Search into that specifc extension file for the string
                        }
                    }
                    else //Without extension 
                    {
                        if(S_ISREG(statbuf.st_mode)!=0)
                            searchFile(fullPath, string, NULL); 
                    }
                }


            }
            else if(fileExtension == NULL) //If no -f was used (no file Extension) && no symlink -l we look for all regular files
            {
                if(S_ISREG(statbuf.st_mode)!=0)//If the file is a regular file
                    searchFile(fullPath,string, NULL); //Search
            }
            else { //If fileExtenion -f was used and symLink -l was not used 
                if(hasExtension(fullPath, fileExtension)) //Only search into files that match the extension from terminal
                    searchFile(fullPath, string, NULL);  //Search into that specifc extension file for the string
            }
        }
    }
}

