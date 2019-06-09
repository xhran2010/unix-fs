#ifndef fs_h
#define fs_h

#include <stdio.h>
#include "define.h"
#include "alloc.h"
#include "update.h"

/* 新建文件，格式化系统并分区块 */
int format(const char* path);
int enter(const char* path);
#endif /* fs_h */
