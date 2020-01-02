// Copyright (C) 2006-2012 Rosen Diankov <rosen.diankov@gmail.com>
//
// This file is part of OpenRAVE.
// OpenRAVE is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// shouldn't include openrave.h!
#ifndef OPENRAVE_BOOST_PYTHON_BINDINGS
#define OPENRAVE_BOOST_PYTHON_BINDINGS

#include <Python.h>
#include <stdint.h>
// numpy
#include <numpy/arrayobject.h>
#include <numpy/arrayscalars.h>

#ifndef USE_PYBIND11_PYTHON_BINDINGS
// boost
#include <boost/multi_array.hpp>
#endif

#ifdef _MSC_VER
#include <boost/typeof/std/string.hpp>
#include <boost/typeof/std/vector.hpp>
#include <boost/typeof/std/list.hpp>
#include <boost/typeof/std/map.hpp>
#include <boost/typeof/std/set.hpp>
#include <boost/typeof/std/string.hpp>

#define FOREACH(it, v) for(BOOST_TYPEOF(v) ::iterator it = (v).begin(); it != (v).end(); (it)++)
#define FOREACH_NOINC(it, v) for(BOOST_TYPEOF(v) ::iterator it = (v).begin(); it != (v).end(); )

#define FOREACHC(it, v) for(BOOST_TYPEOF(v) ::const_iterator it = (v).begin(); it != (v).end(); (it)++)
#define FOREACHC_NOINC(it, v) for(BOOST_TYPEOF(v) ::const_iterator it = (v).begin(); it != (v).end(); )
#define RAVE_REGISTER_BOOST

#else // _MSC_VER
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <string>
#include <stdexcept>

// apparently there's a problem with higher versions of C++
#if __cplusplus > 199711L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <typeinfo>
#define FOREACH(it, v) for(decltype((v).begin()) it = (v).begin(); it != (v).end(); (it)++)
#define FOREACH_NOINC(it, v) for(decltype((v).begin()) it = (v).begin(); it != (v).end(); )
#else
#define FOREACH(it, v) for(typeof((v).begin())it = (v).begin(); it != (v).end(); (it)++)
#define FOREACH_NOINC(it, v) for(typeof((v).begin())it = (v).begin(); it != (v).end(); )
#endif // __cplusplus > 199711L || defined(__GXX_EXPERIMENTAL_CXX0X__)

#define FOREACHC FOREACH
#define FOREACHC_NOINC FOREACH_NOINC

#endif // _MSC_VER

#include <complex>
#include <algorithm>
// openrave
#include <openrave/config.h>
#include <openrave/logging.h>
#include <openrave/smart_ptr.h>

#ifndef _RAVE_DISPLAY
#define _RAVE_DISPLAY(RUNCODE)                                               \
{                                                                              \
    printf(                                                                    \
        "\n%s:%d, [ %s "                                                       \
        "]\n-----------------------------------------------------------------" \
        "--------------\n",                                                    \
        __FILE__, __LINE__, __func__ /*__PRETTY_FUNCTION__*/);                 \
    RUNCODE;                                                                   \
    printf("\n");                                                              \
}
#endif // _RAVE_DISPLAY

