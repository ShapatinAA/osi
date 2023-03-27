
#define EXIT_SUCCESS 0
#define EXUT_FAILURE 1

int main() {

  register int     arg1        asm("rdi") = 1;
  register char*   arg2        asm("rsi") = "Hello, world!\n";
  register int     syscall_no  asm("rax") = 1;
  register int     arg3        asm("rdx") = 14;

  asm("syscall");

  return EXIT_SUCCESS;

}
