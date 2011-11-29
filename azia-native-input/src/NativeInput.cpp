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

const int MAX_META_KEYS = 3;
const int MAX_KEY_SYNONYMS = 3;
int metaKeys[MAX_META_KEYS][MAX_KEY_SYNONYMS];
bool metaKeyDefined[MAX_META_KEYS];
bool metaKeyPressed[MAX_META_KEYS];
bool hasMetaKeys = false;

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
	jboolean isKeyPress = (jboolean) FALSE;
	if (jvm->AttachCurrentThread((void **) &env, NULL) >= 0) {
		switch (wParam) {
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			isKeyPress = (jboolean) TRUE;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			env->CallVoidMethod(javaHostObject, processKeyMethod, isKeyPress,
					p->vkCode);
			break;
		default:
			break;
		}
	}

	if (hasMetaKeys) {
		bool isMetaKeyUp = false;
		for (int i = 0; i < MAX_META_KEYS; i++) {
			if (!metaKeyDefined[i]) {
				continue;
			}

			for (int j = 0; j < MAX_KEY_SYNONYMS; j++) {
				if ((metaKeys[i][j] > 0) && (p->vkCode == metaKeys[i][j])) {
					metaKeyPressed[i] = isKeyPress;
					break;
				}
			}

			if (!metaKeyPressed[i]) {
				isMetaKeyUp = true;
			}
		}

		if (!isMetaKeyUp) {
			return 1; // consume any keystroke for which all meta keys are down
		}
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

JNIEXPORT void JNICALL Java_org_hawkinssoftware_azia_input_NativeInput_defineMetaKey(JNIEnv * env, jobject thisObject, jint index, jint keyCode1, jint keyCode2, jint keyCode3)
{
	metaKeys[index][0] = keyCode1;
	metaKeys[index][1] = keyCode2;
	metaKeys[index][2] = keyCode3;
	metaKeyDefined[index] = true;
	metaKeyPressed[index] = false;
	hasMetaKeys = true;
}

JNIEXPORT void JNICALL Java_org_hawkinssoftware_azia_input_NativeInput_clearMetaKeys(JNIEnv * env, jobject thisObject)
{
	for (int i = 0; i < MAX_META_KEYS; i++)
	{
		for (int j = 0; j < MAX_KEY_SYNONYMS; j++)
		{
			metaKeys[i][j] = 0x0;
		}
		metaKeyDefined[i] = false;
		metaKeyPressed[i] = false;
	}
	hasMetaKeys = false;
}
