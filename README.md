# Shell-gbsh
A Linux Shell written in C Language gbsh, which implements some of the features found in typical shells, like the bash (Bourne Again Shell) or csh (C-Shell).

Here are Some Features of the Shell:

> Prompt
> exit command
> pwd command
> clear command
> ls command
> cd command
> setenv command
> unsetenv command
> environ command
> The shell environment contains shell=<pathname>/gbsh environment variable
> All other commands entered are done by the shell forking and executing the programs as its own child processes
> Zombie Processes are handled
> Support for I/O redirection on either or both stdin and/or stdout is implemented
  e.g
  <cmd> <arg1> <arg2> > <outputfile>
  <cmd> <arg1> <arg2> < <inputfile> > <outputfile>
> Implementation of background processes is also implemented using & symbol at the end of command
> Programs can run together such that one program reads the output from another with no need for an explicit intermediate file. In the following line,
  <cmd1> | <cmd2>
  command <cmd1> is executed, and its output is used as the input of <cmd2>. This is commonly called piping, since the | character is known as a ``pipe''.
> Dynamic chaining of multiple pipes is also implemented
> SIGINT signal does not result in termination of the shell.

To Run the Shell, type make to compile all the files and generate an output file, then type ./gbsh to execute the Shell.
