#include <stdlib.h>    
#include <stdio.h>
#include <unistd.h>     
#include <string.h>     
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>      
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

// Functiile sunt preluate din laborator avand mici modificari necesare

char *compute_get_request(char *host, char *url, char *query_params,
                            char *cookies, char *token, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message, line);


    if (cookies != NULL) { // verificare cookie 
        sprintf(line, "Cookie: connect.sid=%s", cookies); // adaugare cookie
        compute_message(message, line);
    }

    if (token != NULL) { // verificare token
        sprintf(line, "Authorization: Bearer %s", token); // adaugare token
        compute_message(message, line);
    }

    compute_message(message, "");
    return message;
}

char *compute_post_request(char *host, char *url, char* content_type, char *body_data,
                            int body_data_fields_count, char **cookies, char *token, int cookies_count)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    sprintf(line, "Host: %s", host);
    compute_message(message, line);
    
    sprintf(line, "Content-Length: %ld", strlen(body_data));
    compute_message(message, line);

    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

   if (token != NULL) { // verificare token
        sprintf(line, "Authorization: Bearer %s", token); // adaugare token
        compute_message(message, line);
    }

    compute_message(message, "");

    memset(line, 0, LINELEN);
    
    strcat(message, body_data);

    free(line);
    return message;
}

// Functie de delete request

char *compute_delete_request(const char *host, const char *url, char *query_params, char *token) {

    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    if(query_params != NULL) { // parametrii de query
        sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }

    compute_message(message, line);

    sprintf(line, "Host: %s", host); // host
    compute_message(message, line);

    if (token != NULL) { // verificare token
        sprintf(line, "Authorization: Bearer %s", token); // adaugare token
        compute_message(message, line);
    }

    compute_message(message, "");
    return message;
}
