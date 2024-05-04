#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>

int main() {
    key_t key = ftok("shmfile",65);

    int shmid = shmget(key, 1024, 0666 | IPC_CREAT);

    // attach ke shared memory
    char *str = (char*) shmat(shmid,(void*)0,0);

    char *line = strtok(str, "\n");
    char res[1024] = "";
    int stop = 0;
    int curr = 0;
    char name[50];
    float rating = 0;
    while (line != NULL) {
        stop = 0;

        if (strcmp(line, "name, rating") != 0) {
            // printf("%s\n", line);
            if (strstr(line, "trashcan") != NULL) {
                if (curr != 0) {
                    stop = 1;
                    char iTemp[100];
                    sprintf(iTemp, "Name: %s\nRating: %.1f\n\n", name, rating);
                    strcat(res, iTemp);
                    rating = 0;
                }

                char temp[100];
                sprintf(temp, "Type: Trash Can\nFilename: %s\n-------------------\n", line);
                strcat(res, temp);                
            } else if (strstr(line, "parkinglot") != NULL) {
                if (curr != 0) {
                    stop = 1;
                    char iTemp[100];
                    sprintf(iTemp, "Name: %s\nRating: %.1f\n\n", name, rating);
                    strcat(res, iTemp);
                    rating = 0;
                }

                char temp[100];
                sprintf(temp, "Type: Parking Lot\nFilename: %s\n-------------------\n", line);
                strcat(res, temp);
            } else {
                if (!stop) {
                    char curName[50];
                    float curRating = rating;
                    sscanf(line, "%[^,], %f", curName, &curRating);
                    if (curRating > rating) {
                        rating = curRating;
                        strcpy(name, curName);
                    }

                    // printf("@Name: %s\nRating: %.1f\n", name, rating);
                }
            }
        }
        curr++;
        line = strtok(NULL, "\n");
    }
    
    printf("%s", res);
    printf("Name: %s\nRating: %.1f\n", name, rating);
    
    //detach from shared memory
    shmdt(str);

    return 0;
}