//
//  command.h
//  unix-fs
//
//  Created by 辛浩然 on 2019/6/8.
//  Copyright © 2019 Jack. All rights reserved.
//

#ifndef command_h
#define command_h

#include <stdio.h>
#include "fs.h"
#include "datastruct.h"
#include "string.h"
#include "update.h"
#include "alloc.h"

int mkdir_(char* name);
int ls(char** files);
#endif /* command_h */
