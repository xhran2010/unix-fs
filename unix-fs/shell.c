#include "shell.h"
#include "string.h"
#include <stdlib.h>
extern FILE* fp;
extern supblock* super;
extern user* curuser;
extern int userpos;
extern int logout_;
extern direct curdirect;

void shell(){
    printf("[%s@localhost %s]#",curuser->userName,curdirect.directName);
    char cmd[200];
    fgets(cmd,sizeof(cmd)/sizeof(char),stdin);
    /* 去掉换行符 */
    char* find = strchr(cmd, '\n');
    if(find) *find = '\0';
    char* cmd_ele;
    char* cmd_list[5];
    int index = 0;
    cmd_ele = strtok(cmd, " ");
    while(cmd_ele){
        cmd_list[index] = cmd_ele;
        cmd_ele = strtok(NULL, " ");
        index++;
    }// 获取指令
    if(cmd_list[0] == NULL) return;
    if(strcmp(cmd_list[0], "mkdir") == 0 && index == 2){
        int res = mkdir_(cmd_list[1],1774);
        if(res == 0) printf("目录创建成功\n");
        if(res == -4) printf("access denied\n");
        if(res == -2) printf("当前文件夹已满\n");
        if(res == -1) printf("当前文件不是目录\n");
        if(res == -3) printf("目录已存在\n");
        return;
    }
    if(strcmp(cmd_list[0], "ls") == 0 && index == 1){
        char* files[DIRNUM];
        int res = ls(files);
        for(int i = 0;i<res;i++) printf("%s  ",files[i]);
        printf("\n");
        return;
    }
    if(strcmp(cmd_list[0], "cd") == 0 && index == 2){
        if(strcmp(cmd_list[1], "..") == 0 || strcmp(cmd_list[1], "../") == 0){// 返回上级目录
            int res = cd__();
            if(res == -1) printf("当前已经是根目录\n");
            if(res == -3) printf("access denied\n");
            if(res == -2) printf("目录切换失败");
            return;
        }
        else{
            int res = cd(cmd_list[1]);
            if(res == -3) printf("access denied\n");
            if(res == -1) printf("未找到该路径\n");
            if(res == -2) printf("该路径不是一个目录\n");
            return;
        }
    }
    if(strcmp(cmd_list[0], "pwd") == 0 && index == 1){
        char* path = "";
        int res = pwd(&path);
        printf("%s\n",path);
        return;
    }
    if(strcmp(cmd_list[0], "rm") == 0 && index == 2){
        int res = rm(cmd_list[1]);
        if(res == 0) printf("删除成功\n");
        if(res == -1) printf("根目录不能删除\n");
        if(res == -2) printf("access denied\n");
        return;
    }
    if(strcmp(cmd_list[0], "touch") == 0 && index == 2){
        int res = mkdir_(cmd_list[1],2774);
        if(res == 0) printf("文件创建成功\n");
        if(res == -4) printf("access denied\n");
        if(res == -2) printf("当前文件夹已满\n");
        if(res == -1) printf("当前文件不是目录\n");
        if(res == -3) printf("文件已存在\n");
        return;
    }
    if(strcmp(cmd_list[0], "append") == 0 && index == 3){
        int res = append(cmd_list[1], cmd_list[2]);
        if(res == 0) printf("文件修改成功\n");
        if(res == -1) printf("找不到文件\n");
        if(res == -2) printf("access denied\n");
        return;
    }
    if(strcmp(cmd_list[0], "cat") == 0 && index == 2){
        int res = cat(cmd_list[1]);
        if(res == -1) printf("找不到文件\n");
        if(res == -2) printf("access denied\n");
        return;
    }
    if(strcmp(cmd_list[0], "mv") == 0 && index == 3){
        int res = rename_(cmd_list[1], cmd_list[2]);
        if(res == 0) printf("修改文件名成功\n");
        if(res == -1) printf("找不到文件\n");
        if(res == -2) printf("access denied\n");
        return;
    }
    if(strcmp(cmd_list[0], "chmod") == 0 && index == 3){
        int res = chmod_(cmd_list[1], atoi(cmd_list[2]));
        if(res == 0) printf("权限修改成功\n");
        if(res == -1) printf("找不到文件\n");
        if(res == -2) printf("access denied\n");
        return;
    }
    if(strcmp(cmd_list[0], "info") == 0 && index == 1){
        info();
        return;
    }
    if(strcmp(cmd_list[0], "cp") == 0 && index == 3){
        int res = cp(cmd_list[1], cmd_list[2]);
        if(res == 0)printf("拷贝成功\n");
        if(res == -1) printf("找不到文件\n");
        if(res == -2) printf("当前文件夹已满\n");
        if(res == -3) printf("文件已存在\n");
        return;
    }
    if(strcmp(cmd_list[0], "useradd") == 0 && index == 4){
        int res = useradd(cmd_list[1], cmd_list[2], cmd_list[3]);
        if(res == -1) printf("permission denied\n");
        else printf("用户创建成功\n");
    }
    if(strcmp(cmd_list[0], "exit") == 0 && index == 1){
        logout_ = 1;
        return;
    }
    return;
}

int login_(){
    char ch;
    inode* userinode = i_get(18);
    int usernum = userinode->finode.fileSize/sizeof(user);
    user* users=(user*)calloc(usernum,sizeof(user));
    b_read(users,userinode->finode.addr[0],0,sizeof(user),usernum);
    char user[MAXNAME]={0},pwd[MAXPWD]={0};
    printf("请输入账号：");
    scanf("%s",user);
    for(int i=0;i<usernum;i++){
        if(strcmp(users[i].userName,user)==0){
            printf("请输入密码：");
            scanf("%s",pwd);
            if(strcmp(users[i].userPwd,pwd)==0){
                curuser=&users[i];
                userpos=i;
                logout_ = 0;
                getchar();
                return 0;
            }
            else{
                printf("密码错误\n");
                return -1;
            }
        }
    }
    printf("用户不存在\n");
    return -1;
}
