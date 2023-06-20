#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define SIZE 40

#define TEST 0   // 1 - dla testow,  0 - dla oceny automatycznej

void read_vec(double x[], size_t n) {
    for(size_t i=0; i<n; ++i)  scanf("%lf",x++);
}

void print_vec(const double x[], size_t n) {
    for(size_t i=0; i<n; ++i) printf("%.4f ",x[i]);
    printf("\n");
}

void read_mat(double A[][SIZE], size_t m, size_t n) {
    for(size_t i=0; i<m; ++i) {
        for(size_t j=0; j<n; ++j)  scanf("%lf",&A[i][j]);
    }
}

void print_mat(const double A[][SIZE], size_t m, size_t n) {
    for(size_t i=0; i<m; ++i) {
        for(size_t j=0; j<n; ++j)  printf("%.4f ",A[i][j]);
        printf("\n");
    }
}

double absolute(double x){
    if (x < 0){
        return -x;
    }
    else{
        return x;
    }
}

// 5.2.1 Triangularyzacja, wyznacznik i rozwiazanie Ax=b dla  macierzy kwadratowej.
// Wersja rozszerzona: Wybor wiersza z maksymalna waroscia |elementu glownego|
// Przy wymianie wierszy nalezy zastosowac wetor permutacji indeksow wierszy.
// Jezeli maksymalna wartosc |elementu glownego| < eps, to wyznacznik = 0.
// Zwraca wyznacznik det. Jezeli =0,  to triangularyzacja moze byc niedokonczona.
// Jezeli wyznacznik != 0, i b,x != NULL,
// to w wektorze x umieszcza rozwiazanie ukladu rownan Ax=b.


void add_subtract_row(double A[][SIZE], double b[], size_t n, double scalar, int row1, int row2){
    double value1;
    double value2;
    for (int i = 0; i < n; i++){
        value1 = A[row1][i];
        value2 = scalar * value1;
        A[row2][i] += value2;
    }
    b[row2] += scalar * b[row1];
}




int change_rows(double A[][SIZE], double b[], int wiersze[], int start, int pos, int n, double eps){
    int flag = 0;
    double maxi = A[wiersze[start]][pos];
    int maxi_index = pos;
    for (int i = pos + 1; i < n; i++){
        if (absolute(A[wiersze[i]][pos]) > maxi){
            flag = 1;
            maxi = absolute(A[wiersze[i]][pos]);
            maxi_index = i;
        }
    }
    if (maxi < eps){
        return -1;
    }

    int tmp;
    tmp = wiersze[start];
    wiersze[start] = wiersze[maxi_index];
    wiersze[maxi_index] = tmp;
    if (flag == 1){
        return 1;
    }
    else{
        return 0;
    }
}


void input_value(double x[], double A[][SIZE], double b[], int wiersze[], int pos, int n){
    double value = b[wiersze[pos]];
    for (int i = n - 1; i > pos; i--){
        value += - (A[wiersze[pos]][i]) * x[i];
    }
    x[pos] = value / A[wiersze[pos]][pos];
}


double gauss(double A[][SIZE], double b[], double x[], size_t n, double eps) {
    int ctr = 0;
    int wiersze[n];
    for (int i = 0; i < n; i++){
        wiersze[i] = i;
    }
    double scalar;
    for (int i = 0; i < n - 1; i++){
        int sol = change_rows(A, b, wiersze, i, i, n, eps);
        if(sol == -1){
            return 0;
        }
        if(sol == 1){
            ctr += 1;
        }
        for (int j = i + 1; j < n; j++){
            scalar = -(A[wiersze[j]][i] / A[wiersze[i]][i]);
            add_subtract_row(A, b, n, scalar, wiersze[i], wiersze[j]);
        }
    }
    double wyznacznik = 1;
    for(int i = 0; i < n; i++) {
        wyznacznik *= A[wiersze[i]][i];
    }
    if (wyznacznik != 0){
        for (int i = n - 1; i >= 0; i--){
            input_value(x, A, b, wiersze, i, n);
        }
    }
    if (ctr % 2 == 1){
        return -wyznacznik;
    }
    else{
        return wyznacznik;
    }
}

// 5.2.2
// Zwraca wyznacznik i w tablicy B macierz odwrotna (jezlei wyznacznik != 0)
// Jezeli maksymalna bezwzgledna wartosc elementu glownego < eps,
// to funkcja zwraca wartosc wyznacznika det = 0.
// Funkcja zmienia wartosci takze w tablicy A.

