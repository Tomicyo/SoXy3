/********************************************************************************
** Form generated from reading UI file 'notifywin.ui'
**
** Created: Sun May 19 19:28:48 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NOTIFYWIN_H
#define UI_NOTIFYWIN_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QTextBrowser>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_NotifyWin
{
public:
    QLabel *label;
    QTextBrowser *textBrowser;

    void setupUi(QWidget *NotifyWin)
    {
        if (NotifyWin->objectName().isEmpty())
            NotifyWin->setObjectName(QString::fromUtf8("NotifyWin"));
        NotifyWin->resize(366, 171);
        NotifyWin->setStyleSheet(QString::fromUtf8("color:white;"));
        label = new QLabel(NotifyWin);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 20, 331, 121));
        label->setStyleSheet(QString::fromUtf8("background:transparent;"));
        textBrowser = new QTextBrowser(NotifyWin);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));
        textBrowser->setGeometry(QRect(20, 20, 321, 131));
        textBrowser->setStyleSheet(QString::fromUtf8("background:transparent;\n"
"color:white;\n"
"border: 0px;"));

        retranslateUi(NotifyWin);

        QMetaObject::connectSlotsByName(NotifyWin);
    } // setupUi

    void retranslateUi(QWidget *NotifyWin)
    {
        NotifyWin->setWindowTitle(QApplication::translate("NotifyWin", "Form", 0, QApplication::UnicodeUTF8));
        label->setText(QString());
        textBrowser->setHtml(QApplication::translate("NotifyWin", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class NotifyWin: public Ui_NotifyWin {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NOTIFYWIN_H
