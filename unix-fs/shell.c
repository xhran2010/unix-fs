#include "shell.h"
#include "string.h"
extern FILE* fp;
extern supblock* super;

void shell(){
    printf(">");
    char cmd[200];
    fgets(cmd,sizeof(cmd)/sizeof(char), stdin);
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
    if(strcmp(cmd_list[0], "mkdir") == 0 && index == 2){
        int res = mkdir_(cmd_list[1]);
        if(res == 0) printf("目录创建成功\n");
        else printf("目录创建失败\n");
        return;
    }
    if(strcmp(cmd_list[0], "ls") == 0 && index == 1){
        char* files[DIRNUM];
        int res = ls(files);
        if(res > 0) for(int i = 0;i<res;i++) printf("%s  ",files[i]);
        printf("\n");
        return;
    }
    return;
}
