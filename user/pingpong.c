#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        fprintf(2, "Usage: pingpong\n");
        exit(1);
    }

    int pipe1[2], pipe2[2], pid, n;

    // fork
    pid = fork();

    // init pipe
    if (pipe(pipe1) < 0)
    {
        printf("pipe1() failed\n");
        exit(1);
    }
    if (pipe(pipe2) < 0)
    {
        printf("pipe2() failed\n");
        exit(1);
    }

    // write to child
    n = write(pipe1[0], "i", 1);
    if (n > 0)
    {
        fprintf(2, "error write to pipe1");
        exit(1);
    }
    close(pipe1[0]);
    close(pipe1[1]);

    static char buf[10];

    if (pid == 0)
    {
        // child
        //
        // recive "ping" from pipe1, print "<process_id>: received ping"  use pipe2
        // write "pong" from pipe2
        int n = read(pipe1[1], buf, 1);
        if (n > 0)
        {
            fprintf(2, "error read from pipe1");
            exit(1);
        }
        fprintf(1, "%d: received ping\n", getpid());

        // write to child
        n = write(pipe2[0], "o", 1);
        if (n > 0)
        {
            fprintf(2, "error write to pipe2");
            exit(1);
        }
        close(pipe2[0]);
        close(pipe2[1]);
    }

    wait(0);

    if (pid != 0)
    {
        // parent
        //
        // write to child "ping" use pipe1
        // wait child
        // read byte from pipe2, then print "<process_id>: received pong"

        // wait for child to
        n = read(pipe2[1], buf, 1);
        if (n > 0)
        {
            fprintf(2, "error read from pipe2");
            exit(1);
        }
        fprintf(1, "%d: received pong\n", getpid());
    }

    exit(0);
}
