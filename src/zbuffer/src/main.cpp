/**
 *  @file
 *  @author answeror <answeror@gmail.com>
 *  @date 2012-03-16
 *  
 *  @section DESCRIPTION
 *  
 *  
 */

//#include <omp.h>

#include <iostream>

#include <QApplication>

#include "mainwindow.hpp"

int main(int argc, char **argv)
{
    // Get the number of processors in this system
    //int iCPU = omp_get_num_procs();

    // Now set the number of threads
    //omp_set_num_threads(iCPU);

    //std::cout << iCPU << " " << omp_get_thread_num() << std::endl;
    QApplication app(argc, argv);
    cg::mainwindow win;
    win.show();
    try {
        app.exec();
    } catch (...) {
        std::cerr << "Exception raised." << std::endl;
    }
}