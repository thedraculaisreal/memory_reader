#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "memory.h"

void search_mem_all(char *memory_buffer, AllAddresses *addresses, SavedAddresses *saved_addresses, Type type) {
    for (size_t i = 0; i < addresses->count; ++i) {
        unsigned long start_address = addresses->address_pairs[i].start_address;
        unsigned long end_address = addresses->address_pairs[i].end_address;
        for (size_t j = start_address; j < end_address; ++j) {                                
            char new_buffer[type.length + 1];                    
            new_buffer[type.length] = '\0';
            memcpy(new_buffer, &memory_buffer[j], type.length);                                               
            char new_buffer1[10] = "";
            if (type.int32) {
                int value = *(int*)type.value;
                for (size_t k = 0; k < strlen(new_buffer); ++k) {
                    char num_buffer[1];
                    sprintf(num_buffer, "%d", new_buffer[k]);
                    strcat(new_buffer1, num_buffer);                        
                }
                int num = atoi(new_buffer1);
                if (num == value) {                    
                    printf("0x%08lx    ", j);                                            
                    printf("|%d\n", num);
                    saved_addresses->addresses = (unsigned long *)realloc(saved_addresses->addresses, sizeof(unsigned long) * (saved_addresses->count + 1));
                    saved_addresses->addresses[saved_addresses->count] = j;
                    saved_addresses->count++;
                }
            } else if (type.f32) {
                return;
            }                
        }
    }
    
}

size_t read_maps(FILE *maps_fd, AddressPair **addresses) {
    char line[256];
    size_t index = 0;
    while (fgets(line, 256, maps_fd)) {        
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
