#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void
find(char *path, char *name)
{
    int fd;
    char buf[512], *p; // 处理文件名
    struct stat st; // 文件元数据，判断类型
    struct dirent de; // 目录项

    if((fd = open(path, 0)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }
            // 忽略 . ..防止无限递归
             // stat(const char *, struct stat *)
    // 按块读取目录项
    while(read(fd, &de, sizeof(de)) == sizeof(de)) {
        strcpy(buf, path);
        p = buf+strlen(buf);
        *p++ = '/';

        // 一个未使用的目录项
        /*
        inum == 0 表示这个目录项是“空槽位”，在文件系统中并没有有效内容，
        甚至 de.name 里面可能都是垃圾数据或者未初始化的值。
        de.inum的判断要在判断. 和 .. 之前；这里自然完成了
        */
        if(de.inum == 0)
            continue;
        // 将目录项文件名拷贝到p所指处
        // 不使用strcpy，因为de.name不一定null结尾
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;

        // 开始判断FILE 和 DIR
        if(stat(buf, &st) < 0){
            fprintf(2, "find: cannot stat %s\n", buf);
        }

        switch(st.type){
        case T_FILE:
            // 巧妙使用de.name
            if(strcmp(name, de.name) == 0) {
                printf("%s\n", buf);
            }
            break;
        case T_DIR:
            if((strcmp(de.name, "." != 0) && (strcmp(de.name, "..") != 0))){
                find(buf, name);
            }
        }
    }
    
    // 关闭目录
    close(fd);
    return;
}

int
main(int argc, char *argv[])
{
    if(argc != 3) {
        fprintf(2, "Usage: find path name\n");
        exit(1);
    }

    char *path = argv[1];
    char *name = argv[2];

    find(path, name);
    exit(0);
}