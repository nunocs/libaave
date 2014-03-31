/********************************************************************************
** Form generated from reading UI file 'Demo1QT.ui'
**
** Created: Thu Mar 13 17:41:02 2014
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEMO1QT_H
#define UI_DEMO1QT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QWidget>
#include "QVTKWidget.h"

QT_BEGIN_NAMESPACE

class Ui_RenderWindowsQt
{
public:
    QAction *actionOpenFile;
    QAction *actionExit;
    QAction *actionPrint;
    QAction *actionHelp;
    QAction *actionSave;
    QWidget *centralwidget;
    QVTKWidget *qvtkWidget;

    void setupUi(QMainWindow *RenderWindowsQt)
    {
        if (RenderWindowsQt->objectName().isEmpty())
            RenderWindowsQt->setObjectName(QString::fromUtf8("RenderWindowsQt"));
        RenderWindowsQt->resize(1092, 646);
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(RenderWindowsQt->sizePolicy().hasHeightForWidth());
        RenderWindowsQt->setSizePolicy(sizePolicy);
        RenderWindowsQt->setMaximumSize(QSize(1092, 646));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Icons/help.png"), QSize(), QIcon::Normal, QIcon::Off);
        RenderWindowsQt->setWindowIcon(icon);
        RenderWindowsQt->setIconSize(QSize(22, 22));
        actionOpenFile = new QAction(RenderWindowsQt);
        actionOpenFile->setObjectName(QString::fromUtf8("actionOpenFile"));
        actionOpenFile->setEnabled(true);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Icons/fileopen.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpenFile->setIcon(icon1);
        actionExit = new QAction(RenderWindowsQt);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(""), QSize(), QIcon::Normal, QIcon::Off);
        actionExit->setIcon(icon2);
        actionPrint = new QAction(RenderWindowsQt);
        actionPrint->setObjectName(QString::fromUtf8("actionPrint"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/Icons/print.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionPrint->setIcon(icon3);
        actionHelp = new QAction(RenderWindowsQt);
        actionHelp->setObjectName(QString::fromUtf8("actionHelp"));
        actionHelp->setIcon(icon);
        actionSave = new QAction(RenderWindowsQt);
        actionSave->setObjectName(QString::fromUtf8("actionSave"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/Icons/filesave.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave->setIcon(icon4);
        centralwidget = new QWidget(RenderWindowsQt);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy1);
        centralwidget->setFocusPolicy(Qt::StrongFocus);
        qvtkWidget = new QVTKWidget(centralwidget);
        qvtkWidget->setObjectName(QString::fromUtf8("qvtkWidget"));
        qvtkWidget->setGeometry(QRect(0, 10, 900, 631));
        sizePolicy1.setHeightForWidth(qvtkWidget->sizePolicy().hasHeightForWidth());
        qvtkWidget->setSizePolicy(sizePolicy1);
        qvtkWidget->setMinimumSize(QSize(200, 100));
        qvtkWidget->setFocusPolicy(Qt::StrongFocus);
        qvtkWidget->setAutoFillBackground(true);
        RenderWindowsQt->setCentralWidget(centralwidget);

        retranslateUi(RenderWindowsQt);

        QMetaObject::connectSlotsByName(RenderWindowsQt);
    } // setupUi

    void retranslateUi(QMainWindow *RenderWindowsQt)
    {
        RenderWindowsQt->setWindowTitle(QApplication::translate("RenderWindowsQt", "Shoebox Demo", 0, QApplication::UnicodeUTF8));
        actionOpenFile->setText(QApplication::translate("RenderWindowsQt", "Open File...", 0, QApplication::UnicodeUTF8));
        actionExit->setText(QApplication::translate("RenderWindowsQt", "Exit", 0, QApplication::UnicodeUTF8));
        actionPrint->setText(QApplication::translate("RenderWindowsQt", "Print", 0, QApplication::UnicodeUTF8));
        actionHelp->setText(QApplication::translate("RenderWindowsQt", "Help", 0, QApplication::UnicodeUTF8));
        actionSave->setText(QApplication::translate("RenderWindowsQt", "Save", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class RenderWindowsQt: public Ui_RenderWindowsQt {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEMO1QT_H