namespace openravepy {

// https://stackoverflow.com/questions/35041268/how-to-convert-a-vector-to-numpy-array-with-templates-and-boost
template <typename T>
struct select_dtype
{};

template <>
struct select_dtype<double>
{
    static constexpr char type[] = "f8";
};

template <>
struct select_dtype<float>
{
    static constexpr char type[] = "f4";
};

template <>
struct select_dtype<int>
{
    static constexpr char type[] = "i4";
};

template <>
struct select_dtype<uint8_t>
{
    static constexpr char type[] = "u1";
};

template <>
struct select_dtype<uint16_t>
{
    static constexpr char type[] = "u2";
};

template <>
struct select_dtype<uint32_t>
{
    static constexpr char type[] = "u4";
};

template <>
struct select_dtype<uint64_t>
{
    static constexpr char type[] = "u8";
};

template <>
struct select_dtype<bool>
{
    static constexpr char type[] = "?";
};

template <typename T>
struct select_npy_type
{};

template <>
struct select_npy_type<double>
{
    static constexpr NPY_TYPES type = NPY_DOUBLE;
};

template <>
struct select_npy_type<float>
{
    static constexpr NPY_TYPES type = NPY_FLOAT;
};

template <>
struct select_npy_type<int>
{
    static constexpr NPY_TYPES type = NPY_INT;
};

template <>
struct select_npy_type<uint8_t>
{
    static constexpr NPY_TYPES type = NPY_UINT8;
};

template <>
struct select_npy_type<uint16_t>
{
    static constexpr NPY_TYPES type = NPY_UINT16;
};

template <>
struct select_npy_type<uint32_t>
{
    static constexpr NPY_TYPES type = NPY_UINT32;
};

template <>
struct select_npy_type<uint64_t>
{
    static constexpr NPY_TYPES type = NPY_UINT64;
};

template <>
struct select_npy_type<bool>
{
    static constexpr NPY_TYPES type = NPY_BOOL;
};
} // namespace openravepy

// pybind11
#ifdef USE_PYBIND11_PYTHON_BINDINGS
#define OPENRAVEPY_API __attribute__ ((visibility ("default")))
#include <iostream>
// use std::cout temporarily
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <boost/shared_ptr.hpp>
PYBIND11_DECLARE_HOLDER_TYPE(T, OPENRAVE_SHARED_PTR<T>);
namespace pybind11 {
namespace numeric {
// so py::numeric::array = py::array_t<double>
using array = ::pybind11::array_t<double>;
} // namespace pybind11::numeric
template <typename T>
inline T extract(object o) {
    return o.cast<T>();
}
template <typename T>
inline T extract(handle h) {
    return h.cast<T>();
}
template <typename T>
inline object to_object(const T& t) {
    return cast(t);
}
// https://stackoverflow.com/questions/26184190/alias-a-templated-function
template <typename T>
auto handle_to_object(T const& h) -> decltype(to_object<T>(h))
{
    return to_object<T>(h);
}
template <typename T>
inline object to_array_astype(PyObject* pyo) {
    // do we need to implement the conversion?
    return handle(pyo).cast<array_t<T>>();
}
template <typename T>
inline object empty_array_astype() {
    return array_t<T>({}, nullptr);
}
template <typename T>
struct extract_ {
    extract_() = delete; // disable default constructor
    explicit extract_(const object& o) {
        try {
            _data = extract<T>(o);
        }
        catch(...) {
            _bcheck = false;
            RAVELOG_WARN("Cannot extract type " + std::string(typeid(T).name()) + " from a pybind11::object");
        }
    }
    // user-defined conversion:
    // https://en.cppreference.com/w/cpp/language/cast_operator
    // implicit conversion
    operator T() const { return _data; }
    // explicit conversion
    explicit operator T*() const { return _data; }
    bool check() const { return _bcheck; }
    bool _bcheck = true;
    T _data;
}; // struct extract_
using scope_ = object;
inline object none_() {
    return none();
}
using array_int = array_t<int>; // py::array_int
} // namespace pybind11
#define OPENRAVE_PYTHON_MODULE(X) PYBIND11_MODULE(X, m)
#include <openravepy/map.h>
#define PY_ARG_(x) py ::arg(x),
#define PY_ARGS(...) MAP(PY_ARG_, __VA_ARGS__)
/* ==================== BOOST.PYTHON ==================== */
#else // USE_PYBIND11_PYTHON_BINDINGS
/* ==================== BOOST.PYTHON ==================== */
#define OPENRAVEPY_API
#include <boost/python.hpp> // already has #include <boost/shared_ptr.hpp>
#define OPENRAVE_PYTHON_MODULE(X) BOOST_PYTHON_MODULE(X)
// might need a space before "::"?
#define PY_ARGS(...) py::args(__VA_ARGS__),
namespace boost {
namespace python {
template <typename T>
inline object to_object(const T& t) {
    return object(t);
}
inline object handle_to_object(PyObject* pyo) {
    return object(handle<>(pyo));
}
template<typename T>
inline numeric::array to_array_astype(PyObject* pyo) {
    return static_cast<numeric::array>(static_cast<numeric::array>(handle<>(pyo)).astype(openravepy::select_dtype<T>::type));
}
template<typename T>
inline object empty_array_astype() {
    return numeric::array(list()).astype(openravepy::select_dtype<T>::type);
}
template <typename T>
using extract_ = extract<T>;
// https://www.boost.org/doc/libs/1_62_0/libs/python/doc/html/reference/high_level_components/boost_python_scope_hpp.html
using scope_ = scope;
inline object none_() {
    return object();
}
using array_int = object; // py::array_int
} // namespace boost::python
} // namespace boost
#endif // USE_PYBIND11_PYTHON_BINDINGS

