/********************************************************************************
** Form generated from reading UI file 'mainframe.ui'
**
** Created: Sun May 19 19:28:48 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINFRAME_H
#define UI_MAINFRAME_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QTabWidget>
#include <QtGui/QToolButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainFrame
{
public:
    QTabWidget *tabWidget;
    QLabel *title;
    QToolButton *toolButton;

    void setupUi(QWidget *MainFrame)
    {
        if (MainFrame->objectName().isEmpty())
            MainFrame->setObjectName(QString::fromUtf8("MainFrame"));
        MainFrame->resize(560, 362);
        MainFrame->setStyleSheet(QString::fromUtf8("\n"
"QLabel{\n"
"	color: rgb(255, 255, 255);\n"
"}\n"
"\n"
"QCheckBox{\n"
"	color: white;\n"
"}\n"
"\n"
"QTabWidget{\n"
"	border: 2px solid grey;\n"
"	border-radius: 8px;\n"
"}\n"
"\n"
"QTabWidget::pane {\n"
"	background-color: rgba(69, 69, 69, 32);\n"
"	border-width: 1px;\n"
"	border-radius: 8px;\n"
"}\n"
"\n"
"QTabWidget::tab-bar {\n"
"    /*left: 2px;*/\n"
"	alignment: center;\n"
"}\n"
"\n"
"QTabBar::tab {\n"
"	width: 32px;\n"
"	height:32px;\n"
"	background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(59, 59, 59, 192), stop:0.276836 rgba(45, 45, 45, 192), stop:1 rgba(13, 13, 13, 192));\n"
"    border: 2px solid #0c0c0c;\n"
"    /*border-top-color: #393939;*/\n"
"    border-top-left-radius: 2px;\n"
"    border-top-right-radius: 2px;\n"
"    padding: 2px;\n"
"	alignment: center;\n"
"}\n"
"\n"
"QTabBar::tab:selected, QTabBar::tab:hover {\n"
"	border-bottom-color: #dfdfdf;\n"
"}\n"
"\n"
"QTabBar::tab:selected {\n"
"    border-color: #171717;\n"
"    border-bottom-color: #0c0c0c;\n"
"	b"
                        "ackground-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(13, 13, 13, 192), stop:0.254237 rgba(43, 43, 43, 192), stop:0.824859 rgba(21, 21, 21, 192), stop:0.988701 rgba(13, 13, 13, 192));\n"
"}\n"
"\n"
"QTabBar::tab:selected {\n"
"    margin-left: -2px;\n"
"    margin-right: -2px;\n"
"}\n"
"\n"
"QTabBar::tab:first:selected {\n"
"    margin-left: 0;\n"
"}\n"
"\n"
"QTabBar::tab:last:selected {\n"
"    margin-right: 0;\n"
"}\n"
"\n"
"QTabBar::tab:only-one {\n"
"    margin: 0;\n"
"}\n"
"\n"
"\n"
"QListWidget {\n"
"	background-color: rgba(128, 128, 128, 128);\n"
"	border-width: 0px;\n"
"}\n"
"\n"
"QGroupBox {\n"
"     background-color: rgba(80, 80, 80, 128);\n"
"     border: 2px solid gray;\n"
"     border-radius: 5px;\n"
"     margin-top: 1ex; /* leave space at the top for the title */\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"	 color: #FFFFFF;\n"
"     border: 2px solid gray;\n"
"     border-radius: 4px;\n"
"	 font:normal 14px ;\n"
"     subcontrol-origin: margin;\n"
"     subcontrol-position: "
                        "top center; /* position at the top center */\n"
