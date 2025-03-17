#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "file_reading.h"
#include "memory.h"

#define CHUNK_SIZE 16
#define PAGE_SIZE 4096

void setup_files(FILE **maps_fd, size_t pid);
void search_mem(size_t pid);


int main() {
    // change this to whatever process you want to search name, use ps -e to find process name / pid
    char *process_name = "linux_64_client";
    size_t pid = find_pid(process_name);
    printf("Found PID -> %zu\n", pid);
    //read_memory_address(pid, 0x1e01bb60);
    search_mem(pid);            
    return 0;
}

void setup_files(FILE **maps_fd, size_t pid) {
    char maps_file_path[256];    
    sprintf(maps_file_path, "/proc/%zu/maps", pid);    
    (*maps_fd) = fopen(maps_file_path, "r");
}

SavedAddresses saved_addresses;
Type type;

void search_mem(size_t pid) {
    bool first_run = true;
    while (true) {
        if (first_run) {
            char buffer[6];            
            printf("What do you want to search for?(int32, f32): ");
            scanf("%s", buffer);        
            if (strcmp(buffer, "int32") == 0) {
                printf("int32 on\n");
                type.int32 = true;
            }
        }        
        AllAddresses all_addresses;
        AddressPair *address_pairs = NULL;
        FILE *maps_fd;
        setup_files(&maps_fd, pid);
        // testing
        find_heap(maps_fd);
        char value_dec[50];        
        printf("What value do you wish to search for?: ");
        if (type.int32) {
            type.value = malloc(sizeof(int));
            scanf("%d", type.value);
            sprintf(value_dec ,"%d", *(int*)type.value);        
        } else {            
            scanf("%s", &type.value);
        }
        type.length = strlen(value_dec);        
        all_addresses.count = read_maps(maps_fd, &address_pairs);
        all_addresses.address_pairs = address_pairs;
        SavedAddresses match_addresses;
        match_addresses.addresses = NULL;
        match_addresses.count = 0;
        char mem_file_path[256];
        sprintf(mem_file_path, "/proc/%zu/mem", pid);
        if (first_run) {
            search_mem_all(mem_file_path, &all_addresses, &match_addresses, type);
            first_run = false;
        } else {
            search_mem_all_repeat(mem_file_path, &all_addresses, &saved_addresses, &match_addresses, type);
            free(saved_addresses.addresses);
        }        
        saved_addresses.addresses = match_addresses.addresses;
        saved_addresses.count = match_addresses.count;
    }    
}

