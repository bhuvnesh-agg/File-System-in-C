// Meta Info about the file system
// -> number of inodes
// -> number of disk blocks
// -> size of disk blocks

#define BLOCKSIZE 512

struct superblock {
  int num_inodes;
  int num_blocks;
  int size_blocks;
};

struct inode {
  int size;
  char name[8];
  int first_block;
};

struct disk_block {
  int next_block_num;
  char data[BLOCKSIZE];
};

void create_fs ();    //initialise new filesystem
void mount_fs ();     //load a file system
void sync_fs ();      //write the file system

int allocate_file (char name[8]);                     // return filenumber
void set_filesize (int filenum, int size);            // sets file size
void write_byte (int filenum, int pos, char *data);   // writes data into file

void ls_fs();         //list files inside the file system

void print_fs();      //print out info about filesystem
