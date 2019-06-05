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
    format("/Users/Jack/Downloads/test.fs",2,2048);
    return 0;
}
