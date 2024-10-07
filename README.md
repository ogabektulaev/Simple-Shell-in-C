```markdown
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
```

## 🏃 Usage

Run the shell:

```bash
./my_shell
```

You can execute typical shell commands. Here are some examples:

```bash
$ ls -l
$ cd /path/to/directory
$ cat file.txt | grep "search_term"   # Piping
$ ls > output.txt                     # Output redirection
$ sort < file.txt                     # Input redirection
```

## 📜 Built-in Commands

| Command         | Description                     |
|-----------------|---------------------------------|
| `cd [directory]`| Change the current directory.    |
| `exit`          | Exit the shell.                 |

## ✨ Additional Features
- **I/O Redirection**: Supports `>` and `<` for input and output redirection.
- **Piping**: Allows connecting two commands using the `|` symbol.

## 📚 Resources
- **Article**: [Write a Shell in C](https://brennan.io/2015/01/16/write-a-shell-in-c/) by Stephen Brennan.

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

Feel free to contribute or open issues if you encounter any problems!

```
