#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fs.h"

int find_empty_inode (); 
int find_empty_block (); 
void shorten_file (int bn);
int get_block_num (int file, int offset);

struct superblock sb;
struct inode *inodes;
struct disk_block *dbs;


//fs.h FUNCTIONS

//////////////////////////////////////////////////////////////////////////////////////////////////////
// create filesystem
void create_fs(){
  sb.num_inodes = 10;
  sb.num_blocks = 100;
  sb.size_blocks = sizeof(struct disk_block);

  inodes = (struct inode *)malloc(sb.num_inodes * sizeof(struct inode));
  for (int i = 0; i < sb.num_inodes; i++) {
    inodes[i].size = -1;
    inodes[i].first_block = -1;
    strcpy(inodes[i].name, "emptyfi");
  } // init inodes

  dbs = (struct disk_block *)malloc(sb.num_blocks * sizeof(struct disk_block));
  for (int i = 0; i < sb.num_blocks; i++) {
    dbs[i].next_block_num = -1;
  } // init disk blocks

} 
//////////////////////////////////////////////////////////////////////////////////////////////////////


// load filesystem
void mount_fs(){
  FILE *file;
  file = fopen("fs_data_new", "r");
  
  // superblock
  fread(&sb, sizeof(struct superblock), 1, file);

  // inodes
  inodes = (struct inode *)malloc(sb.num_inodes * sizeof(struct inode));
  fread(inodes, sizeof(struct inode), sb.num_inodes, file);

  // data blocks
  dbs = (struct disk_block *)malloc(sb.num_blocks * sizeof(struct disk_block));
  fread(dbs, sizeof(struct disk_block), sb.num_blocks, file);

  fclose(file);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////


// write filesystem
void sync_fs(){
  FILE *file;
  file = fopen("fs_data_new", "w+");
  
  // superblock
  fwrite(&sb, sizeof(struct superblock), 1, file);

  // inodes
  for (int i = 0; i < sb.num_inodes; i++) {
    fwrite(&(inodes[i]), sizeof(struct inode), 1, file);
  }

  // data blocks
  for (int i = 0; i < sb.num_blocks; i++) {
    fwrite(&(dbs[i]), sizeof(struct disk_block), 1, file);
  }

  fclose(file);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////


//print out info about filesystem
void print_fs(){
  printf("\nSuperblock Info: \n");
  printf("\tnumber of inodes : %d \n", sb.num_inodes);
  printf("\tnumber of blocks : %d \n", sb.num_blocks);
  printf("\tsize of each block : %d \n", sb.size_blocks);


  printf("\nInodes Info: \n"); 
  for (int i = 0; i < sb.num_inodes; i++) {
    printf("\tSize: %d, Block: %d, Name: %s\n", inodes[i].size, inodes[i].first_block, inodes[i].name);
  }

  printf("\nBlocks Info: \n"); 
  for (int i = 0; i < sb.num_blocks; i++) {
    printf("\tBlock Number: %d, Next Block Number: %d\n", i, dbs[i].next_block_num);
  }
}; //print_fs
//////////////////////////////////////////////////////////////////////////////////////////////////////

int allocate_file (char name[8]){
  
  //find an empty inode
  int in = find_empty_inode();

  //find disk block
  int block = find_empty_block();

  //claim them
  inodes[in].first_block = block;
  dbs[block].next_block_num = -2;

  strcpy(inodes[in].name, name);

  //return file descriptor
  return in;
} // allocate_file 
//////////////////////////////////////////////////////////////////////////////////////////////////////

int find_empty_inode () {
  for (int i = 0; i < sb.num_inodes; i++) {
    if (inodes[i].first_block == -1) {
      return i;
    }
  }
  return -1;
} // find empty inode 
//////////////////////////////////////////////////////////////////////////////////////////////////////



int find_empty_block () {
  for (int i = 0; i < sb.num_blocks; i++) {
    if (dbs[i].next_block_num == -1) {
      return i;
    }
  }
  return -1;
} // find empty disk block 
//////////////////////////////////////////////////////////////////////////////////////////////////////


// add / delete blocks 
void set_filesize (int filenum, int size){

  // number of blocks needed
  int tmp = size + BLOCKSIZE - 1;

  int num = tmp / BLOCKSIZE;

  int bn  = inodes[filenum].first_block;
  num--;
  // grow file if necessary
  while (num > 0) {
    // check next block number
    int next_num = dbs[bn].next_block_num;
    if (next_num == -2) {
      int empty = find_empty_block();
      dbs[bn].next_block_num = empty;
      dbs[empty].next_block_num = -2;
    }
    bn = dbs[bn].next_block_num;
    num--;
  }

  //Shrink file if needed
  shorten_file(bn);
  dbs[bn].next_block_num = -2;

} //set_filesize
//////////////////////////////////////////////////////////////////////////////////////////////////////

// list files in the file system
void ls_fs(){
  for (int i = 0; i < sb.num_inodes; i++) {
    if (inodes[i].first_block != -1) {
      printf("%s \n", inodes[i].name);
    }
  }
} // ls_fs


//INTERNAL FUNCTIONS

//////////////////////////////////////////////////////////////////////////////////////////////////////
void write_byte (int filenum, int pos, char *data){
  //which block
  int relative_block = pos / BLOCKSIZE;
   
  //find block number
  int bn = get_block_num(filenum, relative_block);
   
  //find block offset
  int offset = pos % BLOCKSIZE;

  //write data
  dbs[bn].data[offset] = *data;

} //write_byte
//////////////////////////////////////////////////////////////////////////////////////////////////////

void shorten_file (int bn){
  int nn = dbs[bn].next_block_num;
  if (nn >= 0) {
    shorten_file(nn);
  } 
  dbs[bn].next_block_num = -1;
} //shorten_file
//////////////////////////////////////////////////////////////////////////////////////////////////////

int get_block_num (int file, int offset) {
  int togo = offset;
  int bn = inodes[file].first_block;

  while (togo) {
    bn = dbs[bn].next_block_num;
    togo--;
  }
  return bn;
} // get_block_num
//////////////////////////////////////////////////////////////////////////////////////////////////////
