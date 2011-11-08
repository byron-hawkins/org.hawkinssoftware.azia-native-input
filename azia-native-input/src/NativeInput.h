#include <jni.h>

/* Header for class MouseHook */
#ifndef _Included_MouseHook
#define _Included_MouseHook
#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL Java_org_hawkinssoftware_azia_input_NativeInput_startNativeEventHook(JNIEnv * env, jobject thisObject);
JNIEXPORT void JNICALL Java_org_hawkinssoftware_azia_input_NativeInput_stopNativeEventHook(JNIEnv * env, jobject thisObject);

JNIEXPORT void JNICALL Java_org_hawkinssoftware_azia_input_NativeInput_defineMetaKey(JNIEnv * env, jobject thisObject, jint index, jint keyCode1, jint keyCode2, jint keyCode3);
JNIEXPORT void JNICALL Java_org_hawkinssoftware_azia_input_NativeInput_clearMetaKey(JNIEnv * env, jobject thisObject);

#ifdef __cplusplus
}
#endif
#endif
