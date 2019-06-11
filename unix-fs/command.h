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

int mkdir_(char* name,int mode);
int ls(char** files);
int cd__(void);
int cd(char* path);
int pwd(char** path);
int rm(char* path);
int append(char* filename,char* content);
int cat(char* filename);
int rename_(char* filename,char* newname);
int chmod_(char* filename,int mode);
int verify(inode* node,int mode);

#endif /* command_h */
