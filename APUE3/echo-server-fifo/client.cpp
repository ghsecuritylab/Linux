#include <fcntl.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <string.h>
#include <stdio.h>

#define MAX(a,b) \
    ((a) >= (b) ? (a) : (b))

int main(int argc, char* arg[]) {
    pid_t pid = getpid();
    std::cout << "pid=" << pid << std::endl;
    std::cout << "Enter 'connect' to connect to server." << std::endl;
    std::cout << "Enter 'echo **' to request the remote echo server." << std::endl;

    std::string client_fifo = std::to_string(pid);
    int err = mkfifoat(AT_FDCWD, client_fifo.c_str(), S_IRUSR |S_IWUSR);
    if (err == -1) {
        std::cout << "mkfifo " << client_fifo << "  failed." << std::endl;
        return 1;
    }

    std::string server_fifo_name = "serverfifo";
    int server_fifo = openat(AT_FDCWD, server_fifo_name.c_str(),O_WRONLY);
    if (server_fifo == -1) {
        std::cout << "openat " << server_fifo_name << "  failed." << std::endl;
        return 1;
    }

    // An open for read-only FIFO without O_NONBLOCK flag would block
    // until some other process opens the FIFO for writing.
    int fifo = openat(AT_FDCWD, client_fifo.c_str(),O_RDWR);
    if (fifo == -1) {
        std::cout << "openat " << client_fifo << "  failed." << std::endl;
        close(server_fifo);

        return 1;
    }

    auto do_clean = [&fifo, & client_fifo,&server_fifo,&server_fifo_name] {
        close(fifo);
        unlink(client_fifo.c_str());

        close(server_fifo);
        unlink(server_fifo_name.c_str());
    };

    fd_set read_set;
    FD_ZERO(&read_set);

    char buf[4096] = { 0 };
    int max_fd = fifo;
    while(true) {
        FD_ZERO(&read_set);
        FD_SET(STDIN_FILENO, &read_set);
        FD_SET(fifo, &read_set);

        int fds = select(max_fd + 1, &read_set, NULL, NULL, NULL);
        if (fds == -1) {
            std::cout << "select failed." << std::endl;
            do_clean();
            return 1;
        }
        else if(fds > 0) {
            // fd_sets are updated by select
            if (FD_ISSET(fifo, &read_set)) {
                memset(buf,0,sizeof(buf));
                int n = read(fifo, buf, sizeof(buf));
                if (n == 0) {
                    // The other end point of this fifo was closed
                    std::cout << "fifo was closed by client." << std::endl;
                    do_clean();
                    return 1;
                }
                std::cout << buf << std::endl;
            }

            if (FD_ISSET(STDIN_FILENO, &read_set)) {
                memset(buf,0,sizeof(buf));
                int n = read(STDIN_FILENO, buf, sizeof(buf));
                if (n > 0 ) {
                    if(strcmp(buf, "q\n") == 0 || strcmp(buf, "quit\n") == 0) {
                        break;
                    }
                    else {
                        // format: [input] [pid]
                        std::string request(buf, strlen(buf));
                        // remove the last '\n'
                        request.pop_back();
                        request.push_back(' ');
                        request += std::to_string(pid);
                        write(server_fifo, request.c_str(), request.length());
                    }
                }
            }          
        }
    }

    do_clean();
    
    return 0;
}