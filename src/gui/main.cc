#include "mainwindow.h"

#include <QApplication>
#include <QSurfaceFormat>
#include <QOpenGLContext>

int main(int argc, char *argv[])
{
   QApplication a(argc, argv);

   QSurfaceFormat format;
   format.setVersion(4, 5);
   format.setDepthBufferSize(32);
   format.setStencilBufferSize(8);
   format.setProfile(QSurfaceFormat::CoreProfile);
   format.setOption(QSurfaceFormat::DebugContext);
   QSurfaceFormat::setDefaultFormat(format);

   MainWindow w;
   w.show();
   return a.exec();
}
