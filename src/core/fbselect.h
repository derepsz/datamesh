#ifndef FBSELECT_H
#define FBSELECT_H

#include <QObject>
#include <QOpenGLFunctions>
#include <QPoint>
#include <QOpenGLShaderProgram>
#include <QPointer>
#include <QPainter>

#include "mesh.h"
#include "global.h"

class FbSelect : public QObject, protected QOpenGLFunctions
{
      Q_OBJECT
   public:
      FbSelect(Mesh&);
      FbSelect(Mesh&, qreal w, qreal h, qreal PixelRatio);
      ~FbSelect();

      void init(qreal w, qreal h, qreal pixelRatio, bool onLoadSession = false);
      void onResize(qreal w, qreal h, qreal pixelRatio);
      void destroy();
      void onMousePress(QPoint);
      void onMouseRelease(QPoint);
      void onMouseMove(QPoint);
      void readSelection(Mesh&, SelectMode mode);
      void getSelectedVerts(Mesh&);
      void saveSelection(QString);
      void loadSelection(QString, Mesh& mesh);
      void paintSelection(QPoint);

      void initSelectShader();
      void initHighlightShader();
      void initHighlightBuffers();
      void drawSelected();
      void drawBox();
      void setMvp(QMatrix4x4);

      QVector<uint>                 selected;
      QHash<QString, QVector<uint>> savedSelections;
      bool                          isInitialized;

   private:
      bool boxSelectActive;
      Mesh&                    m_mesh;
      GLuint                   m_fbo;
      GLuint                   m_rbo;
      GLuint                   m_idtex;
      GLuint                   m_dptex;
      QOpenGLShaderProgram     m_selectShader;
      QOpenGLShaderProgram     m_highlightShader;
      QOpenGLVertexArrayObject m_vao_highlight;
      QOpenGLBuffer            m_vbo_highlight;
      QVector<vec3>            m_verts_selected;
      QVector<float>           m_verts_box;
      float                    m_click_x;
      float                    m_click_y;
      float                    m_move_x;
      float                    m_move_y;
      float                    m_release_x;
      float                    m_release_y;
      float                    m_boxClick_x;
      float                    m_boxClick_y;
      qreal                    m_viewHeight;
      qreal                    m_viewWidth;
      qreal                    m_pixelRatio;

};

#endif // FBSELECT_H
