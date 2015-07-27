/****************************************************************************
** Meta object code from reading C++ file 'PanelWaveview.hpp'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "PanelWaveview.hpp"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PanelWaveview.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SpFlacWaveview[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_SpFlacWaveview[] = {
    "SpFlacWaveview\0\0triggerMidiPlay()\0"
};

void SpFlacWaveview::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SpFlacWaveview *_t = static_cast<SpFlacWaveview *>(_o);
        switch (_id) {
        case 0: _t->triggerMidiPlay(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData SpFlacWaveview::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SpFlacWaveview::staticMetaObject = {
    { &SPad::staticMetaObject, qt_meta_stringdata_SpFlacWaveview,
      qt_meta_data_SpFlacWaveview, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SpFlacWaveview::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SpFlacWaveview::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SpFlacWaveview::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SpFlacWaveview))
        return static_cast<void*>(const_cast< SpFlacWaveview*>(this));
    return SPad::qt_metacast(_clname);
}

int SpFlacWaveview::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = SPad::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
