#include "axes.h"

axes::axes(QObject *parent)
   : QObject{parent}
{
   rotation_matrix.setToIdentity();
}

axes::~axes()
{
   vbo.destroy();
   vao.destroy();
}

void axes::init()
{
   initializeOpenGLFunctions();

   shader.addShaderFromSourceCode(QOpenGLShader::Vertex,
       "#version 450 core\n"
       "layout (location = 0) in vec3 aPos;\n"
       "layout (location = 1) in vec3 aCol;\n"
       "uniform mat4 model;\n"
       "uniform mat4 view;\n"
       "uniform mat4 projection;\n"
       "uniform mat4 transform_axis;\n"
       "out vec3 vColor;\n"
       "void main()\n"
       "{\n"
       "   gl_Position = transform_axis * vec4(aPos, 1.0f);\n"
       "   vColor = aCol;\n"
       "}");

   shader.addShaderFromSourceCode(QOpenGLShader::Fragment,
       "#version 450 core\n"
       "in vec3 vColor;\n"
       "out vec4 color;\n"
       "void main()\n"
       "{\n"
       "  color = vec4(1.0f);\n"
       "}");

   shader.link();
   shader.bind();

   vao.create();
   vao.bind();
   vbo.create();
   vbo.bind();
   vbo.allocate( &vert_pos_cols[0], int(sizeof(vert_pos_cols)) );

   shader.enableAttributeArray(0);
   shader.enableAttributeArray(1);
   shader.setAttributeBuffer(0, GL_FLOAT, 0, 3, 3*sizeof(float));
   shader.setAttributeBuffer(1, GL_FLOAT, 3, 3, 3*sizeof(float));
}

void axes::draw()
{
   shader.bind();
   vao.bind();
   vbo.bind();
   glDrawArrays(GL_LINES, 0, 12);
   vbo.release();
   vao.release();
   shader.release();
}

void axes::rotate(float dx, float dy)
{
   rotation_matrix.rotate(dx, 1.0f, 0.0f);
   rotation_matrix.rotate(dy, 0.0f, 1.0f);
}

QMatrix4x4 axes::getTransform()
{
   QMatrix4x4 transform;
   transform.translate(-0.8f, -0.8f);
   transform.scale(0.2f);
   transform =  transform * rotation_matrix;
   return transform;
}
