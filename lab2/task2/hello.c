
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define SYS_WRITE_CALL_NUMBER 1
#define TRUE 1
#define FALSE 0
#define STD_OUTPUT_DESCRIPTOR 1
#define STRING_SIZE 14


int main() {

  register int     rdi          asm("rdi") = STD_OUTPUT_DESCRIPTOR;             //put stdout descript
  register char*   rsi          asm("rsi") = "Hello, world!\n";                 //put string
  register int     rax          asm("rax") = SYS_WRITE_CALL_NUMBER;             //put numer of syscall
  register int     rdx          asm("rdx") = STRING_SIZE;                       //put buf size

  asm("syscall");                                                               //make syscall


  int sys_write_failure_flag;

  if (rax < 0)                                                                  //checking status of the error
  {
    sys_write_failure_flag = TRUE;
  }
  else
  {
    sys_write_failure_flag = FALSE;
  }



  if (sys_write_failure_flag == TRUE)
  {
    return EXIT_FAILURE;
  }
  else
  {
    return EXIT_SUCCESS;
  }


}
