package com.sdkbox.test.bcx

import android.content.Context
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
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
            return _mainActivity._testcases.size
        }

        override fun getView(position: Int, convertView: View?, parent: ViewGroup?): View {
            val view = View.inflate(_context, android.R.layout.simple_list_item_1,null)
            val textView: TextView =view.findViewById(android.R.id.text1)
            textView.text= _mainActivity._testcases.values.elementAt(position)

            return  view
        }

    }

    private val _tag = "BCX"
    private var _testcases: Map<String, String> = emptyMap()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        initTestCases()


        testList.adapter = TestCaseAdapter(this, this)

        testList.setOnItemClickListener { parent, view, position, id ->
            val element = _testcases.entries.elementAt(position)
            Log.d(_tag, element.toString())
            sendToNative(element.key)
        }
    }

    fun initTestCases() {
        _testcases = mapOf(
            "login" to "Login",
            "transfer" to "Transfer"
        )
    }

    fun nativeCallBack(s:String) {
        runOnUiThread {
            Log.d(_tag, s);
        }
    }

    external fun nativeInit()
    external fun nativeLoop()
    external fun sendToNative(s:String): String

    companion object {

        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("native-lib")
        }
    }
}
