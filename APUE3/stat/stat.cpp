#include <sys/stat.h>
#include <string>
#include <iostream>

std::string get_mode(mode_t mode) {
	if(S_ISREG(mode)) {
		return "regular"; 	
	}
	else if(S_ISDIR(mode)) {
		return "directory"; 	
	}
	else if(S_ISCHR(mode)) {
		return "character"; 	
	}
	else if(S_ISBLK(mode)) {
		return "block"; 	
	}
	else if(S_ISFIFO(mode)) {
		return "fifo"; 	
	}
	else if(S_ISLNK(mode)) {
		return "symbolic"; 	
	}
	else if(S_ISSOCK(mode)) {
		return "socket"; 	
	}
	else {
		return "unknown";
	}
}

int main(int argc, char* argv[]) {
	if (argc <= 1) {
		std::cout << "ERROR: stat needs a file parameter." << std::endl;
		return 1;	
	}

	struct stat stat_data;
	if (lstat(argv[1], &stat_data) == 0) {
		std::cout << "type: " << get_mode(stat_data.st_mode) << std::endl;
		std::cout << "inode: " << stat_data.st_ino << std::endl;
		std::cout << "dev: " << stat_data.st_dev << std::endl;
		std::cout << "rdev: " << stat_data.st_rdev << std::endl;
		std::cout << "nlink: " << stat_data.st_nlink << std::endl;
		std::cout << "uid: " << stat_data.st_uid << std::endl;
		std::cout << "gid: " << stat_data.st_gid << std::endl;
		std::cout << "size: " << stat_data.st_size << std::endl;
		std::cout << "atime: " << stat_data.st_atime << std::endl;
		std::cout << "mtime: " << stat_data.st_mtime << std::endl;
		std::cout << "ctime: " << stat_data.st_ctime << std::endl;
		std::cout << "best I/O block size: " << stat_data.st_blksize << std::endl;
		std::cout << "disk blocks: " << stat_data.st_blocks << std::endl;
	}

	return 0;
}