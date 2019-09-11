# Movie-Database

A movie database written in C++ for NWEN241.
Access movie information stored within a csv file from a simple C++ server, 
via another shell window using a TCP connection.

Commands:
GET -> Sends a message containing all rows of the database
GET %d -> Sends a message containing the row %d of the database.
BYE -> Closes the connection with the client.

If an invalid command is called, the server will send a message
- ERROR: (error message) and wait till further instruction from the
- client.

Reccomended to be run in Linux, as it utilises socket() and fork() system calls.
Instructions for usage can be seen within the assignment brief located within the project root directory.
