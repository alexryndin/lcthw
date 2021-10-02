#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int numbers[4] = { 1, 1, 1, 1  };
    char name[4] = { 'A', 'l', 'e', 'x' };

    // first, print them out raw
    printf("numbers: %d %d %d %d\n",
            numbers[0], numbers[1], numbers[2], numbers[3]);

    printf("name each: %c %c %c %c\n",
            name[0], name[1], name[2], name[3]);

    printf("name: %s\n", name);
    name[0] = -1;
    name[1] = -1;
    name[2] = -1;
    name[3] = -1;
    printf("name as int: %d\n", *(int*)name);
    int nameasstring = -1;
    memcpy(&nameasstring, name, 4);

    printf("name as int #2: %d\n", nameasstring);

    // setup the numbers
    numbers[0] = -1;
    numbers[1] = -1;
    numbers[2] = -1;
    numbers[3] = -1;

    // setup the name
    name[0] = 'A';
    name[1] = 'l';
    name[2] = 'e';
    name[3] = 'x';

    // then print them out initialized
    printf("numbers: %d %d %d %d\n",
            numbers[0], numbers[1], numbers[2], numbers[3]);

    printf("name each: %c %c %c %c\n",
            name[0], name[1], name[2], name[3]);

    // print the name like a string
    printf("name: %s\n", name);

    // another way to use name
    char *another = "eed";

    printf("another: %s\n", another);

    printf("another each: %c %c %c %c\n",
            name[0], name[1], name[2], name[3]);

    return 0;
}
