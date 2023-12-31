
<div align=center>

### **NOTE:** This project is still under heavy development and is not ready for production use.

# LLVM IR


A small C library for generating LLVM IR.

![GitHub](https://img.shields.io/github/license/icxd/llvm-ir?style=for-the-badge)
![GitHub stars](https://img.shields.io/github/stars/icxd/llvm-ir?style=for-the-badge)
![GitHub issues](https://img.shields.io/github/issues/icxd/llvm-ir?style=for-the-badge)
![GitHub pull requests](https://img.shields.io/github/issues-pr/icxd/llvm-ir?style=for-the-badge)

</div>

## Table of Contents

- [Introduction](#introduction)
- [Installation](#installation)
- [Usage](#usage)
- [Testing](#testing)
- [License](#license)

## Introduction

LLVM IR is a small C library for generating LLVM Immediate Representation (IR) code. It is not dependent on LLVM itself, which I personally couldn't get to compile on Windows. It is also not dependent on any other libraries, except my own [base](https://github.com/icxd/llvm-ir/tree/master/lib) library, which is included in this repository.

## Installation

There is no installation process. Just copy the `llvm.h` header and `llvm.c` source files into your project.
Don't forget to compile the `llvm.c` source file with your project.

## Usage

The library is very simple to use. Just include the `llvm.h` header file in your source file and you're good to go!

```c
#include "llvm.h"
```

## Testing

To run the test program, run the following commands:

```console
$ make
$ ./test
```

*This test should support both Windows and Linux, but it hasn't been tested on Linux.*

## License

LLVM IR is licensed under the [GNU General Public License v3.0](LICENSE).

[//]: # ( vim: set tw=80: )