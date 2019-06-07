#include <stdio.h>
#include "fs.h"
#include "alloc.h"

int main(int argc, const char * argv[]) {
    int arg_count = argc - 1;
    char** args = argv + 1;
    if(arg_count == 0){
        printf("参数错误，请重试");
        return 0;
    }
    /* format */
    if(args[0] == "format"){
        if(arg_count != 2) {
            printf("参数错误，请重试");
            return 0;
        }
        int res = format(args[1]);
        if(res == -1) printf("format操作失败");
        else printf("format操作成功");
        return 0;
    }
    /* 进入系统 */
    if(args[0] == "enter"){
        if(arg_count != 2) {
            printf("参数错误，请重试");
            return 0;
        }
        int res = enter(args[1]);
        if(res == -1) printf("进入失败，请检查文件是否有效或损坏");
        return 0;
    }
    return 0;
}
