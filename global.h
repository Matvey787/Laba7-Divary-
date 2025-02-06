#ifndef GLOBAL
#define GLOBAL

typedef struct formula {
    int title;
    unsigned int amount;
} Formula;

typedef struct drug {
    char drug_name[10];
    Formula drug_formula [10];
    int additive[5];
} Drug;

#endif