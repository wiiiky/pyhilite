#include <boost/algorithm/string/predicate.hpp>
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
} SourceHighlightObject;


static std::string getFileDef(const char *file, const char *suffix)
{
    std::string s(file);
    if(!boost::algorithm::ends_with(file, suffix)){
        s += suffix;
    }
    return s;
}

/* 初始化函数 */
static int SourceHighlight_init(SourceHighlightObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[]= {"outlang", "datadir", NULL};

    char *outlang=NULL, *datadir=NULL;
    if(!PyArg_ParseTupleAndKeywords(args,kwds, "|ss",kwlist, &outlang, &datadir)) {
        return -1;
    }
    self->sourceHighlight=NULL;
    if(outlang==NULL) {
        outlang = "html";
    }
    self->sourceHighlight=new srchilite::SourceHighlight(getFileDef(outlang, ".outlang"));

    self->datadir = new std::string();
    if(datadir) {
        self->sourceHighlight->setDataDir(datadir);
        self->datadir->replace(0,self->datadir->length(), datadir);
        try {
            self->sourceHighlight->checkOutLangDef(getFileDef(outlang, ".outlang"));
        } catch(std::exception) {
            PyErr_SetString(PyExc_ValueError, "invalid arguments");
            return -1;
        }
    }
    return 0;
}

