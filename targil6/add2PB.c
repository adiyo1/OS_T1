#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

// int main(int argc, char *argv[]) {
//   if (argc != 2) {
//     fprintf(stderr, "Usage: %s <name> <phone_number>\n", argv[0]);
//     exit(1);
//   }

//   // int name_fd = open(argv[1], O_RDONLY);
//   // if (name_fd == -1) {
//   //   perror("open name");
//   //   exit(1);
//   // }

//   // int phone_fd = open(argv[2], O_RDONLY);
//   // if (phone_fd == -1) {
//   //   perror("open phone number");
//   //   exit(1);
//   // }

//   int phonebook_fd = open("phonebook.txt", O_WRONLY | O_APPEND, 0644);
//   if (phonebook_fd == -1) {
//     perror("open phonebook");
//     exit(1);
//   }

//   int pipe_fd[2];
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
//     close(pipe_fd[0]);  // Close read end in child
//     dup2(pipe_fd[1], STDOUT_FILENO); // Duplicate write end to stdout

//     // Concatenate name and phone number with comma separator (",")
//     execlp("cat", "cat", argv[1], argv[2], NULL);
//     perror("cat");
//     exit(1);
//   } else { // Parent process
//     close(pipe_fd[1]);  // Close write end in parent
//     // close(name_fd);     // Close name file descriptor
//     // close(phone_fd);    // Close phone number file descriptor
//     dup2(pipe_fd[0], STDIN_FILENO);  // Duplicate read end to stdin

//     // Add comma using tr (translate newline to comma)
//     execlp("tr", "tr", "\\n", ",", NULL);
//     perror("tr");
//     exit(1);
//   }

//   close(phonebook_fd); // Close phonebook file descriptor
//   return 0;
// }
int main(int argc, char *argv[]) {
    FILE *fp = fopen("phonebook.txt", "a"); // Replace with your filename
    if (fp == NULL) {
        perror("Error opening file");
        return 1;
    }

    int i;
    fprintf(fp, "%s", argv[1]);
    for (i = 2; i < argc - 1; i++)
    {
        fprintf(fp, " %s", argv[i]);
    }

    fprintf(fp, ",%s\n", argv[i]);

    // **Replace this section with your desired content:**
    // fprintf(fp, "%s\n", argv[1]);
    // fprintf(fp, "This is your second line with another newline.\n");
    // You can add more lines using fprintf or fputs as explained earlier.

    fclose(fp);
    return 0;
}
