#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define SERVER_ADDRESS "127.0.0.1"
#define PORT 8080
#define MAX_COMMAND_LENGTH 100
#define MAX_BUFFER_SIZE 1024
#define MAX_TITLE_LENGTH 100
#define MAX_GENRE_LENGTH 50
#define MAX_DAY_LENGTH 20
#define MAX_STATUS_LENGTH 20
#define MAX_LINE_LENGTH 255

void log_change(const char *type, const char *message) {
    time_t now;
    struct tm *local_time;
    char time_str[20];
    time(&now);
    local_time = localtime(&now);
    strftime(time_str, sizeof(time_str), "%d/%m/%y", local_time);

    FILE *fp = fopen("change.log", "a");
    if (fp != NULL) {
        fprintf(fp, "[%s] [%s] %s\n", time_str, type, message);
        fclose(fp);
    }
}

void send_response(int client_socket, const char *response) {
    send(client_socket, response, strlen(response), 0);
}

void read_csv_file(char *filename, char *result) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        strcat(result, line);
    }
    fclose(file);
}

void handle_show_all(int client_socket) {
    char result[MAX_BUFFER_SIZE] = {0};
    read_csv_file("myanimelist.csv", result);
    send_response(client_socket, result);
}

void handle_show_genre(char *genre, int client_socket) {
    char result[MAX_BUFFER_SIZE] = {0};
    FILE *file = fopen("myanimelist.csv", "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, genre) != NULL) {
            strcat(result, line);
        }
    }
    fclose(file);
    send_response(client_socket, result);
}

void handle_show_day(char *day, int client_socket) {
    char result[MAX_BUFFER_SIZE] = {0};
    FILE *file = fopen("myanimelist.csv", "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, day) != NULL) {
            strcat(result, line);
        }
    }
    fclose(file);
    send_response(client_socket, result);
}

void handle_show_status(char *title, int client_socket) {
    char result[MAX_BUFFER_SIZE] = {0};
    FILE *file = fopen("myanimelist.csv", "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",");
        if (strcmp(token, title) == 0) {
            token = strtok(NULL, ",");
            strcat(result, token);
            break;
        }
    }
    fclose(file);
    send_response(client_socket, result);
}

void handle_add_anime(char *anime_info, int client_socket) {
    FILE *file = fopen("myanimelist.csv", "a");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    fputs(anime_info, file);
    fclose(file);
    send_response(client_socket, "Anime added successfully.");
    log_change("ADD", "Anime added.");
}

void handle_edit_anime(char *anime_info, int client_socket) {
    char temp_file[] = "temp.csv";
    FILE *file = fopen("myanimelist.csv", "r");
    FILE *temp = fopen(temp_file, "w");
    if (file == NULL || temp == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",");
        if (strcmp(token, anime_info) == 0) {
            fputs(anime_info, temp);
        } else {
            fputs(line, temp);
        }
    }
    fclose(file);
    fclose(temp);
    remove("myanimelist.csv");
    rename(temp_file, "myanimelist.csv");
    send_response(client_socket, "Anime edited successfully.");
    log_change("EDIT", "Anime edited.");
}

void handle_delete_anime(char *title, int client_socket) {
    char temp_file[] = "temp.csv";
    FILE *file = fopen("myanimelist.csv", "r");
    FILE *temp = fopen(temp_file, "w");
    if (file == NULL || temp == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",");
        if (strcmp(token, title) != 0) {
            fputs(line, temp);
        }
    }
    fclose(file);
    fclose(temp);
    remove("myanimelist.csv");
    rename(temp_file, "myanimelist.csv");
    send_response(client_socket, "Anime deleted successfully.");
    log_change("DEL", "Anime deleted.");
}

void handle_command(char *command, int client_socket) {
    char response[MAX_BUFFER_SIZE] = {0};

    if (strncmp(command, "SHOW ALL", 8) == 0) {
        handle_show_all(client_socket);
    } else if (strncmp(command, "SHOW GENRE", 10) == 0) {
        char *genre = strtok(command, " ");
        genre = strtok(NULL, " ");
        handle_show_genre(genre, client_socket);
    } else if (strncmp(command, "SHOW DAY", 8) == 0) {
        char *day = strtok(command, " ");
        day = strtok(NULL, " ");
        handle_show_day(day, client_socket);
    } else if (strncmp(command, "SHOW STATUS", 11) == 0) {
        char *title = strtok(command, " ");
        title = strtok(NULL, " ");
        handle_show_status(title, client_socket);
    } else if (strncmp(command, "ADD", 3) == 0) {
        char *anime_info = strtok(command, " ");
        anime_info = strtok(NULL, "\n");
        handle_add_anime(anime_info, client_socket);
    } else if (strncmp(command, "EDIT", 4) == 0) {
        char *anime_info = strtok(command, " ");
        anime_info = strtok(NULL, "\n");
        handle_edit_anime(anime_info, client_socket);
    } else if (strncmp(command, "DELETE", 6) == 0) {
        char *title = strtok(command, " ");
        title = strtok(NULL, "\n");
        handle_delete_anime(title, client_socket);
    } else if (strncmp(command, "exit", 4) == 0) {
        snprintf(response, sizeof(response), "Exiting program...");
        send_response(client_socket, response);
    } else {
        snprintf(response, sizeof(response), "Invalid Command");
        send_response(client_socket, response);
    }
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[MAX_BUFFER_SIZE] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        if (recv(new_socket, buffer, sizeof(buffer), 0) <= 0) {
            break;
        }
        handle_command(buffer, new_socket);
        if (strncmp(buffer, "exit", 4) == 0) {
            break;
        }
    }
    close(new_socket);
    close(server_fd);
    return 0;
}
