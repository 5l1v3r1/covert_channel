# covert_channel

----------------------------------------
## Installation Notes
----------------------------------------

To install and run
- make all
- ./covert -h

----------------------------------------
### Server Mode
----------------------------------------

To run in server mode

./covert -s -f <File to write to> -w 23456


----------------------------------------
### Client Mode
----------------------------------------

To run an client

./covert -d 127.0.0.1 -p 2456 -w 23456 -f <file to send>
