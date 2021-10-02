#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

int
main(int argc, char *argv[])
{
  int x, i = 0, arg_i = 0, dig, flag = 0, pid;
  char *args[MAXARG], buf[512];
  // Exctract arguments passed to program 
  for(x = 1; x < argc; x++){
    args[arg_i++] = argv[x];
  }
  while(read(0,&buf[i],1)){
    if(buf[i] == '\n'){
      i = 0;
      arg_i = argc-1;
      // Begin argument extraction from line, continue until the amount of args dont exceed exec's limit (if token is being processed finish it first)
      while(arg_i < MAXARG || flag == 1){
        dig = buf[i];
        // Find first non-white character, flag start of token, mark its beginning into args
        if(dig >= 33 && dig <= 126 && flag == 0){
          args[arg_i++] = &buf[i];
          flag = 1;
        }
        // If white character is found, end token with null, disable token flag 
        if(dig < 33 || dig > 126){
          buf[i] = '\0';
          flag = 0;
          // If white character found is also newline, break, meaning all tokens from current line extracted
          if(dig == 10){
            break;
          }
        }
        i++;
        // If args limit was reached and token isnt being processed, check for remaining characters 
        if(arg_i == MAXARG && flag == 0){
          // Process buf until its end, if no premature exit called, only white characters remain
          while(i<512){
            dig = buf[i];
            // If non-white character is found, arguemnts in line exceed limit
            if(dig >= 33 && dig <= 126){
              printf("find: too many arguments in line\n");
              exit(1);
            // If newline found, all arguments were processed, break from loop and continue
            } else if(dig == 10){
              break;
            }
          }
        }
      }
      pid = fork();
      if(pid < 0){
        printf("fork() failed\n");
        exit(1);
      }
      
      if(pid == 0){
      	// Execute command with extracted arguments in child
        exec(argv[1],args);
      } else {
      	// Parent waits for child to execute
        wait(0);
      }
      i=0;
    } else {
      // End of buf reached without newline, exit with failure status
      if(i == 511){
        printf("find: input line too long\n");
        exit(1);
      }
      i++;
    }
  }
  exit(0);
}
