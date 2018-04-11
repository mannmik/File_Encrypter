/*********************************************
**Author: Mike Mann
**Description: File Encryptor. Program 4 otp_enc implementation.
********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <fcntl.h>

void error(const char *msg) { perror(msg); exit(0); } //error function used for reporting issues

int main(int argc, char *argv[])
{

int socketFD, portNumber, charsWritten, charsRead;
struct sockaddr_in serverAddress;
struct hostent* serverHostInfo;
char buffer[100000];
char theKey[100000];
int fileDescriptor;
int pTxtSize;
int keySize;
char validLetters[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ' '};

//check usage and args
if(argc < 4)
{
	fprintf(stderr, "Usage: %s hostname port\n", argv[0]);
	exit(0);
}

//set up server address struct
//clear out the address struct
memset((char*)&serverAddress, '\0', sizeof(serverAddress));

//get the port number, convert to an int from a string
portNumber = atoi(argv[3]);

//clear our buffer
memset(buffer, '\0', 100000);

//read in plaintxt
//open up the file name passed in at argv[1] for our plaintext for reading only
fileDescriptor = open(argv[1], O_RDONLY);

if(fileDescriptor < 0)
{
	fprintf(stderr, "ERROR: Unable to open file %s.\n", argv[1]);
	return 1;
}

pTxtSize = read(fileDescriptor, buffer, sizeof(buffer));

//validate we received no bad data
//we count how many of the letters match a valid char option
int i;
int count = 0;
for(i = 0; i < pTxtSize; ++i)
{
	char temp;
	temp = buffer[i];
	int j;
	for(j = 0; j < 27; ++j)
	{
		if(temp == validLetters[j])
			count++;
	}
}
//if one of our chars passed in didnt match a valid char then we received bad input
if(count != (pTxtSize - 1))
{
	fprintf(stderr, "ERROR: Invalid character data in %s\n", argv[1]);
	exit(1);
}

//close the plaintext file
close(fileDescriptor);

//read in key file
//clear theKey
memset(theKey, '\0', 100000);

//open up the file name passed in at argv[2] for our key file for reading only
fileDescriptor = open(argv[2], O_RDONLY);

if(fileDescriptor < 0)
{
	fprintf(stderr, "ERROR: Unable to open file %s.\n", argv[1]);
	return 1;
}

keySize = read(fileDescriptor, theKey, sizeof(theKey));

//replace \n with null character in our key
//try to use theKey[keySize - 2] = '\0' while testing
buffer[strcspn(buffer, "\n")] = '\0';
theKey[strcspn(theKey, "\n")] = '\0';
//close the key file
close(fileDescriptor);

//validate the key is atleast as big as the plaintext file
if(keySize < pTxtSize)
	fprintf(stderr, "ERROR: Key size is smaller than plaintext size.\n");

//create network socket
serverAddress.sin_family = AF_INET;

//store the port number
serverAddress.sin_port = htons(portNumber);

//convert machine name into a special form of address
//we will be using local host
serverHostInfo = gethostbyname("localhost");

//check for host error
if(serverHostInfo == NULL)
{
	fprintf(stderr, "CLIENT: ERROR, no such host\n");
	exit(0);
}

//copy in our address
memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length);

//set up the socket
socketFD = socket(AF_INET, SOCK_STREAM, 0);		//creates the socket
if(socketFD < 0)
{
	error("CLIENT: ERROR opening socket");
}

//connect to server
if(connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) 	//connects socket to addy
{
	fprintf(stderr, "ERROR: unable to connect to port %d.\n", portNumber);
	return 2;
}

//send plaintext message to the server
charsWritten = send(socketFD, buffer, strlen(buffer), 0);	//writes to the server

if(charsWritten < 0)
{
	error("CLIENT: ERROR writing to socket");
}

int checkSend = -5;  // Holds amount of bytes remaining in send buffer
do
{
	ioctl(socketFD, TIOCOUTQ, &checkSend);  // Check the send buffer for this socket
	//printf("checkSend: %d\n", checkSend);  // Out of curiosity, check how many remaining bytes there are
}
while (checkSend > 0);  // Loop forever until send buffer for this socket is empty
if (checkSend < 0) error("ioctl error");  // Check if we actually stopped the loop because of an error

//get return message from the server
memset(buffer, '\0', sizeof(buffer));
//read data from socket leaving \0 at the end
charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0);

if(charsRead < 0)
{
	error("CLIENT: ERROR reading from socket");
}
//printf("CLIENT: I received this from the server: \"%s\"\n", buffer);

//send plaintext message to the server
charsWritten = send(socketFD, theKey, strlen(theKey), 0);	//writes to the server

if(charsWritten < 0)
{
	error("CLIENT: ERROR writing to socket");
}

checkSend = -5;  // Holds amount of bytes remaining in send buffer
do
{
	ioctl(socketFD, TIOCOUTQ, &checkSend);  // Check the send buffer for this socket
	//printf("checkSend: %d\n", checkSend);  // Out of curiosity, check how many remaining bytes there are
}
while (checkSend > 0);  // Loop forever until send buffer for this socket is empty
if (checkSend < 0) error("ioctl error");  // Check if we actually stopped the loop because of an error

//loop and read in the data sent back from otp_enc_d until reading data is complete
memset(buffer, '\0', sizeof(buffer));
do
{
	charsRead = recv(socketFD, buffer, sizeof(buffer), 0);
}while(charsRead > 0);
//printf("CLIENT: I received this from the server: \"%s\"\n", buffer);


/*for(i = 0; i < charsRead - 1; ++i)
{
	printf("%c", buffer[i]);
}*/

printf("%s\n", buffer);

//close our socket
close(socketFD);

return 0;
}
