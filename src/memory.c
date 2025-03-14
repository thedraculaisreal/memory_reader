#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// cat /proc/sys/kernel/pid_max
#define MAXIMUM_PID 4194304

#include "file_reading.h"
#include "memory.h"

void search_mem_all(char *mem_file_path, AllAddresses *addresses, SavedAddresses *saved_addresses, Type type) {
    for (size_t i = 0; i < addresses->count; ++i) {        
        unsigned long start_address = addresses->address_pairs[i].start_address;
        unsigned long end_address = addresses->address_pairs[i].end_address;
        size_t size = end_address - start_address;
        char *memory_buffer = read_file_at_index(mem_file_path, start_address, size);
        for (size_t j = 0; j < size; ++j) {                                
            char new_buffer[type.length + 1];                    
            new_buffer[type.length] = '\0';
            memcpy(new_buffer, &memory_buffer[j], type.length);                                               
            char new_buffer1[10] = "";            
            if (type.int32) {
                int value = *(int*)type.value;
                for (size_t k = 0; k < strlen(new_buffer); ++k) {                  
                    char num_buffer[2];
                    sprintf(num_buffer, "%d", new_buffer[k]);
                    strcat(new_buffer1, num_buffer);                    
                }                
                int num = atoi(new_buffer1);
                if (num == value) {                    
                    printf("0x%08lx    ", j + start_address);                                            
                    printf("|%d\n", num);
                    (*saved_addresses).addresses = (unsigned long *)realloc((*saved_addresses).addresses, sizeof(unsigned long) * ((*saved_addresses).count + 1));
                    (*saved_addresses).addresses[(*saved_addresses).count] = j + start_address;
                    (*saved_addresses).count++;                    
                }
            } else if (type.f32) {
                return;
            }                
        }
    }
    
}

void search_mem_all_repeat(char *mem_file_path, AllAddresses *addresses, SavedAddresses *old_addresses, SavedAddresses *saved_addresses, Type type) {
    for (size_t i = 0; i < addresses->count; ++i) {        
        unsigned long start_address = addresses->address_pairs[i].start_address;
        unsigned long end_address = addresses->address_pairs[i].end_address;
        size_t size = end_address - start_address;
        char *memory_buffer = read_file_at_index(mem_file_path, start_address, size);
        for (size_t j = 0; j < old_addresses->count; ++j) {
            if (old_addresses->addresses[j] > start_address && old_addresses->addresses[j] < end_address) {
                char new_buffer[type.length + 1];                    
                new_buffer[type.length] = '\0';
                memcpy(new_buffer, &memory_buffer[old_addresses->addresses[j] - start_address], type.length);             
                char new_buffer1[10] = "";
                if (type.int32) {
                    int value = *(int*)type.value;
                    for (size_t k = 0; k < strlen(new_buffer); ++k) {
                        char num_buffer[2];
                        sprintf(num_buffer, "%d", new_buffer[k]);
                        strcat(new_buffer1, num_buffer);                        
                    }
                    int num = atoi(new_buffer1);
                    if (num == value) {                    
                        printf("0x%08lx    ", old_addresses->addresses[j]);       
                        printf("|%d\n", num);
                        (*saved_addresses).addresses = (unsigned long *)realloc((*saved_addresses).addresses, sizeof(unsigned long) * ((*saved_addresses).count + 1));
                        (*saved_addresses).addresses[(*saved_addresses).count] = old_addresses->addresses[j];
                        (*saved_addresses).count++;
                    }
                } else if (type.f32) {
                    return;
                }
            } else {
                continue;
            }            
        }
    }
    
}

size_t read_maps(FILE *maps_fd, AddressPair **addresses) {
    char line[256];
    size_t index = 0;
    while (fgets(line, 256, maps_fd)) {
        if (index == 9) break;
        AddressPair address_pair;               
        char perms[5];
        sscanf(line, "%08lx-%08lx %4s", &address_pair.start_address, &address_pair.end_address, perms);
        if (perms[0] != 'r') {
            printf("Memory region not readable.");
            continue;
        }
        // add to hashmap of addresses
        (*addresses) = (AddressPair *)realloc((*addresses), sizeof(AddressPair) * (index + 1));
        (*addresses)[index] = address_pair;
        index++;
    }
    return index;
}

size_t find_pid(char *process_name) {
    for (size_t i = 0; i < MAXIMUM_PID; ++i) {
        char stat_file_path[256];    
        sprintf(stat_file_path, "/proc/%zu/stat", i);        
        char *buffer = read_file_at_index(stat_file_path, 0, 50);        
        if (!buffer) {            
            continue;
        }        
        for (size_t j = 0; j < strlen(buffer); ++j) {
            if (buffer[j] == '(') {                
                char new_buffer[strlen(process_name) + 1];
                new_buffer[strlen(process_name)] = '\0';
                memcpy(new_buffer, &buffer[j + 1], strlen(process_name));                
                if (strncmp(process_name, new_buffer, strlen(process_name)) == 0) {
                    return i;
                }
            }
        }
    }
    return 0;
}
