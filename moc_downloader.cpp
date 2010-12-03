/****************************************************************************
** Meta object code from reading C++ file 'downloader.h'
**
** Created: Thu Nov 11 17:39:58 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "downloader.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'downloader.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Downloader[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      16,   12,   11,   11, 0x08,
      39,   11,   11,   11, 0x08,
      52,   11,   11,   11, 0x08,
      66,   11,   11,   11, 0x08,
      86,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Downloader[] = {
    "Downloader\0\0msg\0onLoggedError(QString)\0"
    "onLoggedIn()\0onLoggedOut()\0"
    "onMetadataUpdated()\0onEndOfTrack()\0"
};

const QMetaObject Downloader::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Downloader,
      qt_meta_data_Downloader, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Downloader::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Downloader::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Downloader::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Downloader))
        return static_cast<void*>(const_cast< Downloader*>(this));
    return QObject::qt_metacast(_clname);
}

int Downloader::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: onLoggedError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: onLoggedIn(); break;
        case 2: onLoggedOut(); break;
        case 3: onMetadataUpdated(); break;
        case 4: onEndOfTrack(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
