#include <stdio.h>
//quite literally a factorial loop to find 8!
//holy moly!!!
int fact8()
{
int num = 1;
for (int i = 1; i < 8; i++) {
    num += num * i;
}
printf("%d", num);
 
return num;
}
