#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"

#define BUFFER_SIZE 256
int main(int argc, char *argv[]){
	int fd;
	if (argc != 4){
		printf(2, "Usage: lseektest <filename> <offset> <string>\n");
		exit();
	}
	// open the file
	if (argv[1] == (char *)0 || (fd = open(argv[1], O_RDWR)) < 0){
		printf(2, "Error: Invalid Filename\n");
		exit();
	}
	// before: fstat
	struct stat fs;
	if (fstat(fd, &fs) < 0){
		printf(2, "Error: fstat error\n");
		close(fd);
		exit();
	}
	uint size= fs.size;
	// before: print the contents
	char buf[BUFFER_SIZE];
	if (lseek(fd, (int)0, SEEK_SET) < 0){
		printf(2, "Error: lseek error\n");
		close(fd);
		exit();
	}
	if (read(fd, (char *)buf, (int)size) < 0){
		printf(2, "Error: read error\n");
		close(fd);
		exit();
	}
	printf(1, "Before: %s\n", buf);
	// modify
	int offset;
	offset = atoi(argv[2]);

	if (lseek(fd, (int)offset, SEEK_SET) < 0){
		printf(2, "Error: lseek error\n");
		close(fd);
		exit();
	}
	if (write(fd, (char *)argv[3], (int)strlen(argv[3])) < 0){
		printf(2, "Error: write error\n");
		close(fd);
		exit();
	}
	// after: fstat
	struct stat new_fs;
	if (fstat(fd, &new_fs) < ){
		printf(2, "Error: fstat error\n");
		close(fd);
		exit();
	}
	uint new_size = new_fs.size;
	// after: print the contents
	if (lseek(fd, (int)0, SEEK_SET) < 0){
		printf(2, "Error: lseek error\n");
		close(fd);
		exit();
	}
	if (read(fd, (char *)buf, (int)new_size) < 0){
		printf(2, "Error: read error\n");
		close(fd);
		exit();
	}
	printf(1, "After: %s\n", buf);
	close(fd);
	exit();
}
