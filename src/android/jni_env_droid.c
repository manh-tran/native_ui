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
#if OS == DROID

#include <native_ui/types.h>
#include <cherry/map.h>
#include <cherry/array.h>
#include <pthread.h>

//JNIEnv *__jni_env_current_thread()
//{
//        pthread_t tid = pthread_self();
//
//        if( ! map_has_key(__jni_env_map, &tid, sizeof(tid))) {
//                JNIEnv* myNewEnv;
//                JavaVMAttachArgs args;
//                args.version                 = JNI_VERSION_1_4;
//                args.name = NULL; // you might want to give the java thread a name
//                args.group = NULL; // you might want to assign the java thread to a ThreadGroup
//
//                (*__jvm)->AttachCurrentThread(__jvm, (void**)&myNewEnv, &args );
//
//                map_set(__jni_env_map, &tid, sizeof(tid), &myNewEnv);
//        }
//
//        return map_get(__jni_env_map, JNIEnv *, &tid, sizeof(tid));
//}

#endif
