#include <fs.h>
#include <ramdisk.h>
#include <device.h>

#define CONCAT3(a, b, c) a ## b ## c
#define load_enum(name) \
  , CONCAT(FD_, name)
#define load_table_read(name)  \
  [CONCAT(FD_, name)] = {TOSTRING(CONCAT(/dev/am/AM_, name)), 0, 0, CONCAT3(AM_, name, _read), invalid_write},
#define load_table_write(name)  \
  [CONCAT(FD_, name)] = {TOSTRING(CONCAT(/dev/am/AM_, name)), 0, 0, invalid_read, CONCAT3(AM_, name, _write)},

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

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_EVENTS, FD_DISPINFO, FD_FB reg_map_read(load_enum) reg_map_write(load_enum)};

static int file_table_size;

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not read here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not write here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
  [FD_EVENTS] = {"/dev/events", 0, 0, events_read, invalid_write},
  [FD_DISPINFO] = {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
  [FD_FB] = {"/dev/fb", 0, 0, invalid_read, fb_write},
  reg_map_read(load_table_read)
  reg_map_write(load_table_write)
#include "files.h"
};

int fs_open(const char *pathname, int flags, int mode) {
  // Log("Open the file: %s", pathname);
  for (int i = 0; i < file_table_size; i++)
  {
    // if (i > 5)
      // Log("%s %s %d", pathname, file_table[i].name, strcmp(pathname, file_table[i].name));
    if (strcmp(pathname, file_table[i].name) == 0)
    {
      file_table[i].open_offset = 0;
      return i;
    }
  }
  Log("File: %s not found!", pathname);
  return -1;
}

size_t fs_read(int fd, void *buf, size_t len) {
  // Log("%d %d %d", file_table[fd].open_offset, len, file_table[fd].size);
  assert(fd >= 0 && fd < file_table_size);
  assert(file_table[fd].open_offset >= 0);
  assert(file_table[fd].open_offset <= file_table[fd].size);
  int res = len;
  if (file_table[fd].size > 0 && file_table[fd].open_offset + len > file_table[fd].size)
    len = file_table[fd].size - file_table[fd].open_offset;
  if (file_table[fd].read != NULL)
    res = file_table[fd].read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  else
    res = ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  if (file_table[fd].size > 0)
    file_table[fd].open_offset += res;
  return res;
}

size_t fs_write(int fd, void *buf, size_t len) {
  assert(fd >= 0 && fd < file_table_size);
  assert(file_table[fd].open_offset >= 0);
  assert(file_table[fd].open_offset <= file_table[fd].size);
  int res = len;
  if (file_table[fd].size > 0 && file_table[fd].open_offset + len > file_table[fd].size)
    len = file_table[fd].size - file_table[fd].open_offset;
  if (file_table[fd].write != NULL)
    res = file_table[fd].write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  else
    res = ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
  if (file_table[fd].size > 0)
    file_table[fd].open_offset += res;
  return res;
}

size_t fs_lseek(int fd, size_t offset, int whence) {
  assert(fd >= 0 && fd < file_table_size);
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
  assert(fd >= 0 && fd < file_table_size);
  return 0;
}

void init_fs() {
  assert(sizeof(file_table) != 0);
  file_table_size = sizeof(file_table) / sizeof(file_table[0]);
  AM_GPU_CONFIG_T config = io_read(AM_GPU_CONFIG);
  file_table[FD_FB].size = config.width * config.height * sizeof(uint32_t);
  // TODO: initialize the size of /dev/fb
}
