#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void processor(int fd);

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        fprintf(2, "Usage: primes\n");
        exit(1);
    }
    // set up main process
    int fds[2];
    pipe(fds);

    int pid = fork();

    if (pid != 0)
    {
        close(fds[0]);

        for (int i = 2; i < 36; i++)
        {
            // write as int
            write(fds[1], &i, 4);
        }
        close(fds[1]);
        wait(0);
    }
    else
    {
        close(fds[1]);
        processor(fds[0]);
        close(fds[0]);
    }
    exit(0);
}

// p = get a number from left neighbor
//     print p
//         loop : n = get a number from left neighbor if (p does not divide n)
//                    send n to right neighbor

void processor(int fd)
{
    int n;
    int f = read(fd, &n, 4);
    if (f==0){
        return;
    }
    fprintf(1, "prime:%d\n", n);
    int fds[2];
    pipe(fds);

    int pid = fork();
    if (pid == 0)
    {
        // in child process, close write side
        close(fds[1]);
        //
        processor(fds[0]);
        return;
    }
    else
    {
        // in parent process, close read side
        close(fds[0]);
    }

    int num;
    while (read(fd, &num, 4) != 0)
    {
        if (num % n != 0)
        {
            write(fds[1], &num, 4);
        }
    }
    close(fd);
    close(fds[1]);
    wait(0);
}