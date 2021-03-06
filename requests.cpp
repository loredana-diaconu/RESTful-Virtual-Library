#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <string.h>
#include <iostream>
#include "helpers.h"
#include "requests.h"

#define HOST "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com"

using namespace std;

char *compute_get_request(char *url, char *cookie, char *token) {
    char *message = (char *)calloc(BUFLEN, sizeof(char));
    char *line = (char *)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "GET %s HTTP/1.1", url);
    compute_message(message, line);

    // Step 2: add the host
    memset(line, 0, LINELEN);
    sprintf(line, "HOST: %s", HOST);
    compute_message(message, line);

    // Step 3: add headers and/or cookies, according to the protocol format
    if (cookie != NULL) {
       memset(line, 0, LINELEN);
       sprintf(line, "Cookie: ");
       strcat(line, cookie);
       compute_message(message, line);
    }

    if (token != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer ");
        strcat(line, token);
        compute_message(message, line);
    }

    // Step 4: add final new line
    compute_message(message, "");
    return message;
}

char *compute_post_request(char *url, char* content_type, char **body_data,
                            int body_data_fields_count, char *cookie,
                            char *token) {
    char *message = (char *)calloc(BUFLEN, sizeof(char));
    char *line = (char *)calloc(LINELEN, sizeof(char));
    char *body_data_buffer = (char *)calloc(LINELEN, sizeof(char));
    long int data_size = 0;

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // Step 2: add the host
    memset(line, 0, LINELEN);
    sprintf(line, "HOST: %s", HOST);
    compute_message(message, line);

    /* Step 3: add necessary headers
       in order to write Content-Length you must first compute the message size
    */
    memset(line, 0, LINELEN);
    string cont_type("Content-Type: ");
    string type(content_type);
    cont_type.append(type);
    strcpy(line, cont_type.c_str());
    compute_message(message, line);

    int i;
    for (i = 0; i < body_data_fields_count - 1; i++) {
        strcat(body_data_buffer, body_data[i]);
        strcat(body_data_buffer, "&");
        data_size += strlen(body_data[i]) + 1;
    }
    strcat(body_data_buffer, body_data[i]);
    data_size += strlen(body_data[i]);

    memset(line, 0, LINELEN);
    sprintf(line, "Content-Length: %ld", data_size);
    compute_message(message, line);

    // Step 4 (optional): add cookies
    if (cookie != NULL) {
       memset(line, 0, LINELEN);
       sprintf(line, "Cookie: ");
       strcat(line, cookie);
       compute_message(message, line);
    }

    // Add Token
    if (token != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer ");
        strcat(line, token);
        compute_message(message, line);
    }

    // Step 5: add new line at end of header
    memset(line, 0, LINELEN);
    compute_message(message, line);

    // Step 6: add the actual payload data
    memset(line, 0, LINELEN);
    compute_message(message, body_data_buffer);

    free(line);
    return message;
}

char *compute_delete_request(char *url, char *cookie, char *token) {
    char *message = (char *)calloc(BUFLEN, sizeof(char));
    char *line = (char *)calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "DELETE %s HTTP/1.1", url);
    compute_message(message, line);

    // Step 2: add the host
    memset(line, 0, LINELEN);
    sprintf(line, "HOST: %s", HOST);
    compute_message(message, line);

    // Step 3: add headers and/or cookies, according to the protocol format
    if (token != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Authorization: Bearer ");
        strcat(line, token);
        compute_message(message, line);
    }

    // Step 4: add final new line
    compute_message(message, "");
    return message;
}
