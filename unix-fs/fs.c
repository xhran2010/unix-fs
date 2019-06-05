//
//  fs.c
//  unix-fs
//
//  Created by 辛浩然 on 2019/6/4.
//  Copyright © 2019 Jack. All rights reserved.
//

#include "fs.h"
#include <stdio.h>

int format(const char* path,size_t inode_block_num,size_t size){
    FILE* fp = fopen(path, "wb+");
    if(fp == NULL) return -1;
    FilSys super;
    /* 写入超级块 */
    size_t inode_num = inode_block_num * INODEPERBLOCK;
    super.s_isize = inode_num;
    super.s_ninode = inode_num;
    //super.s_inode = (int*) malloc(inode_num * sizeof(size_t));
    super.s_fsize = size / 512 - inode_block_num - 1;
    //super.s_nfree = (int*)
    size_t res = fwrite(&super, BLOCKSIZE, 1, fp);
    if(res != 1) return -1;
    /* 写入inode */
    INode inode;
    for(int i = 1;i <= inode_num;i++){
        inode.inum = i;
        inode.length = 0;
        inode.file_name = NULL;
        inode.addr = NULL;
        res = fwrite(&inode,INODESIZE,1, fp);
        if(res != 1) return -1;
    }
    /* 写存储区域 */
    char placeholder = '\0';
    fwrite(&placeholder, super.s_fsize * BLOCKSIZE, 1, fp);
    fclose(fp);
    return 0;
}

