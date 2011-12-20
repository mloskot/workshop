//
// Copyright (C) 2011 Mateusz Loskot <mateusz@loskot.net>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

// Simple example of using Python sub-interpreters to achieve (almost)
// totally separate environment for the execution of Python code.
// Constraints:
// * Serial use of sub-interpreters
// * Single process
// * No Python threads
// * No non-Python threads

#ifdef WITH_VISUALLEAKDETECTOR
#include <vld.h> // Py_InitializeEx gives hundreds of leaks
#endif
#include <cassert>
#include <iostream>
#include <Python.h>

int main()
{
    if (!Py_IsInitialized())
        Py_Initialize(); //Py_InitializeEx(0);

    // See http://docs.python.org/py3k/c-api/init.html#non-python-created-threads
    // Python supports the creation of additional interpreters (using Py_NewInterpreter()),
    // but mixing multiple interpreters and the PyGILState_*() API is unsupported.
    assert(0 == PyEval_ThreadsInitialized());

    PyThreadState* interp_main = PyThreadState_Get();
    PyThreadState* interp_sub1 = Py_NewInterpreter(); // this thread state is made in current (implicit swap)
    PyThreadState* interp_sub2 = Py_NewInterpreter(); // this thread state is made in current
    PyThreadState_Swap(nullptr); // make no interprter active, allow use of sub-interpreters only

    // Switch to sub1
    {
        PyThreadState_Swap(interp_sub1);
        PyRun_SimpleString("print(\'interp_sub1\')");
        PyRun_SimpleString("import sys");
        PyRun_SimpleString("print(dir())");
        PyRun_SimpleString("so = open(\'interp_sub1.log\', mode=\'w\')"); // log in getcwd()
        PyRun_SimpleString("sys.stdout = so");
        PyRun_SimpleString("print(sys.stdout)");
        PyRun_SimpleString("print(dir())");
        PyThreadState_Swap(nullptr);
        //PyRun_SimpleString("print(\'pif paf! dead!\')"); // uncomment to see yourself
    }

    // Switch to sub2
    {
        PyThreadState_Swap(interp_sub2);
        PyRun_SimpleString("print(\'interp_sub2\')");
        PyRun_SimpleString("import os");
        PyRun_SimpleString("print(dir())");
        PyRun_SimpleString("import sys");
        PyThreadState_Swap(nullptr);
    }

    // Switch to sub1 again
    {
        PyThreadState_Swap(interp_sub1);
        PyRun_SimpleString("print(\'interp_sub1 continued\')"); // still into log
        PyRun_SimpleString("so.close()");
        PyThreadState_Swap(nullptr);
    }

    // Clean sub1
    PyThreadState_Swap(interp_sub1);
    Py_EndInterpreter(interp_sub1);
    PyThreadState_Swap(nullptr);
    // Clean sub2
    PyThreadState_Swap(interp_sub2);
    Py_EndInterpreter(interp_sub2);
    PyThreadState_Swap(nullptr);
    // Clean switch to main before finalizing
    PyThreadState_Swap(interp_main);

    Py_Finalize();

    return 0;
}
