
/*********************************************
**Author: Mike Mann
**Description: File Decryptor. Program 4 otp_dec_d implementation. 
********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) { perror(msg); exit(1); } //error function

int main(int argc, char *argv[])
{

int listenSocketFD, establishedConnectionFD, portNumber, charsRead, keyRead, charSend;
socklen_t sizeOfClientInfo;
char buffer[100000];
char theKey[100000];
char decipheredTxt[100000];
struct sockaddr_in serverAddress, clientAddress;
int spawnPid;


//check usage and args
if(argc < 2)
{
	fprintf(stderr, "USAGE: %s port\n", argv[0]);
	exit(1);
}

//set up our valid letters array
char validLetters[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' '};
//set up the address struct for the server
//reset the adddress struct
memset((char *)&serverAddress, '\0', sizeof(serverAddress));

//Get the port number, convert it to an int
portNumber = atoi(argv[1]);

//create network-capable socket
serverAddress.sin_family = AF_INET;
//store the port number
serverAddress.sin_port = htons(portNumber);
//anny address is allowed for the connecton to this process
serverAddress.sin_addr.s_addr = INADDR_ANY;

//set up the socket
//create the socket
listenSocketFD = socket(AF_INET, SOCK_STREAM, 0);
//check for error
if(listenSocketFD < 0)
{
	error("ERROR opening socket");
}

//enable the socket to begin listening
if(bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
{
	error("ERROR on binding");
}
//flip the socket on - it can now receive up to 5 connections
listen(listenSocketFD, 5);

while(1)
{
	//accept a connection, blocking if one is not available until one connects
	sizeOfClientInfo = sizeof(clientAddress);		//gets the size of the client address connecting
	//Accept
	establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);
	//check for error
	if(establishedConnectionFD < 0)
	{
		error("ERROR on accept");
	}
	
	//printf("SERVER: Connected Client at port %d\n", ntohs(clientAddress.sin_port));	

	//create our child process to handle the newly accepted connection
	spawnPid = fork();

	//check for error with fork
	if(spawnPid < 0)
	{
		fprintf(stderr, "Fork failed.\n");
	}
	else if(spawnPid == 0)
	{
		//Get the message from the client and display it
		//clear out our buffer
		memset(buffer, '\0', 100000);

		//read the clients message from the socekt
		charsRead = recv(establishedConnectionFD, buffer, 99999, 0);
		//cehck for error
		if(charsRead < 0)
		{
			error("ERROR reading from socket");
		}

		//send a success msg back to the client
		charSend = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0);

		//cehck for error
		if(charSend < 0)
		{
			error("ERROR reading from socket");
		}

		//reset theKey 
		memset(theKey, '\0', 100000);
		//get our key
		keyRead = recv(establishedConnectionFD, theKey, 99999, 0);
		//cehck for error
		if(keyRead < 0)
		{
			error("ERROR reading from socket");
		}

		//check to make sure key is as long as the input data
		//if not then print error close the connection, and exitthe child process
		if(keyRead < charsRead)
		{
			fprintf(stderr, "ERROR: Key length did not match plaintext length");
			exit(1);	
		}

		//we must decode the encrypted file in buffer with the key
		//to do this we search for the valid letter in our valid letter array
		//then we assign it that value plus 1
		//ie validLetters[0] is 'A', but for our cipher algorithm it will 'A' = 1;
		//reset our decipheredTxt char array
		memset(decipheredTxt, '\0', 100000);
		int track;
		int keyVal;
		int bufferVal;
		int decipherChar;
		int i;
		for(i = 0; i < charsRead; ++i)
		{
			//stores the int value of our key char at i
			for(track = 0; track < 27; track++)
			{
				if(theKey[i] == validLetters[track])
				{
					keyVal = track + 1;
				}
			}
			//stores the int value of our buffer char from plaintext
			for(track = 0; track < 27; track++)
			{
				if(buffer[i] == validLetters[track])
				{
					bufferVal = track + 1;
				}
			}

			//get our decipherChar value
			decipherChar = bufferVal - keyVal;

			//make sure this is a valid option from 1-27
			//if it is too large then subtract 27
			//too large add 27
			if(decipherChar > 27)
				decipherChar -= 27;
			if(decipherChar < 1)
				decipherChar += 27;
			
			//store our new encoded char to our cipher text array
			decipheredTxt[i] = validLetters[decipherChar - 1];
		}
		//send our ciphered text to the client
		charSend = send(establishedConnectionFD, decipheredTxt, charsRead, 0);

		//cehck for error
		if(charSend < 0)
		{
			fprintf(stderr, "ERROR: sending data.\n");
		}

		//close the existing socket which is connected to the client
		close(establishedConnectionFD);
		exit(0);
	}
	else
		close(establishedConnectionFD);
}

//close the listening socket
//close(listenSocketFD);

return 0;
}


 
