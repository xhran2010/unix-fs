#include "alloc.h"
#include <stdio.h>
#include "define.h"
#include "time.h"
#include "fs.h"

extern FILE* fp;
extern supblock* super;
extern inode usedinode[INODESNUM];

/* 盘块写内容 */
int b_write(void * buf,size_t bno,size_t offset,size_t size,int count){
    long int pos;
    size_t ret;
    pos = (BLOCKSTART+bno)*BLOCKSIZE+offset;
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
    pos = (BLOCKSTART+bno)*BLOCKSIZE+offset;
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
    update_super();
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
    update_super();
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
    update_super();
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
    if(ino == -1) return NULL;
    if(fp==NULL) return NULL;
    ipos=BOOTPOS+SUPERSIZE+ino*sizeof(finode);
    fseek(fp,ipos,SEEK_SET);
    ret = fread(&usedinode[ino],sizeof(finode),1,fp);
    if(ret != 1) return NULL;
    if(usedinode[ino].finode.fileLink==0){ // 新文件
        usedinode[ino].finode.fileLink++;
        usedinode[ino].finode.fileSize=0;
        usedinode[ino].inodeID=ino;
        time_t timer;
        time(&timer);
        usedinode[ino].finode.createTime=timer;
        super->freeInodeNum--;
        update_inode(&usedinode[ino]);
        update_super();
    }
    usedinode[ino].userCount++;
    usedinode[ino].inodeID=ino;
    return &usedinode[ino];
}

int rm_inode(inode* node){
    inode* rminode = NULL;
    dir* dir_=(dir*)calloc(1,sizeof(dir));
    int count,addrnum,found;
    count=node->finode.fileSize/sizeof(direct);
    addrnum = count/63 + (count%63>=1?1:0);
    addrnum > 4 ? addrnum=4:NULL;
    for(int addr=0;addr<addrnum;addr++){
        b_read(dir_,node->finode.addr[addr],0,sizeof(dir),1);
        for(int i=0;i<dir_->dirNum;i++){
            rminode = i_get(dir_->direct[i].inodeID);
            if(rminode->finode.mode/1000 == 1) rm_inode(rminode);// 文件夹，递归删除
            /* 删除文件 */
            else{
                int rmaddr=rminode->finode.fileSize/1024+(rminode->finode.fileSize%1024==0?0:1);
                if(rminode->finode.fileSize==0) rmaddr=1;
                for(int i=0;i<rmaddr;i++) b_free(rminode->finode.addr[i]);
                rminode->finode.fileLink--;
                rminode->userCount--;
                update_inode(rminode);
            }
        }
        b_free(node->finode.addr[addr]);
    }
    if(addrnum == 0) b_free(node->finode.addr[0]);
    node->userCount--;
    node->finode.fileLink--;
    update_inode(node);
    if(node->finode.fileLink == 0){
        super->freeInodeNum ++;
        update_super();
    }
    return 0;
}
