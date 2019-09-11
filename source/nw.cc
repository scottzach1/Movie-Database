#include "nw.hh"
#include "mt.hh"
#include <strings.h>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <sstream>

using namespace nwns;
using namespace mtns;
using namespace std;

#define PORT 12455

NetworkServer::NetworkServer() {
    // Initialise fields.
    fd = -1;
    mt = new MovieTable();

    // Setup Socket Address
    addr = new sockaddr_in();
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY;
    addr->sin_port = htons(PORT);

    activeClients.clear();

    // This updates fd.
    setupServer();
}

NetworkServer::~NetworkServer() { shutdownServer(0); }

void NetworkServer::loadMovieTable(const char * fname) {
    // Load csv
    bool r = mt->loadCSV(fname);

    cout << "Init Movie DB...\t";
    if (!r) {
        cout << "FAILED" << endl;
        // Kill process.
        shutdownServer(1);
    }
    else cout << "SUCCESS" << endl;
}

void NetworkServer::clearMovieTable() {
    if (mt != nullptr) mt->clear();
}

void NetworkServer::runServer() {
    if (mt->rows() == 0) {
        cout << "ERROR: no csv has been loaded." << endl;
        return;
    }

    while (true) {
        struct sockaddr_in * client_addr;
        socklen_t addrlen = sizeof(client_addr);

        // Listen
        if (doListen(fd) < 0) shutdownServer(1);

        // Accept
        int client_fd = doAccept(fd, (struct sockaddr *) &client_addr, addrlen);

        // Client Failure, kill connection.
        if (client_fd < 0) {
            close(client_fd);
            break;
        }
        // Fork the process.
        pid_t pid = fork();
        // Child Process, Serve Client.
        if (pid == 0) {
            serveClient(client_fd);
        }
        else if (pid < 0) { // Error Occurred, close connection.
            cout << "FORK FAILED";
            close(client_fd);
            shutdownServer(1);
            break;
        } else { // Parent Process, close client connection.
            close(client_fd);
        }
    }
}

void NetworkServer::setupServer() {
    cout << "Creating Socket...\t";
    // Attempt to create socket.
    fd = socket(AF_INET, SOCK_STREAM, 0);
    // Failure, exit process.
    if (fd == -1) {
        cout << "FAILED" << endl;
        shutdownServer(1);
    }
    cout << "SUCCESS" << endl;

    // Attempt to Bind socket.
    int r = doBind(fd, (struct sockaddr *) &addr, sizeof(addr));

    // Bind socket error, close client and exit.
    if (r != 0) shutdownServer(1);
}

void NetworkServer::serveClient(int client_fd) {
    // Save this client.
    activeClients.insert(client_fd);
    cout << "Serving Client: " << client_fd << endl;
    // Send initial message.
    char msg[] = "HELLO\n";
    ssize_t s = doSend(client_fd, msg, sizeof(msg), 0);

    // Failed to send msg, attempt to close connection, then kill child process.
    if (s == -1) {
        close(client_fd);
        activeClients.erase(client_fd);
        return; // Finished serving client.
    }

    // Continue Serving Client till "bye" or ERROR.
    while (true) {
        // Receive Msg
        bool quit = false;
        char incoming[100] = "";

        // Load received into incoming buffer.
        ssize_t r = doRecv(client_fd, incoming, 100, 0);
        if (r <= 0) break; // Error receiving from client.

        // Store incoming in a less primitive fashion.
        stringstream input(incoming);
        stringstream output;
        string cmd, args;

        input >> cmd >> args;

        // Case 1: msg starts with bye
        if (strncasecmp(cmd.c_str(), "bye", 3) == 0) {
            output << "GOODBYE";
            quit = true; // Send output, then break naturally.
        }
        // Case 2: msg starts with get.
        else if (strncasecmp(cmd.c_str(), "get", 3) == 0) {
            // Case 2.1: Nothing else was given, SHOWALL
            if (args.empty()) {
                output << mt->showAll();
            }
            // Case 2.2: Args were given: Attempt to retrieve number.
            else {
                unsigned long row = getNumber(args.c_str());
                // Case 2.2.1: Failed cast or invalid #
                if (row < 0)  output << "ERROR: Invalid Row #";
                // Case 2.2.2: Successful case & valid #.
                else            output << mt->show(row);
            }
        }
        // Case 3: Unrecognized command.
        else {
            output << "ERROR: Could not recognise that command";
        }

        // Send output to client.
        output << "\n";
        r = doSend(client_fd, output.str().c_str(), output.str().length(), 0);

        // Send failure or "bye", exit naturally.
        if (r < 0 || quit) break;
    }
    close(client_fd);
    // Forget this client.
    activeClients.erase(client_fd);
    cout << "Client " << client_fd << " closed" << endl;
}

int NetworkServer::doBind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    cout << "Binding Socket...\t";
    // Attempting bind.
    int r = bind(sockfd, addr, addrlen);
    // Prints success of bind and returns value.
    cout << ((r < 0) ? "FAILED" : "SUCCESS") << endl;
    cout << "Socket #: " << PORT << endl;
    return r;
}

int NetworkServer::doListen(int sockfd) {
    cout << "Listening...\t\t";
    // Attempting listen.
    int r = listen(sockfd, SOMAXCONN);
    // Prints success of listen and returns value.
    cout << ((r < 0) ? "FAILURE" : "RECEIVED") << endl;
    return r;
}

int NetworkServer::doAccept(int sockfd, struct sockaddr *client_addr, socklen_t client_addrlen) {

    // Attempting to accept.
    int client_fd = accept(sockfd, (struct sockaddr *)&client_addr, (socklen_t*)&client_addrlen);

    // If unsuccessful, print error to console.
    if (client_fd < 0) cout << "Error accepting connection" << endl;

    return client_fd;
}

ssize_t NetworkServer::doSend(int sockfd, const char *buf, size_t len, int flags) {

    // Attempting to send message.
    size_t r = send(sockfd, buf, len, flags);

    // If unsuccessful, print error to console.
    if (r < 0) cout << "Error sending message" << endl;

    return r;
}

ssize_t NetworkServer::doRecv(int sockfd, char *buf, size_t len, int flags) {

    // Attempting to recieve message.
    ssize_t r = recv(sockfd, buf, len, flags);

    // If unsuccessful, print error to console.
    if (r <= 0) {
        cout << "FAILED" << endl;
        cout << "Error receiving message from client " << sockfd << endl;
    }

    return r;
}

unsigned long NetworkServer::getNumber(const char *number) {
    try {
        // (1) cast test (catch exception)
        unsigned long row = stoul(number);
        // (2) is digit test
        for (int i=0; i<strlen(number); ++i) { // -1 avoids \n \r char
            if (!isdigit(number[i])) return -2;
        }
        // (3) valid row test
        if (row < 0 || row > mt->rows()) return -3;

        return row;
    } catch (...) { return -1; }
}

void NetworkServer::shutdownServer(int exitCode) {
    // For every client fd, close the connection.
    for (int cfd : activeClients) close(cfd);
    // Clear the set so that it can be auto destructed.
    activeClients.clear();
    // Close the local fd socket.
    if (fd != -1) close(fd);
    // Destruct the MovieDB.
    delete mt;
    // Delete addr ptr.
    delete addr;
    // Exit with status
    exit(exitCode);
}

int main() {
    auto *s = new NetworkServer();
    s->loadMovieTable("scifi.csv");
    s->runServer();
    s->clearMovieTable();
    delete s;
}