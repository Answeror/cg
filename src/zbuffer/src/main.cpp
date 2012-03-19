/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-16
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

#include <iostream>

#include <QApplication>

#include "mainwindow.hpp"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    cg::mainwindow win;
    win.show();
    try {
        app.exec();
    } catch (...) {
        std::cerr << "Exception raised." << std::endl;
    }
}