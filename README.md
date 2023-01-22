# FT_IRC

## README (Work In Progress)
What's next ? Code's documentation !

## Server configuration

### ```MAKE```

#### Compilation

First execute **make** to compile the project.  
> Not recommended but you can use **make -j** to use multihreading for faster compilation.  
<img width="1077" alt="image" src="https://user-images.githubusercontent.com/89400565/213895829-25c9e6c9-a36d-4d9e-9923-d3cc87275432.png">

#### Arguments at launch

The server needs 2 arguments to get launch, the first one is the port you'll use for it.  
You can use 8080 for example.  
The second one is your server password.  
<img width="381" alt="image" src="https://user-images.githubusercontent.com/89400565/213895943-d3ef4d5c-486b-44c5-a39f-21e8e398c741.png">


#### Next, let's execute our server.

To execute it, write ```./ircserv``` with both arguments and press **ENTER**.  
<img width="368" alt="image" src="https://user-images.githubusercontent.com/89400565/213895950-42c9a70b-6512-4ea5-8e62-2c32b0c8dbb8.png">

Now you can use the client of your choice to get in.  
We recommand **Weechat**, because we used it to build our irc server.  
You can also use **NC**.  


## Commands

Because we only created one server and didn't build multi-server communications, we didn't code commands for.

### Authentification commands.  
If using weechat, you have nothing to do, Client will send ```NICK```, ```USER``` and ```PASS``` command if they are set.  

#### ```USER <USERNAME> <HOSTNAME> <SERVERNAME> <REALNAME>```.  
Command to set your username and realname on the server.  
We don't use **HOSTNAME** and **SERVERNAME** because we don't have multiple server.  
> **USERNAME** set the username of your choice.   
> **REALNAME** set the your realname.  

#### ```NICK <NICKNAME>```.  
Command to set your nickname on the server.  
> **NICKNAME** set your nickname.  

#### ```PASS <PASSWORD>```.  
To get registered on server you must enter the server's password with this command.  
> **PASSWORD** is the server's password.  

#### ```QUIT <QUIT_MSG>```.  
If you want to leave the server, then use this commands.  
> **QUIT_MSG** is the message display at your exit.  

### Communication commands.  
Well, welcome to the most funny part of FT_IRC project, where you can insult your mate on your own server!  

#### ```JOIN <CHANNEL>,{<CHANNEL>} {<key>}```.  
If the channel is alreay created you will join it, else you'll create it, join it and be chan's operator.  
> **CHANNEL** is the channel name you want to create/join. You **MUST** put "#" before <CHANNEL>, you can also use "&".  
> If you put 0 for **CHANNEL** it will make you leave all channels you were in.  
> You can join multiple CHANNEL by separating them with a "," without white space.  
> **KEY** Maybe in a futur patch.  
<img width="780" alt="image" src="https://user-images.githubusercontent.com/89400565/213897737-f2014e70-5f38-418f-a8e9-20d193668fd3.png">

#### ```PRIVMSG <RECEIVER>{,<RECEIVER>} <TEXT_TO_SEND>```.  
Now that you have join your first channel, you may want to send some message in. Let's do it ?  
> **RECEIVER** can be the NICKAME's target you want to send your message to or CHANNEL's name.  
> To send in a channel, you must precise with "#" at the beginning of your **RECEIVER**.  
> As well as **JOIN**, use "," between your **RECEIVER** to send a msg to multiple target's.  
> **TEXT_TO_SEND** is your message.  
<img width="729" alt="image" src="https://user-images.githubusercontent.com/89400565/213898301-43bbdb66-ee2b-4e7f-af09-fe0fd4a57c2d.png">

#### ```NOTICE <RECEIVER>{,<RECEIVER>} <TEXT_TO_SEND>```.  
Notice is a kind of PRIVMSG but it will not return errors or numeric replies.  
Most of the time, this command is use for bots because it's safer for infinity loop.  
> Same args as PRIVMSG, refer to it for more informations.  
<img width="665" alt="image" src="https://user-images.githubusercontent.com/89400565/213898539-de432a4c-b4ad-4944-8218-1b145df4db44.png">

#### ```PART <CHANNEL>{,<CHANNEL>}```.  
The **PART** message causes the client sending the message to be removed from the list of active users for all given channels listed in the parameter string.  
> **CHANNEL** is the channel you want to live.  
> As like previous commands, you can leave multiple channels.	
