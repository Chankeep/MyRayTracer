/********************************************************************************
** Form generated from reading UI file 'MyRaytracer.ui'
**
** Created by: Qt User Interface Compiler version 6.3.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MYRAYTRACER_H
#define UI_MYRAYTRACER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MyRaytracerClass
{
public:
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MyRaytracerClass)
    {
        if (MyRaytracerClass->objectName().isEmpty())
            MyRaytracerClass->setObjectName(QString::fromUtf8("MyRaytracerClass"));
        MyRaytracerClass->resize(669, 426);
        centralWidget = new QWidget(MyRaytracerClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        MyRaytracerClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MyRaytracerClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 669, 19));
        MyRaytracerClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MyRaytracerClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MyRaytracerClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MyRaytracerClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MyRaytracerClass->setStatusBar(statusBar);

        retranslateUi(MyRaytracerClass);

        QMetaObject::connectSlotsByName(MyRaytracerClass);
    } // setupUi

    void retranslateUi(QMainWindow *MyRaytracerClass)
    {
        MyRaytracerClass->setWindowTitle(QCoreApplication::translate("MyRaytracerClass", "MyRaytracer", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MyRaytracerClass: public Ui_MyRaytracerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MYRAYTRACER_H
