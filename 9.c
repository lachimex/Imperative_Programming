#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#define TAB_SIZE  1000
#define BUF_SIZE  1000

int get(int cols, int row, int col, const int *A) {
    return *(A + row * cols + col);
}

void set(int cols, int row, int col, int *AB, int value) {
    *(AB + row * cols + col) = value;
}

void prod_mat(int rowsA, int colsA, int colsB, int *A, int *B, int *AB) {
    int value = 0;
    for (int i = 0; i < rowsA; i++){
        for (int j = 0; j < colsB; j++){
            for (int k = 0; k < colsA; k++) {
                value += get(colsA, i, k, A) * get(colsB, k, j, B);
            }
            set(colsB, i, j, AB, value);
            value = 0;
        }
    }

}

void read_mat(int rows, int cols, int *t) {
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            scanf("%d", t + i * cols + j);
        }
    }
}

void print_mat(int rows, int cols, int *t) {
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            printf("%d ", *(t + i * cols + j));
        }
        printf("\n");
    }
}

int read_char_lines(char *array[]) {
    char wiersz[BUF_SIZE];
    int index = 0, letter;
    fgets(wiersz, BUF_SIZE, stdin);
    do {
        letter = 0;
        for (int i = 0; i < BUF_SIZE; i++){
            if (wiersz[i] != '\n'){
                letter++;
            }
            else{
                array[index] = malloc(letter * sizeof(char));
                strcpy(array[index++], wiersz);
                break;
            }
        }
    }
    while (fgets(wiersz, BUF_SIZE, stdin) != NULL);
    return index;
}

void write_char_line(char *array[], int n) {
    printf("%s", array[n]);
}

void delete_lines(char *array[]) {
    int index = 0;
    for(;;){
        if (array[index] == NULL) break;
        else{
            free(array[index++]);
        }
    }
}

int read_int_lines_cont(int *ptr_array[]) {
    char c_buf[BUF_SIZE], liczba_char[11];
    int liczba_int[TAB_SIZE], lines = 0, numbers = 1, num_of_numbers = 0, index = 0, char_index = 0;
    ptr_array[lines++] = liczba_int;
    fgets(c_buf, BUF_SIZE, stdin);
    do {
        for (int i = 0; i < BUF_SIZE; i++){
            if (c_buf[i] == ' '){
                liczba_char[char_index] = '\0';
                liczba_int[numbers++] = (int) strtol(liczba_char, NULL, 10);
                num_of_numbers++;
                char_index = 0;
            }
            else if (c_buf[i] == '\n'){
                liczba_char[char_index] = '\0';
                char_index = 0;
                liczba_int[numbers++] = (int) strtol(liczba_char, NULL, 10);
                num_of_numbers++;
                ptr_array[lines++] = liczba_int + numbers;
                liczba_int[index] = num_of_numbers;
                index += num_of_numbers + 1;
                num_of_numbers = 0;
                numbers += 1;
                break;
            }
            else{
                liczba_char[char_index++] = c_buf[i];
            }
        }
    }
    while (fgets(c_buf, BUF_SIZE, stdin) != NULL);
    return lines;
}

void write_int_line_cont(int *ptr_array[], int n) {
    int numbers = *(ptr_array[n]);
    for (int i = 1; i < numbers + 1; i++){
        printf("%d ", *(ptr_array[n] + i));
    }
}

typedef struct {
    int *values;
    int len;
    double average;
} line_type;

int read_int_lines(line_type lines_array[]) {
    char c_buf[BUF_SIZE];
    char liczba_char[11];
    int counter = 0;
    fgets(c_buf, BUF_SIZE, stdin);
    do {
        line_type tmp;
        tmp.len = 0;
        tmp.values = malloc((sizeof(c_buf) / sizeof(char) * sizeof(int) / 2));
        int index = 0;
        for (int i = 0; i < BUF_SIZE; i++){
            if (c_buf[i] == ' '){
                liczba_char[index] = '\0';
                *(tmp.values + tmp.len) = (int)strtol(liczba_char, NULL, 10);
                tmp.len++;
                index = 0;
            }
            else if (c_buf[i] == '\n'){
                liczba_char[index] = '\0';
                *(tmp.values + tmp.len) = (int)strtol(liczba_char, NULL, 10);
                tmp.len++;
                int srednia = 0;
                for (int j = 0; j < tmp.len; j++){
                    srednia += *(tmp.values + j);
                }
                srednia /= tmp.len;
                tmp.average = srednia;
                lines_array[counter++] = tmp;
                break;
            }
            else{
                liczba_char[index++] = c_buf[i];
            }
        }
    }
    while(fgets(c_buf, BUF_SIZE, stdin) != NULL);
    return counter;
}

