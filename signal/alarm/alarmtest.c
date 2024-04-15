#include "types.h"
#include "stat.h"
#include "user.h"

void periodic();

volatile int counter;

int
main(int argc, char *argv[])
{
  int i;
  counter = 0;
  printf(1, "alarmtest starting\n");
  alarm(10, periodic);
  for(i = 0; i < 25*50000000; i++){
    if((i % 2500000) == 0)
      write(2, ".", 1);
  }
  if (counter > 10 && counter < 1000)
    printf(1, "PASSED\n");
  exit();
}

void
periodic()
{
  int cs = 0;
  asm ("mov %%cs, %0" : "=r" (cs));
  if ((cs & 3) != 3) {
    exit(); 
  }

  asm volatile (
    "movl $0, %eax\n\t"
    "movl $0, %ebx\n\t"
    "movl $0, %ecx\n\t"
    "movl $0, %edx\n\t"
    "movl $0, %esi\n\t"
    "movl $0, %edi\n\t");
  int* ebp;
  asm volatile ("movl %%ebp, %0" : "=m" (ebp));

  counter++;

  *ebp = 0;

  printf(1, "alarm!\n");
}
