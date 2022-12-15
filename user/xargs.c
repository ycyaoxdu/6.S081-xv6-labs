#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// readline 从标准输入中读取，每次一行
char* readline() {
    // 申请内存
    char* buf = malloc(100);
    // 取首地址
    char* p = buf;
    // 按字节读
    while(read(0, p, 1) != 0){
        // 换行或结尾（退出条件）
        if(*p == '\n' || *p == '\0'){
            // 标志数组末尾，'\0'
            *p = '\0';
            // 返回buf
            return buf;
        }
        // 遍历
        p++;
    }
    // 如果循环退出但p读到值了（检查遗漏）
    if(p != buf) return buf;
    // 运行到此处还未返回，释放buf后返回
    free(buf);
    return 0;
}

int
main(int argc, char *argv[]){
    if(argc < 2) {
        printf("Usage: xargs [command]\n");
        exit(-1);
    }
    char* l;
    // 过滤 xargs 命令，从下标1开始
    argv++;
    // 定义char指针的数组nargv
    char* nargv[16];
    // pna指向nargv
    char** pna = nargv;
    // pa指向argv（命令行接收参数）
    char** pa = argv;
    // 遍历命令行参数
    while(*pa != 0){
        // 命令行参数赋值到pna
        *pna = *pa;
        pna++;
        pa++;
    }
    // 按行读取并执行
    while((l = readline()) != 0){
        //printf("%s\n", l);
        // 行首字符指针p
        char* p = l;
        // 开辟buf
        char* buf = malloc(36);
        // buf头指针
        char* bh = buf;
        // nargc是除去‘xargs’命令本身之外的命令行参数个数
        int nargc = argc - 1;
        // 遍历p（从标准输入读取到的行）
        while(*p != 0){
            if(*p == ' ' && buf != bh){ // 读到空并且之前读到值了，表明该行读完了
                // 标记数组结束
                *bh = 0;
                // buf首地址指针存入nargv中
                nargv[nargc] = buf;
                // buf重新malloc
                buf = malloc(36);
                // buf指针重新赋值
                bh = buf;
                // nargc自增
                nargc++;
            }else{
                // 读如buf
                *bh = *p;
                // buf指针后移
                bh++;
            }
            // 行指针后移
            p++;
        }
        if(buf != bh){ // buf中还有值
            // buf首地址存入nargv
            nargv[nargc] = buf;
            // nargc自增
            nargc++;
        }
        // 标记nargv数组末尾'\0'
        nargv[nargc] = 0;
        // 释放读取的行
        free(l);

        // fork
        int pid = fork();
        if(pid == 0){
            // printf("%s %s\n", nargv[0], nargv[1]);
            // 子进程，执行命令
            exec(nargv[0], nargv);
        }else{
            // 父进程，等待
            wait(0);
        }
    }
    exit(0);
}
// code参考
// 作者：星见遥
// 链接：https://juejin.cn/post/6908529535268945928
// 来源：稀土掘金
// 著作权归作者所有。商业转载请联系作者获得授权，非商业转载请注明出处。