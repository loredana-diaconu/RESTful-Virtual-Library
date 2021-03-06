#ifndef _HELPERS_
#define _HELPERS_

using namespace std;

#define BUFLEN 4096
#define LINELEN 1000
#define SERVER_ADDRESS "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com"
#define REGISTER "/api/v1/tema/auth/register"
#define LOGIN "/api/v1/tema/auth/login"
#define APPJSON "application/json"
#define ENTER "/api/v1/tema/library/access"
#define BOOKS "/api/v1/tema/library/books"
#define LOGOUT "/api/v1/tema/auth/logout"
#define IP "3.8.116.10"
#define PORT 8080

// Current error.
void error(const char *msg);

// Adds a line to a string message.
void compute_message(char *message, const char *line);

// Opens a connection with server host_ip on port portno, returns a socket.
int open_connection(char *host_ip, int portno, int ip_type,
                    int socket_type, int flag);

// Closes a server connection on socket sockfd.
void close_connection(int sockfd);

// Send a message to a server.
void send_to_server(int sockfd, char *message);

// Receives and returns the message from a server.
char *receive_from_server(int sockfd);

// Extracts and returns a JSON from a server response.
string find_json(string response);

// Gets user data in JSON format.
char* parse_usr_pswd(string username, string password);

// Gets book data in JSON format.
char* parse_book_to_json(string title, string author, string genre,
                        string publisher, int pages);

// Checks for errors in a server response.
bool error_found(string response);

// Extracts cookie from server response.
string get_cookie(string response);

// Extracts token from server response.
string get_token(string response);

#endif
