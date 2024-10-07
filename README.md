# 🔧 Simple Shell in C

[![MIT License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

This project is a simple Unix shell implemented in C, inspired by the article ["Write a Shell in C"](https://brennan.io/2015/01/16/write-a-shell-in-c/) by Stephen Brennan. The original tutorial provides a foundation for shell programming, and I have extended it to support I/O redirection and piping functionality.

## 🚀 Features
- 🛠️ **Basic Command Execution**: Executes standard Unix commands.
- 📂 **I/O Redirection**: Supports input (`<`) and output (`>`) redirection.
- 🔗 **Piping**: Allows piping between two commands (`|`).
- 🧠 **Built-in Commands**: Includes `cd`, `exit`, and more.
- ⚠️ **Error Handling**: Provides basic error messages for invalid commands.

## 🛠️ How It Works
The shell reads user input from the terminal, parses the input to separate commands and arguments, and then uses system calls (`fork()`, `exec()`, `wait()`) to execute the commands. Custom features such as I/O redirection and piping were implemented to manage file descriptors and command chaining.

## 🖥️ Compilation

To compile the shell, run:

```bash
gcc -o my_shell shell.c
