//Libaries
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //For getopt()
#include <getopt.h> 
#include "string.h"
#include <dirent.h> // Header for directory traversal 
#include <sys/stat.h> //For File stats 
#include <sys/param.h> //For PATH_MAX

void traverseDirectory(const char *path, char *string, const char *fileExtension, int symLinks); 
void searchFile(const char *filePath, char *findString, const char *symlinkPath); 
void myPrint(const char *myString, ...); 

