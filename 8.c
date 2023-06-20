#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define TEST 0

typedef struct {
    int day;
    int month;
    int year;
} Date;

/////////////////////////////////////////////////////////////////
// 8.1 funkcja bsearch2

#define FOOD_MAX  30   // max. liczba artykułów
#define ART_MAX   15   // max. długość nazwy artykułu +1
#define RECORD_MAX 40  // max. długość rekordu (linii) w pliku

typedef struct {
    char art[ART_MAX];
    float price;
    float amount;
    Date valid_date;
} Food;


typedef int (*ComparFp)(const void *, const void *);


int food_compare(const void *a, const void *b){
    Food f1 = *(Food*)a;
    Food f2 = *(Food*)b;
    int wynik;
    wynik = strcmp(f1.art, f2.art);
    if (wynik) return wynik;
    if (f1.price > f2.price){
        return 1;
    }
    else if (f1.price < f2.price){
        return -1;
    }
    if (f1.valid_date.year == f2.valid_date.year){
        wynik = (f1.valid_date.month * 30 + f1.valid_date.day - f2.valid_date.month * 30 - f2.valid_date.day);
    }
    else{
        wynik = f1.valid_date.year - f2.valid_date.year;
    }
    return wynik;
}


void *bsearch2(const void *key, void *base, size_t nitems, size_t size, ComparFp compar, int *result, int *pozycja) {
    int l = 0;
    int r = nitems - 1;
    Food *tab = (Food*) base;
    if (nitems == 0){
        *result = 0;
        return tab;
    }
    int pivot;
    while (l <= r){
        pivot = floor((l + r) / 2);
        int wynik = compar(tab+pivot, key);
        if (wynik < 0){
            l = pivot + 1;
            if (l > r){
                *pozycja = l;
            }
        }
        else if (wynik > 0){
            r = pivot - 1;
            if (l > r){
                *pozycja = l;
            }
        }
        if(wynik == 0){
            *pozycja = pivot;
            *result = 1;
            return &(*(tab + pivot));
        }
    }
    *result = 0;
    return &(*(tab + pivot));
}

void print_art(Food *p, size_t n, char *art) {
    for (int i = 0; i < n; i++){
        if (strcmp(p[i].art, art) == 0){
            printf("%.2f %.2f %d %d %d\n", p[i].price, p[i].amount, p[i].valid_date.day, p[i].valid_date.month, p[i].valid_date.year);
        }
    }
}


Food* add_record(Food *tab, size_t tab_size, int *np, ComparFp compar, Food *new) {
    int result, pozycja = 0;
    Food *miejsce = bsearch2(new, tab, *np, sizeof(Food), compar, &result, &pozycja);
    if (result == 1){
        miejsce->amount += new->amount;
    }
    else{
        if (*np == FOOD_MAX) {
            return NULL;
        }
        else{
            if (pozycja == *np){
                tab[pozycja] = *new;
                *np += 1;
            }
            else{
                memmove(tab + pozycja + 1, tab + pozycja, sizeof(Food) * *np);
                tab[pozycja] = *new;
                *np += 1;
            }

        }
    }
    return miejsce;
}


int read_stream(Food *tab, size_t size, int no, FILE *stream) {
    int number_of_elements = 0;
    int *p = &number_of_elements;
    for (int i = 0; i < no; i++){
        Food tmp;
        fscanf(stream, "%s %f %f %d %d %d", tmp.art, &tmp.price, &tmp.amount, &tmp.valid_date.day, &tmp.valid_date.month, &tmp.valid_date.year);
        add_record(tab, size, p, food_compare, &tmp);
    }
}


int date_food_cmp(const void *a, const void *b){
    Food f1 = *(Food*)a;
    Food f2 = *(Food*)b;
    int wynik = f1.valid_date.year - f2.valid_date.year;
    if (wynik) return wynik;
    else{
        return f1.valid_date.month * 30 + f1.valid_date.day - f2.valid_date.month*30 - f2.valid_date.day;
    }
}


