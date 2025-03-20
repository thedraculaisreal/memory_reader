#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <inttypes.h>
#include <fcntl.h>

// cat /proc/sys/kernel/pid_max
#define MAXIMUM_PID 4194304

#include "file_reading.h"
#include "memory.h"

unsigned long heap_start_address;
unsigned long heap_end_address;
size_t health_offset = 0;

void search_mem_all(char *mem_file_path, AllAddresses *addresses, SavedAddresses *saved_addresses, Type type) {    
    for (size_t i = 0; i < addresses->count; ++i) {        
        unsigned long start_address = addresses->address_pairs[i].start_address;
        unsigned long end_address = addresses->address_pairs[i].end_address;
        size_t size = end_address - start_address;
        char *memory_buffer = read_file_at_index(mem_file_path, start_address, size);
        if (!type.unsigned_long && !type.unknown) {
            for (size_t j = 0; j < size; ++j) {                                            
                if (type.int32) {
                    int num;
                    memcpy(&num, &memory_buffer[j], sizeof(int));
                    int value = *(int*)type.value;                
                    if (num == value) {                    
                        printf("0x%08lx    ", j + start_address);                                            
                        printf("|%d\n", num);
                        (*saved_addresses).addresses = (Address *)realloc((*saved_addresses).addresses, sizeof(Address) * ((*saved_addresses).count + 1));
                        (*saved_addresses).addresses[(*saved_addresses).count].address = j + start_address;
                        (*saved_addresses).addresses[(*saved_addresses).count].value = malloc(sizeof(num));
                        memcpy((*saved_addresses).addresses[(*saved_addresses).count].value, &num, sizeof(num));
                        (*saved_addresses).count++;                        
                    }
                } else if (type.f32) {
                    double num;
                    memcpy(&num, &memory_buffer[j], sizeof(double));
                    double value = *(double*)type.value;                
                    if (num == value) {                    
                        printf("0x%08lx    ", j + start_address);                                            
                        printf("|%lf\n", num);
                        (*saved_addresses).addresses = (Address *)realloc((*saved_addresses).addresses, sizeof(Address) * ((*saved_addresses).count + 1));
                        (*saved_addresses).addresses[(*saved_addresses).count].address = j + start_address;
                        (*saved_addresses).addresses[(*saved_addresses).count].value = malloc(sizeof(num));
                        memcpy((*saved_addresses).addresses[(*saved_addresses).count].value, &num, sizeof(num));
                        (*saved_addresses).count++;
                    }
                }                
            }
        }        
        else if (type.unsigned_long){
            unsigned long address = *(unsigned long*)type.value;
            if (address > start_address && address < end_address) {                                
                if (type.int32) {
                    char buffer[10];
                    printf("Do you want to examine memory around it?\n");
                    scanf("%s", buffer);
                    if (strcmp(buffer, "yes") == 0) {
                        for (size_t j = (address - start_address) - 128; j < (address - start_address) + 128; ++j) {
                            char buffer_num[sizeof(int) + 1];
                            buffer_num[sizeof(int)] = '\0';
                            int num;
                            memcpy(&num, &memory_buffer[j], sizeof(int));
                            memcpy(buffer_num, &memory_buffer[j], sizeof(int));
                            if (address - start_address == j) {
                                printf("----------------\n");
                            }
                            printf("0x%08lx |%d| ", j + start_address, num);
                            for (size_t k = 0; k < strlen(buffer_num); ++k) {
                                printf("%02X", buffer_num[k]);
                            }
                            printf("\n");
                            if (address - start_address == j) {
                                printf("----------------\n");
                            }
                        }
                    } else {
                        int num;
                        memcpy(&num, &memory_buffer[address - start_address], sizeof(int));
                        printf("0x%08lx  |%d\n", address, num);
                    }                                       
                } else if (type.f32) {
                    char buffer[10];
                    printf("Do you want to examine memory around it?\n");
                    scanf("%s", buffer);
                    if (strcmp(buffer, "yes") == 0) {
                        for (size_t j = (address - start_address) - 128; j < (address - start_address) + 128; ++j) {
                            char buffer_num[sizeof(double) + 1];
                            buffer_num[sizeof(double)] = '\0';
                            double num;
                            memcpy(&num, &memory_buffer[j], sizeof(double));
                            memcpy(buffer_num, &memory_buffer[j], sizeof(double));
                            if (address - start_address == j) {
                                printf("----------------\n");
                            }                            
                            printf("0x%08lx  |%lf", j + start_address, num);                            
                            for (size_t k = 0; k < strlen(buffer_num); ++k) {
                                printf("%02X", buffer_num[k]);
                            }
                            printf("\n");
                            if (address - start_address == j) {
                                printf("----------------\n");
                            }
                        }
                    } else {
                        double num;
                        memcpy(&num, &memory_buffer[address - start_address], sizeof(double)); 
                        printf("0x%08lx    %lf\n", address, num);
                    }                    
                }                
            }
        }
        else if (type.unknown) {            
            for (size_t j = 0; j < size; ++j) {
                if (type.int32) {
                    int num;
                    memcpy(&num, &memory_buffer[j], sizeof(int));
                    (*saved_addresses).addresses = (Address *)realloc((*saved_addresses).addresses, sizeof(Address) * ((*saved_addresses).count + 1));
                    (*saved_addresses).addresses[(*saved_addresses).count].address = j + start_address;
                    (*saved_addresses).addresses[(*saved_addresses).count].value = malloc(sizeof(num));
                    memcpy((*saved_addresses).addresses[(*saved_addresses).count].value, &num, sizeof(num));
                    (*saved_addresses).count++;                                       
                } else if (type.f32) {

                } else {
                    return;
                }
            }
        }
    }
    printf("Read all values! %zu\n", saved_addresses->count);
}

