#include <stdio.h>
#include <locale.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>

#include "global.h"

const size_t Main_Table_Size_Const = 24;
const size_t Hash_Table_Size_Const = 71;
const size_t Temp_String_Const     = 1000;

int main() {

    setlocale (LC_CTYPE,"");

    Drug       passport                          = {0};
    List *     hash_table[Hash_Table_Size_Const] = {0};
    Elem_Table main_table[Main_Table_Size_Const] = {0};

    int    counter                       = 0;
    char   tempString[Temp_String_Const] = {0};
    size_t hash                          = 0;

    read_table  (main_table, "file_element_table");
    read_hash_table(hash_table, "item.txt", &counter);

    while (1)
    {
        printf("d - ввести наркотическое вещество\ns - ввести смесь на исследование\ne - выход\n");

        char choice = 0;

        scanf("%c", &choice);

        if (choice == 'd')
        {
            printf("Введите наркотическое вещество:\n");
            passport = input_drugs(&counter, main_table);

            printf("Паспорт введенного вещества:\n");

            print_substance         (passport, main_table);
            drug_name_string        (tempString, passport, main_table);
            hash = hash_computation (tempString);
            hash_table_entry        (hash_table, passport, hash);
        }
        else if (choice == 's')
        {
            printf("Введите примесь: \n");
            passport = input_sample (main_table);
            drug_name_string        (tempString, passport, main_table);
            hash = hash_computation (tempString);

            int found = 0;

            for (size_t i = 0; i < Hash_Table_Size_Const; i++)
            {
                if (i == hash)
                {
                    for (size_t j = 0; j < 5; j++)
                    {
                        if (passport.additive[j] == hash_table[i]->elem.additive[j] && passport.additive[j] != 0)
                        {
                            found = 1;
                            printf("Полученная смесь - наркота!\n");
                            break;
                        }
                    }
                }
            }
            if (!found)
            {
                printf("Полученная смесь - не наркотик!\n");
            }
        }
        else if (choice == 'e')
        {
            break;
        }
    }

    write_hash_table (hash_table, "item.txt");

    return 0;
}

void read_table (Elem_Table *main_table, const char* fileName)
{
    FILE* rFile = fopen(fileName, "r");

    assert (rFile);

    while (!feof(rFile))
    {
        fscanf(rFile, "%s %d %d %lg",
               main_table->element_name,
              &main_table->atomic_number,
              &main_table->html_cod,
              &main_table->atomic_mass);

        main_table++;
    }

    fclose(rFile);
}

void read_hash_table(List *hash_table[], const char *file_name, int* counter)
{
    FILE *file_pointer = fopen(file_name, "r");
    if (!file_pointer) {
        perror("Failed to open file");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file_pointer))
    {
        line[strcspn(line, "\n")] = '\0';

        char *token = strtok(line, " \t");
        int hash = -1;
        List *current = NULL;

        while (token != NULL)
        {
            if (strcasecmp(token, "hash:") == 0)
            {
                token = strtok(NULL, " \t");
                if (token == NULL) {
                    fprintf(stderr, "Error: Missing hash value.\n");
                    break;
                }
                hash = atoi(token);
                if (hash < 0 || hash >= Hash_Table_Size_Const) {
                    fprintf(stderr, "Error: Invalid hash value: %d\n", hash);
                    break;
                }
                if (hash_table[hash] == NULL)
                {
                    hash_table[hash] = (List *)malloc(sizeof(List));
                    hash_table[hash]->next = NULL;
                    memset(&hash_table[hash]->elem, 0, sizeof(Drug));
                }
                current = hash_table[hash];
                token = strtok(NULL, " \t");
            }
            else if (strcasecmp(token, "name:") == 0)
            {
                token = strtok(NULL, " \t");
                if (token == NULL) {
                    fprintf(stderr, "Error: Missing drug name.\n");
                    break;
                }
                strncpy(current->elem.drug_name, token, 10);
                token = strtok(NULL, " \t");
            }
            else if (strcasecmp(token, "add:") == 0)
            {
                ++(*counter);
                int index = 0;
                while ((token = strtok(NULL, " \t")) != NULL && strcasecmp(token, "formula:") != 0)
                {
                    current->elem.additive[index++] = atoi(token);
                }

                if (token == NULL) {
                    fprintf(stderr, "Error: Expected 'formula:' after 'add:'.\n");
                    break;
                }
            }
            else if (strcasecmp(token, "formula:") == 0)
            {
                int formula_index = 0;
                while ((token = strtok(NULL, " \t")) != NULL && strcasecmp(token, "->") != 0)
                {
                    current->elem.drug_formula[formula_index].title = atoi(token);
                    token = strtok(NULL, " \t");
                    if (token == NULL || strcasecmp(token, "->") == 0) {
                        fprintf(stderr, "Error: Incomplete formula data.\n");
                        break;
                    }
                    current->elem.drug_formula[formula_index].amount = atoi(token);
                    formula_index++;
                }
            }
            else if (strcasecmp(token, "->") == 0)
            {
                if (current == NULL) {
                    fprintf(stderr, "Error: Unexpected '->' token.\n");
                    break;
                }
                current->next = (List *)malloc(sizeof(List));
                current = current->next;
                memset(&current->elem, 0, sizeof(Drug));
                current->next = NULL;
                token = strtok(NULL, " \t");
            }
            else
            {
                fprintf(stderr, "Warning: Unknown token '%s'. Skipping.\n", token);
            }
        }
    }

    fclose(file_pointer);
}

