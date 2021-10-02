#include <stdio.h>

void print_pointers(int argc, char** argv, int* ages) {
    int *cur_age = ages;
    char **cur_name = argv;
    for (cur_name = argv, cur_age = ages;
            (cur_age - ages) < argc; cur_name++, cur_age++) {
        printf("%s lived %d years so far.\n", *cur_name, *cur_age);
    }

}

int main(int argc, char *argv[])
{
    int test[] = { 499999999, 3};
    char *ctest = test;
    for (int i = 0; i < 2; i++) {
        printf("test is %d\n", test[i]);
    }
    for (int i = 0; i < 8; i++) {
        printf("ctest is %d, address is %d\n", *(ctest + i), ctest + i);
    }
    for (int i = 0; i < 2; i++) {
        printf("test is %d, address is %d\n", *(test + i), test + i);
    }
    printf("size of void* is %d, size of void is %d\n", sizeof(void*), sizeof(void));
    // create two arrays we care about
    int ages[] = { 23, 43, 12, 89, 2 };
    char *names[] = {
        "Alan", "Frank",
        "Mary", "John", "Lisa"
    };

    // safely get the size of ages
    int count = sizeof(ages) / sizeof(int);
    int i = 0;

    // first way using indexing
    for (i = count-1; i >= 0; i--) {
        printf("%s has %d years alive.\n", names[i], ages[i]);
    }

    printf("---\n");

    // setup the pointers to the start of the arrays
    int *cur_age = ages;
    char **cur_name = names;

    // second way using pointers
    for (i = count-1 ; i >= 0; i--) {
        printf("%p, %p: %s is %d years old.\n", cur_age+i, cur_name + i,
                *(cur_name + i), *(cur_age + i));
    }

    // second way using pointers
    for (i = 0; i < count; i++) {
        printf("%s is %d years old.\n",
                *(cur_name + i), *(cur_age + i));
    }

    printf("---\n");

    // third way, pointers are just arrays
    for (i = 0; i < count; i++) {
        printf("%s is %d years old again.\n", cur_name[i], cur_age[i]);
    }

    printf("---\n");

    // fourth way with pointers in a stupid complex way
    print_pointers(count, names, ages);

    return 0;
}
