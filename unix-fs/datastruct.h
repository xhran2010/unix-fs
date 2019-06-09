#ifndef datastruct_h
#define datastruct_h
#include "define.h"
typedef struct
{
    size_t freeBlock[BLOCKNUM];    //the stack of the free block
    size_t nextFreeBlock;        //the pointer of the next free block in the stack
    size_t freeBlockNum;            //the totally number of the free block in the disk
    size_t freeInode[INODENUM];    //the stack of the free node
    size_t freeInodeNum;            //the totally number of the free inode in the disk
    size_t nextFreeInode;        //the next free inode in the stack
} supblock;

typedef struct
{
    size_t mode;
    size_t fileSize;
    size_t fileLink;
    size_t owner[MAXNAME];
    size_t group[GROUPNAME];
    size_t modifyTime;
    size_t createTime;
    size_t addr[6];
    char black[45];// 补位
} finode;

typedef struct
{
    finode finode;
    struct inode *parent;
    size_t inodeID;                //the node id
    size_t userCount;            //the number of process using the inode
} inode;

//direct structure
typedef struct
{
    char directName[DIRECTNAME];
    size_t inodeID;
} direct;

//the structure of dir
typedef struct
{
    size_t dirNum;
    direct direct[DIRNUM];
} dir;

//the structure of file
typedef struct
{
    inode* fInode;
    size_t f_curpos;
} file;

//the structure of user

typedef struct
{
    char userName[MAXNAME];
    char userPwd[MAXPWD];
    char userGroup[MAXNAME];
} user;

#endif /* datastruct_h */