void print_table (Elem_Table * main_table, const char* fileName)
{
    FILE* wFile = fopen(fileName, "w");

    assert(wFile);

    for(int i = 0; i < Main_Table_Size_Const; i++)
    {
        fprintf(wFile, "%7s   %d \t%d   %lg\n",
                main_table->element_name,
                main_table->atomic_number,
                main_table->html_cod,
                main_table->atomic_mass);

        main_table++;
    }

    fclose(wFile);
}

size_t searchElem(const char* elemName, Elem_Table* main_table);

Drug input_drugs(int *No, Elem_Table *main_table)
{
    assert(main_table);
    Drug passport = {0};
    sprintf(passport.drug_name, "drug_%d", *No);
    ++(*No);

    size_t numberOfElements = 0;

    printf ("How many elements does the drug contain?\n");
    scanf  ("%lu", &numberOfElements);

    char elem_name[10] = {0};

    for (int i = 0; i < numberOfElements; i++)
    {
        char first_letter;
        int valid_input = 0;
        do
        {
            printf ("Element #%d: Enter first letter of the element: ", i + 1);
            scanf  (" %c", &first_letter);

            while (getchar() != '\n');
        }
        while (!searchElemByFirstLetter(first_letter, main_table));

        int atomic_number = 0;

        printf ("Enter full element name: ");
        scanf  ("%s", elem_name);

        atomic_number = searchElem(elem_name, main_table);
        passport.drug_formula[i].title = atomic_number;

        printf ("Amount:\n");
        scanf  ("%u", &passport.drug_formula[i].amount);
    }

    int j = 0;

    printf("Additive elements: ");

    while (scanf("%s", elem_name) != EOF)
    {
        passport.additive[j] = searchElem(elem_name, main_table);
        j++;
    }

    clearerr(stdin);

    return passport;
}

void print_substance (Drug passport, Elem_Table* main_table)
{
    printf("drug_name: %s\n", passport.drug_name);

    int i = 0;

    while (passport.drug_formula[i].title != 0)
    {
        printf("%c%lc", (passport.drug_formula[i]).title, (passport.drug_formula[i]).amount + 8320);
        i++;
    }

    printf ("\n");
    printf ("additive elemets: ");

    i = 0;

    while (passport.additive[i] != 0)
    {
        printf("%c", passport.additive[i]);

        i++;
    }

    if (i == 0)
    {
        printf("no info about additive elements\n");
    }
    else
    {
        printf("\n");
    }

}

void drug_name_string(char* string, Drug passport, Elem_Table* main_table)
{
    assert(string);
    assert(main_table);

    string[0] = '\0';

    for (int i = 0; i < 10; i++)
    {
        if (passport.drug_formula[i].title == 0) break;

        for (int j = 0; j < Main_Table_Size_Const; j++)
        {
            if (passport.drug_formula[i].title == main_table[j].html_cod)
            {
                strcat(string, main_table[j].element_name);
                break;
            }
        }

        char amount_str[5];
        sprintf(amount_str, "%u", passport.drug_formula[i].amount);
        strcat(string, amount_str);
    }
}

size_t hash_computation(char* string)
{
    assert(string);

    unsigned long long int hash = 0;
    const int prime = 31;
    const int mod = 71;

    for (int i = 0; string[i] != '\0'; i++)
    {
        hash = (hash * prime + string[i]) % mod;
    }
    return (size_t)hash;
}

void intput_table (Elem_Table *main_table)
{
    assert(main_table);
    for(int i = 0; i < 24; i++)
    {
        printf ("element{%d}\nname ", i);
        scanf  ("%s", main_table[i].element_name);

        printf ("atomic_number ");
        scanf  ("%d", &main_table[i].atomic_number);

        printf ("html_cod ");
        scanf  ("%d", &main_table[i].html_cod);

        printf ("atomic_mass ");
        scanf  ("%lg", &main_table[i].atomic_mass);

        printf("\n");
    }
}

