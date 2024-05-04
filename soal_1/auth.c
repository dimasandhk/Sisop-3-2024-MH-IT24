#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>

int main() {
    struct dirent *de;
    DIR *dr = opendir("new-data");
    if (dr == NULL) {
        printf("Gabisa dibuka ngab");
        return 0;
    }

    // Create a shared memory segment
    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, 1024, 0666 | IPC_CREAT);

    // Attach the shared memory segment to the address space of the process
    char *str = (char*) shmat(shmid, (void*)0, 0);
    char *end = str;  // Pointer to the end of the data

    int curr = 0;
    while ((de = readdir(dr)) != NULL) {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
            continue;
        char *filename = strdup(de->d_name);
        char *token = strtok(filename, "_");
        int delete = 1;  // Asumsi awal delete
        while (token != NULL) {
            if (strcmp(token, "trashcan") == 0 || strcmp(token, "parkinglot") == 0) {
                delete = 0;  // Jangan delete
                break;
            }
            token = strtok(NULL, "_.");
        }
        free(filename);
        if (delete) {
            char path[1024];
            snprintf(path, sizeof(path), "new-data/%s", de->d_name);
            if (remove(path) == 0)
                printf("%s dah diapus\n", de->d_name);
        }
        if (!delete) {
            char path[1024];
            snprintf(path, sizeof(path), "new-data/%s", de->d_name);

            // Open the file
            FILE *file = fopen(path, "r");
            if (file == NULL) {
                printf("Cannot open file\n");
                return 0;
            }

            // Get the size of the file
            fseek(file, 0, SEEK_END);
            long fsize = ftell(file);
            fseek(file, 0, SEEK_SET);

            // Read the content of the file into a buffer
            char *content = malloc(fsize + 1);
            fread(content, 1, fsize, file);
            fclose(file);
            content[fsize] = 0;
            
            printf("File name: %s\n", de->d_name);
            // Write the filename and the content of the file to the shared memory
            if (curr == 0) {
                sprintf(end, "\n%s\n%s", de->d_name, content);
                end += strlen(de->d_name) + strlen(content) + 2;  // Move the end pointer
            } else {
                sprintf(end, "\n\n%s\n%s", de->d_name, content);
                end += strlen(de->d_name) + strlen(content) + 1;  // Move the end pointer
            }

            printf("Data written in memory: %s\n",str);

            // Free the buffer
            free(content);
        }
        curr++;
    }

    shmdt(str);  // Detach the shared memory segment

    closedir(dr);
    return 0;
}