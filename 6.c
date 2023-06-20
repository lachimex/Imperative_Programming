#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_REL_SIZE 100
#define MAX_RANGE 100

typedef struct {
    int first;
    int second;
} pair;


// Case 1:

int add_to_set(int *set, int size, int value){
    for (int i = 0; i < size; i++){
        if (set[i] == value){
            return -1;
        }
    }
    set[size] = value;
    return 1;
}

int get_domain(pair *relation, int size, int *domain){
    int set[size], set_size = 0;
    pair working_pair;
    for (int i = 0; i < size; i++) {
        working_pair = relation[i];
        if (add_to_set(set, set_size, working_pair.first) == 1) set_size++;
        if (add_to_set(set, set_size, working_pair.second) == 1) set_size++;
    }
    for (int i = 1; i < set_size; i++){
        for (int j = 0; j < set_size - i; j++){
            if (set[j] > set[j+1]){
                int tmp = set[j];
                set[j] = set[j+1];
                set[j+1] = tmp;
            }
        }
    }
    for (int i = 0; i < set_size; i++){
        domain[i] = set[i];
    }
    return set_size;
}


// The relation R is reflexive if xRx for every x in X
int is_reflexive(pair *relation, int size){
    int set1[size], set_size1 = 0, set2[size], set_size2 = 0;
    pair working_pair;
    for (int i = 0; i < size; i++){
        working_pair = relation[i];
        if (add_to_set(set1, set_size1, working_pair.first) == 1) set_size1++;
        if (add_to_set(set1, set_size1, working_pair.second) == 1) set_size1++;
        if (working_pair.first == working_pair.second){
            if (add_to_set(set2, set_size2, working_pair.first) == 1){
                set_size2++;
            }
        }
    }
    if (set_size1 == set_size2){
        return 1;
    }
    else return 0;
}

// The relation R on the set X is called irreflexive
// if xRx is false for every x in X
int is_irreflexive(pair *relation, int size){
    pair working_pair;
    for (int i = 0; i < size; i++){
        working_pair = relation[i];
        if (working_pair.first == working_pair.second) return 0;
    }
    return 1;
}

// A binary relation R over a set X is symmetric if:
// for all x, y in X xRy <=> yRx
int is_symmetric(pair *relation, int size){
    pair working_pair;
    int true_false[size];
    for (int i = 0; i < size; i++){
        working_pair = relation[i];
        true_false[i] = 0;
        for (int j = 0; j < size; j++){
            if (relation[j].first == working_pair.second && relation[j].second == working_pair.first) true_false[i] = 1;
        }
    }
    for (int i = 0; i < size; i++){
        if (true_false[i] == 0) return 0;
    }
    return 1;
}

// A binary relation R over a set X is antisymmetric if:
// for all x,y in X if xRy and yRx then x=y
int is_antisymmetric(pair *relation, int size){
    pair working_pair;
    for (int i = 0; i < size; i++){
        working_pair = relation[i];
        for (int j = 0; j < size; j++){
            if (relation[j].first == working_pair.second && relation[j].second == working_pair.first){
                if (relation[j].first != working_pair.second) return 0;
            }
        }
    }
    return 1;
}

// A binary relation R over a set X is asymmetric if:
// for all x,y in X if at least one of xRy and yRx is false
int is_asymmetric(pair *relation, int size){
    pair working_pair;
    for (int i = 0; i < size; i++){
        working_pair = relation[i];
        for (int j = 0; j < size; j++){
            if (relation[j].first == working_pair.second && relation[j].second == working_pair.first){
                return 0;
            }
        }
    }
    return 1;
}

// A homogeneous relation R on the set X is a transitive relation if:
// for all x, y, z in X, if xRy and yRz, then xRz
int is_transitive(pair *relation, int size){
    pair working_pair1;
    pair working_pair2;
    for (int i = 0; i < size; i++){
        working_pair1 = relation[i];
        for (int j = 0; j < size; j++){
            if (j != i && working_pair1.second == relation[j].first){
                working_pair2 = relation[j];
                int flag = 0;
                for (int k = 0; k < size; k++){
                    if (relation[k].first == working_pair1.first && relation[k].second == working_pair2.second) {
                        flag = 1;
                        break;
                    }
                }
                if (flag == 0) return 0;
            }
        }
    }
    return 1;
}

// Case 2:

// A partial order relation is a homogeneous relation that is
// reflexive, transitive, and antisymmetric
int is_partial_order(pair *relation, int size){
    if (is_reflexive(relation, size) == 1 && is_transitive(relation, size) == 1 && is_antisymmetric(relation, size) == 1){
        return 1;
    }
    return 0;
}


// Relation R is connected if for each x, y in X:
// xRy or yRx (or both)
int is_connected(pair *relation, int size){
    int domain[size], domain_size;
    domain_size = get_domain(relation, size, domain);
    for (int i = 0; i < domain_size - 1; i++){
        int tmp1 = domain[i];
        for (int k = i + 1; k < domain_size; k++){
            int tmp2 = domain[k];
            int flag = 0;
            for (int j = 0; j < size; j++){
                pair working_pair;
                working_pair = relation[j];
                if (working_pair.first == tmp1 && working_pair.second == tmp2 || working_pair.second == tmp1 && working_pair.first == tmp2) flag = 1;
            }
            if (flag == 0) return 0;
        }
    }
    return 1;
}

