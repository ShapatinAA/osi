#include <sys/syscall.h>
#include <unistd.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define STD_OUT 1
#define BUF_SIZE 16
#define ERROR -1

ssize_t mywrite(int fd, const void* buf, size_t count) {
        return syscall(SYS_write, fd, buf, count);
}

int main() {

    ssize_t status;

    status = mywrite(STD_OUT, "Hello my world!\n", BUF_SIZE);

    if (status == ERROR)
    {
        return EXIT_FAILURE;
    }

    else
    {
    return EXIT_SUCCESS;
    }
}
