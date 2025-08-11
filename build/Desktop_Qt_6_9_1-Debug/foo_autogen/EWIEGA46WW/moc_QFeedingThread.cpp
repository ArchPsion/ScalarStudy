/****************************************************************************
** Meta object code from reading C++ file 'QFeedingThread.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../QFeedingThread.hpp"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QFeedingThread.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN14QFeedingThreadE_t {};
} // unnamed namespace

template <> constexpr inline auto QFeedingThread::qt_create_metaobjectdata<qt_meta_tag_ZN14QFeedingThreadE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QFeedingThread",
        "triggerGapUpdate",
        "",
        "triggerTableUpdate",
        "HexUpdateFile*"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'triggerGapUpdate'
        QtMocHelpers::SignalData<void(qreal)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QReal, 2 },
        }}),
        // Signal 'triggerTableUpdate'
        QtMocHelpers::SignalData<void(HexUpdateFile *)>(3, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 4, 2 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QFeedingThread, qt_meta_tag_ZN14QFeedingThreadE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QFeedingThread::staticMetaObject = { {
    QMetaObject::SuperData::link<QScalarThread::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14QFeedingThreadE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14QFeedingThreadE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN14QFeedingThreadE_t>.metaTypes,
    nullptr
} };

void QFeedingThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QFeedingThread *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->triggerGapUpdate((*reinterpret_cast< std::add_pointer_t<qreal>>(_a[1]))); break;
        case 1: _t->triggerTableUpdate((*reinterpret_cast< std::add_pointer_t<HexUpdateFile*>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (QFeedingThread::*)(qreal )>(_a, &QFeedingThread::triggerGapUpdate, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (QFeedingThread::*)(HexUpdateFile * )>(_a, &QFeedingThread::triggerTableUpdate, 1))
            return;
    }
}

const QMetaObject *QFeedingThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QFeedingThread::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14QFeedingThreadE_t>.strings))
        return static_cast<void*>(this);
    return QScalarThread::qt_metacast(_clname);
}

int QFeedingThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QScalarThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void QFeedingThread::triggerGapUpdate(qreal _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void QFeedingThread::triggerTableUpdate(HexUpdateFile * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}
QT_WARNING_POP
