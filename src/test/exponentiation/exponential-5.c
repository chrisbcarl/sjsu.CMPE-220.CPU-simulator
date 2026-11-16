#include <stdio.h>

int main(void)
{
    int x = 82;
    int scale = 1000;
    int term = scale;
    int out = scale;

    for (int i = 1; i <= 30; i++)
    {
        term = (term * x + i / 2) / i;
        out = out + term;

        if (term == 0)
        {
            break;
        }
    }

    out = (out + scale / 2) / scale;
    printf("Result is %d\n", out);
    return 0;
}
