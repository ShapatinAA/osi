#include <unistd.h>
#include <sys/syscall.h>

ssize_t mywrite(int fd, const void* buf, size_t count) {
        return syscall(SYS_write, fd, buf, count);
}

int main() {

    mywrite(1,"Hello my world!\n", 16);

    return 0;
}