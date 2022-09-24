#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MyRaytracer.h"

class MyRaytracer : public QMainWindow
{
    Q_OBJECT

public:
    MyRaytracer(QWidget *parent = nullptr);
    ~MyRaytracer();

private:
    Ui::MyRaytracerClass ui;
};
