//Libaries
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> //For getopt()
#include <getopt.h>
#include "string.h"
#include <stdbool.h>
#include <sys/param.h>
#include "traverseDirectory.h" //Include the functions in the static librbary 
#include <dirent.h> // Header for directory traversal
#include <sys/stat.h> //For File stats
#include <sys/param.h> //For PATH_MA
#define ANY_CHAR '\x19' //End of medium Character

int isValidRegex(const char *s) {
    int dotCount = 0, starCount = 0, questionCount = 0;
    int parenthesesCount = 0;
    int previousChar = 0;

    for (int i = 0; s[i]; ++i) {
        char c = s[i];

        // Check for alphanumeric and control characters
        if (!isalnum(c)) {
            switch (c) {
                case '.':
                    dotCount++;
                    break;
                case '*':
                    starCount++;
                    break;
                case '?':
                    questionCount++;
                    break;
                case '(':
                    if (parenthesesCount != 0) { // Check for nesting
                        printf("Error: Nested parentheses are not allowed\n");
                        return 0;
                    }
                    parenthesesCount++;
                    break;
                case ')':
                    parenthesesCount--;
                    if (parenthesesCount < 0) { // Check for mismatched parentheses
                        printf("Error: Mismatched parentheses.\n");
                        return 0;
                    }
                    break;
                default:
                    printf("Error: Invalid character '%c'.\n", c);
                    return 0;
            }
        }

        previousChar = c;
    }

    if (parenthesesCount != 0) {
        printf("Error: Unmatched parentheses.\n");
        return 0;
    }

    return 1; // Valid regex
}


typedef struct State State;

typedef struct Transition
{
    char transitionChar; //For epsilon transtion will be \0 character
    State *state; //Where you end up after the transition
} Transition;

struct State
{
    int AcceptingState; //Double circle
    Transition *transition; //Each state {p1,p2,p3,... nodes} has a transition like A -> (char b) -> C
    int numTransitions;
    //Each state has a transition
};

//This just creates a default node with all default values except AccetpingState just in case its the last node
State* createState(int finalState)
{
    State *newState = (State *)malloc(sizeof(State));
    newState->AcceptingState = finalState;
    newState->transition = NULL;//The transition pointer is still NULL
    newState->numTransitions = 0; //Number of transition is also zero.
    return newState;

}

void addTransition(State *currentState, char transChar, State *nextState) //Takes in a State
{
    int newSize = currentState->numTransitions + 1; //Add 1 as we are adding a transition
    //Get the newSize and multiply by size of Each Transiton
    currentState->transition = realloc(currentState->transition, newSize * sizeof(Transition));
    currentState->transition[newSize-1].transitionChar = transChar;
    currentState->transition[newSize-1].state = nextState;
    currentState->numTransitions++;
}

void addEpsilonTransition(State *currentState, State *nextState)
{
    addTransition(currentState, '\0', nextState);
}

/*This is specifcally for the (.) WildCard
It creates a Transition of char ANY_CHAR which is a
defined char of ASCII X19 End of Medium Control which is very
unlikely to be used in the string.
Thus we can check in isMatch if the transitionChar == ANY_CHAR we don't
care about the string input
*/
void addAnyChar(State *current, State *next)
{
    addTransition(current, ANY_CHAR, next);
   
}

void addLoopBackTransition(State *current, State *loopBackTo)
{
    addEpsilonTransition(current, loopBackTo);
}

//SubString = SubNFA inside ()
char* getSubString(char** strStrPtr)
{
    char *subString = malloc(400);  //string from (+1 to )-1
    int index = 0;
    char *charVal = *strStrPtr;
    subString[index] = *charVal;
    charVal++;
    index++;
    while(*charVal!=')')
    {
        subString[index++] = *charVal;
        charVal++;
    }
    *strStrPtr = charVal;
    subString[index] = '\0';
    return subString;
}

//Getting the last State in an NFA
State* finalState(State* current)
{
    State *finalState = current;
    while(finalState != NULL && finalState->AcceptingState !=1)
    {
   
        finalState = finalState->transition[0].state;
    }
    return finalState;
}