Drug input_sample(Elem_Table *main_table)
{
    assert(main_table);
    Drug passport = {0};

    size_t numberOfElements = 0;
    int    c                = 0;

    printf("How many elements does the sample contain?\n");
    c = scanf("%lu", &numberOfElements);

    char elem_name[10] = {0};

    for (int i = 0; i < numberOfElements; i++)
    {
        char first_letter = '\0';
        int  valid_input = 0;
        do
        {
            printf ("Element #%d: Enter first letter of the element: ", i + 1);
            scanf  (" %c", &first_letter);
            while (getchar() != '\n');
        }

        while (!searchElemByFirstLetter(first_letter, main_table));

        int atomic_number = 0;
        int index         = 0;

        do
        {
            printf ("Enter full element name: ");
            scanf  ("%s", elem_name);

            atomic_number = searchElem(elem_name, main_table);
            passport.drug_formula[index++].title = atomic_number;

        } while (atomic_number == 0);

        printf("Amount:\n");
        scanf("%u", &passport.drug_formula[i].amount);
    }

    int j = 0;

    printf("Additive elements: ");

    while (scanf("%s", elem_name) != EOF)
    {
        passport.additive[j] = searchElem(elem_name, main_table);
        j++;
    }

    clearerr(stdin);

    return passport;
}

int searchElemByFirstLetter(char first_letter, Elem_Table *main_table)
{
    int found = 0;

    for (int i = 0; i < Main_Table_Size_Const; i++)
    {
        if (tolower(main_table[i].element_name[0]) == tolower(first_letter))
        {
            printf("%s\n", main_table[i].element_name);
            found = 1;
        }
    }

    if (!found)
    {
        printf("No elements found starting with '%c'. Please try again.\n", first_letter);
    }

    return found;
}

size_t searchElem(const char* elemName, Elem_Table* main_table)
{
    for (int i = 0; i < Main_Table_Size_Const; i++)
    {
        if (strcmp(main_table[i].element_name, elemName) == 0)
        {
            return main_table[i].html_cod;
        }
    }

    printf("No such element in data. Please try again.\n");

    return 0;
}

void print_sample(Drug passport, Elem_Table* main_table)
{
    printf("sample drug formula: ");

    int i = 0;

    while (passport.drug_formula[i].title != 0)
    {
        printf("%c%lc", passport.drug_formula[i].title, passport.drug_formula[i].amount + 8320);
        i++;
    }

    printf("\n");

    printf("additive elemets: ");
    i = 0;

    while (passport.additive[i] != 0)
    {
        printf("%c", passport.additive[i]);
        i++;
    }

    if (i == 0)
    {
        printf("no info about additive elements\n");
    }
    else
    {
        printf("\n");
    }
}

void hash_table_entry(List* hash_table[], Drug passport, unsigned int hash)
{
    assert(hash_table);

    if (hash_table[hash] == NULL)
    {
        hash_table[hash] = (List*) calloc(1, sizeof(List));
        assert(hash_table[hash]);
        hash_table[hash]->elem = passport;
    }
    else
    {
        rec_hash_table_entry(hash_table[hash], passport);
    }
}

void rec_hash_table_entry(List* hash_elem, Drug passport)
{
    assert(hash_elem);

    if (hash_elem->next != nullptr)
    {
        rec_hash_table_entry(hash_elem->next, passport);
    }
    else
    {
        hash_elem->next = (List*)calloc(1, sizeof(List));
        hash_elem->next->elem = passport;
    }
}

void write_hash_table(List* hash_table[], const char* fileName)
{
    assert(hash_table);
    assert(fileName);

    FILE* wFile = fopen(fileName, "w");
    if (!wFile) {
        perror("Failed to open file");
        return;
    }

    for (size_t i = 0; i < Hash_Table_Size_Const; i++)
    {
        if (hash_table[i])
        {
            List* current = hash_table[i];
            fprintf(wFile, "hash: %lu ", i);

            while (current != NULL)
            {
                fprintf(wFile, "name: %s add:", current->elem.drug_name);

                int j = 0;
                while (current->elem.additive[j] != 0)
                {
                    fprintf(wFile, " %d", current->elem.additive[j]);
                    j++;
                }

                fprintf(wFile, " formula:");
                j = 0;
                while (current->elem.drug_formula[j].title != 0)
                {
                    fprintf(wFile, " %d %u", current->elem.drug_formula[j].title,
                            current->elem.drug_formula[j].amount);
                    j++;
                }

                if (current->next != NULL)
                {
                    fprintf(wFile, " -> ");
                }

                current = current->next;
            }

            fprintf(wFile, "\n");
        }
    }

    fclose(wFile);
}

void rec_write_hash_table(List* hash_elem, FILE** wFile)
{
    if (hash_elem == NULL) return;

    fprintf(*wFile, "-> ");

    fprintf(*wFile, "name: %s", hash_elem->elem.drug_name);

    fprintf(*wFile, " add:");
    int j = 0;
    while (hash_elem->elem.additive[j] != 0)
    {
        fprintf(*wFile, " %d", hash_elem->elem.additive[j]);
        j++;
    }

    fprintf(*wFile, " formula:");
    j = 0;
    while (hash_elem->elem.drug_formula[j].title != 0)
    {
        fprintf(*wFile, " %d %u", hash_elem->elem.drug_formula[j].title, hash_elem->elem.drug_formula[j].amount);
        j++;
    }

    fprintf(*wFile, "\n");

    if (hash_elem->next != NULL)
    {
        rec_write_hash_table(hash_elem->next, wFile);
    }
}