// is_none is not supported by older versions of python
#if BOOST_VERSION >= 104300 || defined(USE_PYBIND11_PYTHON_BINDINGS)
#define IS_PYTHONOBJECT_NONE(o) (o).is_none()
#else
#define IS_PYTHONOBJECT_NONE(o) (!!(o))
#endif

namespace openravepy {

#ifdef USE_PYBIND11_PYTHON_BINDINGS
namespace py = pybind11;
#else
namespace py = boost::python;
#endif // USE_PYBIND11_PYTHON_BINDINGS

inline py::object ConvertStringToUnicode(const std::string& s)
{
#ifdef USE_PYBIND11_PYTHON_BINDINGS
    /*
    TGN: Is the following an alternative?
    ```
    PyObject *pyo = PyUnicode_Decode(s.c_str(), s.size(), "utf-8", nullptr);
    return py::handle(pyo).cast<py::array_t<double>>();
    ```
    */
    return py::cast(s);
#else
    return py::handle_to_object(PyUnicode_Decode(s.c_str(), s.size(), "utf-8", nullptr));
#endif
}

#if defined(USE_PYBIND11_PYTHON_BINDINGS) && defined(USE_PYBIND11_PYTHON3_BINDINGS)
// https://py3c.readthedocs.io/en/latest/reference.html
// https://docs.python.org/3/c-api/unicode.html#c.PyUnicode_AsUTF8
inline const char* PyString_AsString(PyObject* pystring) { return PyUnicode_AsUTF8(pystring); }
inline bool PyInt_Check(const PyObject* o) { return PyLong_Check(o); }
inline long PyInt_AsLong(PyObject* o) { return PyLong_AsLong(o); }
inline bool PyString_Check(PyObject* o) { return PyUnicode_Check(o); }
inline Py_ssize_t PyString_GET_SIZE(PyObject *o) { return PyBytes_GET_SIZE(o); }
inline PyObject* PyString_FromStringAndSize(const char *u, Py_ssize_t len) { return PyUnicode_FromStringAndSize(u, len); }
#endif // defined(USE_PYBIND11_PYTHON_BINDINGS) && defined(USE_PYBIND11_PYTHON3_BINDINGS)

class PyVoidHandle
{
public:
    PyVoidHandle() {
    }
    PyVoidHandle(OPENRAVE_SHARED_PTR<void> handle) : _handle(handle) {
    }
    void Close() {
        _handle.reset();
    }
    OPENRAVE_SHARED_PTR<void> _handle;
};

class PyVoidHandleConst
{
public:
    PyVoidHandleConst() {
    }
    PyVoidHandleConst(OPENRAVE_SHARED_PTR<void const> handle) : _handle(handle) {
    }
    void Close() {
        _handle.reset();
    }
    OPENRAVE_SHARED_PTR<void const> _handle;
};

template <typename T>
inline std::vector<T> ExtractArray(const py::object& o)
{
    if( IS_PYTHONOBJECT_NONE(o) ) {
        return {};
    }
    std::vector<T> v;
    try {
        const size_t n = len(o);
        v.resize(n);
        for(size_t i = 0; i < n; ++i) {
            v[i] = py::extract<T>(o[i]);
        }
    }
    catch(...) {
        RAVELOG_WARN("Cannot do ExtractArray for " + std::string(typeid(T).name()));
    }
    return v;
}

template <typename T>
inline std::set<T> ExtractSet(const py::object& o)
{
    std::set<T> v;
    size_t nlen = len(o);
    for(size_t i = 0; i < nlen; ++i) {
        v.insert(py::extract<T>(o[i]));
    }
    return v;
}

#ifndef USE_PYBIND11_PYTHON_BINDINGS
template <typename T>
struct exception_translator
{
    exception_translator() {
        py::register_exception_translator<T>(&exception_translator::translate);

        //Register custom r-value converter
        //There are situations, where we have to pass the exception back to
        //C++ library. This will do the trick
        py::converter::registry::push_back( &exception_translator::convertible, &exception_translator::construct, py::type_id<T>() );
    }

