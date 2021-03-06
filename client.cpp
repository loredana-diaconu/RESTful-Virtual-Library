#include <stdio.h>
#include <stdlib.h>
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
#include "parson.h"

using namespace std;

// Executes the register command.
void execute_register(int &sockfd, string login_cookie) {
    // Checks whether someone is already logged in.
    if (login_cookie.length() != 0) {
        cout << "Please log out first.\n";
    } else {
        // Reads user input.
        string username;
        string password;
        cout << "username=";
        cin >> username;
        cout << "password=";
        cin >> password;

        // Composes the message, sends it to the server. Receives a response.
        sockfd = open_connection((char *)IP, PORT, AF_INET, SOCK_STREAM, 0);
        char *res = parse_usr_pswd(username, password);
        char *to_send = compute_post_request((char *)REGISTER, (char *)APPJSON,
                                            &res, 1, NULL, NULL);
        send_to_server(sockfd, to_send);
        string response(receive_from_server(sockfd));

        // Checks for errors in the response and prints a relevant message.
        if (error_found(response)) {
            cout << "Username already exists.\n";
        } else {
            cout << "Success.\n";
        }
    }
}

// Executes the login command.
void execute_login(int &sockfd, string &login_cookie) {
    // Similar to register.
    // Doesn't execute it if someone is already logged in.
    if (login_cookie.length() != 0) {
        cout << "Somebody is already logged in.\n";
    } else {
        // Reads user input.
        string username;
        string password;
        cout << "username=";
        cin >> username;
        cout << "password=";
        cin >> password;

        // Server communication.
        sockfd = open_connection((char *)IP, PORT, AF_INET, SOCK_STREAM, 0);
        char *res = parse_usr_pswd(username, password);
        char *to_send = compute_post_request((char *)LOGIN, (char *)APPJSON,
                                            &res, 1, NULL, NULL);
        send_to_server(sockfd, to_send);
        string response(receive_from_server(sockfd));

        // Checks for errors.
        if (error_found(response)) {
            cout << "Incorrect username or password.\n";
        } else {
            cout << "Success.\n";
            // Stores login cookie.
            login_cookie = get_cookie(response);
        }
    }
}

// Executes the enter_library command.
void enter_library(int &sockfd, string login_cookie, string &access_token) {
    // Only if a user is logged in.
    if (login_cookie.length() == 0) {
        cout << "You are not logged in.\n";
    } else {
        // Server communication.
        sockfd = open_connection((char *)IP, PORT, AF_INET, SOCK_STREAM, 0);
        char *to_send = compute_get_request((char *)ENTER,
                                           (char *)login_cookie.c_str(), NULL);
        send_to_server(sockfd, to_send);
        string response(receive_from_server(sockfd));

        // Checks for errors (if any).
        if (error_found(response)) {
            cout << "Error.\n";
        } else {
            cout << "Entered library.\n";
            // Stores token.
            access_token = get_token(response);
        }
    }
}

// Executes the get_books command.
void get_books(int &sockfd, string access_token) {
    // Checks for access to the library.
    if (access_token.length() == 0) {
        cout << "Please enter the library.\n";
    } else {
        sockfd = open_connection((char *)IP, PORT, AF_INET, SOCK_STREAM, 0);
        char *to_send = compute_get_request((char *)BOOKS, NULL,
                                           (char *)access_token.c_str());
        send_to_server(sockfd, to_send);
        string response(receive_from_server(sockfd));

        // Checks for errors.
        if (error_found(response)) {
            cout << "Invalid token.\n";
        } else {
            // Prints the requested info.
            cout << find_json(response) << endl;
        }
    }
}

// Executes the get_book command.
void get_book(int &sockfd, string access_token) {
    // Checks for access to the library.
    if (access_token.length() == 0) {
        cout << "Please enter the library.\n";
    } else {
        // Requests ID.
        string id;
        cout << "id=";
        cin >> id;

        // Creates the url for the GET request.
        string url(BOOKS);
        url.append("/").append(id);
        sockfd = open_connection((char *)IP, PORT, AF_INET, SOCK_STREAM, 0);
        char *to_send = compute_get_request((char *)url.c_str(), NULL,
                                           (char *)access_token.c_str());
        send_to_server(sockfd, to_send);
        string response(receive_from_server(sockfd));

        // Checks whether the book was found or not.
        if (error_found(response)) {
            cout << "Could not find book.\n";
        } else {
            // Prints book info (after finding it in the response).
            cout << find_json(response) << endl;
        }
    }
}

