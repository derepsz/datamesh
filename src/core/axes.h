#ifndef AXES_H
#define AXES_H

#include <QObject>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QVector>
#include <QOpenGLContext>
#include <QMatrix4x4>

class axes : public QObject, protected QOpenGLFunctions

{
      Q_OBJECT
   public:
      explicit axes(QObject *parent = nullptr);
      ~axes();

      void init();
      void draw();
      void rotate(float dx, float dy);
      QMatrix4x4 getTransform();
      QMatrix4x4 rotation_matrix;


      QOpenGLShaderProgram shader;

   signals:

   private:
      QOpenGLVertexArrayObject vao;
      QOpenGLBuffer vbo;

      static constexpr GLfloat vert_pos_cols[] = {
         0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
         1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
         0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
         0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
         0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
         0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
      };

};

#endif // AXES_H
