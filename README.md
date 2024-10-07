Simple Shell in C
This project is a simple Unix shell implemented in C, inspired by the article "Write a Shell in C" by Stephen Brennan. The original tutorial provides a foundational understanding of shell implementation, while this project extends the functionality to support additional features like I/O redirection and piping.

Features
Basic Command Execution: Executes standard Unix commands.
I/O Redirection: Supports input (<) and output (>) redirection, a feature added beyond the tutorial.
Piping: Handles piping between two commands (|), allowing the output of one command to be used as the input for another (a custom addition).
Built-in Commands: Includes built-in shell commands such as cd, exit, and more.
Error Handling: Simple error handling for invalid commands.
How It Works
The shell reads user input from the terminal, parses the input to separate commands and arguments, and then uses fork(), exec(), and wait() system calls to execute the commands. Custom features like I/O redirection and piping are handled with additional logic to manage file descriptors and command chaining.

Compilation
To compile the shell, use the following command:

bash
Copy code
gcc -o my_shell shell.c
Usage
After compiling, run the shell using:

bash
Copy code
./my_shell
You can then type commands as you would in any Unix shell. Examples include:

bash
Copy code
$ ls -l
$ cd /path/to/directory
$ cat file.txt | grep "search_term"  # Piping
$ ls > output.txt  # Output redirection
$ sort < file.txt  # Input redirection
Built-in Commands
cd [directory] - Change the current directory.
exit - Exit the shell.
Additional Features
I/O Redirection: Added support for redirecting input and output using > and <.
Piping: Added support for piping two commands using |.
Resources
Article: Write a Shell in C by Stephen Brennan
License
This project is open-source and available under the MIT License.
