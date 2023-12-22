#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
  
#define MAX_KEYS 1000  
#define MAX_NAME_LENGTH 100  
  
typedef struct {  
    char name[MAX_NAME_LENGTH];  
    int key;  
} Record;  
  
Record records[MAX_KEYS];  
int num_records = 0;  
  
void insert_record(char *name, int key) {  
    Record new_record;  
    strcpy(new_record.name, name);  
    new_record.key = key;  
    records[num_records] = new_record;  
    num_records++;  
}  
  
int search_record(int key) {  
    for (int i = 0; i < num_records; i++) {  
        if (records[i].key == key) {  
            return i;  
        }  
    }  
    return -1;  // Record not found  
}  
  
void print_records() {  
    for (int i = 0; i < num_records; i++) {  
        printf("%s\t%d\n", records[i].name, records[i].key);  
    }  
}  
  
int main() {  
    insert_record("Alice", 1);  
    insert_record("Bob", 2);  
    insert_record("Charlie", 3);  
    print_records();  // Output: Alice 1, Bob 2, Charlie 3  
    int index = search_record(2);  // Output: 1 (index of record with key 2)  
    if (index != -1) {  
        printf("Record with key %d found at index %d\n", 2, index);  // Output: Record with key 2 found at index 1  
    } else {  
        printf("Record not found\n");  // Output: Record not found  
    }  
    return 0;  
}