//
//  fs.h
//  unix-fs
//
//  Created by 辛浩然 on 2019/6/4.
//  Copyright © 2019 Jack. All rights reserved.
//

#ifndef fs_h
#define fs_h

#include <stdio.h>

#define INODEPERBLOCK 16
#define BLOCKSIZE 512
#define INODESIZE 32

typedef struct{
    size_t s_isize;// inode块数
    size_t s_fsize;// 存储区域的块数
    size_t s_nfree;// 存储区域空闲块数
    size_t* s_free;// 存储空闲队列
    size_t s_ninode;// inode空闲个数
    size_t* s_inode;// inode空闲队列
} FilSys;

typedef struct{
    size_t inum;// inode 编号
    char* file_name;// 文件名
    size_t length;// 文件长度
    size_t* addr;// 文件存储的块编号
} INode;

/* 新建文件，格式化系统并分区块 */
int format(const char* path,size_t inode_block_num,size_t size);

#endif /* fs_h */