int read_stream0(Food *tab, size_t size, int no, FILE *stream) {
    int number_of_elements = 0;
    int *p = &number_of_elements;
    for (int i = 0; i < no; i++){
        Food tmp;
        fscanf(stream, "%s %f %f %d %d %d", tmp.art, &tmp.price, &tmp.amount, &tmp.valid_date.day, &tmp.valid_date.month, &tmp.valid_date.year);
        add_record(tab, size, p, food_compare, &tmp);
    }
    qsort(tab, number_of_elements, sizeof(Food), date_food_cmp);
    return number_of_elements;
}

void add_days(Date *d, int days)
{
    d->day += days;
    if (d->month == 2 && d->day > 28){
        d->day -= 28;
        d->month += 1;
    }
    else if(d->month % 2 == 0 && d->day > 31){
        d->day -= 31;
        d->month += 1;
        if (d->month > 12){
            d->month -= 12;
            d->year += 1;
        }
    }
    else{
        if (d->day > 30){
            d->day -= 30;
            d->month += 1;
        }
    }
}

float value(Food *food_tab, size_t n, Date curr_date, int anticip) {
    int index = 0, result;
    Food tmp;
    add_days(&curr_date, anticip);
    tmp.valid_date = curr_date;
    bsearch2(&tmp, food_tab, n, sizeof(Food), date_food_cmp, &result, &index);
    int i = 1, j = 1;
    float wynik = food_tab[index].amount * food_tab[index].price;
    while (index - j >= 0){
        if (food_tab[index - j].valid_date.year == tmp.valid_date.year && food_tab[index - j].valid_date.month == tmp.valid_date.month && food_tab[index - j].valid_date.day == tmp.valid_date.day){
            wynik += food_tab[index - j].amount * food_tab[index - j].price;
        }
        j += 1;
    }
    while (index + i < n){
        if (food_tab[index + i].valid_date.year == tmp.valid_date.year && food_tab[index + i].valid_date.month == tmp.valid_date.month && food_tab[index + i].valid_date.day == tmp.valid_date.day){
            wynik += food_tab[index + i].amount * food_tab[index + i].price;
        }
        i += 1;
    }
    return wynik;
}

/////////////////////////////////////////////////////////////////
// 8.3 Sukcesja

#define CHILD_MAX  20   // maksymalna liczba przesuwanej w tablicy grupy osób (elementów tablicy)

enum Sex {F, M};
enum BOOL {no, yes};

struct Bit_data {
    enum Sex sex:1;
    enum BOOL pretendent:1;   // =no - nie pretenduje (panuje albo nie żyje) ale jest w zbiorze dla spójności relacji.
};

typedef struct {
    char *name;
    struct Bit_data bits;
    Date born;
    char *parent;
} Person;

typedef struct {
    char *par_name;
    int index;
} Parent;    // strukturę można rozbudować o pole liczby dzieci

const Date primo_date = { 28, 10, 2011 }; // Data zmiany kolejności sukcesji

//int in_tab(Parent *idx_tab, int parent_tab_size, char* key){
//    for (int i = 0; i < parent_tab_size; i++){
//        if (idx_tab[i].par_name != NULL) {
//            if (strcmp(idx_tab[i].par_name, key) == 0) {
//                return 1;
//            }
//        }
//    }
//    return 0;
//}


int find_in_pers_tab(Person *pers_tab, int size, char* key){
    if (key == NULL){
        return -1;
    }
    for (int i = 1; i < size; i++){
        if (strcmp(pers_tab[i].parent, key) == 0){
            return i;
        }
    }
    return -1;
}


int fill_indices_tab(Parent *idx_tab, Person *pers_tab, int size) {
    int par_size = size;
    for (int i = 0; i < size; i++){
        int index = find_in_pers_tab(pers_tab, size, pers_tab[i].name);
        Parent a;
        a.par_name = pers_tab[i].name;
        a.index = index;
        idx_tab[i] = a;
    }
    return par_size;
}

void print_persons(const Person *person_tab, int n) {
    for(int i=1; i<=n; ++i, ++person_tab){
        printf("%2d %12s %s\n", i, person_tab->name, person_tab->parent);
    }
}

