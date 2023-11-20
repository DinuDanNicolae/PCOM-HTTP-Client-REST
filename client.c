#include <stdio.h>      
#include <stdlib.h>    
#include <unistd.h>     
#include <string.h>     
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>      
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"
#include "parson.c"


int main(int argc, char *argv[]) {

    // variabilele necesare pentru comunicarea cu serverul
    char *message; // ce trimiem
    char *response; // ce primim
    int sockfd; // socket-ul de comunicatie
    char *ip = "34.254.242.81"; // ip-ul serverului
    char *cookie = malloc(100 * sizeof(char)); // cookie-ul de la login
    char *token = malloc(200 * sizeof(char)); // token-ul de la enter_library

    // flags pentru verificare login si acces la biblioteca
    int flag_login = 0;
    int flag_enter_library = 0;

    while (1) {
        
        char *input = (char *) malloc(100 * sizeof(char));

        // citire comanda client
        fgets(input, 100, stdin);

        if (strncmp(input, "exit", 4) == 0) {
            break;
        }

        if (strncmp(input, "register", 8) == 0) { // verificare comanda register

            char *username = malloc(51 * sizeof(char));
            char *password = malloc(51 * sizeof(char));

            printf("username=");
            fgets(username, 50, stdin);
            username[strcspn(username, "\n")] = 0;

            printf("password=");
            fgets(password, 50, stdin);
            password[strcspn(password, "\n")] = 0;

            JSON_Value *root_value = json_value_init_object();
            JSON_Object *root_object = json_value_get_object(root_value);

            json_object_set_string(root_object, "username", username);
            json_object_set_string(root_object, "password", password);

            char *json_string = json_serialize_to_string_pretty(root_value);

            // creez mesajul de tip POST
            message = compute_post_request("34.254.242.81:8080", "/api/v1/tema/auth/register", 
            "application/json", json_string, 1, NULL, NULL, 0);

            // deschid conexiunea cu serverul
            sockfd = open_connection(ip, 8080, AF_INET, SOCK_STREAM, 0);

            // trimit mesajul creeat catre server
            send_to_server(sockfd, message);

            // primesc raspunsul de la server
            response = receive_from_server(sockfd);

            // parsam mesajul pentru a verifica daca exista eroare
            const char *jsonStart = strstr(response, "{");
            JSON_Value *root_value_status = json_parse_string(jsonStart);
           
            JSON_Object *root_object_status = json_value_get_object(root_value_status);
            const char *errorMessage = json_object_get_string(root_object_status, "error");

            if (errorMessage != NULL) {

                // in cazul unei errori afisez mesajul de eroare
                printf("Error: %s\n", errorMessage);

            } else {

                // altfel afisez mesajul de succes
                const char *statusCodeStart = strchr(response, ' ') + 1;
                int statusCode = atoi(statusCodeStart);

                if (statusCode >= 200 && statusCode < 300) {
                    printf("User registered successfully!\n");

                } else {
                    printf("Error: Unable to retrieve error message.\n");
                }
            }
            json_value_free(root_value_status);

        } else if (strncmp(input, "login", 5) == 0) { // verificare comanda login

            // urmeaza pasii similari de la register

            char *username = malloc(51 * sizeof(char));
            char *password = malloc(51 * sizeof(char));

            printf("username=");
            fgets(username, 50, stdin);
            username[strcspn(username, "\n")] = 0;

            printf("password=");
            fgets(password, 50, stdin);
            password[strcspn(password, "\n")] = 0;

            JSON_Value *root_value = json_value_init_object();
            JSON_Object *root_object = json_value_get_object(root_value);

            json_object_set_string(root_object, "username", username);
            json_object_set_string(root_object, "password", password);

            char *json_string = json_serialize_to_string_pretty(root_value);

            message = compute_post_request("34.254.242.81:8080", "/api/v1/tema/auth/login", 
            "application/json", json_string, 1, NULL, NULL, 0);

            sockfd = open_connection(ip, 8080, AF_INET, SOCK_STREAM, 0);

            send_to_server(sockfd, message);

            response = receive_from_server(sockfd);

            const char *jsonStart = strstr(response, "{");
            JSON_Value *root_value_status = json_parse_string(jsonStart);
           
            JSON_Object *root_object_status = json_value_get_object(root_value_status);
            const char *errorMessage = json_object_get_string(root_object_status, "error");

            if (errorMessage != NULL) {
                printf("Error: %s\n", errorMessage);

            } else {

                const char *statusCodeStart = strchr(response, ' ') + 1;
                int statusCode = atoi(statusCodeStart);

                if (statusCode >= 200 && statusCode < 300) {

                    // in cazul in care login-ul a fost cu succes, retin cookie-ul
                    char *ptr = strstr(response, "connect.sid=");
                    ptr += 12;
                    int i = 0;
                    while (ptr[i] != ';') {
                        cookie[i] = ptr[i];
                        i++;
                    }
                    cookie[i] = '\0';

                    if (cookie != NULL) {
                        flag_login = 1;
                        printf("Logged in successfully!\n");
                    }

                } else {
                    printf("Error: Unable to retrieve error message.\n");
                }
            }
            json_value_free(root_value_status);

        } else if (strncmp(input, "enter_library", 13) == 0) { // verificare comanda enter_library

            if (flag_login == 0) { // verific daca user-ul este deja logat

                printf("Error: You need to log in first!\n");
                continue;

            } else {

                // creez mesajul de tip GET si urmeaza pasii similari de la register/login

                message = compute_get_request("34.254.242.81:8080", "/api/v1/tema/library/access", NULL, cookie, NULL, 1);

                sockfd = open_connection(ip, 8080, AF_INET, SOCK_STREAM, 0);

                send_to_server(sockfd, message);

                response = receive_from_server(sockfd);

                const char *jsonStart = strstr(response, "{");
                JSON_Value *root_value_status = json_parse_string(jsonStart);
            
                JSON_Object *root_object_status = json_value_get_object(root_value_status);
                const char *errorMessage = json_object_get_string(root_object_status, "error");

                if (errorMessage != NULL) {
                    printf("Error: %s\n", errorMessage);
                } else {

                    const char *statusCodeStart = strchr(response, ' ') + 1;
                    int statusCode = atoi(statusCodeStart);

                    if (statusCode >= 200 && statusCode < 300) {
                        
                        // in cazul in care am primit raspunsul cu succes, retin token-ul
                        char *token_ptr = strstr(response, "token");
                        if (token_ptr == NULL) {
                            printf("Error: You dont have access!\n");
                        } else {
                            token_ptr += 8;
                            int i = 0;
                            while (token_ptr[i] != '"') {
                                token[i] = token_ptr[i];
                                i++;
                            }
                            token[i] = '\0';
                            printf("You have access to the library!\n");
                            flag_enter_library = 1;
                        }

                    } else {
                        printf("Error: Unable to retrieve error message.\n");
                    }
                }
                json_value_free(root_value_status);

            } 
        } else if (strncmp(input, "get_books", 9) == 0) { // verificare comanda get_books

            if (flag_enter_library == 0) { // verific daca user-ul a intrat in biblioteca

                printf("Error: You dont have access to the library!\n");
                printf("Please enter the library first!\n");
                continue;

            } else {

                // creez mesajul de tip GET si urmeaza pasii similari de la register/login
                message = compute_get_request("34.254.242.81:8080", "/api/v1/tema/library/books", NULL, NULL, token, 1);

                sockfd = open_connection(ip, 8080, AF_INET, SOCK_STREAM, 0);

                send_to_server(sockfd, message);

                response = receive_from_server(sockfd);

                const char *jsonStart = strstr(response, "{");
                JSON_Value *root_value_status = json_parse_string(jsonStart);
            
                JSON_Object *root_object_status = json_value_get_object(root_value_status);
                const char *errorMessage = json_object_get_string(root_object_status, "error");

                if (errorMessage != NULL) {
                    printf("Error: %s\n", errorMessage);
                } else {

                    const char *statusCodeStart = strchr(response, ' ') + 1;
                    int statusCode = atoi(statusCodeStart);

                    if (statusCode >= 200 && statusCode < 300) {
                        
                        // in cazul in care am primit raspunsul cu succes
                        // parsez raspunsul si afisez cartile

                        char *first = strstr(response, "[");
                        char *last = strstr(response, "]");
                        int len = last - first + 1;

                        char *to_print = malloc(len * sizeof(char));
                        strncpy(to_print, first, len);

                        JSON_Value *root_value = json_parse_string(to_print);
                        JSON_Array *books_array = json_value_get_array(root_value);

                        int num_books = json_array_get_count(books_array);

                        if (num_books == 0) { // verificare librarie goala

                            printf("Library empty!\n");
                            printf("Add some books!\n");

                        } else {

                            for (int i = 0; i < num_books; i++) { // iteram prin array-ul de carti
    
                                JSON_Object *book_obj = json_array_get_object(books_array, i);

                                // extragem id-ul si titlul cartii si le afisam
                                int id = (int)json_object_get_number(book_obj, "id");
                                const char *title = json_object_get_string(book_obj, "title");

                                printf("id=%d title=%s\n", id, title);
                            }
                        }

                    } else {
                        printf("Error: Unable to retrieve error message.\n");
                    }
                }
                json_value_free(root_value_status);

            }

        } else if (strncmp(input, "add_book", 8) == 0) { // verificare comanda add_book

            if (flag_enter_library == 0) { // verific daca user-ul a intrat in biblioteca

                printf("Error: You dont have access to the library!\n");
                printf("Please enter the library first!\n");
                continue;

            } else {

                // primim de la tastatura detaliile cartii
                // verificam daca sunt in formatul corect

                char *title = malloc(51 * sizeof(char));
                char *author = malloc(51 * sizeof(char));
                char *genre = malloc(51 * sizeof(char));
                char *publisher = malloc(51 * sizeof(char));
                int page_count;

                printf("title=");
                fgets(title, 50, stdin);

                if(strcmp(title, "") == 0) {
                    printf("Error: Title wrong format!\n");
                    printf("title=");
                    fgets(title, 50, stdin);
                    continue;
                }

                printf("author=");
                fgets(author, 50, stdin);

                if(strcmp(author, "") == 0) {
                    printf("Error: Author wrong format!\n");
                    printf("author=");
                    fgets(author, 50, stdin);
                    continue;
                }

                printf("genre=");
                fgets(genre, 50, stdin);

                if(strcmp(genre, "") == 0) {
                    printf("Error: Genre wrong format!\n");
                    printf("genre=");
                    fgets(genre, 50, stdin);
                    continue;
                }

                printf("publisher=");
                fgets(publisher, 50, stdin);

                if(strcmp(publisher, "") == 0) {
                    printf("Error: Publisher wrong format!\n");
                    printf("publisher=");
                    fgets(publisher, 50, stdin);
                    continue;
                }

                printf("page_count=");
                scanf("%d", &page_count);

                if(page_count < 1) {
                    printf("Error: Page count wrong format!\n");
                    printf("page_count=");
                    scanf("%d", &page_count);
                    continue;
                }

                char *pages_string = malloc(51 * sizeof(char));
                sprintf(pages_string, "%d", page_count);

                // scapam de \n de la sfarsitul fiecarei linii
                title[strcspn(title, "\n")] = 0;
                author[strcspn(author, "\n")] = 0;
                genre[strcspn(genre, "\n")] = 0;
                publisher[strcspn(publisher, "\n")] = 0;

                // construim json-ul pe care il vom trimite la server
                JSON_Value *root_value = json_value_init_object();
                JSON_Object *root_object = json_value_get_object(root_value);

                json_object_set_string(root_object, "title", title);
                json_object_set_string(root_object, "author", author);
                json_object_set_string(root_object, "genre", genre);
                json_object_set_string(root_object, "publisher", publisher);
                json_object_set_number(root_object, "page_count", page_count);

                char *json_string = json_serialize_to_string(root_value);

                // urmam pasii similari cu cei de la enter_library

                message = compute_post_request("34.254.242.81:8080", "/api/v1/tema/library/books", 
                "application/json", json_string, 1, NULL, token, 1);

                sockfd = open_connection(ip, 8080, AF_INET, SOCK_STREAM, 0);

                send_to_server(sockfd, message);

                response = receive_from_server(sockfd);

                const char *jsonStart = strstr(response, "{");
                JSON_Value *root_value_status = json_parse_string(jsonStart);
            
                JSON_Object *root_object_status = json_value_get_object(root_value_status);
                const char *errorMessage = json_object_get_string(root_object_status, "error");

                if (errorMessage != NULL) { // verificam daca am primit eroare
                    printf("Error: %s\n", errorMessage);
                } else {

                    const char *statusCodeStart = strchr(response, ' ') + 1;
                    int statusCode = atoi(statusCodeStart);

                    if (statusCode >= 200 && statusCode < 300) { 
                        // in caz de succes afisam mesajul corespunzator
                        printf("Book added!\n");

                    } else {
                        printf("Error: Unable to retrieve error message.\n");
                    }
                }
                json_value_free(root_value_status);

            }
        } else if(strncmp(input, "get_book", 8) == 0) { // verificare comanda get_book

            if (flag_enter_library == 0) { // verific daca user-ul a intrat in biblioteca

                printf("Error: You dont have access to the library!\n");
                printf("Please enter the library first!\n");
                continue;

            } else {

                // citim id-ul cartii

                char *id_string = malloc(51 * sizeof(char));
                int id;

                printf("id=");
                scanf("%d", &id);

                if(id < 1) {
                    printf("Error: Id wrong format!\n");
                    printf("id=");
                    scanf("%d", &id);
                    continue;
                }

                // construim ruta pe care o vom accesa
                // si trimitem request-ul

                char *route = malloc(100 * sizeof(char));
                memcpy(route, "/api/v1/tema/library/books/", 28);
                sprintf(id_string, "%d", id);
                strcat(route, id_string);

                message = compute_get_request("34.254.242.81:8080", route, NULL, NULL, token, 1);

                sockfd = open_connection(ip, 8080, AF_INET, SOCK_STREAM, 0);

                send_to_server(sockfd, message);

                response = receive_from_server(sockfd);

                const char *jsonStart = strstr(response, "{");
                JSON_Value *root_value_status = json_parse_string(jsonStart);
            
                JSON_Object *root_object_status = json_value_get_object(root_value_status);
                const char *errorMessage = json_object_get_string(root_object_status, "error");

                if (errorMessage != NULL) {
                    printf("Error: %s\n", errorMessage);
                } else {

                    const char *statusCodeStart = strchr(response, ' ') + 1;
                    int statusCode = atoi(statusCodeStart);

                    if (statusCode >= 200 && statusCode < 300) {

                        // in caz de succes parsam json-ul primit

                        char *first = strstr(response, "{");
                        char *last = strstr(response, "}");
                        int len = last - first + 1;

                        if (len < 0) { // verificam daca exista cartea

                            printf("Error: Book not found!\n");
                            continue;

                        } else {

                            // parsam detaliile despre cartea primita si le afisam

                            char *to_print = malloc(len * sizeof(char));
                            strncpy(to_print, first, len);

                            JSON_Value *root_value = json_parse_string(to_print);

                            const char *title = json_object_dotget_string(json_value_get_object(root_value), "title");
                            const char *author = json_object_dotget_string(json_value_get_object(root_value), "author");
                            const char *publisher = json_object_dotget_string(json_value_get_object(root_value), "publisher");
                            const char *genre = json_object_dotget_string(json_value_get_object(root_value), "genre");
                            int page_count = json_object_dotget_number(json_value_get_object(root_value), "page_count");

                            printf("title=%s\n", title);
                            printf("author=%s\n", author);
                            printf("publisher=%s\n", publisher);
                            printf("genre=%s\n", genre);
                            printf("page_count=%d\n", page_count);
                        }

                    } else {
                        printf("Error: Unable to retrieve error message.\n");
                    }
                }
                json_value_free(root_value_status);
            }   

        } else if(strncmp(input, "delete_book", 10) == 0) { // verificare comanda delete_book

            if (flag_enter_library == 0) { // verific daca user-ul a intrat in biblioteca
 
                printf("Error: You dont have access to the library!\n");
                printf("Please enter the library first!\n");
                continue;

            } else {

                // citim id-ul cartii

                char *id_string = malloc(51 * sizeof(char));
                int id;

                printf("id=");
                scanf("%d", &id);

                if(id < 1) {
                    printf("Error: Id wrong format!\n");
                    printf("id=");
                    scanf("%d", &id);
                    continue;
                }

                // urmam pasii de la comanda get_book

                char *route = malloc(100 * sizeof(char));
                memcpy(route, "/api/v1/tema/library/books/", 28);
                sprintf(id_string, "%d", id);
                strcat(route, id_string);

                message = compute_delete_request("34.254.242.81:8080", route, NULL, token);

                sockfd = open_connection(ip, 8080, AF_INET, SOCK_STREAM, 0);

                send_to_server(sockfd, message);

                response = receive_from_server(sockfd);

                const char *jsonStart = strstr(response, "{");
                JSON_Value *root_value_status = json_parse_string(jsonStart);
            
                JSON_Object *root_object_status = json_value_get_object(root_value_status);
                const char *errorMessage = json_object_get_string(root_object_status, "error");

                if (errorMessage != NULL) {
                    printf("Error: %s\n", errorMessage);
                } else {

                    const char *statusCodeStart = strchr(response, ' ') + 1;
                    int statusCode = atoi(statusCodeStart);

                    if (statusCode >= 200 && statusCode < 300) { 
                        // in caz de succes afisam mesajul corespunzator
                        printf("Book deleted!\n");

                    } else {
                        printf("Error: Unable to retrieve error message.\n");
                    }
                }
                json_value_free(root_value_status);
            }   

        } else if (strncmp(input, "logout", 6) == 0) { // verificare comanda logout

            if (flag_login == 0) { // verific daca user-ul este logat

                printf("Error: You are not logged in!\n");
                continue;

            } else {

                // urmam pasii de la comanda register/login

                message = compute_get_request("34.254.242.81:8080", "/api/v1/tema/auth/logout", 
                NULL, cookie, NULL, 1);

                sockfd = open_connection(ip, 8080, AF_INET, SOCK_STREAM, 0);

                send_to_server(sockfd, message);

                response = receive_from_server(sockfd);

                const char *jsonStart = strstr(response, "{");
                JSON_Value *root_value_status = json_parse_string(jsonStart);
            
                JSON_Object *root_object_status = json_value_get_object(root_value_status);
                const char *errorMessage = json_object_get_string(root_object_status, "error");

                if (errorMessage != NULL) {
                    printf("Error: %s\n", errorMessage);
                } else {

                    const char *statusCodeStart = strchr(response, ' ') + 1;
                    int statusCode = atoi(statusCodeStart);

                    if (statusCode >= 200 && statusCode < 300) {
                        
                        // in caz de succes afisam mesajul corespunzator si setam flag-urile pe 0
                        printf("Logout successful!\n");
                        flag_login = 0;
                        flag_enter_library = 0;

                    } else {
                        printf("Error: Unable to retrieve error message.\n");
                    }
                }
                json_value_free(root_value_status);
            }
        }    
    }

    return 0;

}