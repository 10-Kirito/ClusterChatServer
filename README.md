# Cluster chat server



## 1. Introduction

***Technical Points:***

- using `muduo` [network library](https://github.com/chenshuo/muduo) as the core of network module;
- using `json` [library](https://github.com/nlohmann/json) as the private communication protocol;
- configure [nginx](https://nginx.org/en/) to achieve ***load balancing***;
- using [redis](https://redis.io/) as the message queue(publish-subscribe modern);
- using `CMake` to manage the dependencies of the entire project;
- using `MYSQL` relational database as message communication for project data;



The follow is the UI you can see in it:

```shell
--------------------------------login user-------------------------------
current user => account:2019 username:kirito
--------------------------------friend list------------------------------
        ID                NAME                                   STATE
      2001                 kir                                 offline
      2002                 Tom                                 offline
      2020                Samo                                 offline
      2024           addfriend                                 offline
--------------------------------group list--------------------------------
        ID                NAME                                    DESC
         1          happy game                              happy play
         3                test                               test test
    200000           new group                              happy play
    200001                blue                               blue test
    200005            test2020                                   happy
    200011          testupdate                              testupdate
--------------------------------------------------------------------------
Type `help` to see other commands.

->show command list
addfriend       : (add friend)                  usage-> `addfriend:friendid`  
joingroup       : (create group)                uage-> `joingroup:groupid`    
showme          : (show my details)             usage-> `showme`              
list            : (get the users in the group)  usge-> `list:groupid`         
creategroup     : (create group)                usage-> `creategroup:groupname:groupdesc`
quitgroup       : (quit group)                  uage-> `quitgroup:groupid`    
groupchat       : (chat with group)             usage-> `groupchat:groupid:message`
updatelist      : (update the user's list)      usage-> `updatelist`          
chat            : (chat with someone)           usage-> `chat:friendid:message`
help            : (get the help list)           usage-> `help`                
deletegroup     : (delete group)                uage-> `deletegroup:groupid`  
quit            : (exit the chat)               usage-> `quit`                
->
```

## 2. Just try it!

There is two version source codes:

- One doesn't use message queue, so the users who located different servers don't communicate(***main branch***)

But you need to configure the nginx firstly: 

Just add some codes into the `nginx.conf`:

```shell
stream {
    upstream MyServer {
        server 127.0.0.1:6000 weight=1 max_fails=3 fail_timeout=30s;
        server 127.0.0.1:6002 weight=1 max_fails=3 fail_timeout=30s;
    }

    server {
        proxy_connect_timeout 1s;
        listen 8000;
        proxy_pass MyServer;
        tcp_nodelay on;
    }
}
```

Then you can run the follow commands:

```shell
# you can run the build.sh to compile it 
./build.sh
# then the executable file will be ouput into `bin` directory
cd bin

# run the server
./chatserver 127.0.0.1 6000
./chatserver 127.0.0.1 6002

# run the client
./chatclient 127.0.0.1 8000

# run another client
./chatclient 127.0.0.1 8000
```

- Another(***message_queue branch***) works well.

First you should `git checkout messge_queue`, then you can compile it and run it like before.

```shell
# run the server
./chatserver 127.0.0.1 6000
./chatclient 127.0.0.1 8000


# different server but it works well
./chatserver 127.0.0.1 6002
./chatclient 127.0.0.1 8000
```

## 3. What can you learn from this project?

- ***Network Programming Skills:*** Utilizing the `muduo` network library for network programming, you can learn how to design and implement high-performance network communication modules, including handling concurrent connections, message transmission, etc.
- ***Communication Protocol Design***: Designing a private communication protocol using the `json` library enables you to learn how to design suitable communication protocols to meet project requirements, including message formats, data transmission methods, etc.
- ***Load Balancing Configuration and Management***: Configuring and managing `nginx` for load balancing teaches you how to design and implement load balancing strategies, as well as how to configure and manage load balancing servers.
- ***Message Queue Utilization***: Using `redis` as a message queue to implement the publish-subscribe pattern helps you understand how to use message queues to decouple system components, improve system scalability, etc.
- ***Dependency Management and Build Tools***: Managing project dependencies using `CMake` allows you to learn how to use dependency management tools to manage project dependencies and how to use build tools to build and compile projects.
- ***Database Design and Management***: Designing and managing databases, including storing and managing user information, chat records, etc., helps you learn how to design suitable database schemas to meet project requirements and how to use database management tools to manage databases.

