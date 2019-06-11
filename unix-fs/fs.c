#include "fs.h"
#include <stdio.h>

//内存i节点数组，NUM为该文件系统容纳的文件数
inode usedinode[INODESNUM];
//ROOT的内存i节点
inode *root;
//当前节点
inode *current;
//超级块
supblock *super;
//模拟磁盘
FILE * fp;
//当前用户
user* curuser;
//当前文件或者目录名字和对应INODE（通过文件，获取名字和文件的inode id）
direct curdirect;
//用户文件节点
inode* userinode;

int userpos;

int logout_ = 1;

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
    super->freeInodeNum = INODESNUM;
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
    finode* in = (finode*)calloc(1,sizeof(finode));
    for(size_t i = 0;i < INODENUM;i++){
        fwrite(in, sizeof(finode), 1, fp);
        fseek(fp,BOOTPOS+SUPERSIZE+(i+1)*INODE, SEEK_SET);
    }
    /* 初始化根INODE结点 */
    dir* dir_ = (dir*)calloc(1, sizeof(dir));
    inode * tmpinode=i_alloc();
    tmpinode->finode.addr[0]=b_alloc();
    tmpinode->finode.mode=1774;
    tmpinode->finode.parent = -1;
    strcpy(tmpinode->finode.owner,"root");
    strcpy(tmpinode->finode.group,"super");
    update_inode(tmpinode);
    dir_->dirNum = 0;
    b_write(dir_,tmpinode->finode.addr[0],0,sizeof(dir),1);
    /* 初始化用户INODE结点 */
    inode* usernode = i_alloc();
    usernode->finode.addr[0] = b_alloc();
    usernode->finode.mode = 2774;
    usernode->finode.parent = -1;
    usernode->finode.fileSize += 2 * sizeof(user);
    update_inode(usernode);
    user* root = (user*)calloc(1, sizeof(user));
    // 用户名 root 密码 123456 用户组 super
    strcpy(root->userName,"root");
    strcpy(root->userPwd,"123456");
    strcpy(root->userGroup, "super");
    b_write(root, usernode->finode.addr[0], 0, sizeof(user), 1);
    // 用户名 xinhaoran 密码 123456 用户组 guest
    strcpy(root->userName,"xinhaoran");
    strcpy(root->userPwd,"123456");
    strcpy(root->userGroup, "guest");
    b_write(root, usernode->finode.addr[0], sizeof(user), sizeof(user), 1);
    fclose(fp);
    return 0;
}

/* 进入文件系统 */
int enter(const char* path){
    /* 将磁盘中的信息读入内存 */
    fp = fopen(path, "r+b");
    if(fp == NULL) return -1;
    while(login_() == -1){}
    super=(supblock*)calloc(1,sizeof(supblock));
    fseek(fp,BOOTPOS,SEEK_SET);
    fread(super,sizeof(supblock),1,fp);
    current = i_get(19);
    root = current;
    curdirect.inodeID = 19;
    strcpy(curdirect.directName, "/");
    fseek(fp,BOOTPOS,SEEK_SET);
    int res=fwrite(super,sizeof(supblock),1,fp);
    while(logout_ == 0) shell();
    fclose(fp);
    return 0;
}
