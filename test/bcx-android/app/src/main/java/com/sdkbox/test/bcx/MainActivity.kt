package com.sdkbox.test.bcx

import android.content.Context
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.os.Message
import android.util.Log
import android.view.View
import android.view.ViewGroup
import android.widget.BaseAdapter
import android.widget.TextView
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {

    class TestCaseAdapter : BaseAdapter {

        private var _context: Context
        private var _mainActivity:MainActivity

        constructor(context:Context, main:MainActivity) {
            _context = context
            _mainActivity = main
        }

        override fun getItem(position: Int): Any {
            return position
        }

        override fun getItemId(position: Int): Long {
            return position.toLong()
        }

        override fun getCount(): Int {
            return _mainActivity.nativeTestCasesCount()
        }

        override fun getView(position: Int, convertView: View?, parent: ViewGroup?): View {
            val view = View.inflate(_context, android.R.layout.simple_list_item_1,null)
            val textView: TextView =view.findViewById(android.R.id.text1)
            textView.text = _mainActivity.nativeTestCaseName(position)

            return  view
        }

    }

    private var mHandler: Handler? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        mHandler = object : Handler(Looper.getMainLooper()) {
            override fun handleMessage(msg: Message?) {
                super.handleMessage(msg)
                when (msg?.what) {
                    MSG_LOOP -> {
                        nativeLoop()
                        mHandler?.sendEmptyMessageDelayed(MSG_LOOP, 10)
                    }
                    else -> {
                    }
                }
            }
        }

        sendToNative("init")

        testList.adapter = TestCaseAdapter(this, this)

        testList.setOnItemClickListener { parent, view, position, id ->
            val element = nativeTestCaseName(position)
            Log.d(TAG, "to test: $element")
            sendToNative(element)
        }

        mHandler?.sendEmptyMessageDelayed(MSG_LOOP, 100)
    }

    private fun showLog(s:String) {
        var txts = logView.text.split("\n").toMutableList()
        while (txts.size > 3) {
            txts.removeAt(0)
        }
        txts.add(s)
        logView.text = txts.joinToString("\n")
    }

    fun nativeCallBack(s:String) {
        runOnUiThread {
            showLog(s)
        }
    }

    external fun nativeLoop()
    external fun nativeTestCasesCount(): Int
    external fun nativeTestCaseName(idx:Int): String
    external fun sendToNative(s:String): String

    companion object {

        const val TAG = "BCX"
        const val MSG_LOOP = 1001

        init {
            System.loadLibrary("native-lib")
        }
    }
}
