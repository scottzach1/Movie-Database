#ifndef _nw_hh_
#define _nw_hh_

#include "mt.hh"
#include <netinet/in.h>
#include <sys/socket.h>
#include <cstdlib>
#include <vector>
#include <set>

using namespace std;
using namespace mtns;

/** Network Server Name Space */
namespace nwns {

    /**
     * Network Server, accepts forking over multiple connections.
     * This network server can be interacted with via the terminal
     * The network server is connected to a Move Database.
     * There are 3 commands that the server can accept:
     * * GET -> Sends a message containing all rows of the database
     * * GET %d -> Sends a message containing the row %d of the database.
     * * BYE -> Closes the connection with the client.
     * * If an invalid command is called, the server will send a message
     * - ERROR: (error message) and wait till further instruction from the
     * - client.
     */
    class NetworkServer {

    public:
        /**
         * Create a new instance of Network Server
         * in a dormant state.
         */
        NetworkServer();

        /**
         * Destructs the Network server and all of its
         * fields. Will exit with code 0.
         */
        ~NetworkServer();

        /**
         * Loads a csv file to the database,
         * (You can load multiple to the db)
         * @param fname name of file to load.
         */
        void loadMovieTable(const char * fname);

        /**
         * Clears all the rows in the movie table.
         */
        void clearMovieTable();

        /**
         * Starts the server and begins trying to establish a connection.
         * You can see the status of the Server via the terminal on the
         * host computer.
         */
        void runServer();

    private:
        /**
         * Sets up the server such that it is ready to start listening for new
         * client connections.
         */
        void setupServer();

        /**
         * Serves a client with the fd of param
         * @param client_fd fd
         */
        void serveClient(int client_fd);

        /**
         * Attempts to bind socket on port PORT.
         * (Defined in implementation).
         * @param sockfd fd of socket to bind.
         * @param addr tcp address to use.
         * @param addrlen length of tcp address struct.
         * @return status of bind. (< 0 error code)
         */
        int static doBind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

        /**
         * Listen for connections on param
         * @param sockfd fd.
         * @return status (< 0 error code)
         */
        int doListen(int sockfd);

        /**
         * Accept a connection on param
         * @param sockfd fd
         * @param client_addr clients tcp address
         * @param client_addrlen length of tcp adress struct.
         * @return (< 0 error code)
         */
        int doAccept(int sockfd, struct sockaddr *client_addr, socklen_t client_addrlen);

        /**
         * Send a message to the client with fd param
         * @param sockfd fd
         * @param buf message to send.
         * @param len length of the message.
         * @param flags optional flags to send.
         * @return if successful, number of chars sent.
         */
        ssize_t doSend(int sockfd, const char *buf, size_t len, int flags);

        /**
         * Receive a message
         * @param sockfd fd receiving end.
         * @param buf message buffer to store.
         * @param len lis the length of buffer to be sent
         * @param flags optional flags to send.
         * @return if successful, number of characters received.
         */
        ssize_t doRecv(int sockfd, char *buf, size_t len, int flags);

        /**
         * Get a unsigned long from a string.
         * @param number to convert.
         * @return converted number.
         */
        unsigned long getNumber(const char * number);
        
        /**
         * Shuts destructs all fields and closes all sockets / connections.
         * Exits program with exitcode
         * @param exitCode integer
         */
        void shutdownServer(int exitCode);

        /** Fields to store locally. */

        // Local fd
        int fd;
        // Local socket address
        struct sockaddr_in * addr;
        // Local movie table
        MovieTable *mt;
        // All the active client fd's.
        set<int> activeClients;
    };
}

#endif