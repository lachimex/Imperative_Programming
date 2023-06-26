#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define BUFFER_SIZE 1024
#define MAX_RATE 4
#define MEMORY_ALLOCATION_ERROR  (-1)

typedef union {
    int int_data;
    char char_data;
    // ... other primitive types used
    void *ptr_data;
} data_union;

typedef struct ht_element {
    struct ht_element *next;
    data_union data;
} ht_element;

typedef void (*DataFp)(data_union);
typedef void (*DataPFp)(data_union*);
typedef int (*CompareDataFp)(data_union, data_union);
typedef size_t (*HashFp)(data_union, size_t);
typedef data_union (*CreateDataFp)(void*);

typedef struct {
    size_t size;
    size_t no_elements;
    ht_element *ht;
    DataFp dump_data;
    CreateDataFp create_data;
    DataFp free_data;
    CompareDataFp compare_data;
    HashFp hash_function;
    DataPFp modify_data;
} hash_table;

// ---------------------- functions to implement

// initialize table fields
void init_ht(hash_table *p_table, size_t size, DataFp dump_data, CreateDataFp create_data,
             DataFp free_data, CompareDataFp compare_data, HashFp hash_function, DataPFp modify_data) {
    p_table->size = size;
    p_table->no_elements = 0;
    p_table->ht = calloc(size, sizeof(ht_element));
    p_table->dump_data = dump_data;
    p_table->create_data = create_data;
    p_table->free_data = free_data;
    p_table->compare_data = compare_data;
    p_table->hash_function = hash_function;
    p_table->modify_data = modify_data;
}

void dump_whole_list(hash_table *p_table){
    for (int i = 0; i < p_table->size; i++){
        ht_element *tmp = p_table->ht + i;
        printf("kubelek numer %d", i);
        if (tmp->next){
            tmp = tmp->next;
            while(tmp){
                p_table->dump_data(tmp->data);
                tmp = tmp->next;
            }
        }
        printf("\n");
    }
}

// print elements of the list with hash n
void dump_list(const hash_table* p_table, size_t n) {
    for (ht_element *tmp = (p_table->ht + n)->next; tmp; tmp = tmp->next){
        p_table->dump_data(tmp->data);
    }

}

// Free element pointed by data_union using free_data() function
void free_element(DataFp free_data, ht_element *to_delete) {
    if (free_data){
        free_data(to_delete->data);
    }
}

// free all elements from the table (and the table itself)
void free_table(hash_table* p_table) {
    for (int i = 0; i < p_table->size; i++){
        ht_element *tmp = (p_table->ht + i);
        tmp = tmp->next;
        while (tmp){
            ht_element *to_del = tmp;
            tmp = tmp->next;
            free_element(p_table->free_data, to_del);
        }
    }
    free(p_table->ht);
}

// calculate hash function for integer k
size_t hash_base(int k, size_t size) {
    static const double c = 0.618033988; // (sqrt(5.) â€“ 1) / 2.;
    double tmp = k * c;
    return (size_t)floor((double)size * (tmp - floor(tmp)));
}

void insert_element(hash_table *p_table, data_union *data);

void rehash(hash_table *p_table) {
    hash_table* new_table = malloc(sizeof(hash_table));
    init_ht(new_table, p_table->size * 2, p_table->dump_data, p_table->create_data, p_table->free_data,
            p_table->compare_data, p_table->hash_function, p_table->modify_data);
    for (int i = 0; i < p_table->size; i++){
        ht_element *tmp = p_table->ht + i;
        if (tmp->next){
            tmp = tmp->next;
        }
        while(tmp){
            insert_element(new_table, &tmp->data);
            tmp = tmp->next;
        }
    }
    *p_table = *new_table;
}

// find element; return pointer to previous
ht_element *find_previous(hash_table *p_table, data_union *data) {
    size_t index = p_table->hash_function(*data, p_table->size) % p_table->size;
    ht_element *tmp = p_table->ht + index;
    if (tmp){
        for ( ; tmp->next; tmp = tmp->next){
            if (p_table->compare_data(tmp->next->data, *data) == 0){
                return tmp;
            }
        }
    }
    return p_table->ht + index;
}

