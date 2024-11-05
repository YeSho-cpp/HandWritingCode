#include <stdio.h>

void coredump(void)
{
    int *null_ptr = NULL;
    *null_ptr = 10;
}

int main(){
    // 对空指针指向的内存区域写，会发生段错误 -->产生core文件

    printf("hello coredump\n");
    coredump();
    printf("will be never reached\n");
    return 0;
}