void persons_shiftings(Person *person_tab, Parent *idx_tab, int n) {
    for (int i = 0; i < n-1; i++){
        Parent curr_par;
        curr_par = idx_tab[i];
        int j = curr_par.index;
        Person temp_persons[CHILD_MAX];
        Parent temp_parent[CHILD_MAX];
        int a = j;
        if (j > 0){
            int child_number = 0;
            while (a < n && strcmp(person_tab[a].parent, person_tab[i].name) == 0) {
                a++;
                child_number++;
            }
            memcpy(temp_persons, person_tab + j, sizeof(Person) * child_number);
            memcpy(temp_parent, idx_tab + j, sizeof(Parent) * child_number);
            memmove(idx_tab + i + 1 + child_number, idx_tab + i + 1, sizeof(Parent) * (j - i - 1));
            memcpy(idx_tab + i + 1, temp_parent, sizeof(Parent) * child_number);
            for (int z = i + 1; z < n; z++){
                if (idx_tab[z].index < a && idx_tab[z].index >= 0){
                    idx_tab[z].index += child_number;
                }
            }
            memmove(person_tab + i + 1 + child_number, person_tab + i + 1, sizeof(Person) * (j - i - 1));
            memcpy(person_tab + i + 1, temp_persons, sizeof(Person) * child_number);
        }
    }
}

int cleaning(Person *person_tab,int n) {
    for (int i = 0; i < n; i++){
        if (person_tab[i].bits.pretendent == no){
            memmove(person_tab + i, person_tab + i + 1, sizeof(Person) * (n - i));
            n -= 1;
            i -= 1;
        }
    }
    return n;
}

void print_person(const Person *p) {
    printf("%s\n", p->name);
}

int date_persons_cmp(Date a, Date b){
    int wynik = a.year - b.year;
    if (wynik) return wynik;
    else{
        return a.month * 30 + a.day - b.month*30 - b.day;
    }
}

int person_compare(const void *a, const void *b){
    Person p1 = *(Person*) a;
    Person p2 = *(Person*) b;
    if (p1.parent == NULL){
        return -1;
    }
    if (p2.parent == NULL){
        return 1;
    }
    int wynik = strcmp(p1.parent, p2.parent);
    if (wynik) return wynik;
    else {
        if (p1.bits.sex == p2.bits.sex) {
            if (p1.born.year == p2.born.year) {
                return p1.born.month * 30 + p1.born.day - p2.born.month * 30 - p2.born.day;
            }
            return p1.born.year - p2.born.year;
        }
        if (p1.bits.sex == M) {
            if (date_persons_cmp(p1.born, primo_date) < 0) {
                return -1;
            } else {
                return date_persons_cmp(p1.born, p2.born);
            }
        }
        if (p2.bits.sex == M) {
            if (date_persons_cmp(p2.born, primo_date) < 0) {
                return 1;
            } else {
                return date_persons_cmp(p1.born, p2.born);
            }
        }
    }
}

int create_list(Person *person_tab, int n) {
    qsort(person_tab, n, sizeof(Person), person_compare);
    Parent idx_tab[n];
    fill_indices_tab(idx_tab, person_tab, n);
    persons_shiftings(person_tab, idx_tab, n);
    n = cleaning(person_tab, n);
    return n;
}

////////////////////////////////////////////////////////////////

