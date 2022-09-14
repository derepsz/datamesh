#ifndef MESH_H
#define MESH_H

#include <QObject>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QVector>
#include <QProgressDialog>

#include "global.h"

class Mesh : public QObject, protected QOpenGLFunctions
{
      Q_OBJECT
   public:
      explicit Mesh(QObject *parent = nullptr);
      void destroy();

      bool getNumVertsToLoad(QString);
      bool loadMesh(QString file_path);
      void initBuffers(QOpenGLShaderProgram& shader);
      void draw(QOpenGLShaderProgram& shader);
      void initQuad(QOpenGLShaderProgram& shader);
      bool hasMesh();

      QVector<vec3>              verts;
      QVector<vec3>              norms;
      QVector<vec2>              texcoords;
      QVector<GLuint>            idxs;
      GLuint                     num_facets;
      QMatrix4x4                 model_matrix;

   signals :
      void txMeshStatusMessage(QString);

   private:
      QOpenGLVertexArrayObject   vao;
      QOpenGLBuffer              vbo_pos;
      QOpenGLBuffer              vbo_norm;
      QOpenGLBuffer              vbo_uv;
      QOpenGLBuffer              ebo;

      bool                       has_texcoords;
      bool                       has_norms;
      int                        num_verts_to_load;
      int                        num_lines_to_read;
      bool                       verts_counted;




};

#endif // MESH_H
