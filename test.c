#include <stdio.h>
#include "fs.h"

int main(int argc, char *argv[])
{
  create_fs();
  mount_fs();

  /* int file = allocate_file("another");
  set_filesize(file, 5000);
  char data = 'b';
  for (int i = 0; i < 49; i++) {
    write_byte(file, i*100, &data);
  } */

  // ls_fs();
  sync_fs();

  printf("success");
  return 0;
}
