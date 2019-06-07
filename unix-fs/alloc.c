#include "alloc.h"
#include <stdio.h>
#include "define.h"
#include "time.h"

extern FILE* fp;
extern supblock* super;
extern inode usedinode[INODENUM];

/* 盘块写内容 */
int b_write(void * buf,size_t bno,size_t offset,size_t size,int count){
    long int pos;
    size_t ret;
    pos = bno*BLOCKSIZE+offset;
    fseek(fp,pos,SEEK_SET);
    ret = fwrite(buf,size,count,fp);
    fflush(fp);
    if(ret != count) return -1;
    return 0;
}

/* 读盘块内容 */
int b_read(void * buf,size_t bno,size_t offset,size_t size,int count){
    long int pos;
    int ret;
    pos = bno*BLOCKSIZE+offset;
    fseek(fp,pos,SEEK_SET);
    ret = fread(buf,size,count,fp);
    if(ret!=count) return -1;
    return 0;
}

/* 分配空闲盘块 */
size_t b_alloc(){
    unsigned int bno;
    if(super->freeBlockNum<=0) return -1;
    if(super->nextFreeBlock==1){ //空闲盘块号栈内h只剩1个块号，表明要拿一组进来
        bno = super->freeBlock[--super->nextFreeBlock];
        fseek(fp,super->freeBlock[0]*BLOCKSIZE,SEEK_SET);
        int ret=fread(super->freeBlock,sizeof(unsigned int),BLOCKNUM,fp);
        super->nextFreeBlock=ret;
        return bno;
    }
    super->freeBlockNum--;
    super->nextFreeBlock--;
    update_super(fp, super);
    return super->freeBlock[super->nextFreeBlock];
}

/* 回收空闲盘块 */
int b_free(size_t bno){
    if(super->nextFreeBlock==BLOCKNUM){ // 空闲盘块号栈满了
        b_write(&super->freeBlock,bno,0,sizeof(unsigned int),1);
        super->nextFreeBlock=1;
        super->freeBlock[0]=bno;
    }
    else{
        super->freeBlock[super->nextFreeBlock]=bno;
        super->nextFreeBlock++;
    }
    super->freeBlockNum++;
    update_super(fp, super);
    return 0;
}

/* INODE分配 */
inode* i_alloc(){
    if(super->nextFreeInode==0){//if there is no free finode in the stack,get free node from the disk
        int num=0;
        finode* tmp=(finode*)calloc(1,sizeof(finode));
        fseek(fp,BOOTPOS+SUPERSIZE,SEEK_SET);
        for(int i=0;i<BLOCKSNUM;i++){
            fread(tmp,sizeof(finode),1,fp);
            if(tmp->fileLink == 0){
                super->freeInode[num]=i;
                super->nextFreeInode++;
                num++;
            }
            if(num==20) break;
        }
    }
    if(super->nextFreeInode==0) return NULL;
    super->nextFreeInode--;
    update_super(fp, super);
    return i_get(super->freeInode[super->nextFreeInode]);
}

/* 获取INODE结点 */
inode* i_get(int ino){
    int ipos = 0;
    int ret = 0;
    if(usedinode[ino].userCount!=0) {
        usedinode[ino].userCount++;
        return &usedinode[ino];
    }
    if(fp==NULL) return NULL;
    ipos=BOOTPOS+SUPERSIZE+ino*INODE;
    fseek(fp,ipos,SEEK_SET);
    ret = fread(&usedinode[ino],sizeof(finode),1,fp);
    if(ret != 1) return NULL;
    if(usedinode[ino].finode.fileLink==0){        //it is a new file
        usedinode[ino].finode.fileLink++;
        usedinode[ino].finode.fileSize=0;
        usedinode[ino].inodeID=ino;
        time_t timer;
        time(&timer);
        usedinode[ino].finode.createTime=timer;
        super->freeInodeNum--;
        update_inode(&usedinode[ino],fp);
    }
    usedinode[ino].userCount++;
    usedinode[ino].inodeID=ino;
    return &usedinode[ino];
}
