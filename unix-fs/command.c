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

inode* curfile;

int mkdir_(char* name,int mode){
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
    tmpinode->finode.mode=mode;
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
        inode* inode_searcher;
        direct direct_searcher;
        int i;
        if(strcmp(dir_list[0], ".") == 0) {
            inode_searcher = current;
            i = 1;
        }
        // 从当前目录开始寻址
        else {
            inode_searcher = root;
            i = 0;
        }// 从根目录开始寻址
        for(;i < index;i++){
            size_t count=inode_searcher->finode.fileSize/sizeof(direct);
            int addrnum=count/63+(count%63>=1?1:0);
            dir * dir_=(dir*)calloc(1,sizeof(dir));
            addrnum > 4 ? addrnum = 4:NULL;
            int has_found = 0;
            for(int addr = 0;addr < addrnum;addr ++){
                b_read(dir_, inode_searcher->finode.addr[addr], 0, sizeof(dir), 1);
                for(int d = 0;d < dir_->dirNum;d++){
                    if(strcmp(dir_->direct[d].directName,dir_list[i])==0){
                        direct_searcher=dir_->direct[d];
                        inode_searcher = i_get(dir_->direct[d].inodeID);
                        has_found = 1;
                        break;
                    }
                }
                if(has_found == 1) break;
                else return -1;// 没找到
            }
            if(has_found == 0) return -1;// 没找到
        }
        if(inode_searcher->finode.mode/1000 == 1){// 是目录
            current = inode_searcher;
            curdirect = direct_searcher;
            return 0;
        }
        else {
            curfile = inode_searcher;
            return -2;
        }// 不是目录
    }
    return -3;
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
        dir * dir_=(dir*)calloc(1,sizeof(dir));
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

int rm(char* path){
    if(strcmp(path, "/") == 0) return -1;// 根目录不能删除
    inode* old = current;
    int res = cd(path);
    if(res == 0 || res == -2){// 删除目录
        if(res == -2) current = curfile;// 删除文件
        inode* parent = i_get(current->finode.parent);
        size_t count=parent->finode.fileSize/sizeof(direct);
        dir * dir_=(dir*)calloc(1,sizeof(dir));
        size_t addrnum=count/63+(count%63>=1?1:0);
        addrnum>4?addrnum=4:NULL;
        int found = 0;
        for(int addr=0;addr<addrnum;addr++){
            b_read(dir_,parent->finode.addr[addr],0,sizeof(dir),1);
            for(int i=0;i<dir_->dirNum;i++){
                if(dir_->direct[i].inodeID == current->inodeID){
                    for(int j=i;j<dir_->dirNum;j++) dir_->direct[j]=dir_->direct[j+1];
                    dir_->dirNum--;
                    b_write(dir_,parent->finode.addr[addr],0,sizeof(dir),1);
                    found = 1;
                    break;
                }
            }
            if(found) break;
        }
        parent->finode.fileSize -= sizeof(direct);
        update_inode(parent);
        int rm_res = rm_inode(current);
    }
    current = old;
    return 0;
}

int append(char* filename,char* content){
    int count=current->finode.fileSize/sizeof(direct);
    dir* dir_=(dir*)calloc(1,sizeof(dir));
    int addrnum = count/63 + (count%63>=1?1:0);
    addrnum>4?addrnum=4:NULL;
    int found = 0;
    int inodeID = -1;
    for(int addr=0;addr<addrnum;addr++){ // 找到这个文件对应的inode
        b_read(dir_,current->finode.addr[addr],0,sizeof(dir),1);
        for(int i=0;i<dir_->dirNum;i++){
            if(strcmp(dir_->direct[i].directName,filename)==0){
                inodeID=dir_->direct[i].inodeID;
                found = 1;
                break;
            }
        }
        if(found) break;
    }
    if(inodeID==-1) return -1;// 找不到文件
    inode* inode_=i_get(inodeID);
    int fileSize=inode_->finode.fileSize;
    int index=0;
    int addr=fileSize/1024;
    int offset=fileSize%1024;
    int charCount=strlen(content)-index;
    int writeCount=(charCount>1024?1024-offset:charCount);
    b_write(&content[index],inode_->finode.addr[addr],offset,sizeof(char),writeCount);
    index += writeCount;
    inode_->finode.fileSize += writeCount;
    while(index<strlen(content)&&addr<4){
        inode_->finode.addr[++addr] = b_alloc();
        offset=inode_->finode.fileSize % 1024;
        charCount = strlen(content)-index;
        writeCount=(charCount>1024?1024-offset:charCount);
        b_write(&content[index],inode_->finode.addr[addr],offset,sizeof(char),writeCount);
        inode_->finode.fileSize+=writeCount;
    }
    update_inode(inode_);
    return 0;
}

int cat(char* filename){
    int inodeID = -1;
    int count=current->finode.fileSize/sizeof(direct);
    dir* dir_=(dir*)calloc(1,sizeof(dir));
    int addrnum=count/63+(count%63>=1?1:0);
    addrnum>4?addrnum=4:NULL;
    int found = 0;
    for(int addr=0;addr<addrnum;addr++){
        b_read(dir_,current->finode.addr[addr],0,sizeof(dir),1);
        for(int i=0;i<dir_->dirNum;i++){
            if(strcmp(dir_->direct[i].directName,filename)==0){
                inodeID=dir_->direct[i].inodeID;
                found = 1;
                break;
            }
        }
        if(found) break;
    }
    if(inodeID == -1) return -1;// 文件不存在
    inode* inode_=i_get(inodeID);
    int addr=inode_->finode.fileSize/1024;
    int lastCount=inode_->finode.fileSize%1024;
    int i=0;
    for(i=0;i<addr;i++){
        char content[1024]={0};
        b_read(&content,inode_->finode.addr[i],0,sizeof(char),1024);
        printf("%.*s",1024,content);
    }
    char content[1024]={0};
    b_read(&content,inode_->finode.addr[i],0,sizeof(char),lastCount);
    printf("%.*s\n",strlen(content),content);
    return 0;
}

int rename_ (char* filename,char* newname){
    int count=current->finode.fileSize/sizeof(direct);
    dir* dir_=(dir*)calloc(1,sizeof(dir));
    int addrnum = count/63 + (count%63>=1?1:0);
    addrnum>4?addrnum=4:NULL;
    for(int addr=0;addr<addrnum;addr++){ // 找到这个文件
        b_read(dir_,current->finode.addr[addr],0,sizeof(dir),1);
        for(int i=0;i<dir_->dirNum;i++){
            if(strcmp(dir_->direct[i].directName,filename)==0){
                strcpy(dir_->direct[i].directName, newname);
                b_write(dir_, current->finode.addr[addr], 0, sizeof(dir), 1);
                return 0;
            }
        }
    }
    return -1;// 没找到文件
}
