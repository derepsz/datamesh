#ifndef MATERIALHANDLER_H
#define MATERIALHANDLER_H

#include <QObject>
#include <QOpenGLTexture>

class MaterialHandler : public QObject
{
      Q_OBJECT
   public:
      explicit MaterialHandler(QObject *parent = nullptr);
      ~MaterialHandler();
      static const int NUM_TEX {5};
      QOpenGLTexture *texture[NUM_TEX] = {nullptr, nullptr, nullptr, nullptr, nullptr};


      void init();

   signals:

   private:

};

#endif // MATERIALHANDLER_H