int change_rows_inv(double A[][SIZE], int wiersze[], int start, int pos, int n, double eps){
    int flag = 0;
    double maxi = A[wiersze[start]][pos];
    int maxi_index = pos;
    for (int i = pos + 1; i < n; i++){
        if (absolute(A[wiersze[i]][pos]) > maxi){
            flag = 1;
            maxi = absolute(A[wiersze[i]][pos]);
            maxi_index = i;
        }
    }
    if (maxi < eps){
        return -1;
    }

    int tmp;
    tmp = wiersze[start];
    wiersze[start] = wiersze[maxi_index];
    wiersze[maxi_index] = tmp;
//    double temp = b[start];
//    b[start] = b[maxi_index];
//    b[maxi_index] = temp;
    if (flag == 1){
        return 1;
    }
    else{
        return 0;
    }
}



void add_subtract_row2(double A[][SIZE], double B[][SIZE], size_t n, double scalar, int row1, int row2){
    double value1;
    double value2;
    for (int i = 0; i < n; i++){
        value1 = A[row1][i];
        value2 = scalar * value1;
        A[row2][i] += value2;
        value1 = B[row1][i];
        value2 = scalar * value1;
        B[row2][i] += value2;
    }
}


double matrix_inv(double A[][SIZE], double B[][SIZE], size_t n, double eps) {
    double B_working [SIZE][SIZE];
    int ctr = 0;
    int wiersze[n];
    for (int i = 0; i < n; i++){
        wiersze[i] = i;
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) {
                B_working[i][j] = 1;
            } else {
                B_working[i][j] = 0;
            }
        }
    }
    double scalar;
    for (int i = 0; i < n - 1; i++) {
        int sol = change_rows_inv(A, wiersze, i, i, n, eps);
        if (sol == -1){
            return 0;
        }
        else if(sol == 1){
            ctr++;
        }
        for (int j = i + 1; j < n; j++) {
            scalar = -(A[wiersze[j]][i] / A[wiersze[i]][i]);
            add_subtract_row2(A, B_working, n, scalar, wiersze[i], wiersze[j]);
        }
    }
    double wyznacznik = 1;
    for (int i = 0; i < n; i++){
        wyznacznik *= A[wiersze[i]][i];
    }
    if (wyznacznik == 0){
        return wyznacznik;
    }
    for (int i = 0; i < n; i++){
        double divide;
        divide = A[wiersze[i]][i];
        for (int j = 0; j < n; j++){
            A[wiersze[i]][j] /= divide;
            B_working[wiersze[i]][j] /= divide;
        }
    }
    printf("\n");
    int end = (int)n;
    for (int i = end - 1; i >= 0; i--) {
        for (int j = i - 1; j >= 0; j--) {
            scalar = -(A[wiersze[j]][i] / A[wiersze[i]][i]);
            add_subtract_row2(A, B_working, n, scalar, wiersze[i], wiersze[j]);
        }
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            B[i][j] = B_working[wiersze[i]][j];
        }
    }
    if (ctr % 2 == 1){
        return -wyznacznik;
    }
    else {
        return wyznacznik;
    }
}

int main(void) {

    double A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE][SIZE];
    double b[SIZE], x[SIZE], det, eps = 1.e-13;
    int to_do;
    size_t m,n,p,q;
    if(TEST) printf("Wpisz nr zadania ");
    scanf ("%d", &to_do);
    switch (to_do) {
        case 1:
            if(TEST) printf("Wpisz liczbe wierszy i kolumn mac. kwadratowej: ");
            scanf("%d", &n);
            if(TEST) printf("Wpisz macierz A (wierszami): ");
            read_mat(A,n,n);
            if(TEST) printf("Wpisz wektor b: ");
            read_vec(b,n);
            det = gauss(A, b, x, n, eps);
            printf("%.4f\n",det);
            if(det) print_vec(x,n);
            break;
        case 2:
            if(TEST) printf("Wpisz rozmiar macierzy n = ");
            scanf("%d",&n);
            if(TEST) printf("Wpisz elementy macierzy (wierszami): ");
            read_mat(A,n,n);
            det = matrix_inv(A,B,n,eps);
            printf("%.4f\n",det);
            if(det) print_mat(B,n,n);
            break;
        default:
            printf("NOTHING TO DO FOR %d\n", to_do);
    }
    return 0;
}
