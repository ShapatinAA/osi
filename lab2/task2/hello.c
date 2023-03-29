
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define SYS_WRITE 1
#define TRUE 1
#define FALSE 0
#define STD_OUTPUT 1
#define BUF_SIZE 14


int main() {

  register int     rdi          asm("rdi") = STD_OUTPUT;                //put stdout descript
  register char*   rsi          asm("rsi") = "Hello, world!\n";         //put string
  register int     rax          asm("rax") = SYS_WRITE;                 //put numer of syscall
  register int     rdx          asm("rdx") = BUF_SIZE;                  //put buf size

  asm("syscall");                                                       //make syscall



  int sys_write_failure;

  if (rax < 0)                                                          //checking status of the error
  {
    sys_write_failure = TRUE;
  }
  else
  {
    sys_write_failure = FALSE;
  }



  if (sys_write_failure == TRUE)
  {
    return EXIT_FAILURE;
  }
  else
  {
    return EXIT_SUCCESS;
  }


}
