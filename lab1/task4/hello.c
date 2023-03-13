#include <stdio.h>
#include <dlfcn.h>

int main() {
        void *handle;
        void (*hello_runtime)(void);

        handle = dlopen("./libhello-runtime.so", RTLD_NOW);

        hello_runtime =  dlsym(handle, "hello_from_dyn_runtime_lib");

        hello_runtime();

        dlclose(handle);

        return 0;
}
