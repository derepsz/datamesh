#include <algorithm>
#include <type_traits>
#include "fbselect.h"

FbSelect::FbSelect(Mesh& mesh)
   : isInitialized{false}
   , boxSelectActive {false}
   , m_mesh(mesh)
   , m_vbo_highlight(QOpenGLBuffer::VertexBuffer)
   , m_verts_box(24)
{

}

FbSelect::FbSelect(Mesh& mesh, qreal w, qreal h, qreal pixelRatio)
   : boxSelectActive {false}
   , m_mesh(mesh)
   , m_vbo_highlight(QOpenGLBuffer::VertexBuffer)
   , m_verts_box(24)
{
   init(w, h, pixelRatio);
}

FbSelect::~FbSelect()
{
   destroy();
}


void FbSelect::destroy()
{
   if (isInitialized){
      m_verts_selected.clear();
      m_verts_box.clear();
      selected.clear();
      m_vbo_highlight.destroy();
      m_vao_highlight.destroy();
      m_selectShader.removeAllShaders();
      m_highlightShader.removeAllShaders();
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glDeleteFramebuffers(1, &m_fbo);
      glDeleteRenderbuffers(1, &m_rbo);
      glDeleteTextures(1, &m_idtex);
      glDeleteTextures(1, &m_dptex);
   }
}


void FbSelect::init(qreal w, qreal h, qreal pixelRatio, bool onLoadSession)
{
   initializeOpenGLFunctions();
   destroy();
   if (!onLoadSession){
      savedSelections.clear();
   }

   initSelectShader();
   initHighlightShader();
   initHighlightBuffers();

   m_viewHeight = h;
   m_viewWidth = w;
   m_pixelRatio = pixelRatio;

   w *= m_pixelRatio;
   h *= m_pixelRatio;

   GLsizei w_fb = static_cast<GLsizei>(w) + 1;
   GLsizei h_fb = static_cast<GLsizei>(h) + 1;

   glGenFramebuffers(1, &m_fbo);
   glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
   glGenTextures(1, &m_idtex);
   glBindTexture(GL_TEXTURE_2D, m_idtex);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32UI, w_fb, h_fb, 0, GL_RGB_INTEGER, GL_UNSIGNED_INT, NULL);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_idtex, 0);

   glGenTextures(1, &m_dptex);
   glBindTexture(GL_TEXTURE_2D, m_dptex);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_dptex, 0);

   GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
   if (Status != GL_FRAMEBUFFER_COMPLETE) {
      qDebug() << "FB error, status: " << Status;
      exit(1);
   }
   glBindTexture(GL_TEXTURE_2D, 0);
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   isInitialized = true;
}

void FbSelect::onResize(qreal w, qreal h, qreal pixelRatio)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glDeleteFramebuffers(1, &m_fbo);
   glDeleteRenderbuffers(1, &m_rbo);
   glDeleteTextures(1, &m_idtex);
   glDeleteTextures(1, &m_dptex);

   m_viewHeight = h;
   m_viewWidth = w;
   m_pixelRatio = pixelRatio;

   w *= m_pixelRatio;
   h *= m_pixelRatio;

   GLsizei w_fb = static_cast<GLsizei>(w) + 1;
   GLsizei h_fb = static_cast<GLsizei>(h) + 1;

   glGenFramebuffers(1, &m_fbo);
   glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
   glGenTextures(1, &m_idtex);
   glBindTexture(GL_TEXTURE_2D, m_idtex);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32UI, w_fb, h_fb, 0, GL_RGB_INTEGER, GL_UNSIGNED_INT, NULL);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_idtex, 0);

   glGenTextures(1, &m_dptex);
   glBindTexture(GL_TEXTURE_2D, m_dptex);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_dptex, 0);

   GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
   if (Status != GL_FRAMEBUFFER_COMPLETE) {
      qDebug() << "FB error, status: " << Status;
      exit(1);
   }
   glBindTexture(GL_TEXTURE_2D, 0);
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



