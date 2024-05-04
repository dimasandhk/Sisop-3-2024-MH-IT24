#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <time.h>

int main() {
    struct stat st = {0};

    // Check if microservices directory exists, if not create it
    if (stat("microservices", &st) == -1) {
        mkdir("microservices", 0700);
    }

    // Check if microservices/database directory exists, if not create it
    if (stat("microservices/database", &st) == -1) {
        mkdir("microservices/database", 0700);
    }

    // Check if microservices/database/db.log file exists, if not create it
    FILE *logFile = fopen("microservices/database/db.log", "a");

    key_t key = ftok("shmfile",65);

    int shmid = shmget(key, 1024, 0666 | IPC_CREAT);

    // attach ke shared memory
    char *str = (char*) shmat(shmid,(void*)0,0);

    // printf("%s\n", str);
    char *line = strtok(str, "\n");
    char *currName = "";
    while (line != NULL) {
        // printf("%s\n", line);
        // printf("mashok");
        if (strstr(line, "trashcan") != NULL) {
            char temp[100];
            sprintf(temp, "microservices/database/%s", line);
            FILE *file = fopen(temp, "a");
            currName = line;
            char timestamp[20] = "";
            strftime(timestamp, sizeof(timestamp), "%d/%m/%y %H:%M:%S", localtime(&(time_t){time(NULL)}));
            fprintf(logFile, "[%s] [Trash Can] [%s]\n", timestamp, line);
            fclose(file);
        } else if (strstr(line, "parkinglot") != NULL) {
            char temp[100];
            sprintf(temp, "microservices/database/%s", line);
            FILE *file = fopen(temp, "a");
            currName = line;
            char timestamp[20] = "";
            strftime(timestamp, sizeof(timestamp), "%d/%m/%y %H:%M:%S", localtime(&(time_t){time(NULL)}));
            fprintf(logFile, "[%s] [Parking Lot] [%s]\n", timestamp, line);
            fclose(file);
        } else {
            char path[1024];
            strcpy(path, "microservices/database/");
            strcat(path, currName);
            printf("%s = %s\n", path, line);
            FILE *file = fopen(path, "a");
            if (file != NULL) {
                fprintf(file, "%s\n", line);
                fclose(file);
            } else {
                printf("Failed to open file: %s\n", path);
            } 
        }

        line = strtok(NULL, "\n");
    }

    fclose(logFile);
    struct dirent *de;
    DIR *dr = opendir("new-data");
    if (dr == NULL) {
        printf("Gabisa dibuka ngab");
        return 0;
    }

    while ((de = readdir(dr)) != NULL) {
        char path[1024];
        snprintf(path, sizeof(path), "new-data/%s", de->d_name);
        if (remove(path) == 0) printf("%s dah diapus\n", de->d_name);
    }

    shmdt(str);

    return 0;
}