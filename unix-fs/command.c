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
extern direct curdirect;
extern inode *root;

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
    tmpinode->finode.parent = current->inodeID;
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
    size_t count = current->finode.fileSize/sizeof(direct);
    dir * dir_ = (dir*)calloc(1,sizeof(dir));
    int addrnum=count/63+(count%63>=1?1:0);
    addrnum > 4 ? addrnum = 4:NULL;
    for(int addr=0;addr<addrnum;addr++){
        b_read(dir_,current->finode.addr[addr],0,sizeof(dir),1);
        for(int i=0;i<dir_->dirNum;i++) *(files+i) = dir_->direct[i].directName;
    }
    return dir_->dirNum;
}

/* 返回上级目录 */
int cd__(){
    size_t inodeid=current->inodeID;
    inode* inode=i_get(current->finode.parent);
    if(inode == NULL) return -1; //错误，已经是根目录
    size_t count=inode->finode.fileSize/sizeof(direct);
    dir * dir_=(struct dir*)calloc(1,sizeof(dir));
    size_t addrnum=count/63+(count%63>=1?1:0);
    addrnum>4?addrnum=4:NULL;
    for(int addr=0;addr<addrnum;addr++){
        b_read(dir_,inode->finode.addr[addr],0,sizeof(dir),1);
        for(int i=0;i<dir_->dirNum;i++){
            if(dir_->direct[i].inodeID==inodeid){
                curdirect=dir_->direct[i];
                current = inode;
                return 0;
            }
        }
    }
    return -2;// 未知错误
}

int cd(char* path){
    if(path[0] == '/' && strlen(path) == 1){ // 返回根目录
        current = root;
        return 0;
    }
    if(strcmp(path, ".") == 0 || strcmp(path, "./") == 0){ // 当前目录
        return 0;
    }
    char* dir_ele;
    char* dir_list[20];
    int index = 0;
    dir_ele = strtok(path, "/");
    while(dir_ele){
        dir_list[index] = dir_ele;
        dir_ele = strtok(NULL, "/");
        index++;
    }// 获取目录
    if(path[0] == '.' || path[0] == '/'){
        inode* inode_seacher;
        direct direact_searcher;
        int i;
        if(strcmp(dir_list[0], ".") == 0) {
            inode_seacher = current;
            i = 1;
        }
        // 从当前目录开始寻址
        else {
            inode_seacher = root;
            i = 0;
        }// 从根目录开始寻址
        for(;i < index;i++){
            size_t count=inode_seacher->finode.fileSize/sizeof(direct);
            int addrnum=count/63+(count%63>=1?1:0);
            dir * dir_=(dir*)calloc(1,sizeof(dir));
            addrnum > 4 ? addrnum = 4:NULL;
            int has_found = 0;
            for(int addr = 0;addr < addrnum;addr ++){
                b_read(dir_, inode_seacher->finode.addr[addr], 0, sizeof(dir), 1);
                for(int d = 0;d < dir_->dirNum;d++){
                    if(strcmp(dir_->direct[d].directName,dir_list[i])==0){
                        direact_searcher=dir_->direct[d];
                        inode_seacher = i_get(dir_->direct[d].inodeID);
                        has_found = 1;
                        break;
                    }
                }
                if(has_found == 1) break;
                else return -1;// 没找到
            }
            if(has_found == 0) return -1;// 没找到
        }
        current = inode_seacher;
        curdirect = direact_searcher;
        return 0;
    }
    return -1;
}

int pwd(char** path){
    inode* i_searcher = current;
    char* path_list[50];// 文件目录的最大深度定义为50
    int idx = 0;
    if(i_searcher->finode.parent == -1){
        *path = "/";
        return 0;
    }
    while(i_searcher->finode.parent != -1){
        int current_id = i_searcher->inodeID;
        i_searcher = i_get(i_searcher->finode.parent);
        size_t count=i_searcher->finode.fileSize/sizeof(direct);
        dir * dir_=(struct dir*)calloc(1,sizeof(dir));
        size_t addrnum=count/63+(count%63>=1?1:0);
        addrnum>4?addrnum=4:NULL;
        int has_found = 0;
        for(int addr=0;addr<addrnum;addr++){
            b_read(dir_,i_searcher->finode.addr[addr],0,sizeof(dir),1);
            for(int i=0;i<dir_->dirNum;i++){
                if(dir_->direct[i].inodeID==current_id){
                    path_list[idx] = dir_->direct[i].directName;
                    idx++;
                    has_found = 1;
                    break;
                }
            }
            if(has_found) break;
        }
    }
    char tmp[500] = "/";
    for(int i=idx-1;i>=0;i--){
        strcat(tmp, path_list[i]);
        strcat(tmp, "/");
    }
    *path = tmp;
    return 0;
}
