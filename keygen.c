/**********************************
**Author: Mike Mann
**Descrition:   Generates a random key. 
**              File Encryption and Decryption project that uses a randomly generated key for OS 344.
************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main (int argc, char *argv[])
{
    //seed our random number
    srand(time(NULL));
    char keyChar;                   //this will be the random char printed out to stdout
    int  randomNum;                 //random number we get from our rand() function call
    int keySize;                    //size of key passed in as arg

    //checks we got a valid number of arguments
    //used help from http://crasseux.com/books/ctutorial/argc-and-argv.html
    if(argc > 1)
    {
        int i;
        keySize = atoi(argv[1]);      //stores the number passed in as the keySize

        for(i = 0; i < keySize; ++i)
        {
            randomNum = rand() % 27 + 1;        //gets us a random number from 1-27
            
            //assigns A-Z and space to a number 1-27
            if(randomNum == 1)
                keyChar = 'A';
            else if(randomNum == 2)
                keyChar = 'B';
            else if(randomNum == 3)
                keyChar = 'C';
            else if(randomNum == 4)
                keyChar = 'D';
            else if(randomNum == 5)
                keyChar = 'E';
            else if(randomNum == 6)
                keyChar = 'F';
            else if(randomNum == 7)
                keyChar = 'G';
            else if(randomNum == 8)
                keyChar = 'H';
            else if(randomNum == 9)
                keyChar = 'I';
            else if(randomNum == 10)
                keyChar = 'J';
            else if(randomNum == 11)
                keyChar = 'K';
            else if(randomNum == 12)
                keyChar = 'L';
            else if(randomNum == 13)
                keyChar = 'M';
            else if(randomNum == 14)
                keyChar = 'N';
            else if(randomNum == 15)
                keyChar = 'O';
            else if(randomNum == 16)
                keyChar = 'P';
            else if(randomNum == 17)
                keyChar = 'Q';
            else if(randomNum == 18)
                keyChar = 'R';
            else if(randomNum == 19)
                keyChar = 'S';
            else if(randomNum == 20)
                keyChar = 'T';
            else if(randomNum == 21)
                keyChar = 'U';
            else if(randomNum == 22)
                keyChar = 'V';
            else if(randomNum == 23)
                keyChar = 'W';
            else if(randomNum == 24)
                keyChar = 'X';
            else if(randomNum == 25)
                keyChar = 'Y';
            else if(randomNum == 26)
                keyChar = 'Z';
            else if(randomNum == 27)
                keyChar = ' ';
            //print out the random char
            printf("%c", keyChar);
        }
        //make sure we add the new line character at the end
        printf("\n");
        return 0;
    }
    else
    {
        fprintf(stderr, "Invalid number of arguments.\n");
        return 1;
    }

    return 0;
}