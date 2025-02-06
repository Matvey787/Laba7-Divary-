#ifndef GLOBAL
#define GLOBAL


typedef struct element_table {
    char element_name[10];
    unsigned int atomic_number;
    unsigned int html_cod;
    float atomic_mass;
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

#endif