State* createNFAPattern(char *pattern)
{
    //Create a start state
    State *start = createState(0);
    State *currentState = start;
    State *prev; //Keeps track of the previous State
    char prevChar = '\0'; //Top keep track previous Character
   
    while(*pattern) //As long as the pattern is not null
    {
        State *next = createState(0);
        //Pattern matching for the Dot WildCase
        if(*pattern == '.')
            addAnyChar(currentState, next);
       
        //For the ? WildCase
        else if(*pattern == '?')
        {
            //We have to create both an Epsilon transition && Regular transition:
            //ts?b: t->s->b || t->b
            //Rememeber we do prev here cause we don't want to make a State for ?
            if(*(pattern+1) == '\0') //if the ? is the last character
            {
                //The prev is just the final character. No need for Epsilon or OneChar transition
                addEpsilonTransition(prev, currentState);
                currentState->AcceptingState=1;
                return start; //Exit early we are done
            }    
            else
            {
                addEpsilonTransition(currentState, next);
                addEpsilonTransition(prev, currentState);
            }
           
        }
        //For the (*) WilrdCard
        else if(*pattern == '*')
        {//This allows us to esentially skip creating * as a State
            if(*(pattern+1)=='\0')//If * is the last character
            {
                addLoopBackTransition(currentState, prev);

                //Exit the loop early cause we reached end pattern
                prev->AcceptingState = 1;
                return start;
            }
               
            else //Create both Epsilon and LoopBackTransition
            {
                addLoopBackTransition(currentState, prev);
                addEpsilonTransition(currentState, next);
                addEpsilonTransition(prev, next);
            }  
        }
        else if(*pattern == '(')
        {
           
            pattern++; //Move pass the (
            char* subString = getSubString(&pattern);
            State* subNFA = createNFAPattern(subString);
            State* SubNFAFinalState = finalState(subNFA);
            if(*(pattern+1) != '\0')
                SubNFAFinalState->AcceptingState = 0; //Set it to zero
            addEpsilonTransition(currentState, subNFA);
            addEpsilonTransition(SubNFAFinalState, next);
        
           
        }
 
        else
            addTransition(currentState, *pattern, next);

       
        prev = currentState;
        prevChar = *pattern;
        currentState = next;
       
        pattern++;
       
       
    }
    //This is the terminating state of NFA Creating. Thus just be an epsilon trnasition
    currentState->AcceptingState = 1;
    return start;
}


int isMatch (State *begin, char *str, State *reset) {
    State *currentState = begin;
 
/*If we reached the end of the string, two valid options
    1) End position of NFA is valed so we return 1
    2) We havent reached the end of the NFA thus we must go through the entire pattern
        i) The rest of NFA still consits more than epsilon in which case return 0
        ii) Rest is all epsilon transition so acceptable (return 1)
    */  
    if(currentState->AcceptingState == 1)
        return 1;
   
    if (*str=='\0') {
       
        //But if the rest of the NFA is made out of epsilon transition we must traverse through the epsilon transition and return the final State
        for(int x = 0; x < currentState->numTransitions; x++)
        {
           
            Transition finalTrans = currentState->transition[x];
            if(finalTrans.transitionChar == '\0'){
                if(isMatch(finalTrans.state, str, reset))
                    return 1; //Found a Accpeting Path
            }
        }
        return 0; //No Path to an Accepting State aka patte
    }
   
    // Iterate through all transitions of the current state
    for (int i = 0; i < currentState->numTransitions; ++i) {
        Transition t = currentState->transition[i];
       
        // Epsilon transition: Recursively check without consuming the string
        if (t.transitionChar == '\0') {
            if (isMatch(t.state, str, reset)) {
                return 1;
            }
        }
        // Loopback transition: Check if the transition leads to the same state
        else if (t.state == currentState && t.transitionChar == *str) {
            if (isMatch(currentState, str + 1, reset)) {
                return 1;
            }
        }
        // Regular case or ANY_CHAR transition
        else if (t.transitionChar == *str || t.transitionChar == ANY_CHAR) {
           
            if (isMatch(t.state, str + 1, reset)) {
                return 1;
            }
        }
        else //Resetting NFA
        {
           if(isMatch(reset, str + 1, reset))
                return 1;
        }
    }
    // No matching transition was found for the current character
    return 0;
}


/*When fgets reads a file it will read and store the \n chracter and the null terminating sequence to the buffer
Thus for the wild card matching we need it to be the null terminating sequence and thus this function will check
if the line contains a \n and then replace it with \0
*/
void removeNewline(char *line)
{
    if(strlen(line) > 0 && line[strlen(line)-1]== '\n')
        line[strlen(line)-1] = '\0';
}


void searchFile(const char *filePath, char *findString, const char *symlinkPath)
{
    FILE *file = fopen(filePath, "r"); 
    if(file == NULL)
        printf("error opening file"); 

    int lineCounter = 1; 
    char line[1024]; //Creating a buffer to store the contents of the line 
    while(fgets(line, sizeof(line), file)!=NULL)
    {
        removeNewline(line); 
        //If the string was found in the line (strstr finds first occurence of string in that line returns pointer)
        //If pointer is not equal to NULL then it found the string, else it didn't 

        //Error handling
        if(isValidRegex(findString)!=1)
            exit(EXIT_FAILURE); 
        //Creating the NFA from the pattern /String we are finiding
        State *nfa = createNFAPattern(findString); 

        if(isMatch(nfa, line, nfa) ==1) {
            if(symlinkPath != NULL) //This means there exists a symLinkPath so we want to print the symLink 
                printf("Found in path: %s at line %d\n", symlinkPath, lineCounter); 
            else
                printf("Found in path: %s at line %d\n", filePath, lineCounter); }
        lineCounter++; //Increment line counter 


    }

}