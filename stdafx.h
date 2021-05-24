#pragma once

#include <iostream>
#ifdef WindowsBuild
        #define EXPORT extern "C" __declspec(dllexport)
#elif LinuxBuild
        #define EXPORT extern "C"
#endif



//#define uint unsigned int//重复定义了
typedef unsigned char uchar;

#include <mutex>
#include <sstream>
#include <thread>

using namespace std;

void print_to_log(string prefix, const char *input, size_t len);
