/****************************************************************************
** Meta object code from reading C++ file 'usermanagementwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../usermanagementwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'usermanagementwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.5.3. It"
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

#ifdef QT_MOC_HAS_STRINGDATA
struct qt_meta_stringdata_CLASSUserManagementWindowENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSUserManagementWindowENDCLASS = QtMocHelpers::stringData(
    "UserManagementWindow",
    "onAddUserClicked",
    "",
    "onEditUserClicked",
    "onDeleteUserClicked",
    "onRefreshClicked",
    "onSearchTextChanged",
    "text",
    "onUserTypeFilterChanged",
    "index",
    "onUserResponseReceived",
    "requestId",
    "response",
    "onAddUserDialogAccepted",
    "onEditUserDialogAccepted"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSUserManagementWindowENDCLASS_t {
    uint offsetsAndSizes[30];
    char stringdata0[21];
    char stringdata1[17];
    char stringdata2[1];
    char stringdata3[18];
    char stringdata4[20];
    char stringdata5[17];
    char stringdata6[20];
    char stringdata7[5];
    char stringdata8[24];
    char stringdata9[6];
    char stringdata10[23];
    char stringdata11[10];
    char stringdata12[9];
    char stringdata13[24];
    char stringdata14[25];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSUserManagementWindowENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSUserManagementWindowENDCLASS_t qt_meta_stringdata_CLASSUserManagementWindowENDCLASS = {
    {
        QT_MOC_LITERAL(0, 20),  // "UserManagementWindow"
        QT_MOC_LITERAL(21, 16),  // "onAddUserClicked"
        QT_MOC_LITERAL(38, 0),  // ""
        QT_MOC_LITERAL(39, 17),  // "onEditUserClicked"
        QT_MOC_LITERAL(57, 19),  // "onDeleteUserClicked"
        QT_MOC_LITERAL(77, 16),  // "onRefreshClicked"
        QT_MOC_LITERAL(94, 19),  // "onSearchTextChanged"
        QT_MOC_LITERAL(114, 4),  // "text"
        QT_MOC_LITERAL(119, 23),  // "onUserTypeFilterChanged"
        QT_MOC_LITERAL(143, 5),  // "index"
        QT_MOC_LITERAL(149, 22),  // "onUserResponseReceived"
        QT_MOC_LITERAL(172, 9),  // "requestId"
        QT_MOC_LITERAL(182, 8),  // "response"
        QT_MOC_LITERAL(191, 23),  // "onAddUserDialogAccepted"
        QT_MOC_LITERAL(215, 24)   // "onEditUserDialogAccepted"
    },
    "UserManagementWindow",
    "onAddUserClicked",
    "",
    "onEditUserClicked",
    "onDeleteUserClicked",
    "onRefreshClicked",
    "onSearchTextChanged",
    "text",
    "onUserTypeFilterChanged",
    "index",
    "onUserResponseReceived",
    "requestId",
    "response",
    "onAddUserDialogAccepted",
    "onEditUserDialogAccepted"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSUserManagementWindowENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   68,    2, 0x08,    1 /* Private */,
       3,    0,   69,    2, 0x08,    2 /* Private */,
       4,    0,   70,    2, 0x08,    3 /* Private */,
       5,    0,   71,    2, 0x08,    4 /* Private */,
       6,    1,   72,    2, 0x08,    5 /* Private */,
       8,    1,   75,    2, 0x08,    7 /* Private */,
      10,    2,   78,    2, 0x08,    9 /* Private */,
      13,    0,   83,    2, 0x08,   12 /* Private */,
      14,    0,   84,    2, 0x08,   13 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    7,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::QString, QMetaType::QJsonObject,   11,   12,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject UserManagementWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSUserManagementWindowENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSUserManagementWindowENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSUserManagementWindowENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<UserManagementWindow, std::true_type>,
        // method 'onAddUserClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onEditUserClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDeleteUserClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRefreshClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSearchTextChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onUserTypeFilterChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onUserResponseReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonObject &, std::false_type>,
        // method 'onAddUserDialogAccepted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onEditUserDialogAccepted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void UserManagementWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<UserManagementWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onAddUserClicked(); break;
        case 1: _t->onEditUserClicked(); break;
        case 2: _t->onDeleteUserClicked(); break;
        case 3: _t->onRefreshClicked(); break;
        case 4: _t->onSearchTextChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->onUserTypeFilterChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->onUserResponseReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QJsonObject>>(_a[2]))); break;
        case 7: _t->onAddUserDialogAccepted(); break;
        case 8: _t->onEditUserDialogAccepted(); break;
        default: ;
        }
    }
}

const QMetaObject *UserManagementWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UserManagementWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSUserManagementWindowENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int UserManagementWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
    return _id;
}
QT_WARNING_POP
