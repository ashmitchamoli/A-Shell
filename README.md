# A-Shell: A Custom Shell in C
## Requirements
- Linux OS
- GCC Compiler

## How to Run
To run A-Shell, 
- Open terminal in the root directory and run `make`
- run `./shell` to start the shell.

## Functionality and Files
###  Features
- 
- Basic commands `cd`, `pwd`, `echo`, `ls`, `history`, `discover` are supported

### Basic Functionality
- Input handled for all commands in `main.c`, `input.c` and `execute.c`
- Displaying of prompt handled in `prompt.c`, `main.c`
- File redirection supported using `>`,  `>>` and  `<` tokens.
- Autocomplete functionality, detects and autocompletes the name of currently typed file or directory.
- Signal handling : 
    - `CTRL + Z` sends the currently running foreground job to background and changes its state from `running` to `stopped`. Has no effect if no foreground job running.
    - `CTRL + C` interrupts the currently running foreground job by sending the `SIGINT` signal. Has no effect if there is no foreground job running.
    - `CTRL + D` logs the user out of the shell without having any effect on the actual terminal.
-  Piping functionality for commands.

### Commands

### `cd`
- Implemented in `cd.c`

### `echo`
- Print the arguments passed to echo in a **space** seperated manner
- Implemented in `execute.c`

### `pwd`
- Print the present working directory
- Implemented in `execute.c`

### `ls`
- Print the contents of the specified directories (current directory if argument list empty)
- Supports ``-l`` and ``-a`` flags.
- Implemented in `ls.c`

### `system commands`
- Shell executes system in either background or foreground
- Commands invoked with `&` run in the background while all other commands run in the foreground.
- Main functionality in `bg_handler.c` with supplementary code in `execute.c`, `input.c` and `process_list.c`

### `pinfo`
- Prints the process related info of your shell program
- Implemented in `pinfo.c`

### `discover`
- Searches for the target file/directory in the directory given as argument (current directory by default) according to the given flags. (-d searches for directories only while -f searches for files only)
- Implemented in `discover.c`

### `history`
Displays a maximum of latest 10 commands input to the shell accross all instances of the shell created on the system. Following are the basic implementation details:
- Main implementation in `history.c`
- `init_shell()` is called in main to ensure existence of the file `/tmp/A-Shell_history.txt` whenever a shell instance is created
- Updation of `/tmp/A-Shell_history.txt` on every input is handled in `input.c`

### `jobs`
Displays a list of currently existing background processes with their index, status (running or stopped) and name sorted in alphabetical order.
- Main implementation in `jobs.c`

### `sig`
Sends the specified signal to the specified job index as shown in `jobs`.
- Main implementation in `jobs.c`

### `fg`
Brings the specified job index, a currently background job, to the foreground and changes its state to running.
- Main implementation in `jobs.c`

### `bg`
Changes the state of a stopped background process to running in the background. It does nothing if the background job is already running.
- Main implementation in `jobs.c`

