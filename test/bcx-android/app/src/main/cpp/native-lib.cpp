#include <jni.h>
#include <string>
#include <android/log.h>
#include "../../../../../Classes/BCXTest.hpp"

#define  LOG_TAG    "BCXTest"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG,__VA_ARGS__)

static BCXTest BT;
JavaVM *g_VM;

void sendToJava(const std::string& s, jobject jo) {
    const char* s_char = s.c_str();
    LOGD("%s", s_char);

    if (nullptr == jo) {
        return;
    }
    JNIEnv *env;
    bool needDetach = JNI_FALSE;
    int getEnvStat = g_VM->GetEnv((void **)&env, JNI_VERSION_1_6);
    if (getEnvStat == JNI_EDETACHED) {
        if (g_VM->AttachCurrentThread(&env, nullptr) != 0) {
            return;
        }
        needDetach = JNI_TRUE;
    }

    jclass javaClass = env->GetObjectClass(jo);
    if (javaClass == 0) {
        if(needDetach) {
            g_VM->DetachCurrentThread();
        }
        return;
    }

    jmethodID mid = env->GetMethodID(javaClass, "nativeCallBack", "(Ljava/lang/String;)V");
    if (mid == nullptr) {
        return;
    }

    jstring jstr = env->NewStringUTF(s_char);
    env->CallVoidMethod(jo, mid, jstr);
    env->DeleteLocalRef(jstr);

    if(needDetach) {
        g_VM->DetachCurrentThread();
    }
    env = nullptr;
}


extern "C" JNIEXPORT void JNICALL
Java_com_sdkbox_test_bcx_MainActivity_nativeLoop(JNIEnv *env, jobject jo) {
    BT.loop();
}

extern "C" JNIEXPORT jint JNICALL
Java_com_sdkbox_test_bcx_MainActivity_nativeTestCasesCount(JNIEnv *env, jobject jo) {
    return BT.getTestCasesCount();
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_sdkbox_test_bcx_MainActivity_nativeTestCaseName(JNIEnv *env, jobject thiz, jint idx) {
    std::string s = BT.getTestCasesName(idx);
    return env->NewStringUTF(s.c_str());
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_sdkbox_test_bcx_MainActivity_sendToNative(JNIEnv *env, jobject jo, jstring s) {
    jboolean bCopy = false;
    const char *s_char = env->GetStringUTFChars(s, &bCopy);
    std::string evt = s_char;
    env->ReleaseStringUTFChars(s, s_char);
    s_char = nullptr;

    env->GetJavaVM(&g_VM);
    jobject jobj = env->NewGlobalRef(jo);

    if (0 == evt.compare("init")) {
        BT.setNativeLog([jobj](const std::string& s) {
            sendToJava(s, jobj);
        });
        BT.init();
    } else {
        BT.runTestCase(evt);
    }

    return env->NewStringUTF("");
}