"     padding: 0 3px;\n"
"     background-color: #2b579a;\n"
"}\n"
"/*\n"
"QWidget#contents{\n"
"background-color: rgba(128,128,128,128);\n"
"	border-width: 0px;\n"
"}\n"
"QSrollArea{\n"
"background-color: rgba(128,128,128,128);\n"
"}\n"
"*/\n"
"QScrollBar:vertical {\n"
"      border: 2px solid grey;\n"
"	  background-color: rgba(128, 128, 128, 192);\n"
"      width: 15px;\n"
"      margin: 22px 0 22px 0;\n"
"  }\n"
"  QScrollBar::handle:vertical {\n"
"      background:qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(128, 128, 128, 192), stop:0.2 rgba(50, 50, 50, 192), stop:0.8 rgba(50, 50, 50, 192), stop:1 rgba(128, 128, 128, 192));\n"
"      min-height: 20px;\n"
"  }\n"
"  QScrollBar::add-line:vertical {\n"
"      border: 2px solid grey;\n"
"      background: rgba(128, 128, 128, 192);\n"
"      height: 20px;\n"
"      subcontrol-position: bottom;\n"
"      subcontrol-origin: margin;\n"
"  }\n"
"\n"
"  QScrollBar::sub-line:vertical {\n"
"      bor"
                        "der: 2px solid grey;\n"
"      background: rgba(128, 128, 128, 192);\n"
"      height: 20px;\n"
"      subcontrol-position: top;\n"
"      subcontrol-origin: margin;\n"
"  }\n"
"  QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical {\n"
"      border: 0px;\n"
"      width: 11px;\n"
"      height: 20px;\n"
"	background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(128, 128, 128, 192), stop:0.2 rgba(50, 50, 50, 192), stop:0.8 rgba(50, 50, 50, 192), stop:1 rgba(128, 128, 128, 192));\n"
"  }\n"
"\n"
"  QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {\n"
"      background: none;\n"
"}\n"
"\n"
"QPushButton {\n"
"     border: 1px solid #8f8f91;\n"
"     border-radius: 6px;\n"
"     background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                       stop: 0 #f6f7fa, stop: 1 #dadbde);\n"
" }\n"
"\n"
" QPushButton:pressed {\n"
"	 padding-top: 2px;\n"
"     background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"             "
                        "                          stop: 0 #dadbde, stop: 1 #f6f7fa);\n"
" }\n"
"\n"
" QPushButton:flat {\n"
"     border: none; /* no border for a flat push button */\n"
" }\n"
"\n"
" QPushButton:default {\n"
"     border-color: navy; /* make the default button prominent */\n"
" }\n"
""));
        tabWidget = new QTabWidget(MainFrame);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(4, 35, 552, 321));
        tabWidget->setStyleSheet(QString::fromUtf8(""));
        tabWidget->setTabPosition(QTabWidget::South);
        tabWidget->setIconSize(QSize(32, 32));
        title = new QLabel(MainFrame);
        title->setObjectName(QString::fromUtf8("title"));
        title->setGeometry(QRect(0, 0, 560, 28));
        title->setStyleSheet(QString::fromUtf8("QLabel#title{\n"
"	font-family: \"Arial\";\n"
"	color: rgb(0, 0, 0);\n"
"	border-top-left-radius:5px;\n"
"	border-top-right-radius:5px;\n"
"	\n"
"	background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(254, 254, 254, 255), stop:0.903955 rgba(232, 232, 232, 255), stop:1 rgba(128, 128, 128, 255));\n"
"}"));
        title->setTextFormat(Qt::AutoText);
        title->setPixmap(QPixmap(QString::fromUtf8(":/icon/images/title.png")));
        title->setScaledContents(false);
        title->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        title->setWordWrap(false);
        title->setTextInteractionFlags(Qt::NoTextInteraction);
        toolButton = new QToolButton(MainFrame);
        toolButton->setObjectName(QString::fromUtf8("toolButton"));
        toolButton->setGeometry(QRect(534, 3, 20, 20));
        toolButton->setStyleSheet(QString::fromUtf8("QToolButton{\n"
"	border:0px;\n"
"	background:None;\n"
"	background-image: url(:/images/hi.png);\n"
"}\n"
"QToolButton:pressed{\n"
"	background-image: url(:/images_/hi.png);\n"
"}\n"
"QToolButton:hover{\n"
"	background-image: url(:/images/_hi.png);\n"
"}"));

        retranslateUi(MainFrame);

        tabWidget->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(MainFrame);
    } // setupUi

    void retranslateUi(QWidget *MainFrame)
    {
        MainFrame->setWindowTitle(QApplication::translate("MainFrame", "MainFrame", 0, QApplication::UnicodeUTF8));
        title->setText(QString());
        toolButton->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainFrame: public Ui_MainFrame {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINFRAME_H
