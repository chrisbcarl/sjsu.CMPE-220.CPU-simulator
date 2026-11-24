// gcc ./test/factorial8/factorialof8.c -o ./bin/factorialof8 && ./bin/factorialof8
#include <stdio.h>
//quite literally a factorial loop to find 8!
//holy moly!!!
int main()
{
int num = 1;
for (int i = 1; i < 8; i++) {
    num += num * i;
}
printf("%d\n", num);

return 0;  // 0 for success, non-zero for failure, was num
}
