#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "file_reading.h"
#include "memory.h"

#define CHUNK_SIZE 16
#define PAGE_SIZE 4096

void setup_files(FILE **maps_fd, char *argv1);
void search_mem(char *argv1);

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "ERROR: %s <pid>", argv[0]);
        return 1;
    }
    search_mem(argv[1]);            
    return 0;
}

void setup_files(FILE **maps_fd, char *argv1) {
    char maps_file_path[256];    
    sprintf(maps_file_path, "/proc/%s/maps", argv1);    
    (*maps_fd) = fopen(maps_file_path, "r");
}

SavedAddresses saved_addresses;
Type type;

void search_mem(char *argv1) {
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
        setup_files(&maps_fd, argv1);    
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
        sprintf(mem_file_path, "/proc/%s/mem", argv1);
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