// A total order relation is a partial order relation that is connected
int is_total_order(pair *relation, int size){
    if (is_partial_order(relation, size) == 1 && is_connected(relation, size) == 1) return 1;
    else return 0;
}


int find_max_elements(pair *relation, int size, int *max_elements){
    int domain[MAX_REL_SIZE];
    int domain_size = get_domain(relation, size, domain);
    int how_many_max_elements = 0;
    pair working_pair;
    for (int i = 0; i < domain_size; i++){
        int element = domain[i];
        int flag = 1;
        for (int j = 0; j < size; j ++) {
            working_pair = relation[j];
            if (working_pair.second != element && working_pair.first == element) flag = 0;
        }
        if (flag == 1){
            max_elements[how_many_max_elements] = element;
            how_many_max_elements++;
        }
    }
    return how_many_max_elements;
}
int find_min_elements(pair *relation, int size, int *min_elements){
    int domain[MAX_REL_SIZE];
    int domain_size = get_domain(relation, size, domain);
    int how_many_min_elements = 0;
    pair working_pair;
    for (int i = 0; i < domain_size; i++){
        int element = domain[i];
        int flag = 1;
        for (int j = 0; j < size; j ++) {
            working_pair = relation[j];
            if (working_pair.first != element && working_pair.second == element) flag = 0;
        }
        if (flag == 1){
            min_elements[how_many_min_elements] = element;
            how_many_min_elements++;
        }
    }
    return how_many_min_elements;
}


// Add pair to existing relation if not already there
int add_relation (pair *tab, int n, pair new_pair) {
    for (int i = 0; i < n; i++){
        pair a = tab[i];
        if (new_pair.first == a.first && new_pair.second == a.second){
            return -1;
        }
    }
    tab[n] = new_pair;
    return 1;
}


// Case 3:

int composition (pair *relation1, int size1, pair *relation2, int size2, pair *out_relation){
    int out_relation_size = 0;
    pair working_pair1;
    pair working_pair2;
    for (int i = 0; i < size1; i++){
        working_pair1 = relation1[i];
        for(int j = 0; j < size2; j++){
            working_pair2 = relation2[j];
            if (working_pair1.second == working_pair2.first){
                pair a;
                a.first = working_pair1.first;
                a.second = working_pair2.second;
                if (add_relation(out_relation, out_relation_size, a) == 1){
                    out_relation_size++;
                }
            }
        }
    }
    return out_relation_size;
}

// Comparator for pair
int cmp_pair (const void *a, const void *b) {
}

int insert_int (int *tab, int n, int new_element) {
}



// Read number of pairs, n, and then n pairs of ints
int read_relation(pair *relation) {
    int n;
    int size = 0;
    pair new_pair;
    scanf("%d", &n);
    for (int i = 0; i < n; i++){
        scanf("%d%d", &(new_pair.first), &(new_pair.second));
        if (add_relation(relation, size, new_pair) == 1){
            size++;
        }
    }
    return size;
}

void print_int_array(int *array, int n) {
    printf("\n%d\n", n);
    for (int i = 0; i < n; i++){
        printf("%d ", array[i]);
    }
}

int main(void) {
    int to_do;
    pair relation[MAX_REL_SIZE];
    pair relation_2[MAX_REL_SIZE];
    pair comp_relation[MAX_REL_SIZE];
    int domain[MAX_REL_SIZE];
    int max_elements[MAX_REL_SIZE];
    int min_elements[MAX_REL_SIZE];

    scanf("%d",&to_do);
    int size = read_relation(relation);
    int ordered, size_2, n_domain;

    switch (to_do) {
        case 1:
            printf("%d ", is_reflexive(relation, size));
            printf("%d ", is_irreflexive(relation, size));
            printf("%d ", is_symmetric(relation, size));
            printf("%d ", is_antisymmetric(relation, size));
            printf("%d ", is_asymmetric(relation, size));
            printf("%d\n", is_transitive(relation, size));
            break;
        case 2:
            ordered = is_partial_order(relation, size);
            n_domain = get_domain(relation, size, domain);
            printf("%d %d", ordered, is_total_order(relation, size));
            print_int_array(domain, n_domain);
            if (!ordered) break;
            int no_max_elements = find_max_elements(relation, size, max_elements);
            int no_min_elements = find_min_elements(relation, size, min_elements);
            print_int_array(max_elements, no_max_elements);
            print_int_array(min_elements, no_min_elements);
            break;
        case 3:
            size_2 = read_relation(relation_2);
            printf("%d\n", composition(relation, size, relation_2, size_2, comp_relation));
            break;
        default:
            printf("NOTHING TO DO FOR %d\n", to_do);
            break;
    }
    return 0;
}
