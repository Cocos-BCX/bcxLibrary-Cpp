# BCX CPP Library

## Introduction

This is a [BCX](https://www.cocosbcx.io/) SDK source library implemented in CPP. It can be used on iOS/Mac/Android/Win platforms.

## Directory Description

* __bcx__    BCX CPP SDK source code is located
* __test__   debug project, for debugging of each interface
* __tools__  python script for generating static libraries for each platform (based on python 2.7)


## Package

Run the genlibs.py command in the tools directory to generate a sdk library for the corresponding platform.

```bash
./genlibs.py -h
usage: genlibs.py [-h] [-p {i,m,a,w,b}] [-c {y,n}]

optional arguments:
  -h, --help            show this help message and exit
  -p {i,m,a,w,b}        Platfrom: ios(i)/mac(m)/android(a)/both(b)(default)
  -c {y,n}, --clean {y,n}
                        remove temp folder
```

For example, to generate a sdk library for the iOS platform, you can run the following command:

```bash
./genlibs.py -p i
```

The generated sdk library will be stored in the `bcx-sdk` folder in the root directory.

