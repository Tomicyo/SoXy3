/********************************************************************************
** Form generated from reading UI file 'rjwidget.ui'
**
** Created: Fri May 24 10:32:36 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RJWIDGET_H
#define UI_RJWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RJWidget
{
public:
    QComboBox *cbox_nic;

    void setupUi(QWidget *RJWidget)
    {
        if (RJWidget->objectName().isEmpty())
            RJWidget->setObjectName(QString::fromUtf8("RJWidget"));
        RJWidget->resize(552, 281);
        cbox_nic = new QComboBox(RJWidget);
        cbox_nic->setObjectName(QString::fromUtf8("cbox_nic"));
        cbox_nic->setGeometry(QRect(115, 213, 160, 35));
        cbox_nic->setStyleSheet(QString::fromUtf8("QComboBox {\n"
"	 color: white;\n"
"     font-size: 24px;\n"
"     border:0px;\n"
"     padding-right: 20px;\n"
"}\n"
"\n"
"QComboBox:editable {\n"
"	background-image: url(:/img/netcardselect.png);\n"
"}\n"
"\n"
"QComboBox:!editable, QComboBox::drop-down:editable {\n"
"	background-image: url(:/img/netcardselect.png);\n"
"}\n"
"\n"
" /* QComboBox gets the \"on\" state when the popup is open */\n"
"QComboBox:!editable:on, QComboBox::drop-down:editable:on {\n"
"	background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(58, 63, 66, 255), stop:0.661017 rgba(71, 77, 80, 255), stop:1 rgba(86, 93, 97, 255));\n"
"}\n"
"\n"
"QComboBox:on { /* shift the text when the popup opens */\n"
"     padding-top: 3px;\n"
"     padding-left: 4px;\n"
"}\n"
"\n"
"QComboBox::drop-down {\n"
"     subcontrol-origin: padding;\n"
"     subcontrol-position: right;\n"
"\n"
"     border-left-width: 1px;\n"
"     border-left-color: darkgray;\n"
"     border-left-style: solid; /* just a single line */\n"
"     border-to"
                        "p-right-radius: 1px; /* same radius as the QComboBox */\n"
"     border-bottom-right-radius: 1px;\n"
"}\n"
"\n"
"QComboBox::down-arrow{\n"
"	min-width:35px;\n"
"	image: url(:/img/arrow.png);\n"
"}\n"
"\n"
"QComboBox::down-arrow:on { /* shift the arrow when popup is open */\n"
"     top: 4px;\n"
"}\n"
"\n"
"QComboBox QAbstractItemView {\n"
"     border: 1px solid darkgray;\n"
"     selection-background-color: rgb(218,35,14);\n"
"}"));

        retranslateUi(RJWidget);

        QMetaObject::connectSlotsByName(RJWidget);
    } // setupUi

    void retranslateUi(QWidget *RJWidget)
    {
        RJWidget->setWindowTitle(QApplication::translate("RJWidget", "RJWidget", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class RJWidget: public Ui_RJWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RJWIDGET_H
