#include "camerawindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    cameraWindow w;
    w.show();

    return a.exec();
}
