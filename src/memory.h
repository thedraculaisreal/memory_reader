#include <stddef.h>
#include <stdbool.h>

#ifndef MEMORY_H
#define MEMORY_H

typedef struct {
    unsigned long address;
    void *value;
} Address;

typedef struct {
    bool int32;
    bool f32;
    bool unsigned_long;
    bool unknown;
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
    Address *addresses;
    size_t count;
} SavedAddresses;

void search_mem_all(char *mem_file_path, AllAddresses *addresses, SavedAddresses *saved_addresses, Type type);
void search_mem_all_repeat(char *mem_file_path, AllAddresses *addresses, SavedAddresses *old_addresses, SavedAddresses *saved_addresses, Type type);
size_t read_maps(FILE *maps_fd, AddressPair **addresses);
size_t find_pid(char *process_name);
void* read_memory_address(size_t old_pid, unsigned long address);
bool contains_heap(char *line);
void find_health(char *mem_file_path);
bool write_to_mem(Type type, size_t address, char *mem_file_path);

#endif // MEMORY_H
