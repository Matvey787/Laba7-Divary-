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
const size_t Temp_String_Const     = 50;

int main() {

    setlocale (LC_CTYPE,"");

    Drug       passport                          = {0};
    List *     hash_table[Hash_Table_Size_Const] = {0};
    Elem_Table main_table[Main_Table_Size_Const] = {0};

    int    counter                       = 0;
    char   tempString[Temp_String_Const] = {0};
    size_t hash                          = 0;

    read_table  (main_table, "file_element_table");
    read_hash_table(hash_table, "item.txt");
    //print_table (main_table, "file_element_table");

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
            // input_sample(main_table);
            passport = input_sample (main_table);
            drug_name_string        (tempString, passport, main_table);
            hash = hash_computation (tempString);

            int found = 0;
// TODO twice cicle are cringe
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

    printf           ("--before hash_table\n");
    write_hash_table (hash_table, "item.txt");
    printf           ("--end of main\n");

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

/* void read_hash_table(List *hash_table[], const char *file_name)
{
    FILE *file_pointer = fopen(file_name, "r");
    if (!file_pointer) {
        perror("Failed to open file");
        return;
    }

    char line[256]; // Буфер для чтения строки
    while (fgets(line, sizeof(line), file_pointer)) // Читаем файл построчно
    {
        char *token = strtok(line, " \n"); // Разбиваем строку на токены
        int hash = -1;
        List *current = NULL;

        while (token != NULL)
        {
            if (strcasecmp(token, "hash:") == 0)
            {
                // Читаем хэш
                token = strtok(NULL, " \n");
                hash = atoi(token);
                if (hash_table[hash] == NULL)
                {
                    // Создаем новый элемент, если его нет
                    hash_table[hash] = (List *)malloc(sizeof(List));
                    hash_table[hash]->next = NULL;
                    memset(&hash_table[hash]->elem, 0, sizeof(Drug));
                }
                current = hash_table[hash];
            }
            else if (strcasecmp(token, "name:") == 0)
            {
                // Читаем имя
                token = strtok(NULL, " \n");
                strncpy(current->elem.drug_name, token, 10);
            }
            else if (strcasecmp(token, "add:") == 0)
            {
                // Читаем аддитивы
                int index = 0;
                while ((token = strtok(NULL, " \n")) != NULL && strcasecmp(token, "formula:") != 0)
                {
                    current->elem.additive[index++] = atoi(token);
                }
            }
            else if (strcasecmp(token, "formula:") == 0)
            {
                // Читаем формулу
                int index = 0;
                while ((token = strtok(NULL, " \n")) != NULL && strcasecmp(token, "->") != 0)
                {
                    current->elem.drug_formula[index].title = atoi(token);
                    token = strtok(NULL, " \n");
                    if (token == NULL) break; // Проверяем, что есть следующее число
                    current->elem.drug_formula[index].amount = atoi(token);
                    index++;
                }
            }
            else if (strcasecmp(token, "->") == 0)
            {
                // Переходим к следующему элементу связного списка
                current->next = (List *)malloc(sizeof(List));
                current = current->next;
                memset(&current->elem, 0, sizeof(Drug));
                current->next = NULL;
            }

            token = strtok(NULL, " \n"); // Переходим к следующему токену
        }
    }

    fclose(file_pointer);
}
 */

 void read_hash_table(List *hash_table[], const char *file_name)
{
    FILE *file_pointer = fopen(file_name, "r");
    if (!file_pointer) {
        perror("Failed to open file");
        return;
    }

    char line[256]; // Буфер для чтения строки
    while (fgets(line, sizeof(line), file_pointer)) // Читаем файл построчно
    {
        char *token = strtok(line, " \n"); // Разбиваем строку на токены
        int hash = -1;
        List *current = NULL;

        while (token != NULL)
        {
            if (strcasecmp(token, "hash:") == 0)
            {
                // Читаем хэш
                token = strtok(NULL, " \n");
                hash = atoi(token);
                if (hash_table[hash] == NULL)
                {
                    // Создаем новый элемент, если его нет
                    hash_table[hash] = (List *)malloc(sizeof(List));
                    hash_table[hash]->next = NULL;
                    memset(&hash_table[hash]->elem, 0, sizeof(Drug));
                }
                current = hash_table[hash];
            }
            else if (strcasecmp(token, "name:") == 0)
            {
                // Читаем имя
                token = strtok(NULL, " \n");
                strncpy(current->elem.drug_name, token, 10);
            }
            else if (strcasecmp(token, "add:") == 0)
            {
                // Читаем аддитивы
                int index = 0;
                while ((token = strtok(NULL, " \n")) != NULL && strcasecmp(token, "formula:") != 0)
                {
                    current->elem.additive[index++] = atoi(token);
                }
                // После обработки add: следующий токен должен быть formula:
                if (token != NULL && strcasecmp(token, "formula:") == 0) {
                    // Обрабатываем формулу
                    int formula_index = 0;
                    while ((token = strtok(NULL, " \n")) != NULL && strcasecmp(token, "->") != 0)
                    {
                        // Читаем title
                        current->elem.drug_formula[formula_index].title = atoi(token);
                        // Читаем amount
                        token = strtok(NULL, " \n");
                        if (token == NULL || strcasecmp(token, "->") == 0) {
                            break;
                        }
                        current->elem.drug_formula[formula_index].amount = atoi(token);
                        formula_index++;
                    }
                }
            }
            else if (strcasecmp(token, "formula:") == 0)
            {
                // Если formula: встретился отдельно (не после add:)
                int formula_index = 0;
                while ((token = strtok(NULL, " \n")) != NULL && strcasecmp(token, "->") != 0)
                {
                    // Читаем title
                    current->elem.drug_formula[formula_index].title = atoi(token);
                    // Читаем amount
                    token = strtok(NULL, " \n");
                    if (token == NULL || strcasecmp(token, "->") == 0) {
                        break;
                    }
                    current->elem.drug_formula[formula_index].amount = atoi(token);
                    formula_index++;
                }
            }
            else if (strcasecmp(token, "->") == 0)
            {
                // Переходим к следующему элементу связного списка
                current->next = (List *)malloc(sizeof(List));
                current = current->next;
                memset(&current->elem, 0, sizeof(Drug));
                current->next = NULL;
            }

            token = strtok(NULL, " \n"); // Переходим к следующему токену
        }
    }

    fclose(file_pointer);
}


/* void read_hash_table(List *hash_table[], const char *file_name)
{
    FILE *file_pointer = fopen(file_name, "r");
    if (!file_pointer) {
        perror("Failed to open file");
        return;
    }

    char line[256]; // Буфер для чтения строки
    while (fgets(line, sizeof(line), file_pointer)) // Читаем файл построчно
    {
        char *token = strtok(line, " \n"); // Разбиваем строку на токены
        int hash = -1;
        List *current = NULL;

        while (token != NULL)
        {
            if (strcasecmp(token, "hash:") == 0)
            {
                // Читаем хэш
                token = strtok(NULL, " \n");
                hash = atoi(token);
                if (hash_table[hash] == NULL)
                {
                    // Создаем новый элемент, если его нет
                    hash_table[hash] = (List *)malloc(sizeof(List));
                    hash_table[hash]->next = NULL;
                    memset(&hash_table[hash]->elem, 0, sizeof(Drug));
                }
                current = hash_table[hash];
            }
            else if (strcasecmp(token, "name:") == 0)
            {
                // Читаем имя
                token = strtok(NULL, " \n");
                strncpy(current->elem.drug_name, token, 10);
            }
            else if (strcasecmp(token, "add:") == 0)
            {
                // Читаем аддитивы
                int index = 0;
                while ((token = strtok(NULL, " \n")) != NULL && strcasecmp(token, "formula:") != 0)
                {
                    current->elem.additive[index++] = atoi(token);
                }
            }
            else if (strcasecmp(token, "formula:") == 0)
            {
                // Читаем формулу
                int index = 0;
                while ((token = strtok(NULL, " \n")) != NULL && strcasecmp(token, "->") != 0)
                {
                    current->elem.drug_formula[index].title = atoi(token);
                    token = strtok(NULL, " \n");
                    current->elem.drug_formula[index].amount = atoi(token);
                    index++;
                }
            }
            else if (strcasecmp(token, "->") == 0)
            {
                // Переходим к следующему элементу связного списка
                current->next = (List *)malloc(sizeof(List));
                current = current->next;
                memset(&current->elem, 0, sizeof(Drug));
                current->next = NULL;
            }

            token = strtok(NULL, " \n"); // Переходим к следующему токену
        }
    }

    fclose(file_pointer);
} */

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

// ------------------------------ input drugs ------------------------------

// Function to input drug details
Drug input_drugs(int *No, Elem_Table *main_table)
{
    assert(main_table);
    // If INPUTSAMPLE is not defined, create a new drug with a unique name
    Drug passport = {0}; // Declare a drug structure

    #ifndef INPUTSAMPLE
        sprintf(passport.drug_name, "drug_%d", *No);
        ++(*No);
    #endif

    size_t numberOfElements = 0;

    printf ("How many elements does the drug contain?\n");
    scanf  ("%lu", &numberOfElements);

    char elem_name[10] = {0};

    // Loop to input each element'string details
    for (int i = 0; i < numberOfElements; i++)
    {
        char first_letter;
        int valid_input = 0;
        do
        {
            printf ("Element #%d: Enter first letter of the element: ", i + 1); // Prompt the user to enter the first letter
            scanf  (" %c", &first_letter);

            while (getchar() != '\n'); // clear input buffer
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

void drug_name_string (char* string, Drug passport, Elem_Table* main_table)
{
    //printf("drug_name_string started\n");
    assert (string);
    assert (main_table);

    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 24; j++)
        {
            if (passport.drug_formula[i].title == main_table[j].atomic_number)
            {
                strcat(string, main_table->element_name);
            }
        }

        int amount = passport.drug_formula[i].amount;
        char str[5];

        sprintf (str, "%d", amount);
        strcat  (string, str);
    }
}

size_t hash_computation (char* string)
{
    assert(string);
    printf("string = %s\n", string);

    unsigned long long int hash        = 0;
    int                    len         = strlen(string);
    int                    temp_cringe = 0;
    int                    amount      = 0;

    for (int i = 0; i < len; i++)
    {
        temp_cringe = (int) (string[i] - '0' + 1);
        amount     += temp_cringe * pow(37, len - i - 1);
        amount      = abs(amount);
    }

    hash = amount % 71;

    printf("hash = %llu\n", hash);

    return hash;
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
    // If INPUTSAMPLE is not defined, create a new drug with a unique name
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
            printf ("Element #%d: Enter first letter of the element: ", i + 1); // Prompt the user to enter the first letter
            scanf  (" %c", &first_letter);
            while (getchar() != '\n'); // clear input buffer
        }

        while (!searchElemByFirstLetter(first_letter, main_table));

        int atomic_number = 0;
        int index         = 0;

        do
        {
            printf ("Enter full element name: ");
            scanf  ("%s", elem_name);

            atomic_number = searchElem(elem_name, main_table);
            //printf("atomic_number = %d\n", atomic_number);
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
            //printf("atomic number: %u\n", main_table[i].atomic_number);
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

 void hash_table_entry (List* hash_table[], Drug passport, unsigned int hash)
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
        hash_elem->next       = (List*)calloc(1, sizeof(List));
        hash_elem->next->elem = passport;
    }
}

void write_hash_table (List* hash_table[], const char* fileName)
{
    assert (hash_table);
    assert (fileName);

    FILE* wFile = fopen(fileName, "w");

    for (size_t i = 0; i < 71; i++)
    {
        if (hash_table[i])
        {
            printf("Записываю в файл: hash: %lu, name: %s\n", i, hash_table[i]->elem.drug_name);
            fprintf(wFile, "hash: %ld name: %s", i, hash_table[i]->elem.drug_name);
            fprintf(wFile, " add:");

            int j = 0;

            while (hash_table[i]->elem.additive[j] != 0)
            {
                fprintf(wFile, " %d", hash_table[i]->elem.additive[j]);
                j++;
            }

            j = 0;

            fprintf(wFile, " formula:");

            while (hash_table[i]->elem.drug_formula[j].title != 0)
            {
                printf  (" %d %u", hash_table[i]->elem.drug_formula[j].title, hash_table[i]->elem.drug_formula[j].amount);
                fprintf (wFile, " %d %u", hash_table[i]->elem.drug_formula[j].title,
                         hash_table[i]->elem.drug_formula[j].amount);

                j++;
            }
            fprintf(wFile, "\n");

            if (hash_table[i]->next != NULL)
            {
                printf               ("start if\n");
                rec_hash_table_entry (hash_table[i]->next, hash_table[i]->next->elem);
                printf               ("end func\n");
            }
        }
    }

    fclose (wFile);
}

void rec_write_hash_table(List* hash_elem, FILE** wFile)
{
    if (hash_elem == NULL) return;

    // Выводим "->" перед каждым следующим элементом списка
    fprintf(*wFile, "-> ");

    // Выводим имя и хэш
    fprintf(*wFile, "name: %s", hash_elem->elem.drug_name);

    // Выводим аддитивы
    fprintf(*wFile, " add:");
    int j = 0;
    while (hash_elem->elem.additive[j] != 0)
    {
        fprintf(*wFile, " %d", hash_elem->elem.additive[j]);
        j++;
    }

    // Выводим формулу
    fprintf(*wFile, " formula:");
    j = 0;
    while (hash_elem->elem.drug_formula[j].title != 0)
    {
        fprintf(*wFile, " %d %u", hash_elem->elem.drug_formula[j].title, hash_elem->elem.drug_formula[j].amount);
        j++;
    }

    fprintf(*wFile, "\n");

    // Рекурсивно обрабатываем следующий элемент списка
    if (hash_elem->next != NULL)
    {
        rec_write_hash_table(hash_elem->next, wFile);
    }
}