static void SourceHighlight_dealloc(SourceHighlightObject *self)
{
    if(self->sourceHighlight) {
        delete self->sourceHighlight;
    }
    if(self->datadir) {
        delete self->datadir;
    }
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject *SourceHighlight_setDataDir(SourceHighlightObject *self, PyObject *args)
{
    char *datadir=NULL;
    if(!PyArg_ParseTuple(args, "s", &datadir)) {
        return NULL;
    }
    self->sourceHighlight->setDataDir(datadir);
    self->datadir->replace(0,self->datadir->length(), datadir);
    return Py_BuildValue("");
}

static PyObject *Sourcehighlight_setTitle(SourceHighlightObject *self, PyObject *args)
{
    char *title = NULL;
    if(!PyArg_ParseTuple(args, "s", &title)){
        return NULL;
    }
    self->sourceHighlight->setTitle(title);
    return Py_BuildValue("");
}

static PyObject *SourceHighlight_setStyleCssFile(SourceHighlightObject *self, PyObject *args)
{
    char *cssfile = NULL;
    if(!PyArg_ParseTuple(args, "s", &cssfile)) {
        return NULL;
    }
    self->sourceHighlight->setStyleCssFile(cssfile);
    return Py_BuildValue("");
}

static PyObject *SourceHighlight_setStyleFile(SourceHighlightObject *self, PyObject *args)
{
    char *stylefile = NULL;
    if(!PyArg_ParseTuple(args, "s", &stylefile)) {
        return NULL;
    }
    self->sourceHighlight->setStyleFile(stylefile);
    return Py_BuildValue("");
}

static PyObject *SourceHighlight_setGenerateLineNumbers(SourceHighlightObject *self, PyObject *args)
{
    int b;
    if(!PyArg_ParseTuple(args, "p", &b)) {
        return NULL;
    }
    self->sourceHighlight->setGenerateLineNumbers(b);
    return Py_BuildValue("");
}

static PyObject *SourceHighlight_setGenerateLineNumberRefs(SourceHighlightObject *self, PyObject *args)
{
    int b;
    if(!PyArg_ParseTuple(args, "p", &b)) {
        return NULL;
    }
    self->sourceHighlight->setGenerateLineNumberRefs(b);
    return Py_BuildValue("");
}

static PyObject *SourceHighlight_setOptimize(SourceHighlightObject *self, PyObject *args)
{
    int b;
    if(!PyArg_ParseTuple(args, "p", &b)) {
        return NULL;
    }
    self->sourceHighlight->setOptimize(b);
    return Py_BuildValue("");
}

static PyObject *SourceHighlight_checkLangDef(SourceHighlightObject *self, PyObject *args)
{
    char *lang = NULL;
    if(!PyArg_ParseTuple(args, "s", &lang)) {
        return NULL;
    }
    try {
        self->sourceHighlight->checkLangDef(getFileDef(lang, ".lang"));
    } catch(std::exception) {
        Py_RETURN_FALSE;
    }
    Py_RETURN_TRUE;
}

static PyObject *SourceHighlight_checkOutLangDef(SourceHighlightObject *self, PyObject *args)
{
    char *lang = NULL;
    if(!PyArg_ParseTuple(args, "s", &lang)) {
        return NULL;
    }
    try {
        self->sourceHighlight->checkOutLangDef(getFileDef(lang, ".outlang"));
    } catch(std::exception) {
        Py_RETURN_FALSE;
    }
    Py_RETURN_TRUE;
}

static PyObject *SourceHighlight_highlight(SourceHighlightObject *self, PyObject *args, PyObject *kwargs)
{
    char *data=NULL, *input=NULL, *output=NULL, *lang=NULL;
    static char *kwlist1[] = {"data", "lang", "output", NULL};
    static char *kwlist2[] = {"input", "output", "lang", NULL};

    try {
        if(PyArg_ParseTupleAndKeywords(args,kwargs, "ss|s",kwlist1, &data, &lang, &output)) {
            std::string langFile =getFileDef(lang, ".lang");
            std::istringstream in(data);
            if(output) {
                std::ofstream out(output);
                self->sourceHighlight->highlight(in, out, langFile);
                return Py_BuildValue("");
            }
            std::ostringstream out("");
            self->sourceHighlight->highlight(in, out, langFile);
            return Py_BuildValue("s", out.str().c_str());
        }
        PyErr_Clear();
        if(PyArg_ParseTupleAndKeywords(args, kwargs, "s|ss", kwlist2, &input, &output, &lang)) {
            std::string langFile;
            if(lang) {
                langFile=getFileDef(lang, ".lang");
            } else {
                srchilite::LangMap langMap(self->datadir->c_str(), "lang.map");
                langFile = langMap.getMappedFileNameFromFileName(input);
            }
            if(output) {
                self->sourceHighlight->highlight(input, output, langFile);
                return Py_BuildValue("");
            }
            std::ifstream in(input);
            if(!in){
                throw 1;
            }
            std::ostringstream out;
            self->sourceHighlight->highlight(in, out, langFile);
            in.close();
            return Py_BuildValue("s", out.str().c_str());
        }
    } catch (int){
        PyErr_SetString(PyExc_FileNotFoundError, "file not found");
    } catch(std::exception) {
        PyErr_SetString(PyExc_ValueError, "invalid arguments");
    }
    return NULL;
}


static PyMethodDef Sourcehighlight_methods[] = {
    {
        "setDataDir", (PyCFunction)SourceHighlight_setDataDir, METH_VARARGS,
        "设置当前的资源目录，资源目录下存放.lang、.outlang和style文件；一般情况下使用默认的资源目录。"
    },
    {
        "setTitle", (PyCFunction)Sourcehighlight_setTitle, METH_VARARGS,
        "设置标题，好像没有什么用"
    },
    {
        "setStyleFile", (PyCFunction)SourceHighlight_setStyleFile, METH_VARARGS,
        "设置Style文件"
    },
    {
        "setStyleCssFile", (PyCFunction)SourceHighlight_setStyleCssFile, METH_VARARGS,
        "设置CSS文件，可以是文件绝对或者相对路径，或者在资源目录下。可以参考默认的default.css。注意，该函数必须在第一次调用highlight()之前调用，否则无效。"
    },
    {
        "setGenerateLineNumbers", (PyCFunction)SourceHighlight_setGenerateLineNumbers, METH_VARARGS,
        "设置是否显示行号"
    },
    {
        "setGenerateLineNumberRefs", (PyCFunction)SourceHighlight_setGenerateLineNumberRefs, METH_VARARGS,
        ""
    },
    {"setOptimize", (PyCFunction)SourceHighlight_setOptimize, METH_VARARGS, "设置是否开启优化"},
    {
        "highlight", (PyCFunction)SourceHighlight_highlight, METH_VARARGS | METH_KEYWORDS,
        "生成高亮代码。"
    },
    {
        "checkLangDef", (PyCFunction)SourceHighlight_checkLangDef, METH_VARARGS,
        "检查指定的语言是否被支持"
    },
    {
        "checkOutLangDef", (PyCFunction)SourceHighlight_checkOutLangDef, METH_VARARGS,
        "检查指定的输出格式是否被支持"
    },
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
    if(PyType_Ready(&SourceHighlightType) < 0 || mod==NULL) {
        return NULL;
    }
    Py_INCREF(&SourceHighlightType);
    PyModule_AddObject(mod, "SourceHighlight", (PyObject*)&SourceHighlightType);
    return mod;
}
