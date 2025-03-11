#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "file_reading.h"

#define CHUNK_SIZE 16
#define PAGE_SIZE 4096

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "ERROR: %s <pid>", argv[0]);
        return 1;
    }
    bool int32 = false;
    bool first_run = true;
    char type[6];        
    printf("What do you want to search for?(int32, f32): ");
    scanf("%s", type);
    if (strcmp(type, "int32") == 0) {
        printf("int32 on\n");
        int32 = true;
    }
    unsigned long *addresses = NULL;
    size_t address_count = 0;
    while (true) {
        // opening process maps to find where the address range of heap is stored.
        char maps_file_path[256];
        char mem_file_path[256];
        sprintf(maps_file_path, "/proc/%s/maps", argv[1]);
        sprintf(mem_file_path, "/proc/%s/mem", argv[1]);
        FILE *maps_fd = fopen(maps_file_path, "r");
        char line[256];        
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
            // this is hard coded for now
            if (start_address != 0x3e013000) {
                continue;
            }
            size_t size = end_address - start_address;            
            char *memory_buffer = read_file_into_mem(mem_file_path, start_address, size);
            size_t match = 0;
            unsigned long *match_addresses = NULL;
            if (first_run) {
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
                        match_addresses = (unsigned long *)realloc(match_addresses, sizeof(unsigned long) * (match + 1));
                        match_addresses[match] = i;
                        match++;
                    }                
                }
            } else {
                for (size_t i = 0; i < address_count; ++i) {                                
                    char new_buffer[byte_length + 1];
                    new_buffer[byte_length] = '\0';
                    memcpy(new_buffer, &memory_buffer[addresses[i]], byte_length);                    
                    int num = atoi(new_buffer);                
                    if (num == value) {                    
                        printf("0x%08lx    ", addresses[i] + start_address);                    
                        for (size_t j = 0; j < strlen(new_buffer); ++j) {                    
                            printf("%02x", new_buffer[j]);                        
                        }
                        printf("|%d\n", num);
                        match_addresses = (unsigned long *)realloc(match_addresses, sizeof(unsigned long) * (match + 1));
                        match_addresses[match] = i;
                        match++;
                    }                
                }
            }
            free(addresses);
            free(memory_buffer);            
            addresses = match_addresses;            
            address_count = match;
            first_run = false;
        }
    }        
    return 0;
}
