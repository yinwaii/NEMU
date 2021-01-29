#include <fs.h>
#include <ramdisk.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

static int file_table_size;

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, invalid_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, invalid_write},
#include "files.h"
};

int fs_open(const char *pathname, int flags, int mode) {
  for (int i = 0; i < file_table_size; i++)
  {
    if (strcmp(pathname, file_table[i].name) == 0)
      return i;
  }
  panic("File: %s not found!", pathname);
  return -1;
}

size_t fs_read(int fd, void *buf, size_t len) {
  // Log("%d %d %d", file_table[fd].open_offset, len, file_table[fd].size);
  assert(file_table[fd].open_offset >= 0);
  assert(file_table[fd].open_offset <= file_table[fd].size);
  if (file_table[fd].open_offset + len > file_table[fd].size)
    len = file_table[fd].size - file_table[fd].open_offset;
  ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  file_table[fd].open_offset += len;
  return len;
}

size_t fs_write(int fd, void *buf, size_t len) {
  assert(file_table[fd].open_offset >= 0);
  assert(file_table[fd].open_offset <= file_table[fd].size);
  if (file_table[fd].open_offset + len > file_table[fd].size)
    len = file_table[fd].size - file_table[fd].open_offset;
  ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  file_table[fd].open_offset += len;
  return len;
}

size_t fs_lseek(int fd, size_t offset, int whence) {
  switch (whence)
  {
    case SEEK_SET:
      file_table[fd].open_offset = offset;
      break;
    case SEEK_CUR:
      file_table[fd].open_offset += offset;
      break;
    case SEEK_END:
      file_table[fd].open_offset = file_table[fd].size + offset;
      break;
    default:
      panic("Unhandled whence: %d in fs_lseek!", whence);
      return -1;
  }
  assert(file_table[fd].open_offset >= 0);
  assert(file_table[fd].open_offset <= file_table[fd].size);
  // Log("set open_offset to %d", file_table[fd].open_offset);
  return file_table[fd].open_offset;
}

int fs_close(int fd) {
  return 0;
}

void init_fs() {
  assert(sizeof(file_table) != 0);
  file_table_size = sizeof(file_table) / sizeof(file_table[0]);
  // TODO: initialize the size of /dev/fb
}
