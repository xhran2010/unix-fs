//
//  main.c
//  unix-fs
//
//  Created by 辛浩然 on 2019/6/4.
//  Copyright © 2019 Jack. All rights reserved.
//

#include <stdio.h>
#include "fs.h"

int main(int argc, const char * argv[]) {
    int res = format("/Users/Jack/Downloads/my.fs", 2, 2048);
    return 0;
}
