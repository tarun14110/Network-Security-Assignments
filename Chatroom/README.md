# PEER CHAT

How to run:
  - `make`  => compile all files
  - `./register_server` => runs server on port 5555, listening to clients for registerations.
  - `./server <port>` => runs main server on specified port
  - `./client <IP_of_server> <port>` => connects with the server on the specified port.

### Commands

Peer chat uses a number of Commands:

* `/who` - displays all the users logged in (online), including the user who is querying. 
* `/msg` - /msg <username> <messageToSent> , sents private message to the user. Message can be sent only to online users.
* `/create_grp` - /create_grp <groupName> <members_separated_by_space> forms group.
* `/join_grp` - /join_grp <groupName>. Any user can join any existing group. (Handled the case if the user already a member of the group)
* `/send` - /send <username> <filename> , Sends file to the user. User will not receive until he uses "/recv".
* `/recv` - receive all the waiting files.
* `/msg_group` - /msg_group <groupName> <message> , sends message to all the members of the group who are online. Sender won't get his message.
* `/help` - displays all command along with info.
* `/quit or /q` - quit the program.

### Error Handling

* File types allowed are png, jpg, jpeg, pdf, mkv. We can add more types. (just have to add to the list).
* We can tranfer as much file as we can, till hard disk gets full :-). But I limited to 2 gb file transfer allowed.
* Limited number of Connected users to 4 (we can limit that to much more than that, but for testing small number will work). After that users trying to connect will get message "Chat room is full".
* Correct number of arguments checked for every command. and displayed message accordingly.
* Limited and handled length of usernames, group names.
* Checking if the user is online or not before sending personal message and acknowledging user if user is not online or don't exist.
* Whenever a user connects or disconnects, broadcasts to all online users.
