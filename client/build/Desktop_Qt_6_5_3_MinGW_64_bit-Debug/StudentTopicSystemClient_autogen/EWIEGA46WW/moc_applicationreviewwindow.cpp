/****************************************************************************
** Meta object code from reading C++ file 'applicationreviewwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.5.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../applicationreviewwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#if __has_include(<QtCore/qtmochelpers.h>)
#include <QtCore/qtmochelpers.h>
#else
QT_BEGIN_MOC_NAMESPACE
#endif


#include <memory>

#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'applicationreviewwindow.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_CLASSApplicationReviewWindowENDCLASS_t {};
static constexpr auto qt_meta_stringdata_CLASSApplicationReviewWindowENDCLASS = QtMocHelpers::stringData(
    "ApplicationReviewWindow",
    "onApproveClicked",
    "",
    "onRejectClicked",
    "onRefreshClicked",
    "onSearchTextChanged",
    "text",
    "onStatusFilterChanged",
    "index",
    "onTopicFilterChanged",
    "onResponseReceived",
    "requestId",
    "response",
    "showApplicationDetail",
    "appInfo",
    "showRejectDialog"
);
#else  // !QT_MOC_HAS_STRING_DATA
struct qt_meta_stringdata_CLASSApplicationReviewWindowENDCLASS_t {
    uint offsetsAndSizes[32];
    char stringdata0[24];
    char stringdata1[17];
    char stringdata2[1];
    char stringdata3[16];
    char stringdata4[17];
    char stringdata5[20];
    char stringdata6[5];
    char stringdata7[22];
    char stringdata8[6];
    char stringdata9[21];
    char stringdata10[19];
    char stringdata11[10];
    char stringdata12[9];
    char stringdata13[22];
    char stringdata14[8];
    char stringdata15[17];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_CLASSApplicationReviewWindowENDCLASS_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_CLASSApplicationReviewWindowENDCLASS_t qt_meta_stringdata_CLASSApplicationReviewWindowENDCLASS = {
    {
        QT_MOC_LITERAL(0, 23),  // "ApplicationReviewWindow"
        QT_MOC_LITERAL(24, 16),  // "onApproveClicked"
        QT_MOC_LITERAL(41, 0),  // ""
        QT_MOC_LITERAL(42, 15),  // "onRejectClicked"
        QT_MOC_LITERAL(58, 16),  // "onRefreshClicked"
        QT_MOC_LITERAL(75, 19),  // "onSearchTextChanged"
        QT_MOC_LITERAL(95, 4),  // "text"
        QT_MOC_LITERAL(100, 21),  // "onStatusFilterChanged"
        QT_MOC_LITERAL(122, 5),  // "index"
        QT_MOC_LITERAL(128, 20),  // "onTopicFilterChanged"
        QT_MOC_LITERAL(149, 18),  // "onResponseReceived"
        QT_MOC_LITERAL(168, 9),  // "requestId"
        QT_MOC_LITERAL(178, 8),  // "response"
        QT_MOC_LITERAL(187, 21),  // "showApplicationDetail"
        QT_MOC_LITERAL(209, 7),  // "appInfo"
        QT_MOC_LITERAL(217, 16)   // "showRejectDialog"
    },
    "ApplicationReviewWindow",
    "onApproveClicked",
    "",
    "onRejectClicked",
    "onRefreshClicked",
    "onSearchTextChanged",
    "text",
    "onStatusFilterChanged",
    "index",
    "onTopicFilterChanged",
    "onResponseReceived",
    "requestId",
    "response",
    "showApplicationDetail",
    "appInfo",
    "showRejectDialog"
};
#undef QT_MOC_LITERAL
#endif // !QT_MOC_HAS_STRING_DATA
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_CLASSApplicationReviewWindowENDCLASS[] = {

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
       5,    1,   71,    2, 0x08,    4 /* Private */,
       7,    1,   74,    2, 0x08,    6 /* Private */,
       9,    1,   77,    2, 0x08,    8 /* Private */,
      10,    2,   80,    2, 0x08,   10 /* Private */,
      13,    1,   85,    2, 0x08,   13 /* Private */,
      15,    1,   88,    2, 0x08,   15 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::Int,    8,
    QMetaType::Void, QMetaType::QString, QMetaType::QJsonObject,   11,   12,
    QMetaType::Void, QMetaType::QJsonObject,   14,
    QMetaType::Void, QMetaType::QJsonObject,   14,

       0        // eod
};

Q_CONSTINIT const QMetaObject ApplicationReviewWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CLASSApplicationReviewWindowENDCLASS.offsetsAndSizes,
    qt_meta_data_CLASSApplicationReviewWindowENDCLASS,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_CLASSApplicationReviewWindowENDCLASS_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<ApplicationReviewWindow, std::true_type>,
        // method 'onApproveClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRejectClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRefreshClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSearchTextChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'onStatusFilterChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onTopicFilterChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'onResponseReceived'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonObject &, std::false_type>,
        // method 'showApplicationDetail'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonObject &, std::false_type>,
        // method 'showRejectDialog'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QJsonObject &, std::false_type>
    >,
    nullptr
} };

void ApplicationReviewWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ApplicationReviewWindow *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onApproveClicked(); break;
        case 1: _t->onRejectClicked(); break;
        case 2: _t->onRefreshClicked(); break;
        case 3: _t->onSearchTextChanged((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->onStatusFilterChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->onTopicFilterChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->onResponseReceived((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QJsonObject>>(_a[2]))); break;
        case 7: _t->showApplicationDetail((*reinterpret_cast< std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 8: _t->showRejectDialog((*reinterpret_cast< std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *ApplicationReviewWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ApplicationReviewWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CLASSApplicationReviewWindowENDCLASS.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int ApplicationReviewWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
