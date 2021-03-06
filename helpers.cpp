#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <string.h>
#include <iostream>
#include "helpers.h"
#include "buffer.h"
#include "parson.h"

#define HEADER_TERMINATOR "\r\n\r\n"
#define HEADER_TERMINATOR_SIZE (sizeof(HEADER_TERMINATOR) - 1)
#define CONTENT_LENGTH "Content-Length: "
#define CONTENT_LENGTH_SIZE (sizeof(CONTENT_LENGTH) - 1)

using namespace std;

void error(const char *msg) {
    perror(msg);
    exit(0);
}

void compute_message(char *message, const char *line) {
    strcat(message, line);
    strcat(message, "\r\n");
}

int open_connection(char *host_ip, int portno, int ip_type,
                    int socket_type, int flag) {
    struct sockaddr_in serv_addr;
    int sockfd = socket(ip_type, socket_type, flag);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = ip_type;
    serv_addr.sin_port = htons(portno);
    inet_aton(host_ip, &serv_addr.sin_addr);
    /* connect the socket */
    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }
    return sockfd;
}

void close_connection(int sockfd) {
    close(sockfd);
}

void send_to_server(int sockfd, char *message) {
    int bytes, sent = 0;
    int total = strlen(message);
    do {
        bytes = write(sockfd, message + sent, total - sent);
        if (bytes < 0) {
            error("ERROR writing message to socket");
        }
        if (bytes == 0) {
            break;
        }
        sent += bytes;
    } while (sent < total);
}

char *receive_from_server(int sockfd) {
    char response[BUFLEN];
    buffer buffer = buffer_init();
    int header_end = 0;
    int content_length = 0;
    do {
        int bytes = read(sockfd, response, BUFLEN);
        if (bytes < 0) {
            error("ERROR reading response from socket");
        }
        if (bytes == 0) {
            break;
        }
        buffer_add(&buffer, response, (size_t) bytes);
        header_end = buffer_find(&buffer, HEADER_TERMINATOR,
                                HEADER_TERMINATOR_SIZE);
        if (header_end >= 0) {
            header_end += HEADER_TERMINATOR_SIZE;
            int content_length_start = buffer_find_insensitive(&buffer,
                                        CONTENT_LENGTH, CONTENT_LENGTH_SIZE);
            if (content_length_start < 0) {
                continue;
            }
            content_length_start += CONTENT_LENGTH_SIZE;
            content_length = strtol(buffer.data + content_length_start,
                                    NULL, 10);
            break;
        }
    } while (1);

    size_t total = content_length + (size_t) header_end;
    while (buffer.size < total) {
        int bytes = read(sockfd, response, BUFLEN);
        if (bytes < 0) {
            error("ERROR reading response from socket");
        }
        if (bytes == 0) {
            break;
        }
        buffer_add(&buffer, response, (size_t) bytes);
    }
    buffer_add(&buffer, "", 1);
    return buffer.data;
}

string find_json(string response) {
    // Checks for square brackets.
    size_t found_bracket;
    found_bracket = response.find("[");
    string res = response.substr(found_bracket, string::npos);
    return res;
}

char* parse_usr_pswd(string username, string password) {
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    json_object_set_string(root_object, "username", username.c_str());
    json_object_set_string(root_object, "password", password.c_str());
    char *res = json_serialize_to_string_pretty(root_value);
    return res;
}

char* parse_book_to_json(string title, string author,
                        string genre, string publisher, int pages) {
    JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    json_object_set_string(root_object, "title", title.c_str());
    json_object_set_string(root_object, "author", author.c_str());
    json_object_set_string(root_object, "genre", genre.c_str());
    json_object_set_string(root_object, "publisher", publisher.c_str());
    json_object_set_number(root_object, "page_count", pages);
    char *res = json_serialize_to_string_pretty(root_value);
    return res;
}

bool error_found(string response) {
    size_t found;
    found = response.find("error");
    return (found != string::npos);
}

string get_cookie(string response) {
    size_t found_cookie;
    size_t found_semicolon;
    size_t found_whitespace;
    // Finds cookie.
    found_cookie = response.find("Set-Cookie");
    response = response.substr(found_cookie, string::npos);
    found_whitespace = response.find(" ");
    response = response.substr(found_whitespace + 1, string::npos);
    // Where the cookie ends.
    found_semicolon = response.find(";");
    string cookie = response.substr(0, found_semicolon);
    return cookie;
}

string get_token(string response) {
    size_t found_token;
    size_t found_colon;
    // Finds token.
    found_token = response.find("token");
    response = response.substr(found_token, string::npos);
    found_colon = response.find(":");
    string token = response.substr(found_colon + 2, string::npos);
    token = token.substr(0, token.length() - 2);
    return token;
}
