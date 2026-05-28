/****************************************************************************
** Meta object code from reading C++ file 'topicmanagementwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../topicmanagementwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'topicmanagementwindow.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSTopicManagementWindowENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSTopicManagementWindowENDCLASS = QtMocHelpers::stringData(
    "TopicManagementWindow",
    "onAddTopicClicked",
    "",
    "onEditTopicClicked",
    "onDeleteTopicClicked",
    "onRefreshClicked",
    "onSearchTextChanged",
    "text",
    "onStatusFilterChanged",
    "index",
    "onTopicResponseReceived",
    "requestId",
    "response",
    "onAddTopicDialogAccepted",
    "onEditTopicDialogAccepted"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSTopicManagementWindowENDCLASS_t {
    uint offsetsAndSizes[30];
    char stringdata0[22];
    char stringdata1[18];
    char stringdata2[1];
    char stringdata3[19];
    char stringdata4[21];
    char stringdata5[17];
    char stringdata6[20];
    char stringdata7[5];
    char stringdata8[22];
    char stringdata9[6];
    char stringdata10[24];
    char stringdata11[10];
    char stringdata12[9];
    char stringdata13[25];
    char stringdata14[26];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSTopicManagementWindowENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSTopicManagementWindowENDCLASS_t qt_meta_stringdata_CLASSTopicManagementWindowENDCLASS = {
    {
        QT_MOC_LITERAL(0, 21),  // "TopicManagementWindow"
        QT_MOC_LITERAL(22, 17),  // "onAddTopicClicked"
        QT_MOC_LITERAL(40, 0),  // ""
        QT_MOC_LITERAL(41, 18),  // "onEditTopicClicked"
        QT_MOC_LITERAL(60, 20),  // "onDeleteTopicClicked"
        QT_MOC_LITERAL(81, 16),  // "onRefreshClicked"
        QT_MOC_LITERAL(98, 19),  // "onSearchTextChanged"
        QT_MOC_LITERAL(118, 4),  // "text"
        QT_MOC_LITERAL(123, 21),  // "onStatusFilterChanged"
        QT_MOC_LITERAL(145, 5),  // "index"
        QT_MOC_LITERAL(151, 23),  // "onTopicResponseReceived"
        QT_MOC_LITERAL(175, 9),  // "requestId"
        QT_MOC_LITERAL(185, 8),  // "response"
        QT_MOC_LITERAL(194, 24),  // "onAddTopicDialogAccepted"
        QT_MOC_LITERAL(219, 25)   // "onEditTopicDialogAccepted"
    },
    "TopicManagementWindow",
    "onAddTopicClicked",
    "",
    "onEditTopicClicked",
    "onDeleteTopicClicked",
    "onRefreshClicked",
    "onSearchTextChanged",
    "text",
    "onStatusFilterChanged",
    "index",
    "onTopicResponseReceived",
    "requestId",
    "response",
    "onAddTopicDialogAccepted",
    "onEditTopicDialogAccepted"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSTopicManagementWindowENDCLASS[] = {

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

Q_CONSTINIT const QMetaObject TopicManagementWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSTopicManagementWindowENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSTopicManagementWindowENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSTopicManagementWindowENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<TopicManagementWindow, std::true_type>,
        // method 'onAddTopicClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onEditTopicClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDeleteTopicClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRefreshClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSearchTextChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onStatusFilterChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onTopicResponseReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonObject &, std::false_type>,
        // method 'onAddTopicDialogAccepted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onEditTopicDialogAccepted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void TopicManagementWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TopicManagementWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onAddTopicClicked(); break;
        case 1: _t->onEditTopicClicked(); break;
        case 2: _t->onDeleteTopicClicked(); break;
        case 3: _t->onRefreshClicked(); break;
        case 4: _t->onSearchTextChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->onStatusFilterChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->onTopicResponseReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QJsonObject>>(_a[2]))); break;
        case 7: _t->onAddTopicDialogAccepted(); break;
        case 8: _t->onEditTopicDialogAccepted(); break;
        default: ;
        }
    }
}

const QMetaObject *TopicManagementWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TopicManagementWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSTopicManagementWindowENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int TopicManagementWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
