#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int i = 0;
    char *title = "jopa";

    argv[0][1] = 'H';
    argv[0] = title;
    sleep(10);

    // go through each string in argv
    // why am I skipping argv[0]?
    for (i = 0; i < argc; i++) {
        printf("arg %d: %s\n", i, argv[i]);
    }

    // let's make our own array of strings
    char *states[] = {
        NULL, argv[0],
        "Washington", "Texas"
    };

    int num_states = 4;

    for (i = 0; i < num_states; i++) {
        printf("state %d: %s\n", i, states[i]);
    }

    return 0;
}
