#ifndef SHADERHANDLER_H
#define SHADERHANDLER_H

#include <QObject>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLShaderProgram>
#include <QString>

class ShaderHandler : public QObject, protected QOpenGLFunctions_4_5_Core
{
      Q_OBJECT
   public:
      explicit ShaderHandler(QObject *parent = nullptr);

      void init();
      void initSelectShader();
      void initComputeShader();
      void setFragShader(QString);
      void setVertShader(QString);
      void setCompShader(QString);
      QString getLog();

      QOpenGLShaderProgram shader;
      QOpenGLShaderProgram selectShader;
      QOpenGLShaderProgram compute;

      QString              vertShaderText;
      QString              fragShaderText;
      QString              compShaderText;

   public slots:
      void dispatch();

   signals:
      void txShaderLog(QString);

   private:
      void reloadDrawShaders();
      void reloadComputeShader();


      QString              shaderLog;

};

#endif // SHADERHANDLER_H
