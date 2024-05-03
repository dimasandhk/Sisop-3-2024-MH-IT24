#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <string.h> 
#include <sys/wait.h>
#include <math.h>
#include <time.h>

int translateIndoToNum(char *word) {
    if (strcmp(word, "satu") == 0) {
        return 1;
    } else if (strcmp(word, "dua") == 0) {
        return 2;
    } else if (strcmp(word, "tiga") == 0) {
        return 3;
    } else if (strcmp(word, "empat") == 0) {
        return 4;
    } else if (strcmp(word, "lima") == 0) {
        return 5;
    } else if (strcmp(word, "enam") == 0) {
        return 6;
    } else if (strcmp(word, "tujuh") == 0) {
        return 7;
    } else if (strcmp(word, "delapan") == 0) {
        return 8;
    } else if (strcmp(word, "sembilan") == 0) {
        return 9;
    } else {
        return -1;
    }
}

char* translateNumToIndo(int num) {
    static char result[30];
    char* satuan[] = {"nol", "satu", "dua", "tiga", "empat", "lima", "enam", "tujuh", "delapan", "sembilan"};
    char* belasan[] = {"sebelas", "dua belas", "tiga belas", "empat belas", "lima belas", "enam belas", "tujuh belas", "delapan belas", "sembilan belas"};
    char* puluhan[] = {"sepuluh", "dua puluh", "tiga puluh", "empat puluh", "lima puluh", "enam puluh", "tujuh puluh", "delapan puluh", "sembilan puluh"};

    if (num < 10) {
        strcpy(result, satuan[num]);
    } else if (num < 20) {
        strcpy(result, belasan[num - 11]);
    } else {
        strcpy(result, puluhan[num / 10 - 1]);
        if (num % 10 != 0) {
            strcat(result, " ");
            strcat(result, satuan[num % 10]);
        }
    }

    return result;
}

int itung(int num1, int num2, char *operation) {
    double result;
    if (strcmp(operation, "-kali") == 0) {
        result = num1 * num2;
    } else if (strcmp(operation, "-tambah") == 0) {
        result = num1 + num2;
    } else if (strcmp(operation, "-kurang") == 0) {
        result = num1 - num2;
    } else if (strcmp(operation, "-bagi") == 0) {
        result = (double)num1 / num2;
    } else {
        printf("salah argumen. Use -kali, -tambah, -kurang, or -bagi.\n");
        return -1;
    }
    return (int)floor(result);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("harus ada argumennya banh, bisa -kali, -tambah, -kurang, -bagi\n");
        return 1;
    }

    if (strcmp(argv[1], "-kali") != 0 && strcmp(argv[1], "-tambah") != 0 && strcmp(argv[1], "-kurang") != 0 && strcmp(argv[1], "-bagi") != 0) {
        printf("Salah argumen. Use -kali, -tambah, -kurang, or -bagi.\n");
        return 1;
    }

    int fd1[2], fd2[2];
    pid_t p;

    if (pipe(fd1) == -1) {
        fprintf(stderr, "Pipe 1 Failed");
        return 1;
    }

    if (pipe(fd2) == -1) {
        fprintf(stderr, "Pipe 2 Failed");
        return 1;
    }

    char angka1[50];
    char angka2[50];
    scanf("%s %s", angka1, angka2);

    p = fork();

    if (p < 0) {
        fprintf(stderr, "Fork Failed");
        return 1;
    }

    if (p > 0) { // Process parent
        close(fd1[0]);
        int rilAngka1 = translateIndoToNum(angka1);
        int rilAngka2 = translateIndoToNum(angka2);
        int res = itung(rilAngka1, rilAngka2, argv[1]);

        if (res < 0) {
            printf("ERROR\n");
            res = -1;
        }

        write(fd1[1], &res, sizeof(res));
        close(fd1[1]);

        wait(NULL);

        // Mulai dapet dari child
        close(fd2[1]);

        char isiLog[100];
        read(fd2[0], &isiLog, sizeof(isiLog));
        printf("%s\n", isiLog);
        FILE *file = fopen("histori.log", "a");
        if (file == NULL) {
            printf("Error opening file!\n");
            return 1;
        }

        fprintf(file, "%s\n", isiLog);

        fclose(file);
        close(fd2[0]);
    } else { // Process child
        close(fd1[1]);
        int res;
        read(fd1[0], &res, sizeof(res));
        
        if (res != -1) {
            char *operation;
            if (strcmp(argv[1], "-kali") == 0) {
                operation = "perkalian";
            } else if (strcmp(argv[1], "-tambah") == 0) {
                operation = "penjumlahan";
            } else if (strcmp(argv[1], "-kurang") == 0) {
                operation = "pengurangan";
            } else if (strcmp(argv[1], "-bagi") == 0) {
                operation = "pembagian";
            } else {
                printf("salah argumen. Use -kali, -tambah, -kurang, or -bagi.\n");
                return 1;
            }

            char* hasil = translateNumToIndo(res);
            printf("hasil %s %s dan %s adalah %s\n", operation, angka1, angka2, hasil);
        } 

        close(fd1[0]);
        // Mulai kirim ke parent
        close(fd2[0]);

        char timestamp[20] = "";
        strftime(timestamp, sizeof(timestamp), "%d/%m/%y %H:%M:%S", localtime(&(time_t){time(NULL)}));
        char data[200] = "";
        if (strcmp(argv[1], "-kurang") == 0) {
            if (res < 0) {
                sprintf(data, "[%s] [KURANG] ERROR pada pengurangan.", timestamp);
            } else {
                char* hasil = translateNumToIndo(res);
                sprintf(data, "[%s] [KURANG] %s kurang %s sama dengan %s.", timestamp, angka1, angka2, hasil);
            }
        } else if (strcmp(argv[1], "-tambah") == 0) {
            char* hasil = translateNumToIndo(res);
            sprintf(data, "[%s] [TAMBAH] %s ditambah %s sama dengan %s.", timestamp, angka1, angka2, hasil);
        } else if (strcmp(argv[1], "-kali") == 0) {
            char* hasil = translateNumToIndo(res);
            sprintf(data, "[%s] [KALI] %s dikali %s sama dengan %s.", timestamp, angka1, angka2, hasil);
        } else if (strcmp(argv[1], "-bagi") == 0) {
            char* hasil = translateNumToIndo(res);
            sprintf(data, "[%s] [BAGI] %s dibagi %s sama dengan %s.", timestamp, angka1, angka2, hasil);
        }

        write(fd2[1], data, strlen(data) + 1);

        close(fd2[1]);
        exit(0);
    }

    return 0;
}

