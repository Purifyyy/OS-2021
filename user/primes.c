#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
next_prime(int read_parent)
{
  int prime, n, to_child[2], pid;
  // Get num from parent, if write side of pipe closed, close read side, exit, meaning no numbers left
  if(read(read_parent, &prime, sizeof(prime)) == 0){
    close(read_parent);
    exit(0);
  }
  printf("prime %d\n",prime);
  // Create new pipe for new child process
  if(pipe(to_child) < 0){
    printf("primes: pipe() failed\n");
    exit(1);
  }
  // Create another child process
  pid = fork();
  if(pid < 0){
    printf("primes: fork() failed\n");
    exit(1);
  }
  if(pid == 0){
   // Child closes write side of the pipe and sends the read side through to function
   close(to_child[1]);
   next_prime(to_child[0]);
  } else {
    // New parent closes read side of the pipe, while reads numbers from his parent, when the write side of parent is closed, close read side of parent, and write side to your child aswell
    close(to_child[0]);
    while(read(read_parent, &n, sizeof(n)) > 0){
      // If number read is prime, send it to child process, drop the number otherwise 
      if(n % prime != 0){
        write(to_child[1], &n, sizeof(n));
      }
    }
    close(read_parent);
    close(to_child[1]);
  }
  // Wait for child process to exit
  wait(0);
  exit(0);
} 

int
main(int argc, char *argv[])
{
  int to_child[2], num, pid;
  
  if(pipe(to_child) < 0){
    printf("primes: pipe() failed\n");
    exit(1);
  }
  
  pid = fork();
  if(pid < 0){
    printf("primes: fork() failed\n");
    exit(1);
  }
  if(pid == 0){
    // Child closes write side of pipe, sends fd of read side of pipe into function
    close(to_child[1]);
    next_prime(to_child[0]);
  } else {
    // Parent closes the read side of pipe, starts sending numbers through, and closes write side afterwards
    close(to_child[0]);
    for(num = 2; num < 36; num++){
      write(to_child[1], &num, sizeof(num));	
    }
    close(to_child[1]);
  }
  // Wait for child process to exit
  wait(0);
  exit(0);
}
