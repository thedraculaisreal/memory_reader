#include <stddef.h>
#include <stdbool.h>

#ifndef MEMORY_H
#define MEMORY_H

typedef struct {
    bool int32;
    bool f32;
    size_t length;
    void *value;
} Type;

typedef struct {
    unsigned long start_address;
    unsigned long end_address;
} AddressPair;
    
typedef struct {
    AddressPair *address_pairs;
    size_t count;
} AllAddresses;

typedef struct {
    unsigned long *addresses;
    size_t count;
} SavedAddresses;

void search_mem_all(char *mem_file_path, AllAddresses *addresses, SavedAddresses *saved_addresses, Type type);
void search_mem_all_repeat(char *mem_file_path, AllAddresses *addresses, SavedAddresses *old_addresses, SavedAddresses *saved_addresses, Type type);
size_t read_maps(FILE *maps_fd, AddressPair **addresses);
size_t find_pid(char *process_name);
#endif // MEMORY_H
