/*
 * Copyright (C) 2017 Manh Tran
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <native_ui/file.h>
#include <cherry/stdio.h>
#include <cherry/string.h>
#include <cherry/memory.h>

struct string *nfile_fullpath(char *file)
{
        // if(strncmp(file, INNER_PATH, sizeof(INNER_PATH) - 1) == 0) {
        //         return file_full_path(file, FILE_INNER);
        // } else if(strncmp(file, LOCAL_PATH, sizeof(LOCAL_PATH) - 1) == 0) {
        //         return file_full_path(file, FILE_LOCAL);
        // } else if(strncmp(file, SD_PATH, sizeof(SD_PATH) - 1) == 0){
        //         return file_full_path(file, FILE_SDCARD);
        // } else {
        //         return NULL;
        // }
        return file_xpath(file);
}

u8 nfile_type(char *file)
{
        if(strncmp(file, qlkey("inner")) == 0) {
                return FILE_INNER;
        } else if(strncmp(file, qlkey("local")) == 0) {
                return FILE_LOCAL;
        } else if(strncmp(file, SD_PATH, sizeof(SD_PATH) - 1) == 0){
                return FILE_SDCARD;
        } else {
                return -1;
        }
}

struct file *nfile_open(char *path, char *mode)
{
        return file_xopen(path, mode);
        // if(strncmp(path, INNER_PATH, sizeof(INNER_PATH) - 1) == 0) {
        //         return file_open(path, mode, FILE_INNER);
        // } else if(strncmp(path, LOCAL_PATH, sizeof(LOCAL_PATH) - 1) == 0) {
        //         return file_open(path, mode, FILE_LOCAL);
        // } else if(strncmp(path, SD_PATH, sizeof(SD_PATH) - 1) == 0){
        //         return file_open(path, mode, FILE_SDCARD);
        // } else {
        //         return NULL;
        // }
}
