// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <sys/wait.h>

// #define MAX_NAME_LENGTH 100
// #define MAX_LINE_LENGTH 256

// int main(int argc, char *argv[]) {
//   if (argc != 2) {
//     fprintf(stderr, "Usage: %s <name>\n", argv[0]);
//     exit(1);
//   }

//   int pipe_fd[2];  // Pipe for communication between parent and child

//   // Create a pipe for communication
//   if (pipe(pipe_fd) == -1) {
//     perror("pipe");
//     exit(1);
//   }

//   pid_t pid = fork();
//   if (pid == -1) {
//     perror("fork");
//     exit(1);
//   }

//   if (pid == 0) { // Child process
//     // Close unused read end of the pipe in the child process
//     close(pipe_fd[0]);

//     // Duplicate stdout (descriptor 1) to the write end of the pipe (descriptor pipe_fd[1])
//     dup2(pipe_fd[1], 1);

//     // Close unused write end of the pipe (now it's duplicated to stdout)
//     close(pipe_fd[1]);

//     // Execute grep command to search for the name in phonebook.txt
//     execlp("grep", "grep", argv[1], "phonebook.txt", NULL);
//     perror("execlp");  // If execlp fails, print error and exit
//     exit(1);
//   } else { // Parent process
//     // Close unused write end of the pipe in the parent process
//     close(pipe_fd[1]);

//     // Open a temporary file for storing grep output
//     FILE *temp_file = tmpfile();
//     if (temp_file == NULL) {
//       perror("tmpfile");
//       exit(1);
//     }

//     // Redirect child process output (pipe_fd[0]) to the temporary file
//     dup2(pipe_fd[0], fileno(temp_file));
//     close(pipe_fd[0]);

//     // Wait for child process to finish
//     wait(NULL);

//     // Rewind the temporary file to the beginning
//     rewind(temp_file);

//     // Check if grep found any results (empty file indicates no match)
//     if (feof(temp_file)) {
//       fprintf(stderr, "No entry found for %s\n", argv[1]);
//       exit(1);
//     }

//     // Use awk to extract the phone number from the first line (assuming only one match)
//     char line[MAX_LINE_LENGTH];
//     FILE *awk_pipe = popen("awk -F, '{print $2}'", "w");
//     if (awk_pipe == NULL) {
//       perror("popen");
//       exit(1);
//     }

//     // Read the first line from the temporary file (grep output)
//     if (fgets(line, sizeof(line), temp_file) == NULL) {
//       perror("fgets");
//       exit(1);
//     }
//     // fgets(line, sizeof(line), temp_file);

//     // Write the line to awk's standard input for processing
//     fputs(line, awk_pipe);

//     // Read the phone number extracted by awk
//     char phone_number[MAX_NAME_LENGTH];
//     if (fgets(phone_number, sizeof(phone_number), awk_pipe) == NULL) {
//       perror("fgets");
//       exit(1);
//     }
//     // fgets(phone_number, sizeof(phone_number), awk_pipe);
//     // Close pipes and temporary file
//     pclose(awk_pipe);
//     fclose(temp_file);

//     // Remove trailing newline from the phone number
//     phone_number[strcspn(phone_number, "\n")] = '\0';

//     // Print the phone number
//     //printf("Phone number: %s\n", phone_number);
//   }

//   return 0;
// }

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>



#define DEFAULT_PHONEBOOK_FILE "phonebook.txt" // Assuming phonebook.txt is in the same directory

// int main(int argc, char *argv[]) {
//   if (argc < 2 || argc > 3) {
//     fprintf(stderr, "Usage: %s <name> [<phonebook_file>]\n", argv[0]);
//     exit(1);
//   }

//   char *name = argv[1];
//   char *phonebook_file = (argc == 3) ? argv[2] : DEFAULT_PHONEBOOK_FILE;

//   int pipe1[2], pipe2[2]; // Two pipes for chaining commands
//   if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
//     perror("pipe");
//     exit(1);
//   }

//   int pid1 = fork(); // First child process (grep -i for case-insensitive)
//   if (pid1 == -1) {
//     perror("fork");
//     exit(1);
//   }

//   if (pid1 == 0) {
//     dup2(pipe1[1], STDOUT_FILENO); // Redirect grep output to pipe1
//     close(pipe1[0]); // Close unused read end of pipe1
//     close(pipe2[0]); // Close unused read and write ends of pipe2
//     close(pipe2[1]);

//     // Search for the entire line containing the name (case-insensitive)
//     execlp("grep", "grep", "i", name, phonebook_file, NULL);

//     perror("execlp");
//     exit(1);
//   } else {
//     int pid2 = fork(); // Second child process (awk)
//     if (pid2 == -1) {
//       perror("fork");
//       exit(1);
//     }

//     if (pid2 == 0) {
//       dup2(pipe1[0], STDIN_FILENO);  // Redirect awk input from pipe1
//       dup2(pipe2[1], STDOUT_FILENO); // Redirect awk output to pipe2
//       close(pipe1[1]); // Close unused write end of pipe1
//       close(pipe2[0]); // Close unused read end of pipe2
//       close(pipe2[1]); // Close write end after dup (for awk)

//       // Print phone number (handle missing field)
//       execlp("awk", "awk", "{print $3? $3 : \"Not Found\"}", NULL);

//       perror("execlp");
//       exit(1);
//     } else { // Parent process
//       close(pipe1[0]); // Close unused read ends
//       close(pipe1[1]);
//       close(pipe2[1]); // Close unused write end

//       int status;
//       waitpid(pid1, &status, 0); // Wait for grep (pid1)
//       waitpid(pid2, &status, 0); // Wait for awk (pid2)

