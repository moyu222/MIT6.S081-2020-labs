#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

// 读取一行到 \n, 返回读取的字符数
int
getline(char* line){
    int n = 0;
    char c;
    while(1){
        int i = read(0, &c, sizeof(char));
        if(i <= 0) break;
        if(c == '\n') break;
        line[n] = c;
        n++;
    }
    // 涉及到字符串，最后一位更改为\0
    line[n] = '\0';
    return n;
}

int
main(int argc, char *argv[])
{
    if (argc < 2){
       fprintf(2, "Usage: xargs command [argv]\n");
       exit(1);
    }

    // 第一个参数为命令，其他参数也要固定 echo line
    char *command = argv[1];
    char *argvec[MAXARG];
    int i, new_arg_position;

    // 将命令和额外的固定参数复制到 argvec 数组中
    // 以满足 exec 的要求，见笔记例子部分
    for(i = 1; i < argc && i < MAXARG - 1; i++){
        argvec[i-1] = argv[i];
    }

    char line[512];
    char *start, *end;
    int n;
    // 从标准输入读取
    while((n = getline(line)) != 0){
        // 上面for 结束时，i 为 argc
        new_arg_position = i -1;
        start = line;
        end = line;
        for(; (*end) != '\0'; end++){
            if(*end == ' '){
                printf("loop %d, start is %s\n", i, start);
                *end = '\0';
                argvec[new_arg_position++] = start;
                start = end + 1;
            }
        }
        // 注意添加最后一个
        argvec[new_arg_position++] = start;
        // 参数组必须以NULL结尾
        argvec[new_arg_position] = 0;

        if(fork() == 0){
            exec(command, argvec);
            fprintf(2, "exec %s failed\n", command);
            exit(1);
        } else {
            wait((int *) 0);
        }
    }
    exit(0);
}