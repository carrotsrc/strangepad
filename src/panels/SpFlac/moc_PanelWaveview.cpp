/****************************************************************************
** Meta object code from reading C++ file 'PanelWaveview.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "PanelWaveview.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PanelWaveview.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_SpFlacWaveview_t {
    QByteArrayData data[6];
    char stringdata0[68];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SpFlacWaveview_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SpFlacWaveview_t qt_meta_stringdata_SpFlacWaveview = {
    {
QT_MOC_LITERAL(0, 0, 14), // "SpFlacWaveview"
QT_MOC_LITERAL(1, 15, 9), // "guiUpdate"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 15), // "triggerMidiPlay"
QT_MOC_LITERAL(4, 42, 11), // "onGuiUpdate"
QT_MOC_LITERAL(5, 54, 13) // "probeProgress"

    },
    "SpFlacWaveview\0guiUpdate\0\0triggerMidiPlay\0"
    "onGuiUpdate\0probeProgress"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SpFlacWaveview[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    0,   35,    2, 0x0a /* Public */,
       4,    0,   36,    2, 0x0a /* Public */,
       5,    0,   37,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void SpFlacWaveview::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        SpFlacWaveview *_t = static_cast<SpFlacWaveview *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->guiUpdate(); break;
        case 1: _t->triggerMidiPlay(); break;
        case 2: _t->onGuiUpdate(); break;
        case 3: _t->probeProgress(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (SpFlacWaveview::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&SpFlacWaveview::guiUpdate)) {
                *result = 0;
            }
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject SpFlacWaveview::staticMetaObject = {
    { &SPad::staticMetaObject, qt_meta_stringdata_SpFlacWaveview.data,
      qt_meta_data_SpFlacWaveview,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *SpFlacWaveview::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SpFlacWaveview::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_SpFlacWaveview.stringdata0))
        return static_cast<void*>(const_cast< SpFlacWaveview*>(this));
    return SPad::qt_metacast(_clname);
}

int SpFlacWaveview::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SPad::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void SpFlacWaveview::guiUpdate()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
