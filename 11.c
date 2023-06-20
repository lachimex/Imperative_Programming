#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 1000
#define MEMORY_ALLOCATION_ERROR  -1
#define FILE_OPEN_ERROR          -2
#define LIST_ERROR               -3
#define TEST 0               // 1 dla testowania, 0 dla automatycznej oceny

typedef struct tagListElement {
    struct tagListElement *next;
    void                  *data;
} ListElement;

typedef void (*ConstDataFp)(const void*);
typedef void (*DataFp)(void*);
typedef  int (*CompareDataFp)(const void*,const void*);

typedef struct tagList {
    ListElement *head;
    ListElement *tail;
    int         size;
    ConstDataFp dumpData;
    DataFp      freeData;
    CompareDataFp compareData;
    DataFp      modifyData;
} List;

////////////////////////////////////////////////

// Funkcje pomocnicze
void *safe_malloc(size_t size) {
	void *ptr = malloc(size);
	if (ptr) return ptr;
	printf("malloc error\n");
	exit(MEMORY_ALLOCATION_ERROR);
}
char *safe_strdup(char *str) {
	char *ptr = strdup(str);
	if (ptr) return ptr;
	printf("safe_strdup error\n");
	exit(MEMORY_ALLOCATION_ERROR);
}

////////////////////////////////////////////////////////////////
// Funkcje uniwersalne (ogolnego przeznaczenia) obsługi listy

// Inicjuje listę
void init_List(List *list, ConstDataFp dump, DataFp free, CompareDataFp cmp, DataFp modify) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    list->dumpData = dump;
    list->freeData = free;
    list->compareData = cmp;
    list->modifyData = modify;
}

// Wypisuje dane elemntów listy
void dumpList(const List* list) {
    if(list->dumpData == NULL) return;
    ListElement *i;
    for(i=list->head; i!=0; i=i->next) list->dumpData(i->data);
}

