#include <Python.h>

#include "sensorsmodule.h"


/**
 * Duplicate the content of a Python string, like strdup(). NULL is
 * returned if a Python function raised an exception.
 */
char* pystrdup(PyObject *str)
{
#ifndef IS_PY3K
    return strdup(PyString_AsString(str));
#else
    PyObject *bytes = PyUnicode_AsUTF8String(str);

    if (bytes == NULL)
    {
        return NULL;
    }

    char *ret = PyBytes_AsString(bytes);
    Py_DECREF(bytes);

    if (ret == NULL)
    {
        return NULL;
    }

    return strdup(ret);
#endif
}