void FbSelect::onMousePress(QPoint clickCoord)
{
   m_click_x = clickCoord.x() * m_pixelRatio;
   m_click_y = (m_viewHeight - clickCoord.y()) * m_pixelRatio;
   m_boxClick_x = clickCoord.x()/m_viewWidth*2 - 1.0f;
   m_boxClick_y = (m_viewHeight - clickCoord.y())/m_viewHeight*2 - 1.0f;
}

void FbSelect::onMouseRelease(QPoint releaseCoord)
{
   m_release_x = qMax(0.0, qMin(static_cast<qreal>(releaseCoord.x()), m_viewWidth)) * m_pixelRatio;
   m_release_y = qMax(0.0, qMin((m_viewHeight - releaseCoord.y()), m_viewHeight)) * m_pixelRatio;
}

void FbSelect::onMouseMove(QPoint currentCoord)
{
   m_move_x = currentCoord.x()/m_viewWidth*2 - 1.0f;
   m_move_y = (m_viewHeight - currentCoord.y())/m_viewHeight*2 - 1.0f;
   m_verts_box = {
      m_move_x, m_move_y, 0.0f, m_move_x, m_boxClick_y, 0.0f,
      m_move_x, m_boxClick_y, 0.0f, m_boxClick_x, m_boxClick_y, 0.0f,
      m_boxClick_x, m_boxClick_y, 0.0f, m_boxClick_x, m_move_y, 0.0f,
      m_boxClick_x, m_move_y, 0.0f, m_move_x, m_move_y, 0.0f
   };
}

