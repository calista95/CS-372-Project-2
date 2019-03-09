'''
Program: ftclient.py
Argument format: python ftclient.py <server host> <port #> <command> <filename> <dataport>
Description: This is the client of our TCP file transfer program. It connects with the host and requests to either:
1) download a file from the host directory using the -g command, or 
2) view a list of the files in the directory that the host is in
Last modified: 3/8/2019
Name: Calista Wong 
Citations:
Template code for setting up client borrowed from CS372 Lecture 15
File transfer protocol borrowed from: //www.youtube.com/watch?v=LJTaPaFGmM4&t=833s (Python Advanced Tutorial 6.6 - Simple File Server)
'''
from socket import *
import sys
import struct
import os

reload(sys)
sys.setdefaultencoding('utf8')

#function: convert bytes string to an integer
#input: bytes string
#output: integer version
#example: b'5\x00\x00\x00\x00' will return int(5)
def getSizeFile(mystring):
    count=""
    for x in mystring:
        if (str(x).isdigit()):
            count+=str(x)
    count = int(count)    
    return int(count)

#function: minimize data, convert bytes string to a normal string
#input: bytes string
#output: string
def minData(data):
    #print(data)
    mystring=""
    for x in data:
	if (x != '\x00'): #if not null byte
            mystring+=x
    #print("data from function: "+mystring)
    return mystring

#function: get number of characters in the file
#input: file name
#output: integer of file size
def fileSize(filename):
    with open("cat.txt") as infile:
    	lines=0
	words=0
	characters=0
	for line in infile:
	    wordslist=line.split()
	    lines=lines+1
	    words=words+len(wordslist)
	    characters+=sum(len(word) for word in wordslist)
    #print("characters: "+str(characters))
    return characters
		
#initialize variables
command=" "
filename=" "
dataport=" "

#validate number of arguments, we should have a minimum of 4 argumeents
if len(sys.argv) <= 4: 
    print("Number of arguments insufficient. Program exited.")
    sys.exit()

#assign variables
serverHost = sys.argv[1]
serverPort = sys.argv[2]
command = sys.argv[3]
filename = sys.argv[4]
if len(sys.argv) >= 6:
    dataport = sys.argv[5]

#confirm variables
'''
print("serverhost: "+serverHost)
print("serverport: "+serverPort)
print("command: "+command)
print("filename: "+filename)
print("dataport: "+dataport)
'''

#make connection to server
clientSocket = socket(AF_INET, SOCK_STREAM)
clientSocket.connect((serverHost, int(serverPort)))

#send over command 
clientSocket.send(command.encode())
#confirm 
fromServer = clientSocket.recv(1024)
#print("from server: "+fromServer.decode())

#send over filename
clientSocket.send(filename.encode())
#confirm 
fromServer = clientSocket.recv(1024)
#print("from server: "+fromServer.decode())

#send over dataport 
clientSocket.send(dataport.encode())

#if response was "OK", connect to the second port number (data port) 
fromServer = clientSocket.recv(1024)
fromServer = (fromServer.decode())[:2]
#print("Response from server: "+fromServer)
if fromServer == "OK":
    if (command == "-l"):
        dataport = filename
    #make second connection to server
    clientSocket2 = socket(AF_INET, SOCK_STREAM)
    clientSocket2.connect((serverHost, int(dataport)))
    #print("client has connected")
    #send over confirmation
    
    test = "From client: testing second connection"
    clientSocket2.send(test.encode())
    
    if (command == '-g'): #request for file transfer 
        #receive file status from server
        fromServer = clientSocket2.recv(1024)
        fromServer = (fromServer.decode())[:2]
        if fromServer == "FE": #file exists, set up code for accepting file transfer
	    #this code returns the byte size 
     	    filesize = os.path.getsize(filename) #get byte size of the file
	    #print("size of file expected: "+str(filesize))
            f=open(filename, 'w') #open up file to write
	    
	    #receive first chunk of data from server
            data = clientSocket2.recv(1024)
            mystring = minData(data) #function to parse data from the trailing buffer space
	    totalRecv = len(mystring)
            #print("total received: "+ str(totalRecv))
	    f.write(mystring) 
            
	    while totalRecv < filesize: #deal with any files greater than 1024B
                data = clientSocket2.recv(1024)
		mystring = minData(data)
	 	#print(mystring)
                totalRecv += len(mystring)
		#print("total received: "+str(totalRecv))
                f.write(mystring)
            print ("download complete")

        else: #file does not exist 
            print("Server says: file does not exist\n")
    if (command == '-l'): #request for directory
        print("Request for directory listing")
        #receive number of directory files
        dircount = clientSocket2.recv(1024)
        dircount = getSizeFile(dircount) #force this to be an integer 
        #print("dircount: "+str(dircount))
        #receive file names in directory
        #for x in range(dircount):
        fromServer = clientSocket2.recv(1024)
        print(fromServer.decode())
    clientSocket2.close() #close connection to second server 

else:
    print("Command invalid or address already in use. Client exiting.")
	
#send over test- anything after this will not be processed 
test= "Request to end program"
clientSocket.send(test.encode())
#confirm 

fromServer = clientSocket.recv(1024)
#print("from server: "+fromServer.decode())

clientSocket.close()



