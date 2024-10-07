
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h> // Time-related functions
#include <unistd.h>
#include <wait.h>

#define MAX_CMD 1024
#define MAX_ARG 100
#define MAX_PATH 1024 // Maximum input size for printing the current directory

/* THIS IS NEW ********************/

/*
  Function Declarations for builtin shell commands:
 */
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int lsh_time_command(char **args);
int lsh_print_current_directory(char **args);

/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {"cd", "help", "exit", "time", "pwd"};

int (*builtin_func[])(char **) = {&lsh_cd, &lsh_help, &lsh_exit,
                                  &lsh_time_command,
                                  &lsh_print_current_directory};

int lsh_num_builtins() { return sizeof(builtin_str) / sizeof(char *); }

/*
  Builtin function implementations.
*/

/**
   @brief Builtin command: change directory.
   @param args List of args.  args[0] is "cd".  args[1] is the directory.
   @return Always returns 1, to continue executing.
 */
int lsh_cd(char **args) {
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}

/**
   @brief Builtin command: print help.
   @param args List of args.  Not examined.
   @return Always returns 1, to continue executing.
 */
int lsh_help(char **args) {
  int i;
  printf("Stephen Brennan's LSH\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < lsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

/**
   @brief Builtin command: exit.
   @param args List of args.  Not examined.
   @return Always returns 0, to terminate execution.
 */
int lsh_exit(char **args) { return 0; }

int lsh_print_current_directory(
    char **args) {    // function to print the current directory
                      // "/home/remote/starid/Documents/CSIS430 $"
  char cwd[MAX_PATH]; // Buffer to store the current working directory
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    printf("%s \n", cwd); // Print the current working directory with $
  } else {
    perror("getcwd failed"); // Print error message if getcwd fails
  }
  return 1;
}

// Function to measure the execution time of a command
int lsh_time_command(char **args) {
  struct timespec start, end; // Variables to store start and end times
  clock_gettime(CLOCK_MONOTONIC,
                &start); // Get the current time before execution

  pid_t pid = fork(); // Create a new process

  if (pid == 0) {
    // Child process
    if (execvp(args[1], args + 1) < 0) {
      perror("execvp failed"); // Print error message if execvp fails
    }
    exit(EXIT_FAILURE); // Exit the child process with failure status
  } else if (pid > 0) {
    // Parent process waits for the child to complete
    waitpid(pid, NULL, 0);
    clock_gettime(CLOCK_MONOTONIC,
                  &end); // Get the current time after execution

    // Calculate the elapsed time in seconds
    double elapsed =
        (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Elapsed time: %.3f seconds\n", elapsed); // Print the elapsed time
  } else {
    perror("fork failed"); // Print error message if fork fails
  }
  return 1;
}

/**
  @brief Launch a program and wait for it to terminate.
  @param args Null terminated list of arguments (including program).
  @return Always returns 1, to continue execution.
 */
int lsh_launch(char **args) {
  pid_t pid;
  int status;

  // Copy the streams
  int saved_stdin = dup(STDIN_FILENO);
  int saved_stdout = dup(STDOUT_FILENO);
  //////////////////////////////////////////////////////

  for (int j = 0; args[j] != NULL; j++) {
    if (strcmp(args[j], ">") == 0) { // Output redirection
      int fd = open(args[j + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
      if (fd < 0) {
        perror("Open fialed!!!");
        return 1;
      }

      dup2(fd, STDOUT_FILENO);
      close(fd);

      args[j] = NULL; // last argument! need to end somewhere

    } else if (strcmp(args[j], "<") == 0) { // Input redirection
      int fd = open(args[j + 1], O_RDONLY);
      if (fd < 0) {
        perror("Open failed");
        return 1;
      }

      dup2(fd, STDIN_FILENO); // redirection stdin from the file
      close(fd);
      args[j] = NULL; // end the arguments at the redirection point
    }
  }
  //////////////////////////////////////////////////////

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("lsh");
  } else {
    // Parent process
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));

    // Restore stdout or sdtdin
    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdout);
    dup2(saved_stdin, STDIN_FILENO);
    close(saved_stdin);
  }

  return 1;
}

/**
   @brief Execute shell built-in or launch program.
   @param args Null terminated list of arguments.
   @return 1 if the shell should continue running, 0 if it should terminate
 */
int lsh_execute(char **args) {
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}

/**
   @brief Read a line of input from stdin.
   @return The line from stdin.
 */
char *lsh_read_line(void) {
#ifdef LSH_USE_STD_GETLINE
  char *line = NULL;
  ssize_t bufsize = 0; // have getline allocate a buffer for us
  if (getline(&line, &bufsize, stdin) == -1) {
    if (feof(stdin)) {
      exit(EXIT_SUCCESS); // We received an EOF
    } else {
      perror("lsh: getline\n");
      exit(EXIT_FAILURE);
    }
  }
  return line;
#else
#define LSH_RL_BUFSIZE 1024
  int bufsize = LSH_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // Read a character
    c = getchar();

    if (c == EOF) {
      exit(EXIT_SUCCESS);
    } else if (c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      bufsize += LSH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
#endif
}

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
/**
   @brief Split a line into tokens (very naively).
   @param line The line.
   @return Null-terminated array of tokens.
 */
char **lsh_split_line(char *line) {
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char *));
  char *token, **tokens_backup;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens_backup = tokens;
      tokens = realloc(tokens, bufsize * sizeof(char *));
      if (!tokens) {
        free(tokens_backup);
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}
/**************************************************************/

int execute_pipe(char *cmd1, char *cmd2) {
  pid_t pid1;
  pid_t pid2;
  char **args_cmd1;
  char **args_cmd2;
  int fd[2];
  pipe(fd);
  int status;

  args_cmd1 = lsh_split_line(cmd1);
  args_cmd2 = lsh_split_line(cmd2);

  if ((pid1 = fork()) == 0) {
    dup2(fd[1], STDOUT_FILENO);
    close(fd[0]);
    close(fd[1]);
    // execute_command(cmd1);
    lsh_execute(args_cmd1);
    exit(0);
  }
  if ((pid2 = fork()) == 0) {
    dup2(fd[0], STDIN_FILENO);
    close(fd[0]);
    close(fd[1]);
    // execute_command(cmd2);
    lsh_execute(args_cmd2);
    exit(0);
  }

  close(fd[0]);
  close(fd[1]);
  wait(NULL);
  // wait(NULL);
  waitpid(pid1, &status, 0);
  waitpid(pid2, &status, 0);
  status = 1; 

  return status;
}

/**************************************************************/
/**
   @brief Loop getting input and executing it.
 */
void lsh_loop(void) {
  char *line;
  char **args;
  int status;

  do {
    printf("UnderCshell > ");
    line = lsh_read_line();
    char *pipe_pos = strchr(line, '|');
    if (pipe_pos) {
      *pipe_pos = '\0';
      char *cmd1 = line;
      char *cmd2 = pipe_pos + 1;
      status = execute_pipe(cmd1, cmd2);
    } else {
      args = lsh_split_line(line);
      status = lsh_execute(args);
      free(args); // Free the args if not in pipe
    }

    free(line); // Free the input line
    // printf("STATUS: %d\n", status);
  } while (status);
}

/**
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */
int main(int argc, char **argv) {
  // Load config files, if any.

  // Run command loop.
  lsh_loop();

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}