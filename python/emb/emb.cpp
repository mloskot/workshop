#include <functional>
#include <iostream>
#include <Python.h>

namespace emb
{

struct Stdout
{
    PyObject_HEAD
    std::function<void(std::string)> callback;
};

PyTypeObject StdoutType =
{
    PyVarObject_HEAD_INIT(0, 0)
    "emb.StdoutType",     /* tp_name */
    sizeof(Stdout),       /* tp_basicsize */
    0,                    /* tp_itemsize */
    0,                    /* tp_dealloc */
    0,                    /* tp_print */
    0,                    /* tp_getattr */
    0,                    /* tp_setattr */
    0,                    /* tp_reserved */
    0,                    /* tp_repr */
    0,                    /* tp_as_number */
    0,                    /* tp_as_sequence */
    0,                    /* tp_as_mapping */
    0,                    /* tp_hash  */
    0,                    /* tp_call */
    0,                    /* tp_str */
    0,                    /* tp_getattro */
    0,                    /* tp_setattro */
    0,                    /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,   /* tp_flags */
    "emb.Stdout objects", /* tp_doc */
};

PyModuleDef embmodule =
{
    PyModuleDef_HEAD_INIT,
    0, 0, 0, 0
};

PyMODINIT_FUNC PyInit_emb(void) 
{
    StdoutType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&StdoutType) < 0)
        return 0;

    PyObject* m = PyModule_Create(&embmodule);
    if (m)
    {
        Py_INCREF(&StdoutType);
        PyModule_AddObject(m, "Stdout", reinterpret_cast<PyObject*>(&StdoutType));
    }
    return m;
}

} // namespace emb

int main()
{
    Py_Initialize();

    PyRun_SimpleString("print(3.14)");

    Py_Finalize();
}