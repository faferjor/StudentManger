/****************************************************************************
** Meta object code from reading C++ file 'topicbrowsingwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../topicbrowsingwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'topicbrowsingwindow.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSTopicBrowsingWindowENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSTopicBrowsingWindowENDCLASS = QtMocHelpers::stringData(
    "TopicBrowsingWindow",
    "onApplyTopicClicked",
    "",
    "onRefreshClicked",
    "onSearchTextChanged",
    "text",
    "onStatusFilterChanged",
    "index",
    "onTeacherFilterChanged",
    "onTopicResponseReceived",
    "requestId",
    "response",
    "onApplyDialogAccepted",
    "showTopicDetail",
    "topicInfo"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSTopicBrowsingWindowENDCLASS_t {
    uint offsetsAndSizes[30];
    char stringdata0[20];
    char stringdata1[20];
    char stringdata2[1];
    char stringdata3[17];
    char stringdata4[20];
    char stringdata5[5];
    char stringdata6[22];
    char stringdata7[6];
    char stringdata8[23];
    char stringdata9[24];
    char stringdata10[10];
    char stringdata11[9];
    char stringdata12[22];
    char stringdata13[16];
    char stringdata14[10];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSTopicBrowsingWindowENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSTopicBrowsingWindowENDCLASS_t qt_meta_stringdata_CLASSTopicBrowsingWindowENDCLASS = {
    {
        QT_MOC_LITERAL(0, 19),  // "TopicBrowsingWindow"
        QT_MOC_LITERAL(20, 19),  // "onApplyTopicClicked"
        QT_MOC_LITERAL(40, 0),  // ""
        QT_MOC_LITERAL(41, 16),  // "onRefreshClicked"
        QT_MOC_LITERAL(58, 19),  // "onSearchTextChanged"
        QT_MOC_LITERAL(78, 4),  // "text"
        QT_MOC_LITERAL(83, 21),  // "onStatusFilterChanged"
        QT_MOC_LITERAL(105, 5),  // "index"
        QT_MOC_LITERAL(111, 22),  // "onTeacherFilterChanged"
        QT_MOC_LITERAL(134, 23),  // "onTopicResponseReceived"
        QT_MOC_LITERAL(158, 9),  // "requestId"
        QT_MOC_LITERAL(168, 8),  // "response"
        QT_MOC_LITERAL(177, 21),  // "onApplyDialogAccepted"
        QT_MOC_LITERAL(199, 15),  // "showTopicDetail"
        QT_MOC_LITERAL(215, 9)   // "topicInfo"
    },
    "TopicBrowsingWindow",
    "onApplyTopicClicked",
    "",
    "onRefreshClicked",
    "onSearchTextChanged",
    "text",
    "onStatusFilterChanged",
    "index",
    "onTeacherFilterChanged",
    "onTopicResponseReceived",
    "requestId",
    "response",
    "onApplyDialogAccepted",
    "showTopicDetail",
    "topicInfo"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSTopicBrowsingWindowENDCLASS[] = {

 // content:
      11,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   62,    2, 0x08,    1 /* Private */,
       3,    0,   63,    2, 0x08,    2 /* Private */,
       4,    1,   64,    2, 0x08,    3 /* Private */,
       6,    1,   67,    2, 0x08,    5 /* Private */,
       8,    1,   70,    2, 0x08,    7 /* Private */,
       9,    2,   73,    2, 0x08,    9 /* Private */,
      12,    0,   78,    2, 0x08,   12 /* Private */,
      13,    1,   79,    2, 0x08,   13 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    5,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void, QMetaType::QString, QMetaType::QJsonObject,   10,   11,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QJsonObject,   14,

       0        // eod
};

Q_CONSTINIT const QMetaObject TopicBrowsingWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSTopicBrowsingWindowENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSTopicBrowsingWindowENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSTopicBrowsingWindowENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<TopicBrowsingWindow, std::true_type>,
        // method 'onApplyTopicClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRefreshClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSearchTextChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onStatusFilterChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onTeacherFilterChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onTopicResponseReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonObject &, std::false_type>,
        // method 'onApplyDialogAccepted'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'showTopicDetail'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonObject &, std::false_type>
    >,
    nullptr
} };

void TopicBrowsingWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TopicBrowsingWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onApplyTopicClicked(); break;
        case 1: _t->onRefreshClicked(); break;
        case 2: _t->onSearchTextChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->onStatusFilterChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->onTeacherFilterChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->onTopicResponseReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QJsonObject>>(_a[2]))); break;
        case 6: _t->onApplyDialogAccepted(); break;
        case 7: _t->showTopicDetail((*reinterpret_cast< std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *TopicBrowsingWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TopicBrowsingWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSTopicBrowsingWindowENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int TopicBrowsingWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }
    return _id;
}
QT_WARNING_POP
