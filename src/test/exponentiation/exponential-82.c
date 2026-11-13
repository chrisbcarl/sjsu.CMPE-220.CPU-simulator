#include <stdio.h>

const double e_CONSTANT = 2.718281828459045;

int main(void)
{
    double out = 1.0;

    for (int i = 0; i < 82; i++)
    {
        out = out * e_CONSTANT;
    }

    printf("%.15e\n", out);
    return 0;
}
