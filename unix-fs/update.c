//
//  update.c
//  unix-fs
//
//  Created by 辛浩然 on 2019/6/7.
//  Copyright © 2019 Jack. All rights reserved.
//

#include "update.h"

int update_inode(inode* inode,FILE* fp){
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

int update_super(FILE* fp,supblock* super){
    if(fp==NULL) return -1;
    fseek(fp,BOOTPOS,SEEK_SET);
    int writeSize=fwrite(super,sizeof(supblock),1,fp);
    fflush(fp);
    if(writeSize!=1) return -1;
    return 0;
}
