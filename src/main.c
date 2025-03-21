#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
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
    while (true) {
        search_mem(pid);                    
    }        
    return 0;
}

    /*FILE *maps_fd;
    setup_files(&maps_fd, pid);    
    find_heap(maps_fd);
    char mem_file_path[256];
    sprintf(mem_file_path, "/proc/%zu/mem", pid);
    while (true) {
        find_health(mem_file_path);
        sleep(1);
    }*/

void setup_files(FILE **maps_fd, size_t pid) {
    char maps_file_path[256];    
    sprintf(maps_file_path, "/proc/%zu/maps", pid);    
    (*maps_fd) = fopen(maps_file_path, "r");
}

SavedAddresses saved_addresses;

void search_mem(size_t pid) {
    bool first_run = true;
    Type type;                
    while (true) {
        char buffer[10];        
        if (first_run) {
            type.unsigned_long = false;
            type.int32 = false;
            type.f32 = false;
            type.unknown = false;
            type.string = false;
            type.greater = false;
            type.lesser = false;            
            printf("Do you want to search for an address or value? ");
            scanf("%s", buffer);
            if (strcmp(buffer, "address") == 0) {
                type.unsigned_long = true;
                type.value = malloc(sizeof(unsigned long));
                printf("Enter address in hexadecimal: ");
                scanf("%08lx", type.value);                
            }
            memset(buffer, 0, sizeof(buffer));
            printf("What do you want to search for?(int32, f32, string): ");
            scanf("%s", buffer);
            if (strcmp(buffer, "int32") == 0) {
                printf("int32 on\n");
                type.int32 = true;                                
            } else if (strcmp(buffer, "f32") == 0) {
                printf("f32 on\n");
                type.f32 = true;                                 
            } else if (strcmp(buffer, "string") == 0) {
                printf("string on\n");
                type.string = true;
            }
            memset(buffer, 0, sizeof(buffer));
            printf("Do you know the value?(yes/no): ");
            scanf("%s", buffer);
            if (strcmp(buffer, "no") == 0) {
                printf("unknown on\n");
                type.unknown = true;
            }
            memset(buffer, 0, sizeof(buffer));
        }        
        AllAddresses all_addresses;
        AddressPair *address_pairs = NULL;
        FILE *maps_fd;
        setup_files(&maps_fd, pid);        
        if (!type.unsigned_long && !type.unknown) {                        
            printf("What value do you wish to search for?: ");
            if (type.int32) {
                type.value = malloc(sizeof(int));
                scanf("%d", type.value);                    
            } else if (type.f32) {            
                type.value = malloc(sizeof(double));
                scanf("%lf", type.value);        
            } else if (type.string) {
                scanf("%s", buffer);
                type.value = malloc(strlen(buffer) + 1);
                memcpy(type.value, buffer, strlen(buffer));
            }
            memset(buffer, 0, sizeof(buffer));
        } 
        if (type.unknown && !first_run) {
            printf("> or <");
            scanf("%s", buffer);
            if (strcmp(">", buffer) == 0) {
                printf("Greater on\n");
                type.greater = true;
                type.lesser = false;
            } else if (strcmp("<", buffer) == 0) {
                printf("lesser on\n");
                type.greater = false;
                type.lesser = true;
            } else {
                printf("no change on\n");
                type.greater = false;
                type.lesser = false;
            }            
        }            
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
            if (type.unsigned_long) {
                return;
            }
        } else {
            search_mem_all_repeat(mem_file_path, &all_addresses, &saved_addresses, &match_addresses, type);
            free(saved_addresses.addresses);
        }        
        saved_addresses.addresses = match_addresses.addresses;
        saved_addresses.count = match_addresses.count;
    }    
}