    static void translate( const T& err )
    {
        py::object pimpl_err( err );
        py::object pyerr_class = pimpl_err.attr( "py_err_class" );
        py::object pyerr = pyerr_class( pimpl_err );
        PyErr_SetObject( pyerr_class.ptr(), py::incref( pyerr.ptr() ) );
    }

    //Sometimes, exceptions should be passed back to the library.
    static void* convertible(PyObject* py_obj){
        if( 1 != PyObject_IsInstance( py_obj, PyExc_Exception ) ) {
            return 0;
        }

        if( !PyObject_HasAttrString( py_obj, "_pimpl" ) ) {
            return 0;
        }

        py::object pyerr( py::handle<>( py::borrowed( py_obj ) ) );
        py::object pimpl = getattr( pyerr, "_pimpl" );
        py::extract<T> type_checker( pimpl );
        if( !type_checker.check() ) {
            return 0;
        }
        return py_obj;
    }

    static void construct( PyObject* py_obj, py::converter::rvalue_from_python_stage1_data* data)
    {
        typedef py::converter::rvalue_from_python_storage<T> storage_t;

        py::object pyerr( py::handle<>( py::borrowed( py_obj ) ) );
        py::object pimpl = getattr( pyerr, "_pimpl" );

        storage_t* the_storage = reinterpret_cast<storage_t*>( data );
        void* memory_chunk = the_storage->storage.bytes;
        new (memory_chunk) T( py::extract<T>(pimpl) );
        data->convertible = memory_chunk;
    }
}; // struct exception_translator

template<typename T>
struct float_from_number
{
    float_from_number()
    {
        py::converter::registry::push_back(&convertible, &construct, py::type_id<T>());
    }

    static void* convertible( PyObject* obj)
    {
        return PyNumber_Check(obj) ? obj : nullptr;
    }

    static void construct(PyObject* _obj, py::converter::rvalue_from_python_stage1_data* data)
    {
        PyObject* tmp = PyNumber_Float(_obj);
        T* storage = (T*)((py::converter::rvalue_from_python_storage<T>*)data)->storage.bytes;
        *storage = py::extract<T>(tmp);
        Py_DECREF(tmp);
        data->convertible = storage;
    }
};

template<typename T>
struct int_from_number
{
    int_from_number()
    {
        py::converter::registry::push_back(&convertible, &construct, py::type_id<T>());
    }

    static void* convertible( PyObject* obj)
    {
        return PyNumber_Check(obj) ? obj : nullptr;
    }

