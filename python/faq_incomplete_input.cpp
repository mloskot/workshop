//
// A quick and dirty C++ version of the C program presented in Python FAQ:
// http://docs.python.org/py3k/faq/extending.html#how-do-i-tell-incomplete-input-from-invalid-input
// Modifications:
// - do not use readline library, but <iostream>
//
// Tested using Visual C++ 2010 (10.0) and Python 3.2 (custom Debug build)
//
// The "incomplete input" solution presented in the FAQ is incomplete and it does
// not allow multi-line scripts with more than 2 lines of flow control statements:
//
//for n in range(0, 5):
//        if n > 0:
//  File "<stdin>", line 2
//    if n > 0:
//            ^
//IndentationError: expected an indented block
//
#include <cstdio>
#include <iostream>
#include <string>

#include <Python.h>
#include <object.h>
#include <compile.h>
#include <eval.h>

int main (int argc, char* argv[])
{
    int i, j, done = 0;                          /* lengths of line, code */
    char ps1[] = ">>> ";
    char ps2[] = "... ";
    char *prompt = ps1;
    char *msg, *code = NULL;
    char const* line(NULL);
    PyObject *src, *glb, *loc;
    PyObject *exc, *val, *trb, *obj, *dum;
    std::string input_line;

    Py_Initialize ();
    loc = PyDict_New ();
    glb = PyDict_New ();
    PyDict_SetItemString (glb, "__builtins__", PyEval_GetBuiltins ());

    while (!done)
    {
        std::cout << prompt;
        std::getline(std::cin, input_line);

        if (input_line.empty() || input_line[0] == 4 /*EOT*/) /* CTRL-Z or CTRL-D pressed */
        {
            done = 1;
        }
        else
        {
            line = input_line.c_str();
            i = strlen (line);
            if (NULL == code)                        /* nothing in code yet */
                j = 0;
            else
                j = strlen (code);

            code = (char*)realloc (code, i + j + 2);
            if (NULL == code)                        /* out of memory */
                exit (1);

            if (0 == j)                              /* code was empty, so */
                code[0] = '\0';                        /* keep strncat happy */

            strncat (code, line, i);                 /* append line to code */
            code[i + j] = '\n';                      /* append '\n' to code */
            code[i + j + 1] = '\0';

            src = Py_CompileString (code, "<stdin>", Py_single_input);

            if (NULL != src)                         /* compiled just fine - */
            {
                if (ps1  == prompt ||                /* ">>> " or */ 
                                                     /* "... " and double '\n' */
                    ('\r' == code[i + j - 1] &&      /* Windows-specific fix */    
                     '\n' == code[i + j - 2]))           
                {                                               /* so execute it */
                    dum = PyEval_EvalCode (src, glb, loc);
                    Py_XDECREF (dum);
                    Py_XDECREF (src);
                    free (code);
                    code = NULL;
                    if (PyErr_Occurred ())
                        PyErr_Print ();
                    prompt = ps1;
                }
            }                                        /* syntax error or E_EOF? */
            else if (PyErr_ExceptionMatches (PyExc_SyntaxError))
            {
                PyErr_Fetch (&exc, &val, &trb);        /* clears exception! */

                if (PyArg_ParseTuple (val, "sO", &msg, &obj) &&
                    !strcmp (msg, "unexpected EOF while parsing")) /* E_EOF */
                {
                    Py_XDECREF (exc);
                    Py_XDECREF (val);
                    Py_XDECREF (trb);
                    prompt = ps2;
                }
                else                                   /* some other syntax error */
                {
                    PyErr_Restore (exc, val, trb);
                    PyErr_Print ();
                    free (code);
                    code = NULL;
                    prompt = ps1;
                }
            }
            else                                     /* some non-syntax error */
            {
                PyErr_Print ();
                free (code);
                code = NULL;
                prompt = ps1;
            }
        }
    }

    Py_XDECREF(glb);
    Py_XDECREF(loc);
    Py_Finalize();
    exit(0);
}