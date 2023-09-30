# Multithreading Exercise

The objectives of this exercise are:
1. Learning the basics of multithreading,
2. Experience connecting third-party libraries

### Note
To build the program on your computer, you need to connect the [boost library](https://www.boost.org/) to the solution.
In order to test its operation, it is enough to have only MultithreadingExcercise.exe

Branches have the following purposes:
* **Work** - just make the program work using the standard library tools
* **Boost** - to make the program work using the Boost library
* **Dark Woods** - is a violation of the conditions of the task, but the optimal and logical operation of the program

## Original problem:

There is a folder with files on the computer. There are many files, their names are unknown in advance, each file contains an integer (additional check for correctness of the file contents is required). We need to write a win32 console application that takes the path to the folder as input and multithreads the files as follows:

For each file, you need to perform the following order of operations:
1. Read a number from the file
2. Immediately, after receiving the number, output it and the file name to `std::cout`, for example:
```
  hello.txt: 3
  test1: 1
  test2: 7
```
3. After that, the current thread should be put to sleep for 1 second.

After all files have been processed, the sum of all numbers should be reported to the user.

Additional conditions:
- The program should be written in Microsoft Visual Studio IDE
- The program must process files in parallel
