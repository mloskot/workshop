//
// Copyright (C) 2011 Mateusz Loskot <mateusz@loskot.net>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <string>
#include <Python.h>

int main()
{
    try
    {
        Py_Initialize();
        PyRun_SimpleString("print(\'import tkinter\')");
        PyRun_SimpleString("import sys, tkinter");
        PyRun_SimpleString("tkinter.Tk()");
        return EXIT_SUCCESS;
    }
    catch (std::exception const& e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "unknown error\n";
    }
    return EXIT_FAILURE;
}