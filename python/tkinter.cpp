//
// Copyright (C) 2011 Mateusz Loskot <mateusz@loskot.net>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <string>
#include <Python.h>
#include <Windows.h>

#if PY_VERSION_HEX < 0x03030000
#define PyMem_RawFree free
wchar_t *Py_DecodeLocale(char *raw, size_t* s)
{
  int size_needed = MultiByteToWideChar(CP_UTF8, 0, raw, -1, NULL, 0);
  wchar_t *rtn = (wchar_t *) calloc(1, size_needed * sizeof(wchar_t));
  MultiByteToWideChar(CP_UTF8, 0, raw, -1, rtn, size_needed);
  return rtn;
}
#endif

int main(int argc, char** argv)
{
    try
    {
        wchar_t* arg0W = Py_DecodeLocale(argv[0], NULL);
        wchar_t** argvW = new wchar_t *[argc];
        for (int i = 0; i < argc; i++)
        {
            auto arg = Py_DecodeLocale(argv[i], NULL);
            if (!arg) throw std::runtime_error("Fatal error: cannot decode argv[0]\n");
            argvW[i] = arg;
        }
        Py_SetProgramName(argvW[0]);

        Py_Initialize();

        PySys_SetArgv(argc, argvW);

        PyRun_SimpleString("import tkinter\n");
        PyRun_SimpleString("print(tkinter)\n");
        PyRun_SimpleString("t=tkinter.Tk()\n");
        PyRun_SimpleString("t.mainloop()\n");

        Py_Finalize();
        PyMem_RawFree(arg0W);
        for (int i = 0; i < argc; i++)
            PyMem_RawFree(argvW[i]);
        delete [] argvW;

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