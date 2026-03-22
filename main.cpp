#include "CameraWidget.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    CameraWidget window;
    window.show();
    return app.exec();
}