//       if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
//         // Both grep and awk succeeded
//         char buffer[10]; // Assuming max phone number length is 9 digits + 1 separator
//         ssize_t bytes_read = read(pipe2[0], buffer, sizeof(buffer) - 1);
//         if (bytes_read == 0) {
//           fprintf(stderr, "Name not found in phonebook\n");
//         } else {
//           buffer[strcspn(buffer, "\n")] = '\0'; // Remove trailing newline
//           printf("Phone number: %s\n", buffer);
//         }
//       } else {
//         // Error handling for grep or awk
//         perror("Error occurred");
//       }
//     }
//   }

//   return 0;
// }






// int main(int argc, char *argv[]) {
//   if (argc != 3) {
//     fprintf(stderr, "Usage: %s <name> <phonebook_file>\n", argv[0]);
//     exit(1);
//   }

//   int pipe1[2], pipe2[2]; // Two pipes for chaining commands
//   if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
//     perror("pipe");
//     exit(1);
//   }

//   int pid1 = fork(); // First child process (grep)
//   if (pid1 == -1) {
//     perror("fork");
//     exit(1);
//   }

//   if (pid1 == 0) {
//     dup2(pipe1[1], STDOUT_FILENO); // Redirect grep output to pipe1
//     close(pipe1[0]); // Close unused read end of pipe1
//     close(pipe2[0]); // Close unused read and write ends of pipe2
//     close(pipe2[1]);

//     // Search for the entire line containing the name (case-insensitive)
//     execlp("grep", "grep", argv[1], argv[2], NULL);

//     perror("execlp");
//     exit(1);
//   } else {
//     int pid2 = fork(); // Second child process (awk)
//     if (pid2 == -1) {
//       perror("fork");
//       exit(1);
//     }

//     if (pid2 == 0) {
//       dup2(pipe1[0], STDIN_FILENO);  // Redirect awk input from pipe1
//       dup2(pipe2[1], STDOUT_FILENO); // Redirect awk output to pipe2
//       close(pipe1[1]); // Close unused write end of pipe1
//       close(pipe2[0]); // Close unused read end of pipe2
//       close(pipe2[1]); // Close write end after dup (for awk)
//       execlp("awk", "awk", "{print $2}", NULL); // Print second field (phone number)
//       perror("execlp");
//       exit(1);
//     } else { // Parent process
//       close(pipe1[0]); // Close unused read ends
//       close(pipe1[1]);
//       close(pipe2[1]); // Close unused write end

//       int status;
//       waitpid(pid1, &status, 0); // Wait for grep (pid1)
//       waitpid(pid2, &status, 0); // Wait for awk (pid2)

//       if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
//         // Both grep and awk succeeded, read phone number
//         printf("Phone number: ");
//         char buffer[10]; // Assuming max phone number length is 9 digits + 1 separator
//         read(pipe2[0], buffer, sizeof(buffer) - 1);
//         buffer[strcspn(buffer, "\n")] = '\0'; // Remove trailing newline
//         printf("%s\n", buffer);
//       } else {
//         fprintf(stderr, "Name not found in phonebook\n");
//       }
//     }
//   }

//   return 0;
// }

int main(int argc, char *argv[]) {
  if (argc < 2 || argc > 3) {
    fprintf(stderr, "Usage: %s <name> [<phonebook_file>]\n", argv[0]);
    exit(1);
  }

  char *name = argv[1];
  char *phonebook_file = (argc == 3) ? argv[2] : DEFAULT_PHONEBOOK_FILE;

  int pipe1[2], pipe2[2]; // Two pipes for chaining commands
  if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
    perror("pipe");
    exit(1);
  }

  int pid1 = fork(); // First child process (grep)
  if (pid1 == -1) {
    perror("fork");
    exit(1);
  }

  if (pid1 == 0) {
    dup2(pipe1[1], STDOUT_FILENO); // Redirect grep output to pipe1
    close(pipe1[0]); // Close unused read end of pipe1
    close(pipe2[0]); // Close unused read and write ends of pipe2
    close(pipe2[1]);

    // Search for the entire line containing the name (case-insensitive)
    execlp("grep", "grep", "-i", name, phonebook_file, NULL);

    perror("execlp");
    exit(1);
  } else {
    int pid2 = fork(); // Second child process (awk)
    if (pid2 == -1) {
      perror("fork");
      exit(1);
    }

    if (pid2 == 0) {
      dup2(pipe1[0], STDIN_FILENO);  // Redirect awk input from pipe1
      dup2(pipe2[1], STDOUT_FILENO); // Redirect awk output to pipe2
      close(pipe1[1]); // Close unused write end of pipe1
      close(pipe2[0]); // Close unused read end of pipe2

      // Print only the second field (phone number) after splitting by comma
      execlp("awk", "awk", "-F,", "{print $2}", NULL);

      perror("execlp");
      exit(1);
    } else { // Parent process
      close(pipe1[0]); // Close unused read ends
      close(pipe1[1]);
      close(pipe2[1]); // Close unused write end

      int status;
      waitpid(pid1, &status, 0); // Wait for grep (pid1)
      waitpid(pid2, &status, 0); // Wait for awk (pid2)

      if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        // Both grep and awk succeeded, read phone number
        printf("Phone number: ");
        char buffer[12]; // Assuming max phone number length is 9 digits + 1 separator
        ssize_t bytes_read = read(pipe2[0], buffer, sizeof(buffer) - 1);
        if (bytes_read == 0) {
          fprintf(stderr, "Name not found in phonebook\n");
        } else {
          buffer[strcspn(buffer, "\n")] = '\0'; // Remove trailing newline
          printf("%s\n", buffer);
        }
      } else {
        fprintf(stderr, "Name not found in phonebook\n");
      }
    }
  }

  return 0;
}

