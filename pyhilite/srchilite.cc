#include <srchilite/sourcehighlight.h>
#include <srchilite/langmap.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <Python.h>

typedef struct {
    PyObject_HEAD
    srchilite::SourceHighlight *sourceHighlight;
    std::string *datadir;
}SourceHighlightObject;

/* 初始化函数 */
static int SourceHighlight_init(SourceHighlightObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[]= {"outlang", "datadir", NULL};

    char *outlang=NULL, *datadir=NULL;
    if(!PyArg_ParseTupleAndKeywords(args,kwds, "|ss",kwlist, &outlang, &datadir)){
        return -1;
    }
    self->sourceHighlight=NULL;
    if(outlang==NULL){
        outlang = "html";
    }
    self->sourceHighlight=new srchilite::SourceHighlight(std::string(outlang)+".outlang");

    self->datadir = new std::string();
    if(datadir){
        self->sourceHighlight->setDataDir(datadir);
        self->datadir->replace(0,self->datadir->length(), datadir);
        try{
            self->sourceHighlight->checkOutLangDef(std::string(outlang)+".outlang");
        }catch(std::exception){
            PyErr_SetString(PyExc_ValueError, "invalid arguments");
            return -1;
        }
    }
    return 0;
}

static void SourceHighlight_dealloc(SourceHighlightObject *self)
{
    if(self->sourceHighlight){
        delete self->sourceHighlight;
    }
    if(self->datadir){
        delete self->datadir;
    }
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject *SourceHighlight_setDataDir(SourceHighlightObject *self, PyObject *args)
{
    char *datadir=NULL;
    if(!PyArg_ParseTuple(args, "s", &datadir)){
        return NULL;
    }
    self->sourceHighlight->setDataDir(datadir);
    self->datadir->replace(0,self->datadir->length(), datadir);
    return Py_BuildValue("");
}

static PyObject *SourceHighlight_highlight(SourceHighlightObject *self, PyObject *args, PyObject *kwargs)
{
    char *data=NULL, *input=NULL, *output=NULL, *lang=NULL;
    static char *kwlist1[] = {"data", "lang", "output", NULL};
    static char *kwlist2[] = {"input", "output", "lang", NULL};

    try{
        if(PyArg_ParseTupleAndKeywords(args,kwargs, "ss|s",kwlist1, &data, &lang, &output)){
            std::string langFile(lang);
            langFile+=".lang";
            std::istringstream in(data);
            if(output){
                std::ofstream out(output);
                self->sourceHighlight->highlight(in, out, langFile);
                return Py_BuildValue("");
            }else{
                std::ostringstream out("");
                self->sourceHighlight->highlight(in, out, langFile);
                return Py_BuildValue("s", out.str().c_str());
            }
        }
        PyErr_Clear();
        if(PyArg_ParseTupleAndKeywords(args, kwargs, "s|ss", kwlist2, &input, &output, &lang)){
            std::string langFile;
            if(lang){
                langFile=std::string(lang)+".lang";
            }else{
                srchilite::LangMap langMap(self->datadir->c_str(), "lang.map");
                langFile = langMap.getMappedFileNameFromFileName(input);
            }
            if(output){
                self->sourceHighlight->highlight(input, output, langFile);
                return Py_BuildValue("");
            }else{
                std::ifstream in(input);
                std::ostringstream out;
                self->sourceHighlight->highlight(in, out, langFile);
                return Py_BuildValue("s", out.str().c_str());
            }
        }
    }catch(std::exception){
        PyErr_SetString(PyExc_ValueError, "invalid arguments");
    }
    return NULL;
}


static PyMethodDef Sourcehighlight_methods[] = {
    {"setDataDir", (PyCFunction)SourceHighlight_setDataDir, METH_VARARGS,
        "设置当前的资源目录"},
    {"highlight", (PyCFunction)SourceHighlight_highlight, METH_VARARGS | METH_KEYWORDS,
        "高亮代码"},
    {NULL}
};

static PyTypeObject SourceHighlightType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "srchilite.SourceHighlight",
    sizeof(SourceHighlightObject),
    0,                         /* tp_itemsize */
    (void (*)(PyObject *))SourceHighlight_dealloc,      /* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,        /* tp_flags */
    "代码高亮类",           /* tp_doc */
    0,                         /* tp_traverse */
    0,                         /* tp_clear */
    0,                         /* tp_richcompare */
    0,                         /* tp_weaklistoffset */
    0,                         /* tp_iter */
    0,                         /* tp_iternext */
    Sourcehighlight_methods,             /* tp_methods */
    0,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)SourceHighlight_init,      /* tp_init */
    0,                         /* tp_alloc */
    PyType_GenericNew,                 /* tp_new */
};

static PyMethodDef hiliteMethods[] = {
    // {"SourceHighlight",  SourceHighlight, METH_VARARGS,
    //  "Execute a shell command."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef hiliteModule = {
   PyModuleDef_HEAD_INIT,
   "SrouceHighlite",   /* name of module */
   NULL, /* module documentation, may be NULL */
   -1,       /* size of per-interpreter state of the module,
                or -1 if the module keeps state in global variables. */
   hiliteMethods,
};


PyMODINIT_FUNC
PyInit_srchilite(void)
{
    PyObject *mod = PyModule_Create(&hiliteModule);
    if(PyType_Ready(&SourceHighlightType) < 0 || mod==NULL){
        return NULL;
    }
    Py_INCREF(&SourceHighlightType);
    PyModule_AddObject(mod, "SourceHighlight", (PyObject*)&SourceHighlightType);
    return mod;
}
