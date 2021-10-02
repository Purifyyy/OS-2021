#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void
find(char *directory, char *file)
{
  char buf[512], path[128], *p = path, *q;
  int fd;
  struct stat st;
  struct dirent de;

  if((fd = open(directory, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", directory);
    return;
  }
  
  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", directory);
    close(fd);
    return;
  }
  
  memmove(p,directory,strlen(directory));
  p = p+strlen(directory);
  *p++ = '/';
  
  switch(st.type){
  case T_FILE:
    printf("find: %s is not a valid directory\n",directory);
    break;
    
  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, directory);
    q = buf+strlen(buf);
    *q++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
      continue;
      memmove(q, de.name, DIRSIZ);
      q[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("find: cannot stat %s\n", buf);
        continue;
      }
      // Check is current file is T_FILE
      if(st.type == 2 && !strcmp(de.name,file)){
        memmove(p,de.name,strlen(de.name));
        printf("%s\n",path);
      }
      
      // Check if current file is T_DIR
      if(st.type == 1){
      	// Exclude . and .. directories from recursion
        if(strcmp(de.name,".") && strcmp(de.name,"..")){
	  find(buf,file);
        }
      }
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc < 3 || argc > 3){
    printf("usage: find <directory> <filename>\n");
    exit(0);
  }
  find(argv[1],argv[2]);
  exit(0);
}
