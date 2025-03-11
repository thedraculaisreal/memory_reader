#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "file_reading.h"

#define chunk_size 16

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "ERROR: %s <pid>", argv[0]);
        return 1;
    }
    bool int32 = false;
    char type[6];        
    printf("What do you want to search for?(int32, f32): ");
    scanf("%s", type);
    if (strcmp(type, "int32") == 0) {
        printf("int32 on\n");
        int32 = true;
    }    
    while (true) {
        // opening process maps to find where the address range of heap is stored.
        char maps_file_path[256];
        char mem_file_path[256];
        sprintf(maps_file_path, "/proc/%s/maps", argv[1]);
        sprintf(mem_file_path, "/proc/%s/mem", argv[1]);
        FILE *maps_fd = fopen(maps_file_path, "r");
        char line[256];
        int page_size = 4096;
        // we use fgets here because it stops at a new line allowing us to easilty parse maps                
        int value;
        char value_dec[50];        
        printf("What value do you wish to search for?: ");
        if (int32) {            
            scanf("%d", &value);
            sprintf(value_dec ,"%d", value);                    
        } else {            
            scanf("%s", &value);
        }
        size_t byte_length = strlen(value_dec);
        while (fgets(line, 256, maps_fd)) {
            unsigned long start_address;
            unsigned long end_address;
            char perms[5];
            sscanf(line, "%08lx-%08lx %4s", &start_address, &end_address, perms);
            if (perms[0] != 'r') {
                printf("Memory region not readable.");
                continue;
            }
            if (start_address != 0x3e013000) {
                continue;
            }
            size_t size = end_address - start_address;            
            char *memory_buffer = read_file_into_mem(mem_file_path, start_address, size);            
            for (size_t i = 0; i < size; i += byte_length) {                                
                char new_buffer[byte_length + 1];
                new_buffer[byte_length] = '\0';
                memcpy(new_buffer, &memory_buffer[i], byte_length);               
                int num = atoi(new_buffer);                
                if (num == value) {
                    printf("0x%08lx    ", start_address + i);
                    for (size_t j = 0; j < strlen(new_buffer); ++j) {                    
                        printf("%02x", new_buffer[j]);                        
                    }
                    printf("|%d\n", num);
                }                                                
            }            
        }
    }
        
    return 0;
}
