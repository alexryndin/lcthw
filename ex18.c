#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


void die(const char *message) {
    if (errno) {
        perror(message);
    } else {
        printf("ERROR: %s\n", message);
    }

    exit(1);
}

typedef int (*compare_cb) (int a, int b);

int *bubble_sort(int *numbers, int count, compare_cb cmp) {
    int temp = 0;
    int i = 0;
    int j = 0;
    //int *target = malloc(count * sizeof(int));

    //if (!target) die("Memory error.");
    int *target = numbers;

    for (i = 0; i < count; i++) {
        for (j = 0; j < count - 1; j++) {
            if (cmp(target[j], target[j + 1]) > 0) {
                temp = target[j + 1];
                target[j + 1] = target[j];
                target[j] = temp;
            }
        }
    }

    return target;
}

void dump_or_destroy(unsigned char *p, size_t count, int dd) {
    for (int i = 0; i < count; i++) {
        if (!dd) printf("%02X ", p[i]); else p[i] = i;
    }
    printf("\n");
    return;

}

int sorted_order(int a, int b) {
    return a - b;
}

int reverse_order(int a, int b) {
    return b - a;
}

int strange_order(int a, int b) {
    if (a == 0 || b == 0) {
        return 0;
    } else {
        return a % b;
    }
}


void test_sorting(int *numbers, int count, compare_cb cmp) {
    int i = 0;

    int *sorted = bubble_sort(numbers, count, cmp);
    if (!sorted)
        die("Failed to sord as requested");

    for (i = 0; i < count; i++) {
        printf("%d ", sorted[i]);
    }
    printf("\n");

    //free(sorted);
}

int main(int argc, char *argv[]) {
    if (argc < 2) die("Usage: ex18 [numbers]");

    int count = argc - 1;
    int i = 0;
    char **inputs = argv + 1;

    int *numbers = malloc(count * sizeof(int));
    if (!numbers) die("Memory error.");

    for (i = 0; i < count; i++) {
        numbers[i] = atoi(inputs[i]);
    }

    test_sorting(numbers, count, sorted_order);
    test_sorting(numbers, count, reverse_order);
    test_sorting(numbers, count, strange_order);
    // test_sorting(numbers, count, NULL);
    dump_or_destroy(sorted_order, 25, 1);
    free(numbers);
    dump_or_destroy(numbers, 25, 0);
    return 0;
}
