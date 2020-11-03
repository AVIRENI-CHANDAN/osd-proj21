#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"

int main() {
  char buf[512];
  int fd, i, blocks = 0;

  fd = open("big.file", O_CREATE | O_WRONLY);
  if(fd < 0){
    printf(2, "\e[0;31m\e[0;43merror: cannot open big.file for writing\n\e[0m");
    exit();
  }

  memset(buf, 0, sizeof(buf));

  /* writing section */
  printf(1, "\e[0;42m \e[0m\e[0;32mWrite\e[0m |");
  while(1) {
    *(int*)buf = blocks;
    int cc = write(fd, buf, sizeof(buf));
    if(cc <= 0) break;

    if (blocks++ % 10 == 0) printf(1, "\e[0;32mw\e[0m");
  }
  printf(1, "| blocks=%d, bytes=%dK\n", blocks, blocks*BSIZE/1024);
  close(fd);

  /* reading section */
  fd = open("big.file", O_RDONLY);
  if(fd < 0){
    printf(2, "error: cannot re-open big.file for reading\n");
    exit();
  }

  printf(1, "\e[0;43m \e[0m\e[0;33mVerify\e[0m |");
  for(i = 0; i < blocks; i++){
    int cc = read(fd, buf, sizeof(buf));
    if(cc <= 0){
      printf(2, "error: read error at sector %d\n", i);
      exit();
    }
    /* check content */
    if(*(int*)buf != i){
      printf(2, "error: read the wrong data (%d) for sector %d\n", *(int*)buf, i);
      exit();
    }
    if (i % 10 == 0) printf(1, "\e[0;33mv\e[0m");
  }
  printf(1, "| blocks=%d, bytes=%dK\n", blocks, blocks*BSIZE/1024);
  close(fd);

  exit();
}
