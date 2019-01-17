#include <stdio.h>

typedef struct {
    char c;
    int i;
} mystruct_t;

int main(int argc, char const *argv[])
{
    char s[20];
    sprintf(s, "ABC");
    puts(s);
    sprintf(s, "DEF");
    puts(s);

    return 0;
}
