#include "update.h"

extern supblock* super;
extern FILE * fp;

int update_inode(inode* inode){
    int ino;
    int ret;
    int ipos;
    ipos=BOOTPOS+SUPERSIZE+inode->inodeID*sizeof(finode);
    fseek(fp,ipos,SEEK_SET);
    ret=fwrite(&inode->finode,sizeof(finode),1,fp);
    fflush(fp);
    if(ret!=1) return -1;
    return 0;
}

int update_super(){
    if(fp==NULL) return -1;
    fseek(fp,BOOTPOS,SEEK_SET);
    int res=fwrite(super,sizeof(supblock),1,fp);
    fflush(fp);
    if(res != 1) return -1;
    return 0;
}
