#include <stdio.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define ERROR -1

int main() {

        int status;

        status = printf("Hello world!\n");


        if (status == ERROR)
        {
          return EXIT_FAILURE;
        }

        else
        {
          return EXIT_SUCCESS;
        }
}
