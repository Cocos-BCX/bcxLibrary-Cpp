package com.sdkbox.bcx;

public class API {

    public native String test();

    static {
        System.loadLibrary("bcx");
    }

}
