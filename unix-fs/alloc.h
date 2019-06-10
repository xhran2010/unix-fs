#ifndef alloc_h
#define alloc_h

#include <stdio.h>
#include "update.h"
#include "datastruct.h"

/* 盘块写内容 */
int b_write(void * buf,size_t bno,size_t offset,size_t size,int count);
/* 读盘块内容 */
int b_read(void * buf,size_t bno,size_t offset,size_t size,int count);
/* 分配空闲盘块 */
size_t balloc(void);
/* 回收空闲盘块 */
int b_free(size_t bno);
/* 分配INODE */
inode* i_alloc(void);
inode* i_get(int ino);
int rm_inode(inode* i);
#endif /* alloc_h */
