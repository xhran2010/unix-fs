#ifndef fs_h
#define fs_h

#include <stdio.h>

#define INODEPERBLOCK 16
#define BLOCKSIZE 512
#define INODESIZE 32

typedef struct {
    size_t num;
    size_t add[BLOCKSIZE/sizeof(size_t)-1];
} FreeGroup;

typedef struct{
    size_t s_isize;// inode块数
    size_t s_fsize;// 存储区域的块数
    size_t s_nfree;// 存储区域空闲块数
    FreeGroup s_free;// 存储空闲队列
    size_t s_ninode;// inode空闲个数
    size_t* s_inode;// inode空闲队列
} FilSys;

typedef struct{
    size_t inum;// inode 编号
    char* file_name;// 文件名
    size_t length;// 文件长度
    size_t dir_addr[10];// 文件存储的块编号-直接地址
    size_t indir_addr;// 文件存储的块编号-一次间接地址，存的是索引表所在盘块的编号
    /* 一个块可存64个盘块号，最多支持(10+64)*512B = 37KB的文件 */
} INode;

/* 新建文件，格式化系统并分区块 */
int format(const char* path,size_t inode_block_num,size_t size);
/* 挂载到磁盘 */
int fs_mount(char drive, const char* path);
#endif /* fs_h */
