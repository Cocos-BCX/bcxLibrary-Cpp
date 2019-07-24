#include <jni.h>
#include <string>

 #include "bcxtest.hpp"

 static BCXTest test;

extern "C" JNIEXPORT jstring JNICALL
Java_com_sdkbox_bcx_API_test(
        JNIEnv *env,
        jobject /* this */) {

    test.showLog = [](const std::string& s) {
        bcx::log("%s", s.data());
    };
    test.init();
    test.onBtnTest();

    std::string hello = "Hello from BCX lib";

    return env->NewStringUTF(hello.c_str());
}

