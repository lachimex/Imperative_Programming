#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

int rand_from_interval(int a, int b) {
    if(a > b) return INT_MIN;
    if(a == b) return a;
    if(b - a > RAND_MAX) return INT_MAX;
    int mod = (b - a + 1);
    int solution = (rand() % mod) + a;
    return solution;
}


void rand_permutation(int n, int tab[]) {
    if (n >= 0){
        for(int i = 0; i < n; i++){
            tab[i] = i;
        }
        for(int j = 0; j < n - 1; j++){
            int k = rand_from_interval(j, n - 1);
            int tmp = tab[j];
            tab[j] = tab[k];
            tab[k] = tmp;
        }
    }
}

int cbuff_pop(int cbuff[], int *out, int *leng){
    int sol = cbuff[*out%52];
    *leng -= 1;
    *out += 1;
    return sol;
}


void put_ur_card_at_the_end(int cbuff[], int *out, int *leng, int element){
    cbuff[(*out + *leng) % 52] = element;
    *out += 1;
}


void take_from_opponent(int A[], int *out_a, int *len_a, int B[], int *out_b, int *len_b){
    int element = cbuff_pop(B, out_b, len_b);
    A[(*out_a + *len_a) % 52] = element;
    *len_a += 1;
}


void take_cards(int A[], int *out_a, int *len_a, int B[], int *out_b, int *len_b) {
    put_ur_card_at_the_end(A, out_a, len_a, A[*out_a%52]);
    take_from_opponent(A, out_a, len_a, B, out_b, len_b);
}

int karta(int n){
    return (n - n % 4) / 4;
}


void print(int A[], int *out_a, int *lena, int B[], int *out_b, int *lenb){
    for (int i = *out_a; i < *out_a + *lena; i++){
        printf("%d ", A[i % 52]);
    }
    printf("\n");
    for (int j = *out_b; j < *out_b + *lenb; j++){
        printf("%d ", B[j%52]);
    }
}


void print_tables(int A[], int B[]){
    printf("\n");
    for(int i = 0; i < 52; i++){
        printf("%d ", A[i]);
    }
    printf("\n");
    for(int i = 0; i < 52; i++){
        printf("%d ", B[i]);
    }
    printf("\n");
}


int main(){
    int A[52], B[52], len_a = 26, out_a = 0, len_b = 26, out_b = 0;
    int konflikty, wersja, seed, flag = 0;
    int talia[52];
    for(int i = 0; i < 52; i++){
        talia[i] = i;
    }
    scanf("%d", &seed);
    scanf("%d", &wersja);
    scanf("%d", &konflikty);
    srand(seed);
    rand_permutation(52, talia);
    for(int i = 0; i<26; i++){
        A[i] = talia[i];
        B[i] = talia[i+26];
    }
    printf("\n");
    if(wersja == 1) {
        for (int j = 0; j < konflikty; j++) {
            if (karta(A[*(&out_a)%52]) > karta(B[*(&out_b)%52])) {
                flag = 1;
                take_cards(A, &out_a, &len_a, B, &out_b, &len_b);
            } else if (karta(A[*(&out_a)%52]) < karta(B[*(&out_b)%52])) {
                flag = 1;
                take_cards(B, &out_b, &len_b, A, &out_a, &len_a);
            }
            else{
                put_ur_card_at_the_end(A, &out_a, &len_a, A[*(&out_a)%52]);
                put_ur_card_at_the_end(B, &out_b, &len_b, B[*(&out_b)%52]);
            }
            if (*(&len_a) == 0) {
                printf("%d\n", 3);
                print(A, &out_a, &len_a, B, &out_b, &len_b);
                return 0;
            }
            if (*(&len_b) == 0) {
                printf("%d ", 2);
                printf("%d", j + 1);
                return 0;
            }
        }
        if(flag == 0){
            printf("%d ", 0);
            printf("%d ", *(&len_a));
            printf("%d", *(&len_b));

        }
        else{
            printf("%d ", 1);
            printf("%d ", *(&len_a));
            printf("%d", *(&len_b));
        }
    }
    else {
        int nr_of_conflicts = 0;
        for (int j = 0; j < konflikty; j++) {
            nr_of_conflicts++;
            if (karta(A[*(&out_a) % 52]) > karta(B[*(&out_b) % 52])) {
                flag = 1;
                take_cards(A, &out_a, &len_a, B, &out_b, &len_b);
            } else if (karta(A[*(&out_a) % 52]) < karta(B[*(&out_b) % 52])) {
                flag = 1;
                take_cards(B, &out_b, &len_b, A, &out_a, &len_a);
            } else {
                int index = 2;
                while(karta(A[*(&out_a) + index % 52]) == karta(B[*(&out_a) + index % 52])){
                    index += 2;
                }
                if(index + 1> *(&len_a) || index + 1> *(&len_b)){
                    printf("%d ", 1);
                    printf("%d ", *(&len_a));
                    printf("%d ", *(&len_b));
                    return 0;
                }
                else{
                    if (karta(A[*(&out_a) + index % 52]) > karta(B[*(&out_b) + index % 52])) {
                        flag = 1;
                        for(int i = 0; i < index / 2 + 2; i++){
                            put_ur_card_at_the_end(A, &out_a, &len_a, A[*(&out_a) % 52]);
                        }
                        for(int i = 0; i < index / 2 + 2; i++){
                            take_from_opponent(A, &out_a, &len_a, B, &out_b, &len_b);
                        }
                        nr_of_conflicts += index / 2;
                    } else if (karta(A[*(&out_a) + index % 52]) < karta(B[*(&out_b) + index % 52])) {
                        flag = 1;
                        for(int i = 0; i < index / 2 + 2; i++){
                            put_ur_card_at_the_end(B, &out_b, &len_b, B[*(&out_b) % 52]);
                        }
                        for(int i = 0; i < index / 2 + 2; i++){
                            take_from_opponent(B, &out_b, &len_b, A, &out_a, &len_a);
                        }
                        nr_of_conflicts += index / 2;
                    }

                }
            }
            if (*(&len_a) == 0) {
                printf("%d\n", 3);
                print(A, &out_a, &len_a, B, &out_b, &len_b);
                return 0;
            }
            if (*(&len_b) == 0) {
                printf("%d ", 2);
                printf("%d", nr_of_conflicts);
                return 0;
            }
        }
        printf("%d ", 0);
        printf("%d ", *(&len_a));
        printf("%d ", *(&len_b));
    }
}