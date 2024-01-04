#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <chrono>
#include "user_Cache.h"
#include "q_a.h"

using namespace std;

char socket_path[] = "/tmp/tmp1";

int main(int argc, char* argv[]) {
    struct sockaddr_un addr;
    char buf[100];
    int fd, rc;
    bool isRunning = true;

    user_Cache users_cache;

    memset(&addr, 0, sizeof(addr));
    //Create the socket
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        cout << "client1: " << strerror(errno) << endl;
        exit(-1);
    }

    addr.sun_family = AF_UNIX;
    //Set the socket path to a local socket file
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);
    cout << "client1: addr.sun_path: " << addr.sun_path << endl;

    cout << "client1: connect()" << endl;
    //Connect to the local socket
    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        cout << "client1: " << strerror(errno) << endl;
        close(fd);
        exit(-1);
    }

    cout << "TYPE NOW:" << endl;
    //Read from the standard input (keyboard)
    while (isRunning) {
        bzero(buf, 100);
        rc = read(STDIN_FILENO, buf, sizeof(buf));
        //start timer on request   
        auto start = std::chrono::high_resolution_clock::now();

        //collect data for cache
        string ques = buf;

        //check if search has been done in cache
        q_a rep;

        //BYPASS cache if SUM,COUNT or ERRORS is requested
        if (ques.find("SUM") == std::string::npos && ques.find("COUNT") == std::string::npos && ques.find("ERRORS") == std::string::npos)
        {
            rep = users_cache.SearchCache(buf);
        }

        if (rep.is_empty() == false)
        {
            //service request from cache
            string ans = rep.getAnswer();
            cout << "CACHE: The answer is: " << ans << endl;
            bzero(buf, 100);
            //end timer
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> duration = end - start;
            cout << "time for operation: " << duration.count() << "s " << endl;
        }
        else
        {
            if (strncmp("CLEAR CACHE", buf, 11) == 0)
            {
                users_cache.clearCache();
                cout << "Cache is now empty" << endl;
                bzero(buf, 100);
            }
            else
            {
                //Write to the socket
                if (write(fd, buf, rc) != rc) {
                    if (rc > 0) fprintf(stderr, "partial write");
                    else {
                        cout << "client1: " << strerror(errno) << endl;
                        close(fd);
                        exit(-1);
                    }
                }
                int n = 0;



                bzero(buf, 100);
                n = read(fd, buf, 100);
                cout << "SERVER: The result is: " << buf << endl;

                q_a reply(ques, buf);

                //store in cache
                users_cache.storeToCache(reply);

                //end timer
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<float> duration = end - start;
                cout << "time for operation: " << duration.count() << "s " << endl;

                if (strncmp("QUIT", buf, 4) == 0) {
                    cout << "ending connection" << endl;
                    isRunning = false;
                }
            }

        }

    }

    cout << "client1: close(fd)" << endl;
    close(fd);
    return 0;
}