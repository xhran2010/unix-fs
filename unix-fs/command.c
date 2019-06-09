//
//  command.c
//  unix-fs
//
//  Created by 辛浩然 on 2019/6/8.
//  Copyright © 2019 Jack. All rights reserved.
//

#include "command.h"
extern inode *current;
extern FILE* fp;

int mkdir_(char* name){
    if(current->finode.mode/1000 != 1) return -1; // 不是文件夹
    size_t file_num = current->finode.fileSize/sizeof(direct);
    if(file_num > 252) return -2;// 满 FIXME:为什么是252？
    dir* dir_ = (dir*)calloc(1, sizeof(dir));
    size_t addrnum=file_num/63+(file_num%63>=1?1:0);// FIXME: 为什么是63？——因为一个dir结构体存64个子目录哈！
    addrnum>4?addrnum=4:NULL;
    for(int addr = 0;addr < addrnum;addr++){
        b_read(dir_, current->finode.addr[addr], 0, sizeof(dir), 1);// 读出当前目录的dir块
        for(int i=0;i<dir_->dirNum;i++) if(strcmp(name,dir_->direct[i].directName) == 0) return -3;// 文件重复
    }
    current->finode.fileSize += sizeof(direct);
    update_inode(current);// 更新磁盘中的Inode结点
    int addr = file_num/63;
    b_read(dir_,current->finode.addr[addr],0,sizeof(dir),1);
    strcpy(dir_->direct[dir_->dirNum].directName,name);
    inode * tmpinode=i_alloc();
    tmpinode->finode.addr[0]=b_alloc();
    tmpinode->finode.mode=1774;
    //strcpy(tmpinode->finode.owner,curuser->userName);
    //strcpy(tmpinode->finode.group,curuser->userGroup);
    update_inode(tmpinode);
    dir_->direct[dir_->dirNum].inodeID=tmpinode->inodeID;
    dir_->dirNum+=1;
    b_write(dir_,current->finode.addr[addr],0,sizeof(dir),1);
    return 0;
}

int ls(char** files){
    if(current->finode.mode/1000!=1) return -1;// 不是文件夹
    int count = current->finode.fileSize/sizeof(direct);
    dir * dir_ = (dir*)calloc(1,sizeof(dir));
    int addrnum=count/63+(count%63>=1?1:0);
    addrnum > 4 ? addrnum = 4:NULL;
    for(int addr=0;addr<addrnum;addr++){
        b_read(dir_,current->finode.addr[addr],0,sizeof(dir),1);
        for(int i=0;i<dir_->dirNum;i++) *(files+i) = dir_->direct[i].directName;
    }
    return dir_->dirNum;
}