int main(void)
{
    // Wszyscy potomkowie Jerzego VI (w kolejności przypadkowej):
    Person person_tab[33]={
            {"Charles III",M,no,14,11,1948,"Elizabeth II"},
            {"Anne",F,yes,15,8,1950,"Elizabeth II"},
            {"Andrew",M,yes,19,2,1960,"Elizabeth II"},
            {"Edward",M,yes,10,3,1964,"Elizabeth II"},
            {"David",M,yes,3,11,1961,"Margaret"},
            {"Sarah",F,yes,1,5,1964,"Margaret"},
            {"William",M,yes,21,6,1982,"Charles III"},
            {"Henry",M,yes,15,9,1984,"Charles III"},
            {"Peter",M,yes,15,11,1977,"Anne"},
            {"Zara",F,yes,15,5,1981,"Anne"},
            {"Beatrice",F,yes,8,8,1988,"Andrew"},
            {"Eugenie",F,yes,23,3,1990,"Andrew"},
            {"James",M,yes,17,12,2007,"Edward"},
            {"Louise",F,yes,8,11,2003,"Edward"},
            {"Charles",M,yes,1,7,1999,"David"},
            {"Margarita",F,yes,14,5,2002,"David"},
            {"Samuel",M,yes,28,7,1996,"Sarah"},
            {"Arthur",M,yes,6,5,2019,"Sarah"},
            {"George",M,yes,22,7,2013,"William"},
            {"George VI",M,no,14,12,1895,NULL},
            {"Charlotte",F,yes,22,5,2015,"William"},
            {"Louis",M,yes,23,4,2018,"William"},
            {"Archie",M,yes,6,5,2019,"Henry"},
            {"Lilibet",F,yes,4,6,2021,"Henry"},
            {"Savannah",F,yes,29,12,2010,"Peter"},
            {"Isla",F,yes,29,3,2012,"Peter"},
            {"Mia",F,yes,17,1,2014,"Zara"},
            {"Lena",F,yes,18,6,2018,"Zara"},
            {"Elizabeth II",F,no,21,4,1925,"George VI"},
            {"Margaret",F,no,21,8,1930,"George VI"},
            {"Lucas",M,yes,21,3,2021,"Zara"},
            {"Sienna",F,yes,18,9,2021,"Beatrice"},
            {"August",M,yes,9,2,2021,"Eugenie"}
    };

    int to_do, no;
    size_t size, n;
    Food food_tab[FOOD_MAX];
    char buff[ART_MAX];
    FILE *file;
    if(TEST) printf("Wpisz nr zadania (1 - 3) ");
    scanf("%d", &to_do);

    switch (to_do) {
        case 1:  // bsearch2
            if (TEST) printf("Wpisz liczbe linii danych: ");
            scanf("%d",&no); getchar();
            if(TEST) file = stdin;
            else {
                scanf("%s",buff);
                file = fopen(buff,"r");
//            file = fopen("foods0.txt","r");
                if(file==NULL) { printf("Error 1\n"); break; }
            }
            if (TEST) printf("W %d liniach wpisuj dane: nazwa cena ilosc dd mm yyyy: \n",no);
            n = read_stream(food_tab,FOOD_MAX,no,file);
            if (TEST) printf("Wpisz nazwe artykulu: ");
            scanf("%s",buff);
            print_art(food_tab,n,buff);
            break;
        case 2: // proste sortowanie struktur
            if (TEST) printf("Wpisz liczbe linii danych: ");
            scanf("%d",&no); getchar();
            if(TEST) file = stdin;
            else {
                scanf("%s",buff);
                file = fopen(buff,"r");
//            file = fopen("foods0.txt","r");
                if(file==NULL) { printf("Error 1\n"); break; }
            }
            if (TEST) printf("W %d liniach wpisuj dane: nazwa cena ilosc dd mm yyyy: \n",no);
            n = read_stream0(food_tab,FOOD_MAX,no,file);
            Date curr_date;
            int anticip;
            if (TEST) printf("Wpisz date odniesienia dd mm yyyy: ");
            scanf("%d %d %d",&curr_date.day,&curr_date.month,&curr_date.year);
            if (TEST) printf("Wpisz roznice dni: ");
            scanf("%d",&anticip);
            printf("%.2f\n",value(food_tab,n,curr_date,anticip));
            break;
        case 3: // sukcesja
            if(TEST==1) printf("Wpisz pozycję w kolejce (liczona od 1): ");
            scanf("%d",&no);
            int no_persons = sizeof(person_tab) / sizeof(Person);
            no_persons = create_list(person_tab,no_persons);
            if(TEST) print_persons(person_tab,no_persons);
            else print_person(person_tab+no-1);
            break;
        default:
            printf("NOTHING TO DO FOR %d\n", to_do);
    }
    return 0;
}