// return pointer to element with given value
ht_element *get_element(hash_table *p_table, data_union *data) {
    size_t index = p_table->hash_function(*data, p_table->size) % p_table->size;
    ht_element *tmp = p_table->ht + index;
    if (tmp->next){
        tmp = tmp->next;
    }
    else{
        return NULL;
    }
    while (tmp){
        if (p_table->compare_data(tmp->data, *data) == 0){
            return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}

// insert element
void insert_element(hash_table *p_table, data_union *data) {
    ht_element *tmp = get_element(p_table, data);
    if (tmp){
        if(p_table->modify_data){
            p_table->modify_data(data);
        }
    }
    else{
        size_t index = p_table->hash_function(*data, p_table->size) % p_table->size;
        ht_element *a = malloc(sizeof(ht_element));
        a->data = *data;
        if (p_table->modify_data){
            p_table->modify_data(data);
        }
        a->next = (p_table->ht + index)->next;
        (p_table->ht + index)->next = a;
        p_table->no_elements++;
    }

    if (p_table->no_elements / p_table->size >= MAX_RATE){
        rehash(p_table);
    }
}

// remove element
void remove_element(hash_table *p_table, data_union data) {
    ht_element *tmp1 = get_element(p_table, &data);
    if (tmp1){
        tmp1 = find_previous(p_table, &data);
    }
    else{
        return;
    }
    ht_element *tmp2;
    tmp2 = tmp1->next;
    tmp1->next = tmp1->next->next;
    free_element(p_table->free_data, tmp2);
}

// type-specific definitions

// int element

size_t hash_int(data_union data, size_t size) {
    return hash_base(data.int_data, size);
}

void dump_int(data_union data) {
    printf("%d ", data.int_data);
}

int cmp_int(data_union a, data_union b) {
    return b.int_data - a.int_data;
}

data_union create_int(void* value) {
    data_union *a = malloc(sizeof(data_union));
    int x;
    scanf(" %d", &x);
    a->int_data = x;
    if (value){
        value = (void*)a;
    }
    return *a;
}

// char element

size_t hash_char(data_union data, size_t size) {
    return hash_base((int)data.char_data, size);
}

void dump_char(data_union data) {
    printf("%c ", data.char_data);
}

int cmp_char(data_union a, data_union b) {
    return a.char_data - b.char_data;
}

data_union create_char(void* value) {
    data_union *a = malloc(sizeof(data_union));
    char x;
    scanf(" %c", &x);
    a->char_data = x;
    if (value){
        value = (void*)a;
    }
    return *a;
}

// Word element

typedef struct DataWord {
    char *word;
    int counter;
} DataWord;

void dump_word(data_union data) {
    DataWord *a = (DataWord*) data.ptr_data;
    printf("%s %d", a->word, a->counter);
}

void free_word(data_union data) {
    free(data.ptr_data);
}

int cmp_word(data_union a, data_union b) {
    DataWord *a1 = (DataWord*) a.ptr_data;
    DataWord *b1 = (DataWord*) b.ptr_data;
    return strcmp(a1->word, b1->word);
}

size_t hash_word(data_union data, size_t size) {
    int s = 0;
    DataWord *dw = (DataWord*)data.ptr_data;
    char *p = dw->word;
    while (*p) {
        s += *p++;
    }
    return hash_base(s, size);
}

void modify_word(data_union *data) {
    DataWord *tmp = (DataWord*)data->ptr_data;
    tmp->counter++;
}

void lowercase_string(char *p){
    size_t leng = strlen(p);
    while(leng > 0){
        *p = tolower(*p);
        p++;
        leng--;
    }
}

data_union create_data_word(void *value) {
    char *p = (char*)value;
    data_union *a = malloc(sizeof(data_union));
    DataWord *b = malloc(sizeof(DataWord));
    a->ptr_data = b;
    lowercase_string(p);
    b->word = malloc(strlen(p + 1));
    strcpy(b->word, p);
    b->counter = 1;
    return *a;
}


// read text, parse it to words, and insert these words to the hashtable
void stream_to_ht(hash_table *p_table, FILE *stream) {
    data_union *data = malloc(sizeof(data_union));
    char *p, buf[BUFFER_SIZE];
    char delimits[] = " \r\t\n.,?!-;";
    while (fgets(buf, BUFFER_SIZE, stream)) {
        p = strtok(buf, delimits);
        while (p) {
            *data = p_table->create_data((void*)p);
            insert_element(p_table, data);
            p = strtok(NULL,delimits);
        }
    }
}

// test primitive type list
void test_ht(hash_table *p_table, int n) {
    char op;
    data_union data;
    for (int i = 0; i < n; ++i) {
        scanf(" %c", &op);
//		p_table->create_data(&data);
        data = p_table->create_data(NULL); // should give the same result as the line above
        switch (op) {
            case 'r':
                remove_element(p_table, data);
                break;
            case 'i':
                insert_element(p_table, &data);
                break;
            default:
                printf("No such operation: %c\n", op);
                break;
        }
    }
}

int main(void) {
    int to_do, n;
    size_t index;
    hash_table table;
    char buffer[BUFFER_SIZE];
    data_union data;

    scanf ("%d", &to_do);
    switch (to_do) {
        case 1: // test integer hash table
            scanf("%d %zu", &n, &index);
            init_ht(&table, 4, dump_int, create_int, NULL, cmp_int, hash_int, NULL);
            test_ht(&table, n);
            printf ("%zu\n", table.size);
            dump_list(&table, index);
            break;
        case 2: // test char hash table
            scanf("%d %zu", &n, &index);
            init_ht(&table, 4, dump_char, create_char, NULL, cmp_char, hash_char, NULL);
            test_ht(&table, n);
            printf ("%zu\n", table.size);
            dump_list(&table, index);
            break;
        case 3: // read words from text, insert into hash table, and print
            scanf("%s", buffer);
            init_ht(&table, 8, dump_word, create_data_word, free_word, cmp_word, hash_word, modify_word);
            stream_to_ht(&table, stdin);
            printf ("%zu\n", table.size);
            data = table.create_data(buffer);
            ht_element *e = get_element(&table, &data);
            if (e) table.dump_data(e->data);
            if (table.free_data) table.free_data(data);
            break;
        default:
            printf("NOTHING TO DO FOR %d\n", to_do);
            break;
    }
    free_table(&table);

    return 0;
}



