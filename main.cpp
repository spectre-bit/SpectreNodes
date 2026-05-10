/*
A custom ComfyUI Node for generating seeds using Xoshiro256**.
Copyright (C) 2026  spectre-bit

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>
#include <string_view>
#include <charconv>

#include "Seed.hpp"

extern "C" {

    struct PySeedNode {
        PyObject_HEAD
        Seed *seed;
    };

    static PyObject *PySeedNode_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
        PySeedNode *self = reinterpret_cast<PySeedNode*>(type->tp_alloc(type, 0));
        if (self) {
            self->seed = new Seed{};
        }

        return reinterpret_cast<PyObject*>(self);
    }

    static void PySeedNode_dealloc(PySeedNode *self) {
        delete self->seed;
        Py_TYPE(self)->tp_free(reinterpret_cast<PyObject*>(self));
    }

    static int PySeedNode_init(PySeedNode *self, PyObject *args, PyObject *kwds) {
        return 0;
    }

static PyObject *PySeedNode_INPUT_TYPES(PyObject *cls, PyObject* Py_UNUSED(ignored)) {
    PyObject *dict            = nullptr;
    PyObject *required_value  = nullptr;
    PyObject *seed_value_dict = nullptr;
    PyObject *default_value   = nullptr;
    PyObject *STRING          = nullptr;
    PyObject *seed_value      = nullptr;
    PyObject *mode_list       = nullptr;
    PyObject *mode_value      = nullptr;
    PyObject *randomise       = nullptr;
    PyObject *increment       = nullptr;
    PyObject *decrement       = nullptr;
    PyObject *fixed           = nullptr;

    dict = PyDict_New();
    if (!dict) { goto error; }

    required_value = PyDict_New();
    if (!required_value) { goto error; }

    if (PyDict_SetItemString(dict, "required", required_value) < 0) { goto error; }
    Py_DECREF(required_value);
    required_value = nullptr;

    seed_value_dict = PyDict_New();
    if (!seed_value_dict) { goto error; }

    default_value = PyUnicode_FromString("0");
    if (!default_value) { goto error; }
    if (PyDict_SetItemString(seed_value_dict, "default", default_value) < 0) { goto error; }
    Py_DECREF(default_value);
    default_value = nullptr;

    STRING = PyUnicode_FromString("STRING");
    if (!STRING) { goto error; }

    seed_value = PyTuple_Pack(2, STRING, seed_value_dict);
    Py_DECREF(STRING);
    STRING = nullptr;
    Py_DECREF(seed_value_dict);
    seed_value_dict = nullptr;
    if (!seed_value) { goto error; }

    if (PyDict_SetItemString(PyDict_GetItemString(dict, "required"), "seed_value", seed_value) < 0) { goto error; }
    Py_DECREF(seed_value);
    seed_value = nullptr;

    randomise = PyUnicode_FromString("randomise");
    if (!randomise) { goto error; }

    increment = PyUnicode_FromString("increment");
    if (!increment) { goto error; }

    decrement = PyUnicode_FromString("decrement");
    if (!decrement) { goto error; }

    fixed = PyUnicode_FromString("fixed");
    if (!fixed) { goto error; }

    mode_list = PyList_New(4);
    if (!mode_list) { goto error; }

    PyList_SetItem(mode_list, 0, randomise);
    randomise = nullptr;
    PyList_SetItem(mode_list, 1, increment);
    increment = nullptr;
    PyList_SetItem(mode_list, 2, decrement);
    decrement = nullptr;
    PyList_SetItem(mode_list, 3, fixed);
    fixed = nullptr;

    mode_value = PyTuple_Pack(1, mode_list);
    Py_DECREF(mode_list);
    mode_list = nullptr;
    if (!mode_value) { goto error; }

    if (PyDict_SetItemString(PyDict_GetItemString(dict, "required"), "mode", mode_value) < 0) { goto error; }
    Py_DECREF(mode_value);
    mode_value = nullptr;

    return dict;

error:
    Py_XDECREF(dict);
    Py_XDECREF(required_value);
    Py_XDECREF(seed_value_dict);
    Py_XDECREF(default_value);
    Py_XDECREF(STRING);
    Py_XDECREF(seed_value);
    Py_XDECREF(mode_list);
    Py_XDECREF(mode_value);
    Py_XDECREF(randomise);
    Py_XDECREF(increment);
    Py_XDECREF(decrement);
    Py_XDECREF(fixed);
    return nullptr;
}

    static PyObject *PySeedNode_execute(PyObject *self, PyObject *args, PyObject *kwds) {
        uint64_t seed         = 0;
        PyObject *modeObj     = nullptr;
        PyObject *seedObj     = nullptr;
        PyObject *seed_result = nullptr;
        PyObject *uiValue     = nullptr;
        PyObject *list        = nullptr;
        PyObject *seedUI      = nullptr;
        PyObject *result      = nullptr;

        if (!kwds) {
            PyErr_SetString(PyExc_TypeError, "execute requires keyword arguments");
            return nullptr;
        }

        PyObject *seedArg = PyDict_GetItemString(kwds, "seed_value");
        if (!seedArg) {
            PyErr_SetString(PyExc_KeyError, "missing argument: seed_value");
            return nullptr;
        }
        if (!PyUnicode_Check(seedArg)) {
            PyErr_SetString(PyExc_TypeError, "seed_value must be a string");
            return nullptr;
        }

        const char *seed_str = PyUnicode_AsUTF8(seedArg);
        if (!seed_str) { return nullptr; }
        std::size_t len = strlen(seed_str);

        auto [ptr, ec] = std::from_chars(seed_str, seed_str + len, seed);
        if (ec == std::errc::invalid_argument) {
            PyErr_SetString(PyExc_ValueError, "seed_value must be a valid unsigned 64-bit integer");
            return nullptr;
        }
        if (ec == std::errc::result_out_of_range) {
            PyErr_SetString(PyExc_OverflowError, "seed_value is out of uint64_t range");
            return nullptr;
        }
        if (ptr != seed_str + len) {
            PyErr_SetString(PyExc_ValueError, "seed_value contains invalid characters");
            return nullptr;
        }

        modeObj = PyDict_GetItemString(kwds, "mode");
        if (!modeObj) {
            PyErr_SetString(PyExc_KeyError, "missing argument: mode");
            return nullptr;
        }
        if (!PyUnicode_Check(modeObj)) {
            PyErr_SetString(PyExc_TypeError, "mode must be a string");
            return nullptr;
        }

        PySeedNode *node = reinterpret_cast<PySeedNode *>(self);
        node->seed->setSeed(seed);

        const char *raw_mode = PyUnicode_AsUTF8(modeObj);
        if (!raw_mode) { return nullptr; }

        std::string_view mode{raw_mode};
        if      (mode == "randomise")  { node->seed->newSeed(); }
        else if (mode == "increment")  { node->seed->increment(); }
        else if (mode == "decrement")  { node->seed->decrement(); }

        seedObj = PyLong_FromUnsignedLongLong(node->seed->getSeed());
        if (!seedObj) { goto error; }

        seed_result = PyTuple_Pack(1, seedObj);
        Py_DECREF(seedObj);
        seedObj = nullptr;
        if (!seed_result) { goto error; }

        uiValue = PyDict_New();
        if (!uiValue) { goto error; }

        list = PyList_New(1);
        if (!list) { goto error; }

        seedUI = PyUnicode_FromFormat("%llu", static_cast<unsigned long long>(node->seed->getSeed()));
        if (!seedUI) { goto error; }

        PyList_SetItem(list, 0, seedUI);
        seedUI = nullptr;

        if (PyDict_SetItemString(uiValue, "seed_value", list) < 0) { goto error; }
        Py_DECREF(list);
        list = nullptr;

        result = PyDict_New();
        if (!result) { goto error; }

        if (PyDict_SetItemString(result, "ui", uiValue) < 0) { goto error; }
        Py_DECREF(uiValue);
        uiValue = nullptr;

        if (PyDict_SetItemString(result, "result", seed_result) < 0) { goto error; }
        Py_DECREF(seed_result);
        seed_result = nullptr;

        return result;

    error:
        Py_XDECREF(result);
        Py_XDECREF(uiValue);
        Py_XDECREF(list);
        Py_XDECREF(seedUI);
        Py_XDECREF(seed_result);
        return nullptr;
    }

    static PyObject *PySeedNode_IS_CHANGED(PyObject *cls, PyObject *args, PyObject *kwds) {
        if (!kwds) { Py_RETURN_NONE; }

        PyObject *modeObj = PyDict_GetItemString(kwds, "mode");
        if (!modeObj || !PyUnicode_Check(modeObj)) { Py_RETURN_NONE; }

        const char *raw_mode = PyUnicode_AsUTF8(modeObj);
        if (!raw_mode) { Py_RETURN_NONE; }

        std::string_view mode{raw_mode};
        if (mode == "randomise" || mode == "increment" || mode == "decrement") {
            return PyFloat_FromDouble(std::numeric_limits<double>::quiet_NaN());
        }

        Py_RETURN_NONE;
    }

    static PyMethodDef PySeedNode_methods[] = {
        {"INPUT_TYPES", reinterpret_cast<PyCFunction>(PySeedNode_INPUT_TYPES), METH_NOARGS | METH_CLASS, "returns the input types"},
        {"IS_CHANGED",  reinterpret_cast<PyCFunction>(PySeedNode_IS_CHANGED),  METH_VARARGS | METH_KEYWORDS | METH_CLASS, "cache invalidation"},
        {"execute",     reinterpret_cast<PyCFunction>(static_cast<PyCFunctionWithKeywords>(PySeedNode_execute)), METH_VARARGS | METH_KEYWORDS, "executes the node logic"},
        {nullptr}
    };
    static PyType_Slot PySeedNode_slots[] = {
        {Py_tp_new,     reinterpret_cast<void*>(PySeedNode_new)},
        {Py_tp_init,    reinterpret_cast<void*>(PySeedNode_init)},
        {Py_tp_dealloc, reinterpret_cast<void*>(PySeedNode_dealloc)},
        {Py_tp_methods, reinterpret_cast<void*>(PySeedNode_methods)},
        {Py_tp_doc,     reinterpret_cast<void*>(const_cast<char*>("A Seed Node using the Xoshiro256** algorithm"))},
        {0, nullptr}
    };

    static PyType_Spec PySeedNode_spec = {
        .name      = "SpectreNodes.SeedNode",
        .basicsize = sizeof(PySeedNode),
        .itemsize  = 0,
        .flags     = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        .slots     = PySeedNode_slots,
    };

    static PyModuleDef spectre_nodes_def = {PyModuleDef_HEAD_INIT, "SpectreNodes", nullptr, -1, nullptr};

    static bool PySeedNode_fill_dict(PyTypeObject *typeobj) {
        PyObject *return_type = PyUnicode_FromString("INT");
        if (!return_type) { return false; }
        PyObject *return_types_tuple = PyTuple_Pack(1, return_type);
        Py_DECREF(return_type);
        if (!return_types_tuple) { return false; }
        if (PyDict_SetItemString(typeobj->tp_dict, "RETURN_TYPES", return_types_tuple) < 0) {
            Py_DECREF(return_types_tuple); return false;
        }
        Py_DECREF(return_types_tuple);

        PyObject *return_name = PyUnicode_FromString("seed");
        if (!return_name) { return false; }
        PyObject *return_names_tuple = PyTuple_Pack(1, return_name);
        Py_DECREF(return_name);
        if (!return_names_tuple) { return false; }
        if (PyDict_SetItemString(typeobj->tp_dict, "RETURN_NAMES", return_names_tuple) < 0) {
            Py_DECREF(return_names_tuple); return false;
        }
        Py_DECREF(return_names_tuple);

        PyObject *function = PyUnicode_FromString("execute");
        if (!function) { return false; }
        if (PyDict_SetItemString(typeobj->tp_dict, "FUNCTION", function) < 0) {
            Py_DECREF(function); return false;
        }
        Py_DECREF(function);

        PyObject *category = PyUnicode_FromString("Spectre Nodes");
        if (!category) { return false; }
        if (PyDict_SetItemString(typeobj->tp_dict, "CATEGORY", category) < 0) {
            Py_DECREF(category); return false;
        }
        Py_DECREF(category);

        return true;
    }

    PyMODINIT_FUNC PyInit_SpectreNodes(void) {
        PyObject *m = PyModule_Create(&spectre_nodes_def);
        if (!m) { return nullptr; }

        PyObject *type = PyType_FromSpec(&PySeedNode_spec);
        if (!type) { Py_DECREF(m); return nullptr; }

        PyTypeObject *typeobj = reinterpret_cast<PyTypeObject*>(type);

        if (!PySeedNode_fill_dict(typeobj)) {
            Py_DECREF(type);
            Py_DECREF(m);
            return nullptr;
        }

        if (PyModule_AddObject(m, "SeedNode", type) < 0) {
            Py_DECREF(type);
            Py_DECREF(m);
            return nullptr;
        }

        return m;
    }
}