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


int main (int argc, char *argv[])
{
    int opt; //Store the opt return type (when -1 done finding all flags)
    char  *pathName = NULL; //pathName
    char *fileType = NULL;  //fileType -f 
    int includeLinks = 0; 
    char *findString = NULL; //The string we are looking for 

        while((opt = getopt(argc, argv, "p:f:s:l"))!=-1) //While flagtypes still exist in command line
        {
        //Using switch case on opt 
        switch(opt)
        {
            case 'p': //Path name 
                pathName = optarg; 
                break; 
            case 'f': //File type 
                fileType = optarg; 
                if((strcmp(fileType, "c")!=0) && (strcmp(fileType, "h") != 0) && (strcmp(fileType, "S")!=0))
                {
                    printf("Error"); 
                    return 1; //This is an error 
                }
                break; 
            case 'l':  //Regular file + links 
                includeLinks = 1; //Meaning include links
                break; 
            case 's': //Look for this string 
                findString = optarg; 
                break; 
            //Should be an error handling case

        }
        
        }
        traverseDirectory(pathName, findString, fileType, includeLinks); 
    return 0; 
}


