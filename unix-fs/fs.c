#include "fs.h"
#include <stdio.h>

//内存i节点数组，NUM为该文件系统容纳的文件数
inode usedinode[INODENUM];
//ROOT的内存i节点
inode *root;
//当前节点
inode *current;
//已经打开文件的数组
file* opened[];
//超级块
supblock *super;
//模拟磁盘
FILE * fp;
//当前用户
user* curuser;
//当前文件或者目录名字（通过文件，获取名字和文件的inode id）
direct curdirect;
//当前目录
dir* curdir;
//退出
//用户文件节点
inode* userinode;

int userpos;

int logout_ = 0;

/* 新建模拟磁盘并分区格式化 */
int format(const char* path){
    fp = fopen(path, "wb+");
    if(fp == NULL) return -1;
    super = (supblock*)calloc(1,sizeof(supblock));
    fseek(fp, BOOTPOS, SEEK_SET);// 跳过0#块
    /* 初始化超级块 */
    for(size_t i = 0;i < BLOCKNUM ;i++) super->freeBlock[i] = BLOCKSTART + i;// 初始化空闲盘块栈
    super->nextFreeBlock = BLOCKNUM;// 初始化空闲盘块栈指针
    super->nextFreeInode = INODENUM;// 初始化空闲INODE栈指针
    for(size_t i = 0;i < INODENUM;i++) super->freeInode[i] = i;// 初始化空闲INODE栈
    super->freeBlockNum = BLOCKSNUM;
    super->freeInodeNum = INODENUM;
    fwrite(super, sizeof(supblock), 1, fp);
    /* 初始化存储块 */
    fseek(fp,BLOCKSTART*1024,SEEK_SET);
    size_t group[BLOCKNUM];
    for(int i=0;i<BLOCKNUM;i++) group[i]=i+912;
    for(int i=0;i<363;i++){
        for(int j=0;j<BLOCKNUM;j++) group[j]+=BLOCKNUM;
        fseek(fp,(BLOCKSTART+i*20)*1024,SEEK_SET);
        fwrite(group,sizeof(size_t),BLOCKNUM,fp);
    }
    /* 初始化INODE块 */
    fseek(fp, BOOTPOS+SUPERSIZE, SEEK_SET);
    inode* in = (inode*)calloc(1,sizeof(inode));
    for(size_t i = 0;i < INODENUM;i++) fwrite(in, sizeof(inode), 1, fp);
    fclose(fp);
    return 0;
}

/* 进入文件系统 */
int enter(const char* path){
    fp = fopen(path, "r+b");
    return 0;
}