// Executes the add_book command.
void add_book(int &sockfd, string access_token) {
    // Checks for access to the library.
    if (access_token.length() == 0) {
        cout << "Please enter the library.\n";
    } else {
        string title;
        string author;
        string genre;
        string publisher;
        string pg;
        int pages;

        // Requests book info.
        cout << "title=";
        cin.ignore();
        getline(cin, title);
        cout << "author=";
        getline(cin, author);
        cout << "genre=";
        getline(cin, genre);
        cout << "publisher=";
        getline(cin, publisher);

        while (1) {
            // Makes sure page_count is an integer.
            // Reads from stdin until it gets a valid input.
            cout << "page_count=";
            getline(cin, pg);
            int ok = 1;
            for (unsigned int i = 0; i < pg.length(); i++) {
                if (!isdigit(pg.at(i))) {
                    // The user is informed that the field needs to be a number
                    ok = 0;
                    cout << "Please enter a number.\n";
                    break;
                }
            }
            if (ok) {
                break;
            }
        }

        pages = stoi(pg);
        sockfd = open_connection((char *)IP, PORT, AF_INET, SOCK_STREAM, 0);

        char *res = parse_book_to_json(title, author, genre, publisher, pages);
        char *to_send = compute_post_request((char *)BOOKS, (char *)APPJSON,
                                            &res, 1, NULL,
                                            (char *)access_token.c_str());
        send_to_server(sockfd, to_send);
        string response(receive_from_server(sockfd));

        // Checks for errors.
        if (error_found(response)) {
            cout << "Could not add book.\n";
        } else {
            cout << "Book added successfully.\n";
        }
    }
}

// Executes the delete_book command.
void delete_book(int &sockfd, string access_token) {
    // Checks for access to the library.
    if (access_token.length() == 0) {
        cout << "Please enter the library.\n";
    } else {
        // Requests book ID.
        string id;
        cout << "id=";
        cin >> id;
        string url(BOOKS);
        url.append("/").append(id);
        sockfd = open_connection((char *)IP, PORT, AF_INET, SOCK_STREAM, 0);
        char *to_send = compute_delete_request((char *)url.c_str(), NULL,
                                              (char *)access_token.c_str());
        send_to_server(sockfd, to_send);
        string response(receive_from_server(sockfd));
        // Checks whether the book was found or not.
        if (error_found(response)) {
            cout << "Could not find book.\n";
        } else {
            cout << "Book deleted successfully.\n";
        }
    }
}

// Executes the logout command.
void execute_logout(int &sockfd, string &login_cookie, string &access_token) {
    sockfd = open_connection((char *)IP, PORT, AF_INET, SOCK_STREAM, 0);
    char *to_send = compute_get_request((char *)LOGOUT,
                                       (char *)login_cookie.c_str(), NULL);
    send_to_server(sockfd, to_send);
    string response(receive_from_server(sockfd));
    // Checks for errors (e.g. no one is logged in).
    if (error_found(response)) {
        cout << "Not logged in.\n";
    } else {
        // Clears login cookie and access token.
        login_cookie.clear();
        access_token.clear();
        cout << "Logout successful.\n";
    }
}

// Manages commands. Stores cookies and tokens for the current user.
int main(int argc, char *argv[]) {
    int sockfd;
    string command;
    string login_cookie;
    string access_token;

    // Reads commands from stdin until it receives "exit" and executes them.
    while (1) {
        cin >> command;
        if (command == "exit") {
            break;
        }
        if (command == "register") {
            execute_register(sockfd, login_cookie);
        } else {
            if (command == "login") {
                execute_login(sockfd, login_cookie);
            } else {
                if (command == "enter_library") {
                    enter_library(sockfd, login_cookie, access_token);
                } else {
                    if (command == "get_books") {
                        get_books(sockfd, access_token);
                    } else {
                        if (command == "get_book") {
                            get_book(sockfd, access_token);
                        } else {
                            if (command == "add_book") {
                                add_book(sockfd, access_token);
                            } else {
                                if (command == "delete_book") {
                                    delete_book(sockfd, access_token);
                                } else {
                                    if (command == "logout") {
                                        execute_logout(sockfd, login_cookie,
                                                       access_token);
                                    } else {
                                        cout << "Command not found.\n";
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}
