#include "system.h"
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

namespace apue {
	int system(const char* cmdstring) {
		int status = -1;
		if(!cmdstring) {
			return 1;  //return none-zero code to indicate the system call is available
		}
		
		pid_t pid;
		if((pid = fork()) < 0) {
			status = -1;
		}
		else if (pid == 0) {  //child
			execl("/bin/bash", "bash", "-c", cmdstring, (char*)0);
			_exit(127);
		}
		else {
            /*
                Return value:
                1. returns the process ID (a positive value) of the child whose state has changed on calling successfully.
                2. returns 0 if WNOHANG was specified and one or more child(ren) specified by pid exist, but have not yet changed state.
                3. returns -1 on error.
            */
			while(waitpid(pid, &status, 0) < 0) { //waitpid() error
				if(errno != EINTR) { //EINTR: waitpid() was interrupted by a signal. The value of *status_ptr is undefined.
					status = -1; //error other than EINTR from waitpid()
					break;
				}
			}
		}
	}
}