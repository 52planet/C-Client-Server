#include <iostream>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

//state variables
fstream storage_file;
int COUNT = 0;
int ERRORS = 0;
double SUM = 0;

//
int fd, cl, rc;
char buf[100];
char buffer[50];

//
pthread_mutex_t lock_x;

void* thread_func(void* arg);
bool digits_only(const std::string& str);
void GetData();
void updateData();

char socket_path[] = "/tmp/tmp1";

int main(int argc, char* argv[]) {


    //connection variables
    struct sockaddr_un addr;


    bool isRunning = true;
    pid_t childpid;
    int client;


    memset(&addr, 0, sizeof(addr));
    //Create the socket
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        cout << "server: " << strerror(errno) << endl;
        exit(-1);
    }

    addr.sun_family = AF_UNIX;

    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);
    cout << "server: addr.sun_path:" << addr.sun_path << endl;
    unlink(socket_path);

    cout << "server: bind()" << endl;

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        cout << "server: " << strerror(errno) << endl;
        close(fd);
        exit(-1);
    }

    cout << "server: listen()" << endl;

    if (listen(fd, 5) == -1) {
        cout << "server: " << strerror(errno) << endl;
        unlink(socket_path);
        close(fd);
        exit(-1);
    }

    while (isRunning) {
        cout << "server: accept()" << endl;


        if ((cl = accept(fd, NULL, NULL)) == -1) {
            cout << "server: " << strerror(errno) << endl;
            unlink(socket_path);
            close(fd);
            exit(-1);
        }

        if ((childpid = fork()) == 0) {

            while (1)
            {
                GetData();
                int n = 0;
                recv(cl, buffer, 50, 0);


                //Continue reading until the client sends "QUIT" 
                if (strncmp("QUIT", buffer, 4) == 0) {
                    cout << "connection ended" << endl;
                    snprintf(buffer, 50, "QUIT");

                    int n = write(cl, buffer, 50);
                    if (n < 0)
                    {
                        cout << "failure" << endl;
                    }
                    bzero(buffer, sizeof(buffer));
                    break;
                }

                else if (strncmp("SUM", buffer, 3) == 0) {
                    snprintf(buffer, 50, "%lf", SUM);
                    n = write(cl, buffer, 50);
                    if (n < 0)
                    {
                        cout << "failure" << endl;
                    }
                    bzero(buffer, sizeof(buffer));
                }
                else if (strncmp("COUNT", buffer, 5) == 0) {
                    snprintf(buffer, 50, "%d", COUNT);

                    n = write(cl, buffer, 50);
                    if (n < 0)
                    {
                        cout << "failure" << endl;
                    }
                    bzero(buffer, sizeof(buffer));
                }
                else if (strncmp("ERRORS", buffer, 6) == 0) {
                    snprintf(buffer, 50, "%d", ERRORS);

                    n = write(cl, buffer, 50);
                    if (n < 0)
                    {
                        cout << "failure" << endl;
                    }
                    bzero(buffer, sizeof(buffer));
                }

                else
                {
                    //Start thread
                    pthread_t tid;

                    int ret = pthread_create(&tid, NULL, thread_func, NULL);
                    if (ret != 0) {
                        cout << "Cannot create thread" << endl;
                        close(fd);
                        return -1;
                    }
                    else
                    {
                        //update data file
                        pthread_join(tid, NULL);

                        updateData();
                        GetData();

                    }

                }
            }
        }
    }


    if (rc < 0) {
        //Cannot read from this local socket file
        cout << "server: " << strerror(errno) << endl;
        unlink(socket_path);
        close(fd);
        close(cl);
        exit(-1);
    }
    else if (rc == 0) {
        //Clean up
        cout << "EOF" << endl;
        unlink(socket_path);
        close(fd);
        close(cl);
    }

    cout << "server: close(fd), close(cl)" << endl;
    unlink(socket_path);
    close(fd);
    close(cl);
    return 0;
}



