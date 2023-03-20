#include <unistd.h>
#include <sys/syscall.h>

int main() {

    write(1, "Hello world!\n", 13);
    syscall(SYS_write, 1, "Hello my world!\n", 16);

    return 0;
}