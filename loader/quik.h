/*
 * Global procedures and variables for the quik second-stage bootstrap.
 *
 * Copyright (C) 2013 Andrei Warkentin <andrey.warkentin@gmail.com>
 * Copyright (C) 1996 Paul Mackerras.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef QUIK_QUIK_H
#define QUIK_QUIK_H

#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>

#define ALIGN_UP(addr, align) (((addr) + (align) - 1) & (~((align) - 1)))
#define ALIGN(addr, align) (((addr) - 1) & (~((align) - 1)))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define SIZE_1M 0x100000
#define SIZE_4K 0x1000
#define NULL ((void *) 0)

typedef uint32_t vaddr_t;
typedef uint64_t offset_t;
typedef uint32_t length_t;

/*
 * Preboot script can follow the executable if
 * we are a raw binary loaded from the bootsector.
 *
 * Presumably if similar functionality is used on
 * non-PMACs you can use a CHRP script around the
 * ELF.
 */
extern uint32_t _preboot_script;
#define preboot_script ((char *) &_preboot_script)

/*
 * Loaded kernels described by this code.
 */
typedef struct {
  vaddr_t buf;
  vaddr_t linked_base;
  vaddr_t text_offset;
  length_t text_len;
  vaddr_t  entry;
} load_state_t;

#define QUIK_ERR_LIST                                                   \
   QUIK_ERR_DEF(ERR_NONE, "no error")                                   \
   QUIK_ERR_DEF(ERR_NOT_READY, "operation should be retried")           \
   QUIK_ERR_DEF(ERR_MALLOC_INIT, "malloc claim failed")                \
   QUIK_ERR_DEF(ERR_OF_INIT_NO_CHOSEN, "no OF /chosen")                 \
   QUIK_ERR_DEF(ERR_OF_INIT_NO_MEMORY, "no OF /memory")                 \
   QUIK_ERR_DEF(ERR_OF_INIT_NO_OPTIONS, "no OF /options")               \
   QUIK_ERR_DEF(ERR_OF_INIT_NO_OPROM, "no OF /openprom")                \
   QUIK_ERR_DEF(ERR_OF_INIT_NO_ROOT, "no OF /")                         \
   QUIK_ERR_DEF(ERR_OF_OPEN, "cannot open device")                      \
   QUIK_ERR_DEF(ERR_DEV_SHORT_READ, "short read on device")             \
   QUIK_ERR_DEF(ERR_PART_NOT_MAC, "partitioning not macintosh")         \
   QUIK_ERR_DEF(ERR_PART_NOT_DOS, "partitioning not dos")               \
   QUIK_ERR_DEF(ERR_PART_NOT_PARTITIONED, "invalid disk partitioning")  \
   QUIK_ERR_DEF(ERR_PART_NOT_FOUND, "partition not found")              \
   QUIK_ERR_DEF(ERR_PART_BEYOND, "access beyond partition size")        \
   QUIK_ERR_DEF(ERR_FS_OPEN, "cannot open volume as file system")       \
   QUIK_ERR_DEF(ERR_FS_PATH, "bad path")                                \
   QUIK_ERR_DEF(ERR_FS_NOT_FOUND, "file not found")                     \
   QUIK_ERR_DEF(ERR_FS_NOT_DIR, "not a directory")                      \
   QUIK_ERR_DEF(ERR_FS_NOT_REG, "not a regular file")                   \
   QUIK_ERR_DEF(ERR_FS_NOT_EXT2, "FS is not ext2")                      \
   QUIK_ERR_DEF(ERR_FS_CORRUPT, "FS is corrupted")                      \
   QUIK_ERR_DEF(ERR_FS_LOOP, "symlink loop detected")                   \
   QUIK_ERR_DEF(ERR_ELF_NOT, "invalid kernel image")                    \
   QUIK_ERR_DEF(ERR_ELF_WRONG, "invalid kernel architecture")           \
   QUIK_ERR_DEF(ERR_ELF_NOT_LOADABLE, "not a loadable image")           \
   QUIK_ERR_DEF(ERR_KERNEL_RETURNED, "kernel returned")                 \
   QUIK_ERR_DEF(ERR_KERNEL_OLD_BIG, "pre-2.4 kernel too large ")        \
   QUIK_ERR_DEF(ERR_NO_MEM, "malloc failed")                            \
   QUIK_ERR_DEF(ERR_CONFIG_NOT_FOUND, "no configuration file found")    \
   QUIK_ERR_DEF(ERR_ENV_DEFAULT_BAD, "bad default device path")         \
   QUIK_ERR_DEF(ERR_ENV_CURRENT_BAD, "bad current device path")         \
   QUIK_ERR_DEF(ERR_ENV_PREBOOT_BAD, "no boot-file set by preboot script") \
   QUIK_ERR_DEF(ERR_CMD_UNKNOWN, "unknown command")                     \
   QUIK_ERR_DEF(ERR_CMD_BAD_PARAM, "invalid parameters")                \
   QUIK_ERR_DEF(ERR_INVALID, "invalid error, likely a bug")             \

