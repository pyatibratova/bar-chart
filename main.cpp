#include "chartwidget.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QMainWindow window;
    ChartWidget *widget = new ChartWidget();
    window.setCentralWidget(widget);
    window.resize(900, 600);
    window.show();
    return QApplication::exec();
}

