#include <jni.h>
#include <string>
#include <android/log.h>
#include "BCX.hpp"

#define  LOG_TAG    "BCXTest"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG,__VA_ARGS__)

void sendToJava(const std::string& s) {
    logI(s.c_str());

}


extern "C" JNIEXPORT jstring JNICALL
Java_com_sdkbox_test_bcx_MainActivity_sendToNative(JNIEnv *env, jobject jo, jstring s) {
    jboolean bCopy = false;
    const char *s_char = env->GetStringUTFChars(s, &bCopy);
    std::string evt = s_char;
    env->ReleaseStringUTFChars(s, s_char);
    s_char = nullptr;

    if (0 == evt.compare("init")) {
        bcx::Config cfg;
        cfg.autoConnect = true;
        cfg.wsNode = "ws://test.cocosbcx.net";
        cfg.faucetURL = "http://test-faucet.cocosbcx.net";
        bcx::BCX::init(cfg);
        bcx::BCX::connect([](const std::string& s) {
            sendToJava("Connect Status::" + s);
        });
    } else if (0 == evt.compare("login")) {
        bcx::BCX::login("hugo1111", "111111", [](const std::string& json) {
            sendToJava(json);
        });
    } else if (0 == evt.compare("transfer")) {
        bcx::BCX::transfer("huang", "COCOS", 33, "", [](const std::string& json) {
            sendToJava(json);
        });
    } else {
        LOGD("3 unknow event");
    }

    return env->NewStringUTF("");
}

