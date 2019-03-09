/*
 * Program: ftserver.c
 * Compile: gcc -o server ftserver.c
 * Argument: server <port #>
 * Description: This is the server part of our TCP file transfer program. It connects with the client, and depending on what the client wants, it will
 * 1) transfer the file contents of the specified file, or 
 * 2) tell the client the name of all the files in the directory it is in
 * If arguments are invalid, it will return an error. 
 * Last modified: 3/8/2019
 * Name: Calista Wong 
 * Citations: 
 * Template code for setting up server borrowed from CS344/Block 4/Sample client-server code
 * File transfer protocol borrowed from: //www.youtube.com/watch?v=9g_nMNJhRVk&t=556s (Transferring a text file in Socket Programming in TCP | Socket Programming | Tutorial No 8)
 * Directory count borrowed from: https://stackoverflow.com/questions/12489/how-do-you-get-a-directory-listing-in-c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>
#include <stdbool.h>
#include <dirent.h>

/**************************HELPER FUNCTIONS**************************/
int getdircount() //get count of files in current working directory 
{
    DIR *dp;
    struct dirent *ep;     
    dp = opendir ("./");
    int dircount=0; //keep track of number items in directory 
    if (dp != NULL)
        {
            while (ep = readdir (dp))
                    dircount++;
            (void) closedir (dp);
        }
    else
        perror ("Couldn't open the directory");
    dircount = dircount-2; //because we have "." and ".."
    //printf("dircount: %i\n", dircount);
    return dircount;
}

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