    static void construct(PyObject* _obj, py::converter::rvalue_from_python_stage1_data* data)
    {
        PyObject* tmp = PyNumber_Long(_obj);
        T* storage = (T*)((py::converter::rvalue_from_python_storage<T>*)data)->storage.bytes;
        *storage = py::extract<T>(tmp);
        Py_DECREF(tmp);
        data->convertible = storage;
    }
};
#endif // USE_PYBIND11_PYTHON_BINDINGS

inline std::string GetPyErrorString()
{
    PyObject *error, *value, *traceback, *str;
    PyErr_Fetch(&error, &value, &traceback);
    PyErr_NormalizeException(&error, &value, &traceback);
    std::string s_out;
    if(error != nullptr) {
        str = PyObject_Str(value);
        if(str != nullptr) {
            s_out.assign(PyString_AsString(str));
            Py_DECREF(str);
        }
    }
    // Does nothing when the ptr is nullptr
    Py_DECREF(error);
    Py_DECREF(value);
    Py_DECREF(traceback);
    return s_out;
}

/// should call in the beginning of all BOOST_PYTHON_MODULE
void init_python_bindings();

#ifdef OPENRAVE_BINDINGS_PYARRAY

#ifdef USE_PYBIND11_PYTHON_BINDINGS
template <typename T>
inline py::array_t<T> toPyArrayN(const T* pvalues, const size_t N)
{
    // one-dimension numpy array
    std::vector<npy_intp> dims {(long int)N};
    return py::array_t<T>(dims, pvalues); 
}

template <typename T>
inline py::array_t<T> toPyArrayN(const T* pvalues, std::vector<npy_intp>& dims)
{
    // n-dimension numpy array
    return py::array_t<T>(dims, pvalues); 
}
// ========================================
#else // USE_PYBIND11_PYTHON_BINDINGS
// ========================================
template <typename T>
inline py::numeric::array toPyArrayN(const T* pvalues, const size_t N)
{
    if( N == 0 ) {
        return static_cast<py::numeric::array>(py::empty_array_astype<T>());
    }
    npy_intp dims[] = { npy_intp(N) };
    PyObject *pyvalues = PyArray_SimpleNew(1, dims, select_npy_type<T>::type);
    if( pvalues != nullptr ) {
        memcpy(PyArray_DATA(pyvalues), pvalues, N * sizeof(T));
    }
    return static_cast<py::numeric::array>(py::handle<>(pyvalues));
}

template <typename T>
inline py::numeric::array toPyArrayN(const T* pvalues, std::vector<npy_intp>& dims)
{
    if( dims.empty() ) {
        return static_cast<py::numeric::array>(py::empty_array_astype<T>());
    }
    size_t numel = 1;
    for(npy_intp dim : dims) {
        numel *= dim;
    }
    if( numel == 0 ) {
        return static_cast<py::numeric::array>(py::empty_array_astype<T>());
    }
    PyObject *pyvalues = PyArray_SimpleNew(dims.size(), dims.data(), select_npy_type<T>::type);
    if( pvalues != nullptr ) {
        memcpy(PyArray_DATA(pyvalues), pvalues, numel * sizeof(T));
    }
    return static_cast<py::numeric::array>(py::handle<>(pyvalues));
}
#endif // USE_PYBIND11_PYTHON_BINDINGS

template <typename T>
inline 
#ifdef USE_PYBIND11_PYTHON_BINDINGS
py::array_t<T>
#else
py::numeric::array
#endif 
toPyArray(const std::vector<T>& v)
{
    return toPyArrayN(v.data(), v.size());
}

template <typename T>
inline
#ifdef USE_PYBIND11_PYTHON_BINDINGS
py::array_t<T>
#else
py::numeric::array
#endif
toPyArray(const std::vector<T>& v, std::vector<npy_intp>& dims)
{
    if( v.empty() ) {
        return toPyArrayN((T*)nullptr, dims);
    }
    size_t numel = 1;
    for(npy_intp dim : dims) {
        numel *= dim;
    }
    BOOST_ASSERT(numel == v.size());
    return toPyArrayN(v.data(), dims);
}

template <typename T, int N>
inline
#ifdef USE_PYBIND11_PYTHON_BINDINGS
py::array_t<T>
#else
py::numeric::array
#endif
toPyArray(const boost::array<T, N>& v)
{
    return toPyArrayN(v.data(), N);
}

template <typename T, int N>
inline
#ifdef USE_PYBIND11_PYTHON_BINDINGS
py::array_t<T>
#else
py::numeric::array
#endif
toPyArray(const std::array<T, N>& v)
{
    return toPyArrayN(v.data(), N);
}

#endif // OPENRAVE_BINDINGS_PYARRAY

} // namespace openravepy

#endif // OPENRAVE_BOOST_PYTHON_BINDINGS
