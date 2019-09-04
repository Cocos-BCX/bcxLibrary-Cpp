# BCX CPP Library

## 介绍

这个是一个用 CPP 实现的 [BCX](https://www.cocosbcx.io/) SDK 源码库. 可用于 iOS/Mac/Android/Win 平台.

## 目录说明

* __bcx__ BCX CPP 的源码所在
* __test__ 调试工程, 用于各个接口的调试
* __tools__ 生成各平台静态库的 python 脚本 (基于 python 2.7)


## 打包

在 tools 目录下 运行 genlibs.py 命令, 可生成对应平台的包

```bash
./genlibs.py -h
usage: genlibs.py [-h] [-p {i,m,a,w,b}] [-c {y,n}]

optional arguments:
  -h, --help            show this help message and exit
  -p {i,m,a,w,b}        Platfrom: ios(i)/mac(m)/android(a)/both(b)(default)
  -c {y,n}, --clean {y,n}
                        remove temp folder
```

例如, 想生成 iOS 平台的包, 可以运行如下命令:

```bash
./genlibs.py -p i
```

生成的包会在存放在根目录的 bcx-sdk 文件夹中

