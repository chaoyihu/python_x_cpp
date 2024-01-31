#define PY_SSIZE_T_CLEAN

// Pulls in the python api. Must be included before 
// any standard headers, as it may have pre-processor 
// definitions affecting standard headers.
#include <Python.h>

/* A C function designed to be used as a Python C extension, 
 * providing a way to execute shell commands and retrieve 
 * their exit status in a Python script. The command to be 
 * executed is passed as a string argument to the function, 
 * and the exit status is returned as a Python integer. */
static PyObject *
mycli_system(PyObject *self, PyObject *args)
{
    const char *command;
    int sts;

    if (!PyArg_ParseTuple(args, "s", &command))
        return NULL;
    sts = system(command);
    return PyLong_FromLong(sts);
}

/* The Method Table*/
/* https://docs.python.org/3/c-api/structures.html#c.PyMethodDef */
static PyMethodDef SpamMethods[] = {
    // ...
    {
        "system",                   /* Name of method*/
        mycli_system,                /* Pointer to the C implementation*/
        METH_VARARGS,               /* Flags bits indicating how the call
                                       should be constructed*/
        "Execute a shell command."  /* Points to the contents of the docstring*/
    },
    // ...
    {NULL, NULL, 0, NULL}           /* sentinel */
};


/* The Module Definition*/
static struct PyModuleDef
myclimodule = {
    PyModuleDef_HEAD_INIT,
    "mycli",     /* Name of module */
    NULL,   /* module doc, may be null */
    -1,         /* Size of per-interpreter state of the module
                   or -1 if the module keeps state in global vars*/
    SpamMethods /* The method table must be referenced in 
                   the module definition*/
};


/* The Module's Initialization Function*/
/* Must be named PyInit_name */
PyMODINIT_FUNC
PyInit_mycli(void) {
    return PyModule_Create(&myclimodule);
}

/* Not directly relevant to Python C Extension. Only as entry point if you
   compile this C source as standalone executable. */
int
main(int argc, char *argv[])
{
    wchar_t *program = Py_DecodeLocale(argv[0], NULL);
    if (program == NULL) {
        fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
        exit(1);
    }

    /* Add a built-in module, before Py_Initialize */
    if (PyImport_AppendInittab("mycli", PyInit_mycli) == -1) {
        fprintf(stderr, "Error: could not extend in-built modules table\n");
        exit(1);
    }

    /* Pass argv[0] to the Python interpreter */
    Py_SetProgramName(program);

    /* Initialize the Python interpreter.  Required.
       If this step fails, it will be a fatal error. */
    Py_Initialize();

    /* Optionally import the module; alternatively,
       import can be deferred until the embedded script
       imports it. */
    PyObject *pmodule = PyImport_ImportModule("mycli");
    if (!pmodule) {
        PyErr_Print();
        fprintf(stderr, "Error: could not import module 'spam'\n");
    }

    // Get a reference to the "system" function from the module
    PyObject *pfunction = PyObject_GetAttrString(pmodule, "system");

    // Call the "system" function with the argument "free"
    PyObject *pargs = PyTuple_Pack(1, PyUnicode_DecodeFSDefault("free"));

    // Retrieve and display the results
    PyObject *presult = PyObject_CallObject(pfunction, pargs);
    if (!presult) {
        PyErr_Print();
        fprintf(stderr, "Error: calling 'system' function failed\n");
    } else {
        printf("Result: %ld\n", PyLong_AsLong(presult));
    }

    PyMem_RawFree(program);
    return 0;
}
