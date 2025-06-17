#include "kernel/types.h"
#include "user/user.h"
#define PRIME_NUM 35
#define READEND 0
#define WRITEEND 1

void
child(int* pl)
{
    // pl 为上一个的右端口，就是当前的左端口
    // 尝试从左端口读取
    int n;
    int read_num = read(pl[READEND], &n, sizeof(int));
    if(read_num == 0) {
        // 递归终止
        exit(0);
    }

    // fork 右端口，判断整除并传给右端口
    int pr[2];
    pipe(pr);
    if(fork() == 0){
        child(pr); // 递归进行
    } else {
        close(pr[READEND]);
        int prime = n;
        printf("prime %d", prime);
        while(read(pl[READEND], &n, sizeof(int)) != 0) {
            if(n%prime != 0) {
                write(pr[WRITEEND], &n, sizeof(int));
            }
        }
        close(pr[WRITEEND]);
        wait((int *) 0); // same as main
        exit(0);
    }
}

int
main(int argc, char *argv[]) 
{
    int p[2]; // 第一次筛选，向图中右边（子进程）写入2-35
    pipe(p);
    if(fork() == 0) {
        child(p);
    } else {
        close(p[READEND]);
        for(int i = 2; i <= 35; i++) {
            write(p[WRITEEND], &i, sizeof(int)); // 直接写入4bytes的int型
        }
        close(p[READEND]);
        wait((int *) 0);
        exit(0);
    }
}