// Zwalnia pamięć elementów listy i danych
void freeList(List *list) {
    ListElement *current = 0;
    current = list->head;
    while(current) {
        ListElement *todelete = current;
        current = current->next;
        if(list->freeData) list->freeData(todelete->data); // zwolnienie pamięci struktury data
        free(todelete);              // zwolnienie pamięci elementu
    }
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

// Uniwersalne funkcje obsługi elementów listy

// Dodaje element na początku listy
void pushFront(List *list, void *data) {
    ListElement* a = safe_malloc(sizeof(ListElement));
    a->data = data;
    if (list->head){
        a->next = list->head;
        list->head = a;
    }
    else{
        list->head = list->tail = a;
        list->tail->next = NULL;
    }
    list->size++;
}

// Dodaje element na końcu listy
void pushBack(List *list, void *data) {
    ListElement* a = safe_malloc(sizeof(ListElement));
    a->data = data;
    if (list->head){
        list->tail->next = a;
        list->tail = a;
    }
    else{
        list->head = list->tail = a;
    }
    list->tail->next = NULL;
    list->size++;
}

// Usuwa pierwszy element listy
void popFront(List *list) {
    if (list->head){
        ListElement* a = list->head;
        list->head = list->head->next;
        free(a);
        list->size--;
    }
    else{
        printf("%d POP FROM EMPTY LIST", LIST_ERROR);
    }
}

// Odwraca kolejność elementów listy
void reverse(List *list) {
    if(list->size > 2){
        ListElement *a = list->head;
        ListElement *b = list->head->next;
        ListElement *c = list->head->next->next;
        list->tail = a;
        a->next = NULL;
        while (c != NULL){
            b->next = a;
            a = b;
            b = c;
            c = c->next;
        }
        b->next = a;
        list->head = b;
    }
    else if(list->size == 2){
        list->head->next = NULL;
        list->tail->next = list->head;
        ListElement *tmp = list->head;
        list->head = list->tail;
        list->tail = tmp;
    }
}

// Funkcje pomocnicze dla list uporzadkowanych
// (porządek wyznacza funkcja wskazana polem compareData)

// Zwraca element w posortowanej liście, a jeżeli nie ma, to element poprzedzający (nie ma, to NULL)
ListElement* findInsertionPoint(const List *list, ListElement *element) {
    ListElement* insertion_point = NULL;
    if (list->head){
        ListElement* i = list->head;
//        list->dumpData(i->data);
        while (i && list->compareData(i->data, element->data) <= 0){
            insertion_point = i;
            i = i->next;
        }
    }
    return insertion_point;
}


// Drugi parametr przekazuje do funkcji adres przydzielonej dla "nowych" danych pamięci.
// Jeżeli takie dane są już w elemencie listy, to dane tego elementu są modyfikowane
// funkcją wskazaną w polu modifyData, a pamięć "nowych" danych jest zwalniana.
// Jeżeli w liście takiego elementu nie ma, to nowy element jest tworzony
// i dopisany do listy zachowując uporządkowanie listy.
void insertInOrder(List *list, void *a) {
    ListElement* new = safe_malloc(sizeof(ListElement));
    new->data = a;
    ListElement * x = findInsertionPoint(list, new);
    if (x == NULL){
        pushFront(list, a);
    }
    else{
        if(list->compareData(x->data, a) == 0){
            if (list->modifyData) list->modifyData(x->data);
            free(a);
            return;
        }
        if (x->next == NULL){
            pushBack(list, a);
            return;
        }
        new->next = x->next;
        x->next = new;
    }
   }


/////////////////// Funkcje do zadania 11.1.2

// Dodaje element na końcu listy bez korzystania z pola tail
void pushBack_v0(List *list, void *data) {
    ListElement *b = safe_malloc(sizeof(ListElement));
    b->data = data;
    b->next = NULL;
    if(list->size > 1){
        ListElement *a = list->head;
        while (a->next != NULL){
            a = a->next;
        }
        b->data = data;
        a->next = b;
    }
    else{
        list->tail = list->head = b;
    }
    list->size++;
}


// Funkcje warstwy obsługi danych wskazywanych w polach struktury List


//////  Dla zadania 11.1.1 i 11.1.2
// 1. Dana jest liczbą typu int

typedef int DataInt;

void dump_int(const void *d) {
    int a = *(int*)d;
    printf("%d ", a);
}

void free_int(void *d) {
    d = (int *) d;
    free(d);
}

int cmp_int(const void *a, const void *b) {
    int x = *(int *) a;
    int y = *(int *) b;
    return x - y;
}

// Przydziela pamięć i zapisuje w niej daną o wartości v
void *create_data_int(int v) {
    int* a = safe_malloc(sizeof(int));
    *a = v;
    return a;
}

//////  Dla zadania 11.1.3 i 11.1.4
// 2. Dana jest zapisana w  strukturze typu DataWord

typedef struct DataWord {
    char *word;
    int counter;
} DataWord;

void dump_word (const void *d) {
    DataWord* a = (DataWord *)d;
    printf("%s ", a->word);
}

void free_word(void *d) {
    free(d);
}

int cmp_word_alphabet(const void *a, const void *b) {
    DataWord* a1 = (DataWord *) a;
    DataWord* b1 = (DataWord *) b;
    return strcasecmp(a1->word, b1->word);
}

int cmp_word_counter(const void *a, const void *b) {
    DataWord* a1 = (DataWord *) a;
    int a2 = *(int *) b;
//    printf("%d\n", a2);
//    printf("porywnywane slowo to %s i ma %d wystapienie\n", a1->word, a1->counter);
    return a1->counter - a2;
}

void modify_word(void *a) {
    DataWord * a1 = (DataWord *) a;
    a1->counter++;
}

void * lowercase_word(void* a){
    void* b = a;
    char *a1 = (char *) a;
    for( ; *a1 != '\0'; a1++){
        *a1 = tolower(*a1);
    }
    return b;
}


// Wypisz dane elementów spełniających warunek równości sprawdzany funkcją
// wskazywaną w polu compareData nagłówka listy
void dumpList_word_if(List *plist, int n) {
    if(plist->dumpData == NULL){
        return;
    }
    ListElement *i;
    for(i=plist->head; i!=0; i=i->next){
        if (plist->compareData(i->data, &n) == 0){
            DataWord* tmp = i->data;
            while (*tmp->word != '\0'){
                printf("%c", tolower(*tmp->word++));
            }
            printf(" ");
            plist->dumpData(i->data);
        }
    }
}

// Przydziela pamięć dla łańcucha string i struktury typu DataWord.
// Do przydzielonej pamięci wpisuje odpowiednie dane.
// Zwraca adres struktury.
void *create_data_word(char *string, int counter) {
    DataWord* a = safe_malloc(sizeof(DataWord));
    a->word = safe_malloc(strlen(string) + 1);
//    a->word = string;
    memcpy(a->word, string, strlen(string) + 1);
    a->counter = counter;
    return a;
}

//////////////////////////////////////////////////

/////////////////// Funkcje pomocnicze dla zadan 11.1.3 i 11.1.4

// Wyrazy ze strumienia wejściowego zapisuje w elementach listy
// w kolejności określonej ostatnim parametrem funkcji (cmp)
// - adresem funkcji porównującej typu CompareDataFp. Adres zerowy oznacza,
// że kolejność elementów listy jest zgodna z kolejnością
void stream_to_list(List *list, FILE *stream, CompareDataFp cmp) {
    DataWord *data;
    char *p, buf[BUFFER_SIZE];
    char delimits[] = " \r\t\n.,?!-;";
    if (cmp) list->compareData = cmp;
    while (fgets(buf, BUFFER_SIZE, stream)) {
        p = strtok(buf, delimits);
        while (p) {
            data = create_data_word(p, 1);
            if (cmp) insertInOrder(list, data);
            else pushBack(list, data);
            p = strtok(NULL, delimits);
        }
    }
}


void list_test(List *plist, int n) {
	char op[2];
	int v;
	ListElement element, *pelement;
	DataInt data;
	for (int i = 0; i < n; ++i) {
        if(TEST) printf("Wpisz kod polecenia ");
		scanf("%s", op);
		switch (op[0]) {
			case 'f': // dodaj na początku listy
                if(TEST) printf("Wpisz wartość ");
				scanf("%d", &v);
				pushFront(plist,create_data_int(v));
				break;
			case 'b': // dodaj na końcu listy
                if(TEST) printf("Wpisz wartość ");
				scanf("%d", &v);
				pushBack(plist,create_data_int(v));
				break;
			case 'd': // usuń pierwszy element
			    popFront(plist);
				break;
            case 'r': // odwróć kolejność
                reverse(plist);
                break;
			case 'i': // dodaj wg porządku rosnącego (int) lub alfabetycznego (char),
			          // jeżeli element już istnieje, to akcja zależna od typu danych
                if(TEST) printf("Wpisz wartość ");
				scanf("%d",&v);
				insertInOrder(plist,create_data_int(v));
                break;
			default:
				printf("No such operation: %s\n", op);
				break;
		}
	}
}

int main(void) {
    int to_do, n, size;
    char buf[257];
    clock_t t_start, t_end;
    double t_total;
    FILE *stream;
    List list;
    ListElement *head;

	if(TEST) printf("Wpisz nr zadania ");
	scanf ("%d", &to_do);
	switch (to_do) {
case 1:
	    if(TEST) printf("Wpisz liczbę poleceń ");
        scanf("%d",&n);
        head = NULL;
        init_List(&list, dump_int, free_int, cmp_int, NULL);
//        list_test(&list, n, int (cmp)(const void*, const void*))
        list_test(&list, n);
        dumpList(&list);
        freeList(&list);
        break;
case 2:
        if(TEST) printf("Wpisz liczbę elementów ");
        scanf("%d",&size);
        init_List(&list, dump_int, free_int, cmp_int, NULL);
        t_start = clock();
        for(int i=0; i<size; ++i) pushBack_v0(&list,create_data_int(i));
        t_end = clock();
        t_total = (double)(t_end - t_start) / CLOCKS_PER_SEC;
        printf("n = %d. Back bez tail.  Czas = %f s.\n",size,t_total);
        freeList(&list);

        init_List(&list, dump_int, free_int, cmp_int, NULL);
        t_start = clock();
        for(int i=0; i<size; ++i) pushFront(&list,create_data_int(i));
        reverse(&list);
        t_end = clock();
        t_total = (double)(t_end - t_start) / CLOCKS_PER_SEC;
        printf("n = %d. Front + revers. Czas = %f s.\n",size,t_total);
        freeList(&list);

        init_List(&list, dump_int, free_int, cmp_int, NULL);
        t_start = clock();
        for(int i=0; i<size; ++i) pushBack(&list,create_data_int(i));
        t_end = clock();
        t_total = (double)(t_end - t_start) / CLOCKS_PER_SEC;
        printf("n = %d. Back z tail.    Czas = %f s.\n",size,t_total);
        freeList(&list);
        break;
case 3:      // wypisz wszystkie słowa wczytane do elementów listy
        init_List(&list,dump_word,free_word,NULL,NULL);
        if(TEST) {
            stream = stdin;
            printf("Wpisz linie tekstu\n");
            stream_to_list(&list, stream, NULL);
            rewind(stream);
        }
        else {
            stream = fopen("Tekst_1.txt","r");
            if(stream == NULL) exit(FILE_OPEN_ERROR);
            stream_to_list(&list, stream, NULL);
            fclose(stream);
        }
        dumpList(&list);
        freeList(&list);
        break;
case 4:      // wypisz w porządku alfabetycznym słowa o zadanej krotności
        if(TEST) printf("Wpisz krotność słowa ");
        scanf("%d",&n);
        init_List(&list,dump_word,free_word,NULL,modify_word);
        if(TEST) {
            stream = stdin;
            printf("Wpisz linie tekstu\n");
            stream_to_list(&list, stream, cmp_word_alphabet);
            rewind(stream);
        }
        else {
            stream = fopen("Tekst_C.txt","r");
            if(stream == NULL) exit(FILE_OPEN_ERROR);
            stream_to_list(&list, stream, cmp_word_alphabet);
            fclose(stream);
        }
        list.compareData = cmp_word_counter;
        dumpList_word_if(&list,n); printf("\n");
        freeList(&list);
        break;
default:
		printf("NOTHING TO DO FOR %d\n", to_do);
	}
    return 0;
}

