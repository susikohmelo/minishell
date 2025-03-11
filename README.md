# Minishell <img align="left" src="https://github.com/susikohmelo/minishell/blob/main/readme_files/score.png" height="120"/>
A fully functional shell that closely follows the behavior of bash. <br/>
All the essential features are here - commands, pipes, error codes, paths, expansions etc.

<br/>

<img src="https://github.com/susikohmelo/minishell/blob/main/readme_files/minishell_minidemo.gif"/>

## Features
* **Commands** <br/>
<sup> in PATH, relative & absolute
* **I/O redirection & heredoc** <br/>
<sup> '<' '<<' '>' '>>' <sup/>
* **Variable expansion** <br/>
<sup> '$USER' '$?' etc <sup/>
* **Our own implementations of builtin cmds** <br/>
<sup> cd/echo/env etc. <sup/>
* **Command history** <br/>
<sup> remember previous commands <sup/>
* **Signals & error codes** <br/>
<sup> Signals are handled and the exit codes are correc <sup/>
* **Wildcards** <br/>
<sup> expand the '*' character <sup/>
* **Pipes**
## Install & run
Download the repository, go to project_files and `make`. "minishell" is the name of the executable, just run it `./minishell`<br/>
Or copy paste the entire thing from here:
```
git clone git@github.com:susikohmelo/minishell.git
cd minishell/project_files
make
./minishell
```
