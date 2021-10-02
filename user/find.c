#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void
find_rec(char *directory, char *buf, int fd, struct stat st, char *file)
{
  char *q;
  int fd_rec;
  struct dirent de;
  struct stat st_rec;
  
  if(strlen(directory) + 1 + DIRSIZ + 1 > 512){
      printf("find: path too long\n");
      return;
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
      printf("%s\n",buf);
    }  
    // Check if current file is T_DIR, exclude . and .. directories from recursion
    if(st.type == 1 && strcmp(de.name,".") && strcmp(de.name,"..")){
      fd_rec = open(buf, 0);
      find_rec(buf, buf, fd_rec, st_rec, file);
      close(fd_rec);
    }
  }
}

void
find(char *directory, char *file)
{
  char buf[512];
  int fd;
  struct stat st;

  if((fd = open(directory, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", directory);
    return;
  }
  
  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", directory);
    close(fd);
    return;
  }
  // If starting point valid directory, start recursion
  if(st.type == 1){
    find_rec(directory, buf, fd, st, file);
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
