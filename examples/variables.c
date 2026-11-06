#include <stdio.h>
int main(void) {
    int score = 3;
    score = score + 2;
    printf("%d\n", score);
    int a = 4;
    int b = a;
    a = 9;
    printf("%d\n", b);
    return a == 9 ? 0 : 1;
}