/*************************MAIN FUNCTION************************/
int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
    	int listenSocketFD_2, establishedConnectionFD_2, portNumber_2;
    	//char command[100];  <--
    	//char filename[100]; <--moved all of these down, for some reason it is buggy when up here 
    	//char dataport[5];   <--
	socklen_t sizeOfClientInfo;
    	socklen_t sizeOfClientInfo_2;
	char buffer[256];
	struct sockaddr_in serverAddress, clientAddress;
    	struct sockaddr_in serverAddress_2, clientAddress_2;

	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

    while(1)
    {
	    // Accept a connection, blocking if one is not available until one connects
	    sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
	    establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
	    if (establishedConnectionFD < 0) error("ERROR on accept");

	    // Get the command from the client and display it
            char command[100];
	    memset(command, '\0', 100);
	    charsRead = recv(establishedConnectionFD, command, 100, 0); // Read the client's message from the socket
	    if (charsRead < 0) error("ERROR reading from socket");
	    //printf("SERVER: I received this command from the client: \"%s\"\n", command);
	    // Send a Success message back to the client
	    charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
	    if (charsRead < 0) error("ERROR writing to socket");

            //Get the filename from the client 
            char filename[100];
            memset(filename, '\0', 100);
	    charsRead = recv(establishedConnectionFD, filename, 100, 0); // Read the client's message from the socket
	    if (charsRead < 0) error("ERROR reading from socket");
	    //printf("SERVER: I received this filename from the client: \"%s\"\n", filename);
	    // Send a Success message back to the client
	    charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
	    if (charsRead < 0) error("ERROR writing to socket");

            //Get the dataport from the client 
            char dataport[100];
            memset(dataport, '\0', 100);
	    charsRead = recv(establishedConnectionFD, dataport, 100, 0); // Read the client's message from the socket
	    if (charsRead < 0) error("ERROR reading from socket");
	    //printf("SERVER: I received this dataport from the client: \"%s\"\n", dataport);
	   
        //Checking for validity of arguments; basically, we are looking for two things- 
        // 1) Command -g must have a filename value and dataport value.
        // 2) Command -l must have a filename value ONLY. Reassign this filename value to be the dataport value in the code below. 
        if ((strcmp(command, "-g") ==0 && strcmp(filename, " ") !=0 && strcmp(dataport, " ") !=0 ) || (strcmp(command, "-l") ==0 && strcmp(filename, " ") !=0 && strcmp(dataport, " ") ==0 ))
        {
            if (strcmp(command, "-l") ==0)
            {
                //filename will hold dataport instead, we need to assign dataport to filename
                strcpy(dataport, filename);
                //printf("dataport corrected: %s\n", dataport);
            }
            
            //printf("start up second server\n");
            //start up second connection (same server address, different port number)
            memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
			portNumber_2 = atoi(dataport); // Get the port number, convert to an integer from a string
			serverAddress.sin_family = AF_INET; // Create a network-capable socket
			serverAddress.sin_port = htons(portNumber_2); // Store the port number
			serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

            // Set up the socket
			listenSocketFD_2 = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
			if (listenSocketFD_2 < 0) error("ERROR opening socket");

			// Enable the socket to begin listening
			if (bind(listenSocketFD_2, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
				error("ERROR on binding");
			listen(listenSocketFD_2, 5); // Flip the socket on - it can now receive up to 5 connections

            // Send a Success message back to the client, tell it that the socket is ready to take connections 
            memset(buffer, '\0', 255);
            strcpy(buffer, "OK");
	        charsRead = send(establishedConnectionFD, buffer, 255, 0); // Send success back
	        if (charsRead < 0) error("ERROR writing to socket");
            
            // Accept a connection, blocking if one is not available until one connects
	        sizeOfClientInfo_2 = sizeof(clientAddress_2); // Get the size of the address for the client that will connect
	        establishedConnectionFD_2 = accept(listenSocketFD_2, (struct sockaddr *)&clientAddress_2, &sizeOfClientInfo_2); // Accept
	        if (establishedConnectionFD_2 < 0) error("ERROR on accept");
            
            //receive test message from client 
            memset(buffer, '\0', 255);
	        charsRead = recv(establishedConnectionFD_2, buffer, 255, 0); // Read the client's message from the socket
            if (charsRead < 0) error("ERROR reading from socket");
            //printf("Testing second connection: %s\n",buffer);

            //handle separate cases for both -g and -l here

            if (strcmp(command, "-g") ==0) //request for file transfer 
            {
                //see if file exists in directory 
                if (access(filename, F_OK) != -1) //file exists 
                {
                    //printf("file exists\n");
                    //send response
                    memset(buffer, '\0', 255);
                    strcpy(buffer, "FE"); //file exists
	                charsRead = send(establishedConnectionFD_2, buffer, 255, 0); // Send success back and notifiy client that file exists
	                if (charsRead < 0) error("ERROR writing to socket");
		    
		    //get file size
		    FILE *f = fopen(filename, "r");
		    fseek(f, 0L, SEEK_END);
		    int size = ftell(f);
		    //printf("size of file: %i\n", size);	               
     
                    rewind(f);
                    char ch;
                    while(ch != EOF)
                    {
				bzero(buffer, 255);
		                fscanf(f , "%s" , buffer);
				strcat(buffer, " "); //append space 
		                //printf("%s\n" , buffer);	
		                int charsRead = write(establishedConnectionFD_2,buffer,255);
				if (charsRead < 0) error("ERROR writing to socket");
		                ch = fgetc(f);
                    }
	            //printf("The file was sent successfully\n");

                }
                else //file does not exist 
                {
                    //printf("file does not exist\n");
                    //send response
                    memset(buffer, '\0', 255);
                    strcpy(buffer, "NE"); //not exist
	                charsRead = send(establishedConnectionFD_2, buffer, 255, 0); // Send success back
	                if (charsRead < 0) error("ERROR writing to socket");
                }
            }
            if (strcmp(command, "-l") ==0) //request for directory listing 
            {
                //printf("Request for list dir:\n");
                int dircount = getdircount();
                //send count to client 
                char snum[5];
                sprintf(snum, "%d", dircount); //assign words as a string 
                charsRead = send(establishedConnectionFD_2, snum, 5, 0); // Send success back
	            if (charsRead < 0) error("ERROR writing to socket");
                //send directory files one by one 
                DIR *dp;
                struct dirent *ep;     
                dp = opendir ("./");
                char filename[25];
                if (dp != NULL)
                {
                    while (ep = readdir (dp))
                    {
                        memset(filename, '\0', 25);
                        strcpy(filename, ep->d_name);
                        if (strcmp(filename,".") !=0 && strcmp(filename, "..") !=0 )
			{
			    strcat(filename, " ");
                            charsRead = send(establishedConnectionFD_2, filename, 25, 0); // Send success back
	                    if (charsRead < 0) error("ERROR writing to socket");
			}
                    }
                    (void) closedir (dp);
                }
                else
                perror ("Couldn't open the directory");
            }
            

            close(establishedConnectionFD_2); //close the second connection 
       
        }
        else //send an error message back to client due to invalid arguments 
        {
            memset(buffer, '\0', 255);
            strcpy(buffer, "NO"); //not ok 
	    charsRead = send(establishedConnectionFD, buffer, 255, 0); // Send success back
	    if (charsRead < 0) error("ERROR writing to socket");
        }
        
        /*********************************************************************************/

        //Anything after this will not be processed 
        memset(buffer, '\0', 100);
	charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket
	if (charsRead < 0) error("ERROR reading from socket");
	//printf("SERVER: I received this from the client: \"%s\"\n", buffer);
        
	// Send a Success message back to the client (last one to terminate program)
        memset(buffer, '\0', 100);
        strcpy(buffer, "Program closed to client");
	charsRead = send(establishedConnectionFD, buffer, 100, 0); // Send success back
	if (charsRead < 0) error("ERROR writing to socket");

	close(establishedConnectionFD); // Close the existing socket which is connected to the client
    }
	
    close(listenSocketFD); // Close the listening socket
    
	return 0; 
}
