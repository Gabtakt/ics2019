#include "fs.h"

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(void *buf, size_t offset, size_t len);

/* pa3.3
 * 2020-12-13
 */
int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  // pa3.3
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

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
  {"stdin", 0, 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, 0, invalid_read, invalid_write},
  {"stderr", 0, 0, 0, invalid_read, invalid_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

/* pa3.3
 * 2020-12-13
 * NOTE: nnaos-lite allows all user program w/r a file, so parameter 
 * 'flags' and 'mode' can be ignored
 */
int fs_open(const char *pathname, int flags, int mode) {
  int fd = 0;
  for(; fd < NR_FILES; fd++) {
    if (strcmp(file_table[fd].name, pathname) == 0) {
      file_table[fd].open_offset = 0;
      return fd;
    }
  }
  panic("file %s not found.", pathname);
  assert(0);
  return -1;
}

/* pa3.3
 * 2020-12-13
 */
size_t fs_read(int fd, void *buf, size_t len) {
  assert(fd >= 0 && fd < NR_FILES);
  // out of file size range, change it
  if (file_table[fd].size > 0 && len > file_table[fd].size - file_table[fd].open_offset) {
    len = file_table[fd].size - file_table[fd].open_offset;
  }
  assert(len >= 0);
  size_t offset = file_table[fd].disk_offset + file_table[fd].open_offset;
  size_t ret = 0;
  switch(fd) {
    // ignore the next file operation
    case FD_STDIN:
    case FD_STDOUT:
    case FD_STDERR:
      break;
    default:
      ret = ramdisk_read(buf, offset, len);
      fs_lseek(fd, ret, SEEK_CUR);
      break;
  }
  return ret;
}

/* pa3.3
 * 2020-12-13
 */
size_t fs_write(int fd, const void *buf, size_t len) {
  assert(fd >= 0 && fd < NR_FILES);
  // out of file size range, change it
  if (file_table[fd].size > 0 && len > file_table[fd].size - file_table[fd].open_offset) {
    len = file_table[fd].size - file_table[fd].open_offset;
  }
  assert(len >= 0);
  size_t offset = file_table[fd].disk_offset + file_table[fd].open_offset;
  size_t ret = 0;
  switch(fd) {
    // ignore the next file operation
    case FD_STDIN:
      break;
    // call _putc() to output to the serial port
    case FD_STDOUT:
    case FD_STDERR:
    {
      int i = 0;
      for ( ; i < len; i++) {
        _putc(((char *)buf)[i]);
      }
      ret = len;
      break;
    }
    default:
      ret = ramdisk_write(buf, offset, len);
      fs_lseek(fd, ret, SEEK_CUR);
      break;
  }
  return ret;
}

/* pa3.3
 * 2020-12-13
 */
size_t fs_lseek(int fd, size_t offset, int whence) {
  assert(fd >= 0 && fd < NR_FILES);
  switch(whence) {
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
      panic("Unkown whence : %d", whence);
  }
  return file_table[fd].open_offset;
}

/* pa3.3
 * 2020-12-13
 * NOTE: PA do not need to close file, so always return 0
 */
int fs_close(int fd) {
  return 0;
}