#include <unistd.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define STD_OUT 1
#define BUF_SIZE 13
#define ERROR -1

int main() {

    ssize_t status;

    status = write(STD_OUT, "Hello world!\n", BUF_SIZE);

    if (status == ERROR)
    {
        return EXIT_FAILURE;
    }
    else
    {
        return EXIT_SUCCESS;
    }
}
