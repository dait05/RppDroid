/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#define LOG_TAG "sec_env"

#include "utils/Log.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>		/* open */
#include "swl_monitor_SecEnvManager.h"
#include "jni.h"
#include "SecEnv.h"

/*
 * Helper function to throw an arbitrary exception.
 *
 * Takes the exception class name, a format string, and one optional integer
 * argument (useful for including an error code, perhaps from errno).
 */
static void throwException(JNIEnv* env, const char* ex, const char* fmt,
    int data) {

    if (jclass cls = env->FindClass(ex)) {
        if (fmt != NULL) {
            char msg[1000];
            snprintf(msg, sizeof(msg), fmt, data);
            env->ThrowNew(cls, msg);
        } else {
            env->ThrowNew(cls, NULL);
        }

        /*
         * This is usually not necessary -- local references are released
         * automatically when the native code returns to the VM.  It's
         * required if the code doesn't actually return, e.g. it's sitting
         * in a native event loop.
         */
        env->DeleteLocalRef(cls);
    }
}

int JNICALL Java_swl_monitor_SecEnvManager_native_1SecEnvOpen(JNIEnv* env, jobject thiz)
{
	int file_desc, ret_val;
	char *msg = (char*)"Message passed by ioctl\n";

	file_desc = open(DEVICE_FILE_NAME, 0);
	if (file_desc < 0) {
		LOGE("ERROR: Can't open device file: %s\n", DEVICE_FILE_NAME);
	}
       return file_desc;
}

int JNICALL JNICALL Java_swl_monitor_SecEnvManager_native_1SecEnvGetRuleNum(JNIEnv* env, jobject thiz, jint desc)
{
	int rule_num = 0;
	ioctl_get_rule_num(desc, &rule_num); 
	return rule_num;
}

void JNICALL Java_swl_monitor_SecEnvManager_native_1SecEnvClearRules(JNIEnv* env, jobject thiz, jint desc)
{
	ioctl_clear_rules(desc);
}

void JNICALL Java_swl_monitor_SecEnvManager_native_1SecEnvAddRule(JNIEnv* env, jobject thiz, jint desc, jstring str)
{
    if (str == NULL) {
        throwException(env, "java/lang/NullPointerException", NULL, 0);
    }

     //Get the native string from javaString
     const char *nativeString = env->GetStringUTFChars(str, 0);
 
     //Do something with the nativeString
 
    struct sec_policy sp1;
    memcpy(sp1.rule, nativeString, sizeof(sp1.rule));
    ioctl_add_rule(desc, &sp1);

    //DON'T FORGET THIS LINE!!!
    env->ReleaseStringUTFChars(str, nativeString);

}

jstring JNICALL Java_swl_monitor_SecEnvManager_native_1SecEnvGetNthRule(JNIEnv * env, jobject thiz, jint desc, jint index)
{
	jstring javaString = NULL;
	struct sec_policy sp;
	int ret = ioctl_get_nth_rule(desc, index, &sp);
	if(ret == 0)
	{
		javaString = env->NewStringUTF(sp.rule);
	}
	return javaString;
}

void JNICALL Java_swl_monitor_SecEnvManager_native_1SecEnvClose(JNIEnv* env, jobject thiz, jint desc)
{
    close(desc);
}

static JNINativeMethod sMethods[] = {
     /* name, signature, funcPtr */
	{"native_SecEnvOpen", "()I", (void*)JNICALL Java_swl_monitor_SecEnvManager_native_1SecEnvOpen},
	{"native_SecEnvClearRules", "(I)V", (void*)JNICALL Java_swl_monitor_SecEnvManager_native_1SecEnvClearRules},
	{"native_SecEnvAddRule", "(ILjava/lang/String;)V", (void*)JNICALL Java_swl_monitor_SecEnvManager_native_1SecEnvAddRule},
	{"native_SecEnvGetRuleNum", "(I)I", (void*)JNICALL Java_swl_monitor_SecEnvManager_native_1SecEnvGetRuleNum},
	{"native_SecEnvGetNthRule", "(II)Ljava/lang/String;", (void*)JNICALL Java_swl_monitor_SecEnvManager_native_1SecEnvGetNthRule},
	{"native_SecEnvClose", "(I)V", (void*)JNICALL Java_swl_monitor_SecEnvManager_native_1SecEnvClose},
};

/*
 * Explicitly register all methods for our class.
 *
 * While we're at it, cache some class references and method/field IDs.
 *
 * Returns 0 on success.
 */
static int registerMethods(JNIEnv* env) {
    static const char* const kClassName = "swl/monitor/SecEnvManager";
    jclass clazz;

    /* look up the class */
    clazz = env->FindClass(kClassName);
    if (clazz == NULL) {
        LOGE("Can't find class %s\n", kClassName);
        return -1;
    }

    /* register all the methods */
    if (env->RegisterNatives(clazz, sMethods,
            sizeof(sMethods) / sizeof(sMethods[0])) != JNI_OK)
    {
        LOGE("Failed registering methods for %s\n", kClassName);
        return -1;
    }

    return 0;
}

// ----------------------------------------------------------------------------

/*
 * This is called by the VM when the shared library is first loaded.
 */
jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env = NULL;
    jint result = -1;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        LOGE("ERROR: GetEnv failed\n");
        goto bail;
    }
    assert(env != NULL);

    if (registerMethods(env) != 0) {
        LOGE("ERROR: SecEnvManager native registration failed\n");
        goto bail;
    }

    /* success -- return valid version number */
    result = JNI_VERSION_1_4;

bail:
    return result;
}

