#include <QApplication>
#include <QStyleFactory>
#include "MainWindow.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    app.setApplicationName("Polynomial");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("University");
    
    app.setStyle(QStyleFactory::create("Fusion"));
    
    MainWindow window;
    window.setWindowTitle("Polinomial - Assignment 3");
    window.setMinimumSize(900, 700);
    window.show();
    
    return app.exec();
}