void write_int_line(line_type lines_array[], int n) {
    line_type line = lines_array[n];
    for (int i = 0; i < line.len; i++){
        printf("%d ", *(line.values + i));
    }
    printf("\n");
    printf("%.2f", line.average);
}

void delete_int_lines(line_type array[], int line_count) {
    for (int i = 0; i < line_count; i++){
        free(array[i].values);
    }
}

int cmp (const void *a, const void *b) {
    line_type line1 = *(line_type*)a;
    line_type line2 = *(line_type*)b;
    if (line1.average > line2.average){
        return 1;
    }
    else return -1;
}

void sort_by_average(line_type lines_array[], int line_count) {
    qsort(lines_array, line_count, sizeof(line_type), cmp);
}

typedef struct {
    int r, c, v;
} triplet;

int read_sparse(triplet *triplet_array, int n_triplets) {
    triplet tmp;
    for (int i = 0; i < n_triplets; i++){
        scanf("%d %d %d", &tmp.r, &tmp.c, &tmp.v);
        triplet_array[i] = tmp;
    }
    return n_triplets;
}

int cmp_triplets(const void *t1, const void *t2) {
    triplet a = *(triplet*) t1;
    triplet b = *(triplet*) t2;
    if (a.r != b.r){
        return a.r - b.r;
    }
    else{
        return a.c - b.c;
    }
}

void make_CSR(triplet *triplet_array, int n_triplets, int rows, int *V, int *C, int *R) {
    qsort(triplet_array, n_triplets, sizeof(triplet), cmp_triplets);
    for (int i = 0; i <= rows; i++){
        R[i] = 0;
    }
    for (int i = 0; i < n_triplets; i++){
        V[i] = triplet_array[i].v;
        C[i] = triplet_array[i].c;
        R[triplet_array[i].r + 1]++;
    }
    for (int i = 0; i < rows; i++){
        R[i + 1] += R[i];
    }
}

void multiply_by_vector(int rows, const int *V, const int *C, const int *R, const int *x, int *y) {
    for (int i = 0; i < rows; i++){
        y[i] = 0;
        for (int j = R[i]; j < R[i+1]; j++){
            y[i] += V[j] * x[C[j]];
        }
    }
}

void read_vector(int *v, int n) {
    for (int i = 0; i < n; i++){
        scanf("%d", v + i);
    }
}

void write_vector(int *v, int n) {
    for (int i = 0; i < n; i++){
        printf("%d ", v[i]);
    }
    printf("\n");
}

int read_int() {
    char c_buf[BUF_SIZE];
    fgets(c_buf, BUF_SIZE, stdin);
    return (int)strtol(c_buf, NULL, 10);
}

int main(void) {
    int to_do = read_int();

    int A[TAB_SIZE], B[TAB_SIZE], AB[TAB_SIZE];
    int n, lines_counter, rowsA, colsA, rowsB, colsB;
    int rows, cols, n_triplets;
    char *char_lines_array[TAB_SIZE] = { NULL };
    int continuous_array[TAB_SIZE];
    int *ptr_array[TAB_SIZE];
    triplet triplet_array[TAB_SIZE];
    int V[TAB_SIZE], C[TAB_SIZE], R[TAB_SIZE];
    int x[TAB_SIZE], y[TAB_SIZE];
    line_type int_lines_array[TAB_SIZE];

    switch (to_do) {
        case 1:
            scanf("%d %d", &rowsA, &colsA);
            read_mat(rowsA, colsA, A);
            scanf("%d %d", &rowsB, &colsB);
            read_mat(rowsB, colsB, B);
            prod_mat(rowsA, colsA, colsB, A, B, AB);
            print_mat(rowsA, colsB, AB);
            break;
        case 2:
            n = read_int() - 1; // we count from 1 :)
            ptr_array[0] = continuous_array;
            read_int_lines_cont(ptr_array);
            write_int_line_cont(ptr_array, n);
            break;
        case 3:
            n = read_int() - 1;
            read_char_lines(char_lines_array);
            write_char_line(char_lines_array, n);
            delete_lines(char_lines_array);
            break;
        case 4:
            n = read_int() - 1;
            lines_counter = read_int_lines(int_lines_array);
            sort_by_average(int_lines_array, lines_counter);
            write_int_line(int_lines_array, n);
            delete_int_lines(int_lines_array, lines_counter);
            break;
        case 5:
            scanf("%d %d %d", &rows, &cols, &n_triplets);
            n_triplets = read_sparse(triplet_array, n_triplets);
            read_vector(x, cols);
            make_CSR(triplet_array, n_triplets, rows, V, C, R);
            multiply_by_vector(rows, V, C, R, x, y);
            write_vector(V, n_triplets);
            write_vector(C, n_triplets);
            write_vector(R, rows + 1);
            write_vector(y, rows);
            break;
        default:
            printf("NOTHING TO DO FOR %d\n", to_do);
            break;
    }
    return 0;
}