void search_mem_all_repeat(char *mem_file_path, AllAddresses *addresses, SavedAddresses *old_addresses, SavedAddresses *saved_addresses, Type type) {
    for (size_t i = 0; i < addresses->count; ++i) {        
        unsigned long start_address = addresses->address_pairs[i].start_address;
        unsigned long end_address = addresses->address_pairs[i].end_address;
        size_t size = end_address - start_address;
        char *memory_buffer = read_file_at_index(mem_file_path, start_address, size);
        if (!type.unknown) {
            for (size_t j = 0; j < old_addresses->count; ++j) {
                if (old_addresses->addresses[j].address > start_address && old_addresses->addresses[j].address < end_address) { 
                    if (type.int32) {
                        int num;
                        memcpy(&num, &memory_buffer[old_addresses->addresses[j].address - start_address], sizeof(int));
                        int value = *(int*)type.value;                
                        if (num == value) {         
                            printf("0x%08lx    ", old_addresses->addresses[j].address);       
                            printf("|%d\n", num);
                            (*saved_addresses).addresses = (Address *)realloc((*saved_addresses).addresses, sizeof(Address) * ((*saved_addresses).count + 1));
                            (*saved_addresses).addresses[(*saved_addresses).count].address = old_addresses->addresses[j].address;
                            (*saved_addresses).addresses[(*saved_addresses).count].value = malloc(sizeof(num));
                            memcpy((*saved_addresses).addresses[(*saved_addresses).count].value, &num, sizeof(num));
                            (*saved_addresses).count++;
                        }
                    } else if (type.f32) {
                        double num;
                        memcpy(&num, &memory_buffer[old_addresses->addresses[j].address - start_address], sizeof(double));
                        double value = *(double*)type.value;
                        if (num == value) {         
                            printf("0x%08lx    ", old_addresses->addresses[j]);       
                            printf("|%d\n", num);
                            (*saved_addresses).addresses = (Address *)realloc((*saved_addresses).addresses, sizeof(Address) * ((*saved_addresses).count + 1));
                            (*saved_addresses).addresses[(*saved_addresses).count].address = old_addresses->addresses[j].address;
                            (*saved_addresses).addresses[(*saved_addresses).count].value = malloc(sizeof(num));
                            memcpy((*saved_addresses).addresses[(*saved_addresses).count].value, &num, sizeof(num));
                            (*saved_addresses).count++;
                        }                    
                    }
                } else {
                    continue;
                }            
            }            
        } else {

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
        if (contains_heap(line)) {
            heap_start_address = address_pair.start_address;
            heap_end_address = address_pair.end_address;            
        }
        // add to hashmap of addresses
        (*addresses) = (AddressPair *)realloc((*addresses), sizeof(AddressPair) * (index + 1));
        (*addresses)[index] = address_pair;
        index++;
    }
    return index;
}

/*void find_heap(FILE *maps_fd) {
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
        if (contains_heap(line)) {
            heap_start_address = address_pair.start_address;
            heap_end_address = address_pair.end_address;
            printf("Found heap 0x%08lx-0x%08lx\n", heap_start_address, heap_end_address);            
        }        
    }    
}*/

size_t find_pid(char *process_name) {
    for (size_t i = 0; i < MAXIMUM_PID; ++i) {
        char stat_file_path[256];    
        sprintf(stat_file_path, "/proc/%zu/stat", i);        
        char buffer[50]; 
        int fd = open(stat_file_path, O_RDONLY);
        if (read(fd, buffer, sizeof(buffer)) >= 0) {                    
            for (size_t j = 0; j < strlen(buffer); ++j) {
                if (buffer[j] == '(') {                
                    char new_buffer[strlen(process_name) + 1];
                    new_buffer[strlen(process_name)] = '\0';
                    memcpy(new_buffer, &buffer[j + 1], strlen(process_name));                               
                    if (strcmp(process_name, new_buffer) == 0) {
                        return i;
                    }
                }
            } 
        }
        close(fd);
    }
    return 0;
}

bool contains_heap(char *line) {
    char *heap_name = "heap";
    for (size_t i = 0; i < strlen(line); ++i) {
        char buffer[strlen(heap_name) + 1];
        memcpy(buffer, &line[i], strlen(heap_name));
        buffer[strlen(heap_name)] = '\0';        
        if (strcmp(buffer, heap_name) == 0) {
            return true;
        }
    }
    return false;
}

void find_health(char *mem_file_path) {    
    size_t size = heap_end_address - heap_start_address;
    char *memory_buffer = read_file_at_index(mem_file_path, heap_start_address, size);
    size_t health_offset1 = 0x1D860;
    size_t health_offset2 = 0x1D920;
    int health;
    if (health_offset == 0) {                
        memcpy(&health, &memory_buffer[health_offset1], sizeof(int));
        if (health > 100 || health <= 0) {
            memset(&health, 0, sizeof(health));
            memcpy(&health, &memory_buffer[health_offset2], sizeof(int));
            health_offset = health_offset2;
        }        
    } else {
        memcpy(&health, &memory_buffer[health_offset], sizeof(int));
    }
    if (health_offset == 0) {
        health_offset = health_offset1;
    }
    printf("Local_Player health = %d\n", health);
}

bool write_to_mem(Type type, size_t address, char *mem_file_path) {        
    // read and write to mem file
    int mem_fd = open(mem_file_path, O_RDWR);    
    if (type.int32) {
        int value = *(int*)type.value;
        pwrite(mem_fd, &value, sizeof(value), address);
        printf("0x%08lx -> %d", address, value);
        return true;
    } else if (type.f32) {
        double value = *(double*)type.value;
        pwrite(mem_fd, &value, sizeof(value), address);
        printf("0x%08lx -> %lf", address, value);
        return true;
    }
    
    return false;
}
