#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "file_reading.h"

char* read_file_into_mem(const char *file_path) {
    int ret;
    FILE *fd = fopen(file_path, "r");
    if (!fd) {        
        return NULL;
    }
    
    // place position at end of file to get byte offset for buffer size
    ret = fseek(fd, 0L, SEEK_END);
    if (ret < 0) {
        perror("fseek");
        return NULL;
    }
    
    long buffer_size = ftell(fd);
    // set size of buffer
    // we also need to malloc, you may be asking why if you dont code in c and i will attempt to explain.
    // the reason we malloc instead of creating a buffer[] or a const char*, is be because it will create these variables
    // on the stack, and when we return from this function the stack will change, therefore we use malloc to create
    // the variable on the heap explained here https://stackoverflow.com/questions/7777524/c-buffer-memory-allocation
    char* buffer = malloc(sizeof(char) * (buffer_size + 1));
    // place position back to start of file.
    ret = fseek(fd, 0L, SEEK_SET);
    if (ret < 0) {
        perror("fseek");
        return NULL;
    }
    
    ret = fread(buffer, sizeof(char), buffer_size, fd);
    
    if (ferror(fd) != 0) {
        perror("Error reading file");
        return NULL;
    } else {
        // not sure if you need to do this but always good practice to null terminate c strings.
        buffer[buffer_size + 1] = '\0';
    }
    
    fclose(fd);
    return buffer;
}

char* read_file_at_index(const char *file_path, unsigned long start_address, size_t size) {
    int ret;
    int fd = open(file_path, O_RDONLY);
    if (!fd) {
        perror("open");
        return NULL;
    }
    char* buffer = (char *)malloc(size);
    // place position at end of file to get byte offset for buffer size
    if (lseek(fd, start_address, SEEK_SET) == -1) {
        perror("lseek");
        free(buffer);
        return NULL;
    }
        
    ret = read(fd, buffer, size);
    if (ret <= 0) {
        free(buffer);
        perror("read");
        return NULL;
    }
    close(fd);    
    return buffer;
}
