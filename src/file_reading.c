#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "file_reading.h"

char* read_file_into_mem(const char *file_path, unsigned long start_address, size_t size) {
    int ret;
    int fd = open(file_path, O_RDONLY);
    if (!fd) {
        perror("open");
        return false;
    }
    char* buffer = (char *)malloc(size);
    // place position at end of file to get byte offset for buffer size
    if (lseek(fd, start_address, SEEK_SET) == -1) {
        perror("lseek");
        free(buffer);
        return false;
    }
        
    ret = read(fd, buffer, size);
    if (ret <= 0) {
        free(buffer);
        perror("read");
        return false;
    }
        
    return buffer;
}


