# f0xhole hardware backdoor
Linux backdoor written in C. It was created as a basic demonstration on how to implement a backdoor into Linux based firmware images for hardware hacking.

## Build

Download the c code and compile it using GCC or cmake into whatever linux architecture you are targeting

## Use

Once its running on the target machine you send a keyword using `echo -n "secret_trigger" | nc -u 127.0.0.1 1337` which will then open up the actual backdoor.
You can then connect to the backdoor using anything, example `nc <target ip> 1338`

## Pull Requests

If you modify or improve the code at all please put in a pull request so that we can implement it. 

## TODO
- [ ] Add a prompt
- [ ] Add a message to let you know the trigger worked
- [ ] Develop an obfuscated version that is less obvious
