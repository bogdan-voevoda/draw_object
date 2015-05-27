#include <QApplication>
#include <QLabel>

#ifndef QT_NO_OPENGL
#include "mainwindow.h"
#endif

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("template");
    app.setApplicationVersion("0.1");
#ifndef QT_NO_OPENGL
    MainWindow widget;
    widget.show();
#else
    QLabel note("OpenGL Support required");
    note.show();
#endif
    return app.exec();
}
