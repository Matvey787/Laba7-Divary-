#ifndef GLOBAL
#define GLOBAL


typedef struct element_table {
    char element_name[10];
    unsigned int atomic_number;
    unsigned int html_cod;
    double atomic_mass;
} Elem_Table;

typedef struct formula {
    int title;
    unsigned int amount;
} Formula;

typedef struct drug {
    char drug_name[10];
    Formula drug_formula [10];
    int additive[5];
} Drug;

typedef struct list{
    Drug elem;
    struct list *next;
} List;

void   read_table              (Elem_Table *main_table, const char* fileName);
void   print_table             (Elem_Table *main_table, const char* fileName);
int    searchElemByFirstLetter (char first_letter, Elem_Table *main_table);
//int    searchElem              (char *elem_name, Elem_Table *main_table);
drug   input_drugs             (int *No, Elem_Table *main_table);
//int    searchElemByFirstLetter (char first_letter, Elem_Table *main_table);
size_t searchElem              (const char* elemName, Elem_Table* main_table);
void   print_substance         (Drug passport, Elem_Table* main_table);
void   drug_name_string        (char* s, Drug passport, Elem_Table* main_table);
size_t hash_computation        (char* s);
void   intput_table            (Elem_Table *main_table);
Drug   input_sample            (Elem_Table *main_table);
void   print_sample            (Drug passport, Elem_Table* main_table);
void read_hash_table(List *hash_table[], const char *file_name, int* counter);

void hash_table_entry     (List* hash_table[], Drug passport, unsigned int hash);
void rec_hash_table_entry (List* hash_elem, Drug passport);
void write_hash_table     (List* hash_table[], const char* fileName);
void rec_write_hash_table (List* hash_elem, FILE** wFile);

#endif