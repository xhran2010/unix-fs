#ifndef define_h
#define define_h

/*
 0# 启动块（用不上，但保留
 1# 超级块
 2#-911# inode区域，inode大小为128B，盘块大小为1KB，每个盘块8个inode，inode总共有8*910=7280个
 912#-8191# 文件存储区域
 */

#define INODENUM 20 // 每组盘块数
#define BLOCKNUM 20 // 每组盘块数
#define MAXNAME 20 // 暂时用不到
#define MAXPWD 20 // 暂时用不到
#define DIRECTNAME 14 // 暂时用不到
#define GROUPNAME 15 // 暂时用不到
#define DIRNUM 63 // 暂时用不到
#define BOOTPOS 1024 // 超级块开始位置
#define SUPERSIZE 1024 // 超级块大小
#define INODESIZE 931840 // inode总大小，7280*128 = 931840
#define INODENUM 7280
#define INODE 128 // 每个inode的大小
#define BLOCKSTART 912 // 存储block开始编号
#define BLOCKSNUM 7280 // 存储block的个数
#define BLOCKSIZE 1024 // 每个block的大小
#define BLOCKSIZE 1024

#endif /* define_h */
