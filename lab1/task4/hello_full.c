#include <stdio.h>
#include <dlfcn.h>

int main() {
        void *handle;
        void (*hello_runtime)(void);
        char *error;

        handle = dlopen("./libhello-runtime.so", RTLD_NOW);
        if (!handle) {
            fprintf(stdout, "%s\n", dlerror());
            return 1;
        }

        dlerror();    /* Clear any existing error */

        hello_runtime =  dlsym(handle, "hello_from_dyn_runtime_lib");

        error = dlerror();
        if (error != NULL) {
            fprintf(stdout, "%s\n", error);
            return 1;
        }

        hello_runtime();

        dlclose(handle);
        if (error != NULL) {
            fprintf(stdout, "%s\n", error);
            return 1;
        }
        return 0;
}