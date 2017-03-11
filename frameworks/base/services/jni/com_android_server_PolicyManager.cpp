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

#define LOG_TAG "policy"

#include "utils/Log.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>		/* open */
#include "com_android_server_PolicyManager.h"
#include "jni.h"
#include "Policy.h"


//added by daiting for loge error
//#define LOG_TAG "libJNIExInterface"
//#define LOGI(...) android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS)
//#define LOGE(...) android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS)


namespace android {

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

int JNICALL Java_swl_monitor_PolicyManager_native_1PolicyOpen(JNIEnv* env, jobject thiz)
{
	int file_desc, ret_val;
	char *msg = (char*)"Message passed by ioctl\n";
	
	file_desc = open(DEVICE_FILE_NAME, O_RDWR);
	if (file_desc < 0) {
		ALOGE("ERROR: Can't open device file: %s\n", DEVICE_FILE_NAME);
	}
       return file_desc;
}

int JNICALL JNICALL Java_swl_monitor_PolicyManager_native_1PolicyGetRuleNum(JNIEnv* env, jobject thiz, jint desc)
{
	int rule_num = 0;
	ioctl_get_rule_num(desc, &rule_num); 
	return rule_num;
}

void JNICALL Java_swl_monitor_PolicyManager_native_1PolicyClearRules(JNIEnv* env, jobject thiz, jint desc)
{
	ioctl_clear_rules(desc);
}

void JNICALL Java_swl_monitor_PolicyManager_native_1PolicyAddRule(JNIEnv* env, jobject thiz, jint desc, jstring str)
{
	if (str == NULL) {
	ALOGE("*********************daiting str is null  add rule @@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
        throwException(env, "java/lang/NullPointerException", NULL, 0);
    }

     //Get the native string from javaString
     const char *nativeString = env->GetStringUTFChars(str, 0);
 
     //Do something with the nativeString
    struct sec_policy sp1;
    memcpy(sp1.rule, nativeString, sizeof(sp1.rule));
//ALOGE("*********************daiting before ioctl_add_rule @@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    ioctl_add_rule(desc, &sp1);
//ALOGE("*********************daiting after ioctl_add_rule @@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    //DON'T FORGET THIS LINE!!!
    env->ReleaseStringUTFChars(str, nativeString);

}

void JNICALL Java_swl_monitor_PolicyManager_native_1PolicyExternalStorage(JNIEnv* env, jobject thiz, jint desc, jstring str)
{
     if (str == NULL) {
	ALOGE("*********************str is null @@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
        throwException(env, "java/lang/NullPointerException", NULL, 0);
     }

     //Get the native string from javaString
     const char *nativeString = env->GetStringUTFChars(str, 0);
 
     //Do something with the nativeString
    struct sec_external_storage sp1;
    memcpy(sp1.storage, nativeString, sizeof(sp1.storage));
    ioctl_external_storage(desc, &sp1);

    //DON'T FORGET THIS LINE!!!
    env->ReleaseStringUTFChars(str, nativeString);

}


/*
jstring JNICALL Java_swl_monitor_PolicyManager_native_1PolicyGetNthRule(JNIEnv * env, jobject thiz, jint desc, jint index)
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
*/
void JNICALL Java_swl_monitor_PolicyManager_native_1PolicyClose(JNIEnv* env, jobject thiz, jint desc)
{
    close(desc);
}

/*
static JNINativeMethod sMethods[] = {
      name, signature, funcPtr 
	{"native_PolicyOpen", "()I", (void*)JNICALL Java_swl_monitor_PolicyManager_native_1PolicyOpen},
	{"native_PolicyClearRules", "(I)V", (void*)JNICALL Java_swl_monitor_PolicyManager_native_1PolicyClearRules},
	{"native_PolicyAddRule", "(ILjava/lang/String;)V", (void*)JNICALL Java_swl_monitor_PolicyManager_native_1PolicyAddRule},
	{"native_PolicyGetRuleNum", "(I)I", (void*)JNICALL Java_swl_monitor_PolicyManager_native_1PolicyGetRuleNum},
	{"native_PolicyGetNthRule", "(II)Ljava/lang/String;", (void*)JNICALL Java_swl_monitor_PolicyManager_native_1PolicyGetNthRule},
	{"native_PolicyClose", "(I)V", (void*)JNICALL Java_swl_monitor_PolicyManager_native_1PolicyClose},
};
*/

static JNINativeMethod sMethods[] = {
     /* name, signature, funcPtr */
    {"native_PolicyOpen", "()I", (void*)JNICALL Java_swl_monitor_PolicyManager_native_1PolicyOpen},
    {"native_PolicyClearRules", "(I)V", (void*)JNICALL Java_swl_monitor_PolicyManager_native_1PolicyClearRules},
    {"native_PolicyAddRule", "(ILjava/lang/String;)V", (void*)JNICALL Java_swl_monitor_PolicyManager_native_1PolicyAddRule},
    {"native_PolicyGetRuleNum", "(I)I", (void*)JNICALL Java_swl_monitor_PolicyManager_native_1PolicyGetRuleNum},
    {"native_PolicyClose", "(I)V", (void*)JNICALL Java_swl_monitor_PolicyManager_native_1PolicyClose},
};

/*
 * Explicitly register all methods for our class.
 *
 * While we're at it, cache some class references and method/field IDs.
 *
 * Returns 0 on success.
 */


int register_android_server_PolicyManager(JNIEnv* env) {
	
	static const char* const kClassName = "com/android/server/PolicyManager";
    jclass clazz;

    /* look up the class */
    clazz = env->FindClass(kClassName);
    if (clazz == NULL) {
        ALOGE("Can't find class %s\n", kClassName);
        return -1;
    }

    /* register all the methods */
    if (jniRegisterNativeMethods(env, kClassName, sMethods, NELEM(sMethods)) != JNI_OK)
    {
        ALOGE("Failed registering methods for %s\n", kClassName);
        return -1;
    }
ALOGE("daiting succeeded in registering methods for %s\n", kClassName);
    return 0;
}

} // namespace android