void GetData()
{

    //initalize the mutex
    pthread_mutex_init(&lock_x, NULL);

    //VARS
    vector<string> result;
    int n = 0;

    //get prior data if any.
    storage_file.open("state_data.txt");


    if (storage_file.is_open())
    {
        pthread_mutex_lock(&lock_x);
        string data;
        storage_file >> data;
        std::stringstream ss(data);
        while (ss.good())
        {
            string substr;
            getline(ss, substr, ',');
            result.push_back(substr);
        }
        //format data
        COUNT = std::stoi(result[0]);
        ERRORS = std::stoi(result[1]);
        SUM = std::stoi(result[2]);
        storage_file.close();
        pthread_mutex_unlock(&lock_x);
    }
    else
    {
        cout << "error opening file" << endl;
    }



}

void updateData()
{
    //initalize the mutex
    pthread_mutex_init(&lock_x, NULL);

    string s1 = to_string(COUNT);
    string str = s1 + ",";
    string s2 = to_string(ERRORS);
    string str1 = s2 + ",";
    string s3 = to_string(SUM);
    string str2 = s3;
    string halfString = str.append(str1);
    string fullString = halfString.append(str2);

    storage_file.open("state_data.txt");


    if (storage_file.is_open())
    {
        pthread_mutex_lock(&lock_x);
        storage_file << "";
        storage_file << fullString;
        storage_file.flush();
        storage_file.close();
        pthread_mutex_unlock(&lock_x);
    }
    else
    {
        cout << "error opening file" << endl;
    }
}


void* thread_func(void* arg)
{

    COUNT++; //INCREASE THE COUNT
   //variables
    string first_number = "";
    string second_number = "";
    string _operator = "";
    //format data and look for errors

     //convert to numbers
    size_t string_length = strlen(buffer);

    for (int i = 0; i < string_length; i++)
    {
        if (buffer[i] == '+' || buffer[i] == '-' || buffer[i] == '*' || buffer[i] == '/')
        {

            //set the operator 
            _operator = buffer[i];

            //get second number
            int j = i + 1;
            //set i to breakout of prior loop
            i = string_length;
            for (j; j < string_length; j++)
            {
                second_number += buffer[j];
            }
        }
        else
        {
            first_number += buffer[i];
        }

    }


    //Data is loaded so we can now look to see if its formatted properly
    bool res = digits_only(first_number);
    bool res2 = digits_only(second_number);

    if (!res && !res2)
    {
        //send reply to the client
        snprintf(buffer, 50, "Bad Input, Only numbers allowed");
        int n = write(cl, buffer, 50);
        ERRORS++;
        updateData();
        GetData();
        bzero(buffer, sizeof(buffer));
        pthread_exit(NULL);
    }

    //Passed checks so now we can calculate
     //convert to doubles and calculate
    double _first_number = 0;
    double _second_number = 0;

    _first_number = atof(first_number.c_str());
    _second_number = atof(second_number.c_str());

    double answer = 0;

    //do proper operation based on operator
    if (_operator == "+")
    {
        answer = _first_number + _second_number;
    }
    else if (_operator == "-")
    {
        answer = _first_number - _second_number;
    }
    else if (_operator == "*")
    {
        answer = _first_number * _second_number;
    }
    else if (_operator == "/")
    {
        answer = _first_number / _second_number;
    }
    else
    {

        ERRORS++;
        bzero(buffer, sizeof(buffer));
        updateData();
        GetData();
        //send reply to the client
        snprintf(buffer, 50, "Bad Input, operator not recognized");
        int n = write(cl, buffer, 50);
        if (n < 0)
        {
            cout << "failure" << endl;
        }
        pthread_exit(NULL);
    }

    bzero(buffer, sizeof(buffer));

    //increase the SUM
    SUM += answer;

    //send reply to the client
    snprintf(buffer, 50, "%lf", answer);

    int n = write(cl, buffer, 50);
    if (n < 0)
    {
        cout << "failure" << endl;
    }

    pthread_exit(NULL);

}

bool digits_only(const std::string& str)
{
    return str.find_first_not_of(".0123456789*+-/ ") == std::string::npos;
}


