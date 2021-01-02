#include "util.h"

int gcd(int a, int b) {
    if (b == 0) {
        return a;
    }
    return gcd(b, a % b);
}

int lcm(int input[], int n) {
    int output = input[0];

    for (int i = 0; i < n; ++i) {
        output = ((input[i] * output) / (gcd(input[i], output)));
    }
    return output;
}
