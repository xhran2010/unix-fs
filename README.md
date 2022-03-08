# unix-fs
## 实现功能
* 用文件模拟磁盘
* UNIX文件系统，引入索引结点、成组链接法机制
* 文件重名检测、共享、安全控制（多用户和用户组）
* 分块，包括启动块（保留但无用）、超级块、INODE块和数据块，可实时查看剩余盘块和inode信息
* 支持文件（夹）创建、删除、拷贝、改名
* 实现的指令有：mkdir/ls/cd/pwd/rm/append/cat/mv/cp/chmod/infouseradd
## 项目结构
* main 主函数
* fs 文件系统的format和命令行的进入
* alloc 盘块、INODE的分配和回收
* define 一些宏定义
* update 将内存中的INODE和超级块信息更新到磁盘中
* datastruct 文件系统的相关数据结构
* shell 控制命令行界面的输入和输出
* command 指令的支持
## 使用说明

格式化

```bash
unix-fs format <virtual_disk_path>
```

进入系统

```bash
unix-fs enter <virtual_disk_path>
```

## 使用说明

* 默认root用户，账号：root 密码：123456
* 根INODE结点的INODEID为19，存储用户信息的INODEID为18
* cat/append/cp/chmod命令暂时只支持操作当前目录下的文件（夹），即参数不支持路径寻址
