#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>


int* a = NULL;
int sz = 0;
#define START 1
#define SIZE(x, y) (abs((y) - (x)) + 1)
int total_cmp = 0;


static void parse(char* file, int* a)
{
    FILE* f = fopen(file, "r");
    int n;
    int i = START;
    if (!f) {
        printf("Error: File: %s not opened\n", file);
        exit(-1);
    }
    while (fscanf(f, "%d", &n) == 1) {
        a[i++] = n;
    }
    fclose(f);

}

static void swap( int *a, int *b )
{
    int t = *b;
    *b = *a;
    *a = t;
}

static void print_arr(int* a, int l)
{
    int i = 0;
    for (i = START; i <= l; ++i) {
        printf("%d\n", a[i]);
    }
}

static int ChoosePivot(int* a, int start, int end)
{
    // Median of three
    int first = 0;
    int middle = 0;
    int last = 0;
    int mid = start + (end - start) / 2;
    int swapIdx = start;

    first = a[start];
    last = a[end];
    middle = a[mid];
    if( first > last ) {
        if( first > middle ) {
            if( middle > last ) {
                swapIdx = mid;
            } else {
                swapIdx = end;
            }
        } else {
            swapIdx = start;
        }
    } else {
        if( first < middle ) {
            if( middle < last ) {
                swapIdx = mid;
            } else {
                swapIdx = end;
            }
        } else {
            swapIdx = start;
        }
    }
    printf( "first: %d, middle: %d, last: %d, start: %d, midIdx: %d, end: %d, swapIdx: %d\n",
            first, middle, last, start, mid, end, swapIdx );
    swap(&a[start], &a[swapIdx]);
    return a[start];
}

static void partition(int* a, int s, int e)
{
    int p = a[s];
    int i = s + 1;
    int j = s + 1;
    
    for (j = s + 1; j <= e; ++j) {
        if (a[j] < p) {
            swap(&a[i], &a[j]);
            ++i;
        }
    }
    swap(&a[s], &a[i - 1]);
}

static void my_sort(int* a, int start, int end)
{
    int p = 0;
    int l = 0;
    int r = 0;

    if (end <= start) return;
    total_cmp += SIZE(start, end) - 1;

    p = ChoosePivot(a, start, end);
    partition(a, start, end);
    my_sort(a, start, p - 1);
    my_sort(a, p + 1, end);
}

int main(int argc, char* argv[])
{
    char *file  = argv[1];
    int   bytes = 0;
    int num_comparisons = 0;

    sz    = atoi(argv[2]);
    bytes = sizeof(int) * (sz + 1);
    a     = (int *) malloc(bytes);
    printf("File: %s, arr size: %d\n", file, sz);

    memset(a, 0, bytes);
    parse(file, a);
    print_arr(a, sz);
    total_cmp = 0;
    my_sort(a, START, sz);
    printf("Sorted Array:\n");
    print_arr(a, sz);
    printf("Number of comparisons: %d\n", total_cmp);

    return 0;
}
/*******************************************************************/
