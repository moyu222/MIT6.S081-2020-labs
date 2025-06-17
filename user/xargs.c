#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

int
main(int argc, char *argv[])
{
    if(argc < 2){
        fprintf(2, "Usage: xargs command [argv]\n");
        exit(1);
    }

    char *command = argv[1];
    char *arg_list[MAXARG];
    int fd;

    char buf[1];
    char *p;
    int i = 0;
    p = arg_list[i];
    while(1)
    {
        if(i = MAXARG){
            fprinf(2, "args are too much\n");
            exit(1);
        }

        int read_num = read(0, buf, sizeof(char));
        if(strcmp(buf, '\n') == 0){
            i++;
            p = arg_list[i];
            continue;
        }

        if(read_num == 0){
            break;
        }
        memmove(p, buf, 1);
        p = p + 1;
    }
}