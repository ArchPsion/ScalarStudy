/****************************************************************************
** Meta object code from reading C++ file 'QScalarWindow.hpp'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../QScalarWindow.hpp"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QScalarWindow.hpp' doesn't include <QObject>."
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
struct qt_meta_tag_ZN13QScalarWindowE_t {};
} // unnamed namespace

template <> constexpr inline auto QScalarWindow::qt_create_metaobjectdata<qt_meta_tag_ZN13QScalarWindowE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "QScalarWindow",
        "analyse",
        "",
        "deleteCandlestick",
        "feed",
        "locate",
        "reset",
        "scan",
        "showNegativeCandlesticks",
        "QModelIndex",
        "showPositiveCandlesticks",
        "toggleFeed",
        "toggleScan",
        "triggerSearch",
        "HexUpdateFile*",
        "updateGap",
        "updateVolume"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'analyse'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'deleteCandlestick'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'feed'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'locate'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'reset'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'scan'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'showNegativeCandlesticks'
        QtMocHelpers::SlotData<void(const QModelIndex &)>(8, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 9, 2 },
        }}),
        // Slot 'showPositiveCandlesticks'
        QtMocHelpers::SlotData<void(const QModelIndex &)>(10, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 9, 2 },
        }}),
        // Slot 'toggleFeed'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'toggleScan'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'triggerSearch'
        QtMocHelpers::SlotData<void(HexUpdateFile *)>(13, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 14, 2 },
        }}),
        // Slot 'updateGap'
        QtMocHelpers::SlotData<void(qreal)>(15, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QReal, 2 },
        }}),
        // Slot 'updateVolume'
        QtMocHelpers::SlotData<void(int)>(16, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 2 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<QScalarWindow, qt_meta_tag_ZN13QScalarWindowE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject QScalarWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13QScalarWindowE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13QScalarWindowE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13QScalarWindowE_t>.metaTypes,
    nullptr
} };

void QScalarWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<QScalarWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->analyse(); break;
        case 1: _t->deleteCandlestick(); break;
        case 2: _t->feed(); break;
        case 3: _t->locate(); break;
        case 4: _t->reset(); break;
        case 5: _t->scan(); break;
        case 6: _t->showNegativeCandlesticks((*reinterpret_cast< std::add_pointer_t<QModelIndex>>(_a[1]))); break;
        case 7: _t->showPositiveCandlesticks((*reinterpret_cast< std::add_pointer_t<QModelIndex>>(_a[1]))); break;
        case 8: _t->toggleFeed(); break;
        case 9: _t->toggleScan(); break;
        case 10: _t->triggerSearch((*reinterpret_cast< std::add_pointer_t<HexUpdateFile*>>(_a[1]))); break;
        case 11: _t->updateGap((*reinterpret_cast< std::add_pointer_t<qreal>>(_a[1]))); break;
        case 12: _t->updateVolume((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *QScalarWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *QScalarWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13QScalarWindowE_t>.strings))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int QScalarWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 13;
    }
    return _id;
}
QT_WARNING_POP