void FbSelect::readSelection(Mesh &mesh, SelectMode mode)
{
   glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
   glClearColor(-1, -1, -1, -1);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   mesh.draw(m_selectShader);

   GLint selectMin_x = qMin(m_click_x, m_release_x);
   GLint selectMin_y = qMin(m_click_y, m_release_y);
   GLint selectMax_x = qMax(m_click_x, m_release_x);
   GLint selectMax_y = qMax(m_click_y, m_release_y);
   GLint read_w = qMax(1, selectMax_x - selectMin_x);
   GLint read_h = qMax(1, selectMax_y - selectMin_y);
   GLint sizePixelData = read_w*read_h*3;
   GLuint *readPixelData = new GLuint[sizePixelData];

   glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
   glReadBuffer(GL_COLOR_ATTACHMENT0);
   glReadPixels(selectMin_x, selectMin_y, read_w, read_h,
                GL_RGB_INTEGER, GL_UNSIGNED_INT, &readPixelData[0]);

   uint nullid = 3212836864;
   QSet<GLuint> newSelection;
   for (int i{0} ; i < sizePixelData ; i += 3)
   {
      if (readPixelData[i] != nullid) {
         newSelection.insert(readPixelData[i]);
      }
   }

   auto addNewFaceIds = [this, newSelection](){
      for (const auto& faceId : newSelection){
         selected.push_back(faceId);
      }
   };

   switch (mode) {
      case SelectMode::NEW: selected.clear(); addNewFaceIds(); break;
      case SelectMode::ADD: addNewFaceIds(); break;
      case SelectMode::REMOVE:
         for (const auto& faceId : qAsConst(newSelection)){
            selected.removeAll(faceId);
         }
         break;
      default: break;
   }

   if (!selected.isEmpty()){
      getSelectedVerts(mesh);
   }

   delete[] readPixelData;
   readPixelData = nullptr;
   glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
   glReadBuffer(GL_NONE);
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void FbSelect::paintSelection(QPoint)
{
   glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
   glClearColor(-1, -1, -1, -1);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   m_mesh.draw(m_selectShader);

   GLuint *readPixelData = new GLuint[3];

   glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
   glReadBuffer(GL_COLOR_ATTACHMENT0);
   glReadPixels(m_move_x, m_move_y, 1, 1,
                GL_RGB_INTEGER, GL_UNSIGNED_INT, &readPixelData[0]);

   uint nullid = 3212836864;
   GLuint newSelection{};
      if (readPixelData[0] != nullid) {
         newSelection = readPixelData[0];
      }
      if (!selected.contains(newSelection)) {
         selected.push_back(newSelection);
      }

   if (!selected.isEmpty()){
      getSelectedVerts(m_mesh);
   }

   delete[] readPixelData;
   readPixelData = nullptr;
   glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
   glReadBuffer(GL_NONE);
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

}

void FbSelect::getSelectedVerts(Mesh& mesh)
{
   m_verts_selected.clear();
   m_verts_selected.reserve(mesh.idxs.size());
   for (const auto& face : selected) {
      m_verts_selected.push_back(mesh.verts[mesh.idxs[face*3 + 0]]);
      m_verts_selected.push_back(mesh.verts[mesh.idxs[face*3 + 1]]);
      m_verts_selected.push_back(mesh.verts[mesh.idxs[face*3 + 2]]);
   }
}

void FbSelect::saveSelection(QString name)
{
   savedSelections.insert(name, selected);
}

void FbSelect::loadSelection(QString name, Mesh& mesh)
{
   selected = savedSelections[name];
   getSelectedVerts(mesh);
}




void FbSelect::setMvp(QMatrix4x4 mvp)
{
   m_selectShader.bind();
   m_selectShader.setUniformValue("mvp", mvp);
   m_highlightShader.bind();
   m_highlightShader.setUniformValue("mvp", mvp);
}


void FbSelect::initSelectShader()
{
   m_selectShader.bind();
   m_selectShader.addShaderFromSourceFile(
            QOpenGLShader::Vertex,
            "://shaders/fbselect.vert");
   m_selectShader.addShaderFromSourceFile(
            QOpenGLShader::Fragment,
            "://shaders/fbselect.frag");
   m_selectShader.link();
}


void FbSelect::initHighlightShader()
{
   m_highlightShader.bind();
   m_highlightShader.addShaderFromSourceFile(
            QOpenGLShader::Vertex,
            "://shaders/fbhighlight.vert");
   m_highlightShader.addShaderFromSourceFile(
            QOpenGLShader::Fragment,
            "://shaders/fbhighlight.frag");
   m_highlightShader.link();
   m_highlightShader.release();
}


void FbSelect::initHighlightBuffers()
{
   m_highlightShader.bind();

   if (m_vao_highlight.isCreated()){
      m_vbo_highlight.destroy();
      m_vao_highlight.destroy();
   }
   m_vao_highlight.create();
   m_vao_highlight.bind();

   m_vbo_highlight.create();
   m_vbo_highlight.bind();
   m_vbo_highlight.setUsagePattern(QOpenGLBuffer::StaticDraw);
   m_highlightShader.enableAttributeArray(3);
   m_highlightShader.setAttributeBuffer(3, GL_FLOAT, 0, 3);

   m_vao_highlight.release();
   m_highlightShader.release();
}

void FbSelect::drawSelected()
{
   glDisable(GL_DEPTH_TEST);
   m_highlightShader.bind();
   m_vao_highlight.bind();
   m_vbo_highlight.allocate(&m_verts_selected[0], int(m_verts_selected.size() * sizeof(vec3)));
   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   glDrawArrays(GL_TRIANGLES, 0, selected.size()*3);
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   m_vao_highlight.release();
   m_highlightShader.release();
   glEnable(GL_DEPTH_TEST);
}

void FbSelect::drawBox()
{
   glDisable(GL_DEPTH_TEST);
   m_highlightShader.bind();
   m_highlightShader.setUniformValue("projectionOff", 1);
   m_vao_highlight.bind();
   m_vbo_highlight.allocate(&m_verts_box[0], sizeof(m_verts_box) * sizeof(float));
   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   glDrawArrays(GL_LINES, 0, 8);
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   m_vao_highlight.release();
   m_highlightShader.setUniformValue("projectionOff", 0);
   m_highlightShader.release();
   glEnable(GL_DEPTH_TEST);
}
