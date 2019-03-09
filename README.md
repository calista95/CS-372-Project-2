# CS-372-Project-2

Description: This is a client-server program that has two functionalities: 
1) transfer a specified file from server to client
2) list directory files from server side to client 

The server is written in C, and the client is written in Python. 

Running the server:  
```
gcc -o server ftserver.c
server <port number> 
```

Running the client: 
```
python ftclient.py <hostname> <port number> <command> <textfile name if applicable> <dataport>
```

Basic procedure:  
- ftserver and ftclient establish a TCP control connection P on <SERVER_PORT>  
- ftserver waits on connection P for ftclient to send a command.  
- ftclient sends a command (-l (list) or -g <FILENAME> (get)) on connection P  
- ftserver receives command on connection P  
- If ftclient sent an invalid command   
    • ftserver sends an error message to ftclient on connection P, and ftclient displays the message on-screen.   
- Otherwise    
    • ftserver initiates a TCP data connection with ftclient on <DATA_PORT>. (Call this connection Q)   
    • If ftclient has sent the -l command, ftserver sends its directory to ftclient on connection Q, and ftclient displays the directory on-screen.   
    • If ftclient has sent -g <FILENAME>, ftserver validates FILENAME, and either    
         - sends the contents of FILENAME on connection Q.   ftclient saves the file in the current default directory (handling "duplicate file name" error if necessary), and displays a "transfer complete" message on-screen  or    
         - sends an appropriate error message (“File not found”, etc.) to ftclient on connection P, and ftclient displays the message on-screen.   
 - ftserver closes connection Q   
 - ftclient closes connection P and terminates.   
 - ftserver repeats from 2 (above) until terminated by a supervisor (SIGINT).   
    
  Examples of error messages:   
  ```
  python ftclient.py 127.0.0.1 9999 -g exampletext.txt //missing dataport number
  python ftclient.py 127.0.0.1 9999 -l exampletext.txt 9998 //file name not needed for directory listing 
  python ftclient.py 127.0.0.1 9999 -a exampletext.txt 9998 //no such -a command 
  ```
