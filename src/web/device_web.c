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
#include <cherry/platform.h>

#if OS == WEB

#include <native_ui/device.h>
#include <cherry/string.h>
#include <cherry/memory.h>
#include <cherry/stdio.h>
#include <cherry/array.h>
#include <cherry/map.h>

struct string *ndevice_get()
{
        return string_alloc_chars(qlkey("webdevice"));
}

#endif
