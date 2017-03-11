/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class swl_monitor_PolicyManager */

#ifndef _Included_swl_monitor_PolicyManager
#define _Included_swl_monitor_PolicyManager
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     swl_monitor_PolicyManager
 * Method:    native_PolicyOpen
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_swl_monitor_PolicyManager_native_1PolicyOpen
  (JNIEnv *, jobject);

/*
 * Class:     swl_monitor_PolicyManager
 * Method:    native_PolicyClearRules
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_swl_monitor_PolicyManager_native_1PolicyClearRules
  (JNIEnv *, jobject, jint);

/*
 * Class:     swl_monitor_PolicyManager
 * Method:    native_PolicyAddRule
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_swl_monitor_PolicyManager_native_1PolicyAddRule
  (JNIEnv *, jobject, jint, jstring);

/*
 * Class:     swl_monitor_PolicyManager
 * Method:    native_PolicyGetRuleNum
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_swl_monitor_PolicyManager_native_1PolicyGetRuleNum
  (JNIEnv *, jobject, jint);

/*
 * Class:     swl_monitor_PolicyManager
 * Method:    native_PolicyExternalStorage
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_swl_monitor_PolicyManager_native_1PolicyExternalStorage
  (JNIEnv *, jobject, jint, jstring);


/*
 * Class:     swl_monitor_PolicyManager
 * Method:    native_PolicyGetNthRule
 * Signature: (II)Ljava/lang/String;

JNIEXPORT jstring JNICALL Java_swl_monitor_PolicyManager_native_1PolicyGetNthRule
  (JNIEnv *, jobject, jint, jint);
*/
/*
 * Class:     swl_monitor_PolicyManager
 * Method:    native_PolicyClose
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_swl_monitor_PolicyManager_native_1PolicyClose
  (JNIEnv *, jobject, jint);

#ifdef __cplusplus
}
#endif
#endif