#define QUIK_ERR_DEF(e, s) e,
typedef enum {
  QUIK_ERR_LIST
} quik_err_t;
#undef QUIK_ERR_DEF

typedef struct {
   char *device;
   unsigned part;
   bool part_valid;
} env_dev_t;

typedef struct {

   /* Real OF entry. */
   vaddr_t prom_entry;

   /* Our shim, if any. */
   vaddr_t prom_shim;

#define CONFIG_VALID          (1 << 1)
#define PAUSE_BEFORE_BOOT     (1 << 2)
#define DEBUG_BEFORE_BOOT     (1 << 3)
#define TRIED_AUTO            (1 << 4)
#define BOOT_PRE_2_4          (1 << 5)
#define SHIM_OF               (1 << 6)
#define WITH_PREBOOT          (1 << 7)
#define HAVE_IMAGES           (1 << 8)
   unsigned flags;

   /* Config file path. E.g. /etc/quik.conf */
   char *config_file;

   /* Our env which we get from OF. */
   char *of_bootargs_buf;
   char *of_bootfile_buf;

   /*
    * Operating env.
    *
    * Device/partition for config_file, then
    * used as the default device path unless
    * overriden in a path spec.
    */
   env_dev_t default_dev;
   char *bootargs;

   /* Pause message. */
   char *pause_message;

   /*
    * Initrd location, here because it might be needed by
    * PROM shimming.
    */
   vaddr_t initrd_base;
   length_t initrd_len;
} boot_info_t;

extern boot_info_t *bi;

typedef int key_t;
#define KEY_NONE (-1)

#define DEFAULT_TIMEOUT -1
#define PREBOOT_TIMEOUT 50
#define TIMEOUT_TO_SECS(t) (t / 10)

quik_err_t elf_parse(void *load_buf,
                     length_t load_buf_len,
                     load_state_t *image);
quik_err_t elf_relo(load_state_t *image);
quik_err_t elf_boot(load_state_t *image,
                    char *params);

int cfg_parse(char *cfg_file, char *buff, int len);
char *cfg_get_strg(char *image, char *item);
int cfg_get_flag(char *image, char *item);
void cfg_print_images(void);
char *cfg_get_default(void);

quik_err_t malloc_init(void);
void *malloc(unsigned int size);
void free(void *);
void *realloc(void *ptr, unsigned int size);

void env_dev_set_part(env_dev_t *dp, unsigned part);
quik_err_t env_dev_update_from_default(env_dev_t *cur_dev);
quik_err_t env_dev_is_valid(env_dev_t *dp);
quik_err_t env_init(void);

void spinner(int freq);
void flush_cache(vaddr_t base, length_t len);
void vprintk(char *fmt, va_list adx);
void printk(char *fmt, ...);

typedef struct {
   unsigned count;
   unsigned max_col;
   unsigned pad;
} table_print_t;
void table_print_start(table_print_t *t, unsigned cols, unsigned pad);
void table_print(table_print_t *t, char *s);
void table_print_end(table_print_t *t);

uint32_t swab32(uint32_t value);
uint16_t swab16(uint16_t value);
char *strstr(const char *s1, const char *s2);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, length_t n);
char *strcat(char *dest, const char *src);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, length_t n);
length_t strlen(const char *s);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
void *memset(void *s, int c, length_t n);
void bcopy(const void *src, void *dest, length_t n);
void *memcpy(void *dest, const void *src, length_t n);
void *memmove(void *dest, const void *src, length_t n);
int memcmp(const void *s1, const void *s2, length_t n);
int tolower(int c);
int strcasecmp(const char *s1, const char *s2);
void *strdup(char *str);
int strtol(const char *nptr,
           char **endptr,
           int base);
char *chomp(char *str);
void word_split(char **linep,
                char **paramsp);
key_t wait_for_key(int timeout,
                   key_t timeout_key);

#endif /* QUIK_QUIK_H */
