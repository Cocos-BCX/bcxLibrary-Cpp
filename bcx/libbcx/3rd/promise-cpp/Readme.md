# C++ promise/A+ library in Javascript style.

<!-- TOC -->

- [C++ promise/A+ library in Javascript style.](#c-promisea-library-in-javascript-style)
    - [What is promise-cpp ?](#what-is-promise-cpp-)
    - [Examples](#examples)
        - [Examples list](#examples-list)
        - [Compiler required](#compiler-required)
        - [Build tips](#build-tips)
        - [Sample code 1](#sample-code-1)
        - [Sample code 2](#sample-code-2)
    - [Global functions](#global-functions)
        - [Defer newPromise(FUNC func);](#defer-newpromisefunc-func)
        - [Defer resolve(const RET_ARG... &ret_arg);](#defer-resolveconst-ret_arg-ret_arg)
        - [Defer reject(const RET_ARG... &ret_arg);](#defer-rejectconst-ret_arg-ret_arg)
        - [Defer all(const PROMISE_LIST &promise_list);](#defer-allconst-promise_list-promise_list)
        - [Defer race(const PROMISE_LIST &promise_list);](#defer-raceconst-promise_list-promise_list)
        - [Defer doWhile(FUNC func);](#defer-dowhilefunc-func)
    - [Class Defer - type of promise object.](#class-defer---type-of-promise-object)
        - [Defer::resolve(const RET_ARG... &ret_arg);](#deferresolveconst-ret_arg-ret_arg)
        - [Defer::reject(const RET_ARG... &ret_arg);](#deferrejectconst-ret_arg-ret_arg)
        - [Defer::then(FUNC_ON_RESOLVED on_resolved, FUNC_ON_REJECTED on_rejected)](#deferthenfunc_on_resolved-on_resolved-func_on_rejected-on_rejected)
        - [Defer::then(FUNC_ON_RESOLVED on_resolved)](#deferthenfunc_on_resolved-on_resolved)
        - [Defer::fail(FUNC_ON_REJECTED on_rejected)](#deferfailfunc_on_rejected-on_rejected)
        - [Defer::finally(FUNC_ON_FINALLY on_finally)](#deferfinallyfunc_on_finally-on_finally)
        - [Defer::always(FUNC_ON_ALWAYS on_always)](#deferalwaysfunc_on_always-on_always)
    - [And more ...](#and-more-)
        - [about exceptions](#about-exceptions)
        - [about the chaining parameter](#about-the-chaining-parameter)
        - [Match rule for chaining parameters](#match-rule-for-chaining-parameters)
            - [Resolved parameters](#resolved-parameters)
            - [Rejected parameters](#rejected-parameters)
            - [Omit parameters](#omit-parameters)
        - [copy the promise object](#copy-the-promise-object)
        - [handle uncaught exceptional or rejected parameters](#handle-uncaught-exceptional-or-rejected-parameters)
        - [about multithread](#about-multithread)
        - [working in embedded chips ?](#working-in-embedded-chips-)

<!-- /TOC -->

## What is promise-cpp ?

Promise-cpp is **header only** library that implements promise/A+ standard.
  
Promise-cpp is easy to use, just #include "promise.hpp" is enough. With promise-cpp, you can resolve or reject any type of data without writing complex template code.

Promise-cpp is designed to built by c++11 compilers and almost no dependencies. Although some of the examples are linked against boost library, promise-cpp itself is absolutely **workable without boost library** and can be used together with other asynchronized libraries.

Promise-cpp can be the base component in event-looped asychronized programming, which is NOT std::promise.

## Examples

### Examples list 

* [test/test0.cpp](test/test0.cpp): a simple test code for promise resolve/reject operations. (no dependencies)

* [test/simple_timer.cpp](test/simple_timer.cpp): simple promisified timer. (no dependencies)

* [test/simple_benchmark_test.cpp](test/simple_benchmark_test.cpp): benchmark test for simple promisified asynchronized tasks. (no dependencies)

* [test/asio_timer.cpp](test/asio_timer.cpp): promisified timer based on asio callback timer. (boost::asio required)

* [test/asio_benchmark_test.cpp](test/asio_benchmark_test.cpp): benchmark test for promisified asynchronized tasks in asio. (boost::asio required)

* [test/asio_http_client.cpp](test/asio_http_client.cpp): promisified flow for asynchronized http client. (boost::asio, boost::beast required)

* [test/asio_http_server.cpp](test/asio_http_server.cpp): promisified flow for asynchronized http server. (boost::asio, boost::beast required)

Please check folder ["build"](build) to get the codelite/msvc projects for the test code above, or use cmake to build from [CMakeLists.txt](CMakeLists.txt).

### Compiler required

The library has passed test on these compilers --

* gcc 5

* Visual studio 2015 sp3

* clang 3.4.2

### Build tips

Some of the [examples](test) use boost::asio as io service, and use boost::beast as http service. 
You need to install [boost_1_66](https://www.boost.org/doc/libs/1_66_0/more/getting_started/index.html)
 or higher to build the examples.

For examples, on windows, you can build boost library in these steps --

```
> cd *boost_source_folder*
> bootstrap.bat
> b2.exe stage variant=release runtime-link=static threading=multi
> b2.exe stage variant=debug runtime-link=static threading=multi
```

After have boost installed, modify path of boost library in the example's project file according to the real path.

### Sample code 1

This sample code shows converting a timer callback to promise object.

```cpp
#include <stdio.h>
#include <boost/asio.hpp>
#include "asio/timer.hpp"

using namespace promise;
using namespace boost::asio;

/* Convert callback to a promise (Defer) */
Defer myDelay(boost::asio::io_service &io, uint64_t time_ms) {
    return newPromise([&io, time_ms](Defer &d) {
        setTimeout(io, [d](bool cancelled) {
            if (cancelled)
                d.reject();
            else
                d.resolve();
        }, time_ms);
    });
}


Defer testTimer(io_service &io) {

    return myDelay(io, 3000).then([&] {
        printf("timer after 3000 ms!\n");
        return myDelay(io, 1000);
    }).then([&] {
        printf("timer after 1000 ms!\n");
        return myDelay(io, 2000);
    }).then([] {
        printf("timer after 2000 ms!\n");
    }).fail([] {
        printf("timer cancelled!\n");
    });
}

int main() {
    io_service io;

    Defer timer = testTimer(io);

    delay(io, 4500).then([=] {
        printf("clearTimeout\n");
        clearTimeout(timer);
    });

    io.run();
    return 0;
}
```

### Sample code 2

This sample code shows promise resolve/reject flows.

```cpp
#include <stdio.h>
#include <string>
#include "promise.hpp"

using namespace promise;

#define output_func_name() do{ printf("in function %s, line %d\n", __func__, __LINE__); } while(0)

void test1() {
    output_func_name();
}

int test2() {
    output_func_name();
    return 5;
}

void test3(int n) {
    output_func_name();
    printf("n = %d\n", n);
}

Defer run(Defer &next){

    return newPromise([](Defer d){
        output_func_name();
        d.resolve(3, 5, 6);
    }).then([](const int &a, int b, int c) {
        printf("%d %d %d\n", a, b, c);
        output_func_name();
    }).then([](){
        output_func_name();
    }).then([&next](){
        output_func_name();
        next = newPromise([](Defer d) {
            output_func_name();
        });
        //Will call next.resole() or next.reject() later
        return next;
    }).then([](int n, char c) {
        output_func_name();
        printf("n = %d, c = %c\n", (int)n, c);
    }).fail([](char n){
        output_func_name();
        printf("n = %d\n", (int)n);
    }).fail([](short n) {
        output_func_name();
        printf("n = %d\n", (int)n);
    }).fail([](int &n) {
        output_func_name();
        printf("n = %d\n", (int)n);
    }).fail([](const std::string &str) {
        output_func_name();
        printf("str = %s\n", str.c_str());
    }).fail([](uint64_t n) {
        output_func_name();
        printf("n = %d\n", (int)n);
    }).then(test1)
    .then(test2)
    .then(test3)
    .always([]() {
        output_func_name();
    });
}

int main(int argc, char **argv) {
    Defer next;

    run(next);
    printf("======  after call run ======\n");

    next.resolve(123, 'a');
    //next.reject('c');
    //next.reject(std::string("xhchen"));
    //next.reject(45);

    return 0;
}
```

## Global functions

### Defer newPromise(FUNC func);
Creates a new Defer object with a user-defined function.
The user-defined functions, used as parameters by newPromise, must have a parameter Defer d. 
for example --

```cpp
return newPromise([](Defer d){
})
```

### Defer resolve(const RET_ARG... &ret_arg);
Returns a promise that is resolved with the given value.
for example --

```cpp
return resolve(3, '2');
```

### Defer reject(const RET_ARG... &ret_arg);
Returns a promise that is rejected with the given arguments.
for example --

```cpp
return reject("some_error");
```

### Defer all(const PROMISE_LIST &promise_list);
Wait until all promise objects in "promise_list" are resolved or one of which is rejected.
The "promise_list" can be any container that has Defer as element type.

> for (Defer &defer : promise_list) { ... }

for example --

```cpp
Defer d0 = newPromise([](Defer d){ /* ... */ });
Defer d1 = newPromise([](Defer d){ /* ... */ });
std::vector<Defer> promise_list = { d0, d1 };

all(promise_list).then([](){
    /* code here for all promise objects are resolved */
}).fail([](){
    /* code here for one of the promise objects is rejected */
});
```

### Defer race(const PROMISE_LIST &promise_list);
Rturns a promise that resolves or rejects as soon as one of
the promises in the iterable resolves or rejects, with the value
or reason from that promise.
The "promise_list" can be any container that has Defer as element type.

> for (Defer &defer : promise_list) { ... }

for example --

```cpp
Defer d0 = newPromise([](Defer d){ /* ... */ });
Defer d1 = newPromise([](Defer d){ /* ... */ });
std::vector<Defer> promise_list = { d0, d1 };

race(promise_list).then([](){
    /* code here for one of the promise objects is resolved */
}).fail([](){
    /* code here for one of the promise objects is rejected */
});
```

### Defer doWhile(FUNC func);
"While loop" for promisied task.
A promise(Defer) object will passed as parameter when call func, which can be resolved to continue with the "while loop", or be rejected to break from the "while loop". 

for example --

```cpp
doWhile([](Defer d){
    // Add code here for your task in "while loop"
    
    // Call "d.resolve();" to continue with the "while loop",
    
    // or call "d.reject();" to break from the "while loop", in this case,
    // the returned promise object will be in rejected status.
});

```

## Class Defer - type of promise object.

class Defer is the type of promise object.

### Defer::resolve(const RET_ARG... &ret_arg);
Resolve the promise object with arguments, where you can put any number of ret_arg with any type.
(Please be noted that it is a method of Defer object, which is different from the global resolve function.)
for example --

```cpp
return newPromise([](Defer d){
    //d.resolve();
    //d.resolve(3, '2', std::string("abcd"));
    d.resolve(9567);
})
```

### Defer::reject(const RET_ARG... &ret_arg);
Reject the promise object with arguments, where you can put any number of ret_arg with any type.
(Please be noted that it is a method of Defer object, which is different from the global reject function.)
for example --

```cpp
return newPromise([](Defer d){
    //d.reject();
    //d.reject(std::string("oh, no!"));
    d.reject(-1, std::string("oh, no!"))
})
```

### Defer::then(FUNC_ON_RESOLVED on_resolved, FUNC_ON_REJECTED on_rejected)
Return the chaining promise object, where on_resolved is the function to be called when 
previous promise object calls function resolve, on_rejected is the function to be called
when previous promise object calls function reject.
for example --

```cpp
return newPromise([](Defer d){
    d.resolve(9567, 'A');
}).then(

    /* function on_resolved */ [](int n, char ch){
        printf("%d %c\n", n, ch);   //will print 9567 here
    },

    /* function on_rejected */ [](){
        printf("promise rejected\n"); //will not run to here in this code 
    }
);
```

### Defer::then(FUNC_ON_RESOLVED on_resolved)
Return the chaining promise object, where on_resolved is the function to be called when 
previous promise object calls function resolve.
for example --

```cpp
return newPromise([](Defer d){
    d.resolve(9567);
}).then([](int n){
    printf("%d\n", n);  b //will print 9567 here
});
```

### Defer::fail(FUNC_ON_REJECTED on_rejected)
Return the chaining promise object, where on_rejected is the function to be called when
previous promise object calls function reject.

This function is usually named "catch" in most implements of Promise library. 
  https://www.promisejs.org/api/

In promise_cpp, function name "fail" is used instead of "catch", since "catch" is a keyword of c++.

for example --

```cpp
return newPromise([](Defer d){
    d.reject(-1, std::string("oh, no!"));
}).fail([](int err, string &str){
    printf("%d, %s\n", err, str.c_str());   //will print "-1, oh, no!" here
});
```

### Defer::finally(FUNC_ON_FINALLY on_finally)
Return the chaining promise object, where on_finally is the function to be called whenever
the previous promise object is be resolved or rejected.

The returned promise object will keeps the resolved/rejected state of current promise object.

for example --

```cpp
return newPromise([](Defer d){
    d.reject(std::string("oh, no!"));
}).finally([](){
    printf("in finally\n");   //will print "in finally" here
});
```

### Defer::always(FUNC_ON_ALWAYS on_always)
Return the chaining promise object, where on_always is the function to be called whenever
the previous promise object is be resolved or rejected.

The returned promise object will be in resolved state whenever current promise object is
resolved or rejected.

for example --

```cpp
return newPromise([](Defer d){
    d.reject(std::string("oh, no!"));
}).always([](){
    printf("in always\n");   //will print "in always" here
});
```

## And more ...

### about exceptions
To throw any object in the callback functions above, including on_resolved, on_rejected, on_always, 
will same as d.reject(the_throwed_object) and returns immediately.
for example --

```cpp
return newPromise([](Defer d){
    throw std::string("oh, no!");
}).fail([](string &str){
    printf("%s\n", str.c_str());   //will print "oh, no!" here
});
```
For the better performance, we suggest to use function reject instead of throw.

### about the chaining parameter
Any type of parameter can be used when call resolve, reject or throw, except that the plain string or array.
To use plain string or array as chaining parameters, we may wrap it into an object.

```cpp
newPromise([](Defer d){
    // d.resolve("ok"); may cause a compiling error, use the following code instead.
    d.resolve(std::string("ok"));
})
```

### Match rule for chaining parameters

"then" and "fail" function can accept multiple promise parameters and they follows the below rule --

#### Resolved parameters

Resolved parameters must match the next "then" function, otherwise it will throw an exception and can be caught by the following "fail" function.

#### Rejected parameters

First let's take a look at the rule of c++ try/catch, in which the thrown value will be caught in the block where value type is matched.
If type in the catch block can not be matched, it will run into the default block catch(...) { }.

```cpp
try{
    throw (short)1;
}catch(int a){
    // will not go to here
}catch(short b){
    // (short)1 will be caught here
}catch(...){
    // will not go to here
}
```

"Promise-cpp" implement "fail" chain as the match style of try/catch.

```cpp
newPromise([](Defer d){
    d.reject(3, 5, 6);
}).fail([](std::string str){
    // will not go to here since parameter types are not match
}).fail([](const int &a, int b, int c) {
    // d.reject(3, 5, 6) will be caught here
}).fail([](){
    // Will not go to here sinace previous rejected promise was caught.
});
```

#### Omit parameters

The number of parameters in "then" or "fail" chain can be lesser than that's in resolve function.
```cpp
newPromise([](Defer d){
    d.resolve(3, 5, 6);
}).then([](int a, int b) {
    // d.resolve(3, 5, 6) will be caught here since a, b matched with the resolved parameters and ignore the 3rd parameter.
});
```

A function in "then" chain without any parameters can be used as default promise caught function.
```cpp
newPromise([](Defer d){
    d.resolve(3, 5, 6);
}).then([]() {
    // Function without parameters will be matched with any resolved values,
    // so d.resolve(3, 5, 6) will be caught here.
});
```

The reject parameters follows the the same omit rule as resolved parameters.

### copy the promise object
To copy the promise object is allowed and effective, please do that when you need.

```cpp
Defer d = newPromise([](Defer d){});
Defer d1 = d;  //It's safe and effective
```

### handle uncaught exceptional or rejected parameters

The uncaught exceptional or rejected parameters are ignored by default. We can specify a handler function to do with these parameters --

```
handleUncaughtException([](Defer &d) {
    d.fail([](int n, int m) {
        //go here if the uncaught parameters match types "int n, int m".
    }).fail([](char c) {
        //go here if the uncaught parameters match type "char c".
    }).fail([]() {
        //go here for all other uncaught parameters.
    });
});
```

### about multithread

The "Defer" object is not thread safe by default for better performance.

To make it workable with multithread, define PM_MULTITHREAD before include "promise.hpp"

```
#define PM_MULTITHREAD
#include "promise.hpp"
```

### working in embedded chips ?

Yes, it works!
Please use [promise_embed](https://github.com/xhawk18/promise_embed), which is the special version optimized for embedded chips, such as Cortex M0/M3(STM32, etc...).

[Promise_embed](https://github.com/xhawk18/promise_embed) provides multitask function in single thread even through there's no operation system.
