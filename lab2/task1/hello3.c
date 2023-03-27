#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define STD_OUT 1
#define BUF_SIZE 16
#define ERR_STATUS -1

ssize_t mywrite(int fd, const void* buf, size_t count) {
        return syscall(SYS_write, fd, buf, count);
}

int main() {

    ssize_t status;

    status = mywrite(STD_OUT, "Hello my world!\n", BUF_SIZE);

    if (status == ERR_STATUS) {

        printf("ERROR OCCURED");
        return EXIT_FAILURE;

    }

    return EXIT_SUCCESS;
}
