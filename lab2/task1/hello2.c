#include <unistd.h>
#include <stdio.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define STD_OUT 1
#define BUF_SIZE 13
#define ERR_STATUS -1

int main() {

    sszie_t status;

    status = write(STD_OUT, "Hello world!\n", BUF_SIZE);

    if (status == ERR_STATUS) {

        printf("ERROR OCCURED");
        return EXIT_FAILURE;

    }

    return EXIT_SUCCESS;
}
