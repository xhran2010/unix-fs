//
//  update.h
//  unix-fs
//
//  Created by 辛浩然 on 2019/6/7.
//  Copyright © 2019 Jack. All rights reserved.
//

#ifndef update_h
#define update_h

#include <stdio.h>
#include "define.h"
#include "fs.h"

int update_inode(inode* inode,FILE* fp);
int update_super(FILE* fp,supblock* super);

#endif /* update_h */
