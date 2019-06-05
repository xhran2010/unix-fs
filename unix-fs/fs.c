//
//  fs.c
//  unix-fs
//
//  Created by 辛浩然 on 2019/6/4.
//  Copyright © 2019 Jack. All rights reserved.
//
#include "fs.h"
#include <stdio.h>

FILE* drives[26];

void fs_init(){
    for(int i = 0;i < 26;i++) drives[i] = NULL;
}

int format(const char* path,size_t inode_block_num,size_t size){
    FILE* fp = fopen(path, "wb+");
    if(fp == NULL) return -1;
    FilSys super;
    /* 写入超级块 */
    size_t inode_num = inode_block_num * INODEPERBLOCK;
    super.s_isize = inode_num;
    super.s_ninode = inode_num;
    super.s_inode = (int*) malloc(inode_num * sizeof(size_t));
    super.s_fsize = size / 512 - inode_block_num - 1;
    super.s_nfree = size / 512 - inode_block_num - 1;
    size_t free_group_num = super.s_fsize / (BLOCKSIZE/sizeof(size_t)-1) + 1;
    size_t start_num = inode_block_num + 2;
    FreeGroup fg;
    if(free_group_num > 1) fg.num = BLOCKSIZE/sizeof(size_t)-1;
    else fg.num = super.s_fsize;
    /* FIXME:无需额外盘块时结束添0 */
    for(size_t i = 0;i < fg.num;i++){
        fg.add[i] = start_num;
        start_num ++;
    }
    super.s_free = fg;
    size_t res = fwrite(&super, BLOCKSIZE, 1, fp);
    if(res != 1) return -1;
    /* 写入inode */
    INode inode;
    for(int i = 1;i <= inode_num;i++){
        inode.inum = i;
        inode.length = 0;
        inode.file_name = NULL;
        res = fwrite(&inode,INODESIZE,1, fp);
        if(res != 1) return -1;
    }
    /* 写存储区域 */
    /* 成组链接法初始化 */
    for(size_t i = 2;i <= free_group_num;i++){
        FreeGroup fg;
        if(i != free_group_num) fg.num = BLOCKSIZE/sizeof(size_t)-1;
        else fg.num = super.s_fsize % (BLOCKSIZE/sizeof(size_t)-1);
        for(size_t j = 0;j < fg.num;j++){
            fg.add[j] = start_num;
            start_num ++;
        }
        if(i == free_group_num) fg.add[fg.num] = 0;
        fwrite(&fg, sizeof(fg), 1, fp);
    }
    char placeholder = '\0';
    fwrite(&placeholder, (super.s_fsize - free_group_num + 1)* BLOCKSIZE, 1, fp);
    fclose(fp);
    return 0;
}

int fs_mount(char drive, const char* path){
    if(drive >= 'a' && drive <= 'z') drive -= 32;// 小写变大写字母
    if(!(drive >= 'A' && drive <= 'Z')) return -1;// 盘符不符合条件
    if(drives[drive-'A'] != NULL) return -2;// 错误，已经被挂载
    FILE* fp = fopen(path, "r+b");
    drives[drive - 'A'] = fp;// 挂载成功
    return 0;
}
