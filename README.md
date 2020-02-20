# Project-1-Accio
Name: Joel Sare
NUID: 35125075

*IMPORTANT* for whatever reason, when I use "hostname" for the client's host name, it doesn't connect, so I used "127.0.0.1" and that worked.

High level design:
My server makes a file when it gets a connection, after 10 seconds of not receiving data, it writes "ERROR" to the file and closes the connection. If there's a connection and the client sends data, I copy the file to the specified directory. If the client connects and sends data but does nothing else for 10 seconds, I write "ERROR" to the file and discard any partial input. I handled multiple clients by using the fork() command instead of threading. Multi-threading made no sense to me, it would be nice if we went over that in class. I read a lot about the fork() method and basically did 3 different things depending on what value it returns (either -1, 0, or >1).

Problems:
I ran into so many problems, this is probably one of the hardest programming assignments I've ever done. One big problem was finding out how to close a connection if the client doesn't do anything for 10 seconds. I found out you can just set a property to the server socket to close if nothing is sent for 10 seconds. 

Additional libraries:
none

Online tutorials:
https://stackoverflow.com/questions/5577564/what-exactly-does-fork-return
https://linux.die.net/man/7/socket
