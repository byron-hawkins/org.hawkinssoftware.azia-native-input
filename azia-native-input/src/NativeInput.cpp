#include <windows.h>
#include <jni.h>
#include <iostream>
#include "NativeInput.h"

HINSTANCE hInst = NULL;

JavaVM * jvm = NULL;
DWORD hookThreadId = 0;

jobject javaHostObject = NULL;
jmethodID processMouseButtonMethod = NULL;
jmethodID processMouseMoveMethod = NULL;
jmethodID processKeyMethod = NULL;

LONG mouseLocationX = -1, mouseLocationY = -1;
bool controlDown = false;
bool gDown = false;

LRESULT CALLBACK MouseHook(int nCode, WPARAM wParam, LPARAM lParam) {
	JNIEnv* env;
	if (jvm->AttachCurrentThread((void **) &env, NULL) >= 0) {
		if (nCode == HC_ACTION) {
			MOUSEHOOKSTRUCT* pStruct = (MOUSEHOOKSTRUCT*) lParam;
			switch (wParam) {
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
				env->CallVoidMethod(javaHostObject, processMouseButtonMethod,
						(jint) wParam);
				break;
			case WM_MOUSEMOVE:
				if (pStruct->pt.x != mouseLocationX
						|| pStruct->pt.y != mouseLocationY) {
					env->CallVoidMethod(javaHostObject, processMouseMoveMethod,
							(jint) pStruct->pt.x, (jint) pStruct->pt.y);
					mouseLocationX = pStruct->pt.x;
					mouseLocationX = pStruct->pt.y;
				}
				break;
			default:
				break;
			}
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam) {
	JNIEnv* env;
	KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT *) lParam;
	jboolean transitionState = (jboolean) FALSE;
	if (jvm->AttachCurrentThread((void **) &env, NULL) >= 0) {
		switch (wParam) {
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			transitionState = (jboolean) TRUE;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			env->CallVoidMethod(javaHostObject, processKeyMethod,
					transitionState, p->vkCode);
			break;
		default:
			break;
		}
	}

	if (p->vkCode == 0xA3) // RCONTROL
	{
		controlDown = (transitionState == TRUE);
	}
	else if (p->vkCode == 0x47) // G
	{
		gDown = (transitionState == TRUE);
	}
	if (controlDown && gDown)
	{
		return 1; // consume these
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

JNIEXPORT void JNICALL Java_org_hawkinssoftware_azia_input_NativeInput_startNativeEventHook(JNIEnv * env, jobject thisObject)
{
	HHOOK mouseHandle = SetWindowsHookEx(WH_MOUSE_LL,MouseHook,hInst,0);
	HHOOK keyboardHandle = SetWindowsHookEx(WH_KEYBOARD_LL,KeyboardHook,hInst,0);

	javaHostObject = env->NewGlobalRef(thisObject);
	jclass cls = env->GetObjectClass(javaHostObject);
	processMouseButtonMethod = env->GetMethodID(cls,"mouseButtonEvent","(I)V");
	processMouseMoveMethod = env->GetMethodID(cls,"mouseMoveEvent","(II)V");
	processKeyMethod = env->GetMethodID(cls,"keyboardEvent","(ZI)V");

	env->GetJavaVM(&jvm);
	hookThreadId = GetCurrentThreadId();

	MSG message;
	while(GetMessage(&message,NULL,0,0)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	UnhookWindowsHookEx(mouseHandle);
	UnhookWindowsHookEx(keyboardHandle);
}

JNIEXPORT void JNICALL Java_org_hawkinssoftware_azia_input_NativeInput_stopNativeEventHook(JNIEnv * env, jobject thisObject)
{
	if(hookThreadId==0) return;
	PostThreadMessage(hookThreadId,WM_QUIT,0,0L);
}

