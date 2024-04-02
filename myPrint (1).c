#include <stdio.h>
#include <unistd.h>
#include <string.h>

/* Convert the integer D to a string and save the string in BUF. If
   BASE is equal to 'd', interpret that D is decimal, and if BASE is
   equal to 'x', interpret that D is hexadecimal. */
void itoa (char *buf, int base, int d)
{
  char *p = buf;
  char *p1, *p2;
  unsigned long ud = d;
  int divisor = 10;
     
  /* If %d is specified and D is minus, put `-' in the head. */
  if (base == 'd' && d < 0)
    {
      *p++ = '-';
      buf++;
      ud = -d;
    }
  else if (base == 'x')
    divisor = 16;
     
  /* Divide UD by DIVISOR until UD == 0. */
  do
    {
      int remainder = ud % divisor;
     
      *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
    }
  while (ud /= divisor);
     
  /* Terminate BUF. */
  *p = 0;
     
  /* Reverse BUF. */
  p1 = buf;
  p2 = p - 1;
  while (p1 < p2)
    {
      char tmp = *p1;
      *p1 = *p2;
      *p2 = tmp;
      p1++;
      p2--;
    }
}





void myPrint(const char *myString, ...)
{
    int arg2, arg3, arg4, arg5, arg6, num;
    void *stackAddr;
    asm ("movl %%esi, %0" : "=r"(arg2));
    asm ("movl %%edx, %0" : "=r"(arg3));
    asm ("movl %%ecx, %0" : "=r"(arg4));
    asm ("movl %%r8d, %0" : "=r"(arg5));
    asm ("movl %%r9d, %0" : "=r"(arg6));

    asm ("movq %%rbp, %%rax;" // Move rbp to rax
     "addq $16, %%rax;"  // Add 16 to rax
     "movq %%rax, %0;"   // Move the result into stackAddr
     : "=r" (stackAddr)  // Output operand
     :                    // No input operands
     : "rax"              // Tell the compiler rax is modified
    );

   
    void **argument = stackAddr;
   
//Store the 6 register values except the str
    int argList [5];
    argList[0] = arg2;
    argList[1] = arg3;
    argList[2] = arg4;
    argList[3] = arg5;
    argList[4] = arg6;
    char buffer [100]; //Char buffer
   
   
    int counter = 0;
    int numArg = 1;
    int value;
    int inc = 0; //The memory address inc value

   while(*myString) //Not empty
   {
       if(*myString == '%' && (counter<5)) //The first 5 stored in registers
       {
           myString++; //To go to format specifer
           switch(*myString) //First 6 registers
           {
               case 'd': //Tested
                    itoa(buffer, 'd', argList[counter]);
                    write(1, buffer, strlen(buffer));
                    counter++;
                    myString++;
                    break;
               case 'u':  //Tested
                    itoa(buffer, 10, argList[counter]);
                    write(1, buffer, strlen(buffer));
                    counter++;
                    myString++;
                    break;
                case 'c': //Tested
                    char tempChar = (char)argList[counter];
                    write(1, &tempChar, 1);
                    counter++;
                    myString++;
                    break;
                case 'x':
                    itoa(buffer, 'x', argList[counter]); //(char *buf, int base, int d)
                    write(1, buffer, strlen(buffer));
                    counter++;
                    myString++;
                    break;
                case 's':
                    char *charPtr = (char *)(argList[counter]);
                    write(1, charPtr, strlen(charPtr));
                    counter++;
                    myString++;
                    break;
           }
           
       }
       //Beyond 6 registers
       else if(*myString == '%' && (counter>=5))
       {
           myString++;
           switch(*myString){
                case 'd':
                    int num = *(int*)(argument);
                    itoa(buffer, 'd', num);
                    write(1, buffer, strlen(buffer));
                    argument++;
                    myString++;
                    break;
                   
                case 's':
                    char *str = *(char**)(argument);
                    write(1, str, strlen(str));
                    argument++;
                    myString++;
                    break;
                case 'c':
                    char c = *(char*)(argument);
                    write(1, &c, 1);
                    argument++;
                    myString++;
                    break;
                case 'u':
                    num = *( int*)(argument);
                    itoa(buffer, 'd', num);
                    write(1, buffer, strlen(buffer));
                    argument++;
                    myString++;
                    break;
               
                case 'x':
                    num = *( int*)(argument);
                    itoa(buffer, 'x', num);
                    write(1, buffer, strlen(buffer));
                    myString++;
                    break;
            }
       }
       write(1, myString, 1);  //No match cases just print one character
       myString++;
       
    }
}

//Should I add the Null Terminating sequence myself?
//DSCC example crash
int main() {
    myPrint("%s\n", "Hello");
    myPrint("%d %d %d %d\n", 1, 2, 3, 4);
    myPrint("%c %c %c\n", 'D', 'E', 'F');
    myPrint("%d %d %d %d %d %d %d %d %d\n", 22, 12, 30, 45, 2, 34, 90, 12, 20);
    myPrint("%c %c %c %c %c %c %c %c %c\n", 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i');
    myPrint("%s %d %c %s %d %c %s %d %c\n", "he", 10, 'd', "ckt", 4, 'z', "tt", 9, 'r');
    myPrint("%s %s %s %s %s %s %s %s %s\n", "abc", "bcd", "cde", "def", "efg", "fgh", "ghi", "hij", "ijk");
    myPrint("%x %u test test test", 12321, 3432);
    return 0;
}

