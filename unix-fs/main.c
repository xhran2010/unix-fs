#include <stdio.h>
#include "string.h"
#include "shell.h"

int main(int argc, const char * argv[]) {
    int arg_count = argc - 1;
    char** args = argv + 1;
    if(arg_count == 0){
        printf("参数错误，请重试\n");
        return 0;
    }
    /* format */
    if(strcmp(args[0], "format") == 0){
        if(arg_count != 2) {
            printf("参数错误，请重试\n");
            return 0;
        }
        int res = format(args[1]);
        if(res == -1) printf("format操作失败\n");
        else printf("format操作成功\n");
        return 0;
    }
    /* 进入系统 */
    if(strcmp(args[0], "enter") == 0){
        if(arg_count != 2) {
            printf("参数错误，请重试\n");
            return 0;
        }
        int res = init(args[1]);
        if(res == -1) printf("进入失败，请检查文件是否有效或损坏\n");
        while(login_() == -1){}
        while(shell() == 0){}
        exit_();
        return 0;
    }
    return 0;
}

