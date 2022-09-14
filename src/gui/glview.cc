#include <QMatrix4x4>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QStringTokenizer>

#include "glview.h"

GLView::GLView(QWidget *parent)
   : QOpenGLWidget(parent)
   , sessionManager(mesh, datalayer, fbSelect)
   , paramVal(DataLayer::NUM_PARAM, 0.0f)
   , paramActive(DataLayer::NUM_PARAM, false)
//   , shaderProgram()
//   , mesh()
//   , material()
//   , datalayer()
//   , camera()
//   , fbSelect(mesh)
//   , debugLogger()
//   , debugLoggerActive(true)
//   , selected_id()
//   , solo_selected(false)
//   , box_select_active(false)
//   , shaderWatcher()
//   , clock()
//   , timeLast(0)
{
   QObject::connect(
            &mesh, SIGNAL(txMeshStatusMessage(QString)),
            this, SIGNAL(txStatusMessage(QString)));
   QObject::connect(
            &sessionManager, SIGNAL(txSessionManagerStatus(QString)),
            this, SIGNAL(txStatusMessage(QString)));
   QObject::connect(
            &shaderWatcher, SIGNAL(txDebugMessage(QString)),
            this, SIGNAL(txDebugMessage(QString)));
}


GLView::~GLView()
{
   makeCurrent();
   mesh.destroy();
   fbSelect.destroy();
}


void GLView::initializeGL()
{
   initializeOpenGLFunctions();
   shaderProgram.init();
   glEnable(GL_DEPTH_TEST);
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

   // TODO: LOAD DEFAULT SESSION
   emit txShaderProgramReady();
   emit txStatusMessage("HELO EARL!");

   QOpenGLContext *ctx = QOpenGLContext::currentContext();
   auto hasDebug = ctx->hasExtension(QByteArrayLiteral("GL_KHR_debug"));
   ctx = nullptr;

   if (!hasDebug) {
      emit txStatusMessage("<<WARNING>> OPENGL DEBUG CONTEXT UNAVAILABLE");
   } else {
      glEnable(GL_DEBUG_OUTPUT);
      debugLogger.initialize();
   }
   shaderWatcher.initDebugSSBO();
   material.init();

}


void GLView::resizeGL(int w, int h)
{
   camera.setAspectRatio(w, h);
   if (mesh.hasMesh()) {
      fbSelect.onResize(w, h, devicePixelRatio());
   }
   update();
}


void GLView::paintGL()
{
   if (!datalayer.isBusy()){
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_CULL_FACE);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      if (mesh.hasMesh()) {
         updateUniforms();
         for (int i {0}; i < material.NUM_TEX; ++i){
            material.texture[i]->bind(i);
         }
         mesh.draw(shaderProgram.shader);
         if(!fbSelect.selected.isEmpty()){
            fbSelect.drawSelected();
         }
         if (box_select_active) {
            fbSelect.drawBox();
         }
         update();
      }
   }
}

void GLView::mousePressEvent(QMouseEvent* event)
{
   if (event->button() == Qt::LeftButton) {
      makeCurrent();
      fbSelect.onMousePress(event->pos());
   }
   if (event->button() == Qt::MiddleButton) {
      mouse_last_pos = event->pos();
   }
   update();
}

void GLView::mouseReleaseEvent(QMouseEvent* event)
{
   if (event->button() == Qt::LeftButton) {
      makeCurrent();
      fbSelect.onMouseRelease(event->pos());
      SelectMode mode = SelectMode::NEW;

      if (event->modifiers() & Qt::ShiftModifier) {
         mode = SelectMode::ADD;
      }
      if (event->modifiers() & Qt::ControlModifier) {
         mode = SelectMode::REMOVE;
      }
      if (fbSelect.isInitialized) {
         fbSelect.readSelection(mesh, mode);
      }
      box_select_active = false;
      if (fbSelect.selected.size() == 1) {
         solo_selected = true;
         selected_id = fbSelect.selected[0];
         emit txDebugMessage("SELECTED FID " + QString::number(fbSelect.selected[0]) + " ");
      } else {
         solo_selected = false;
      }
   }
   update();
}

void GLView::mouseMoveEvent(QMouseEvent *event)
{
   if (event->buttons() & Qt::LeftButton) {
      makeCurrent();
      fbSelect.onMouseMove(event->pos());
// TODO: PAINT
//      if (event->modifiers() & Qt::ALT) {
//         makeCurrent();
//         fbSelect.paintSelection(event->pos());
//      } else {
         box_select_active = true;
//      }
      update();
   }
   if (event->buttons() & Qt::MiddleButton) {
      float dx = static_cast<float>(event->pos().x() - mouse_last_pos.x());
      float dy = static_cast<float>(event->pos().y() - mouse_last_pos.y());
      if (event->modifiers() & Qt::ShiftModifier)
      {
         camera.translate(dx, dy);
      } else{
         camera.rotate(dx, dy);
      }
      mouse_last_pos = event->pos();
      update();
   }
}

void GLView::wheelEvent(QWheelEvent* event)
{
   camera.setRadius(event->angleDelta().y());
   update();
}

bool GLView::loadMesh(QString path) {
   if (!mesh.getNumVertsToLoad(path)) {
       return false;
   }
   if (!mesh.loadMesh(path)) {
      return false;
   } else {
      return true;
   }
}

void GLView::getGlError()
{
   makeCurrent();
   for (const QOpenGLDebugMessage &message : debugLogger.loggedMessages()) {
      if (message.message() != lastDebugMessage){
         emit txDebugMessage(message.message());
         lastDebugMessage = message.message();
      }
   }
   update();
}


void GLView::rxFragShaderString(QString frag_shader)
{
   sessionManager.fragShader = frag_shader;
   shaderProgram.setFragShader(frag_shader);
   emit txDebugMessage(shaderProgram.getLog());
   update();
}

void GLView::rxVertShaderString(QString vert_shader)
{
   sessionManager.vertShader = vert_shader;
   shaderProgram.setVertShader(vert_shader);
   emit txDebugMessage(shaderProgram.getLog());
   update();
}

void GLView::rxCompShaderString(QString compute_shader)
{
   sessionManager.compShader = compute_shader;
   shaderProgram.setCompShader(compute_shader);
   emit txDebugMessage(shaderProgram.getLog());
   update();
}

void GLView::rxNewSession(QString file_path)
{
   makeCurrent();
   if (!loadMesh(file_path)) {
      emit txStatusMessage("<< WARNING : NO MESH LOADED >>  File read failed or aborted by user");
   } else {
      emit txNewSession();
      makeCurrent();
      mesh.initBuffers(shaderProgram.shader);
      fbSelect.init(QWidget::width(), QWidget::height(), devicePixelRatio());
      datalayer.initShaderStorage(mesh.num_facets);
      shaderWatcher.initDebugSSBO();
   }
   update();
}

void GLView::rxTriggerShaderWatcher()
{
   if (mesh.hasMesh()){
      makeCurrent();
      shaderWatcher.printDebugSSBO();
      update();
   }
}


void GLView::rxTriggerCompute()
{
   makeCurrent();
   shaderProgram.compute.bind();
   glDispatchCompute(8, 8, 8);  // TODO: LINK SIZES TO DEFAULT SHADER
   glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
   update();
}
// TODO: ADD COMPUTE PULSE SWITCH
//   auto time_now = time.currentTime().msecsSinceStartOfDay();
//   if ((time_now - m_timeLast) > COMPUTE_FREQ) {
//      m_timeLast = time_now;
//      shaderProgram.dispatch();
//   }

void GLView::saveSession()
{
   QString fileName = QFileDialog::getSaveFileName(
                         this,
                         tr("Save Session Data"), "",
                         tr("Session Data File(*.dat);"));
   if (fileName.isEmpty()) {
      return;
   } else {
      sessionManager.saveSession(fileName);
   }
}

void GLView::loadSession()
{
   QString fileName = QFileDialog::getOpenFileName(
                         this,
                         tr("Load Session Data"), "",
                         tr("Session Data File(*.dat);"));
   if (fileName.isEmpty()) {
      return;
   } else {
      makeCurrent();
      sessionManager.loadSession(fileName);
      mesh.initBuffers(shaderProgram.shader);
      fbSelect.init(QWidget::width(), QWidget::height(), devicePixelRatio(), true);
      datalayer.onLoadSession();
      // TODO: POKE_YOKE BELOW
      // first call resets UI and pushes shader text
      // second adds items to listwidget
      emit txSessionLoaded();
      for (auto pair {fbSelect.savedSelections.begin()};
           pair != fbSelect.savedSelections.end(); ++pair) {
         emit txSavedSelectionsOnLoad(pair.key());
      }
      update();

   }
}

void GLView::rxSaveSelection(QString name)
{
   fbSelect.saveSelection(name);
}

void GLView::rxLoadSelection(QString name)
{
   fbSelect.loadSelection(name, mesh);
   update();
}

void GLView::rxBuildConnectivity()
{
   if (mesh.hasMesh()){
   makeCurrent();
   datalayer.initNeighbors(mesh);
}
}

void GLView::updateUniforms()
{
   mvp_matrix = camera.projection_matrix * camera.view_matrix * mesh.model_matrix;

   shaderProgram.shader.bind();
   shaderProgram.shader.setUniformValue("mvp", mvp_matrix);
   shaderProgram.shader.setUniformValue("camPos", camera.getPosition());
   shaderProgram.shader.setUniformValue("selectedID", selected_id);
   shaderProgram.shader.setUniformValue("soloSelected", solo_selected);
   shaderProgram.shader.setUniformValue("time", clock.currentTime().msecsSinceStartOfDay());
   shaderProgram.shader.setUniformValue("albedotex", 0);
   shaderProgram.shader.setUniformValue("aotex", 1);
   shaderProgram.shader.setUniformValue("metallictex", 2);
   shaderProgram.shader.setUniformValue("normaltex", 3);
   shaderProgram.shader.setUniformValue("roughnesstex", 4);
   fbSelect.setMvp(mvp_matrix);

   shaderProgram.compute.bind();
   shaderProgram.compute.setUniformValue(1, clock.currentTime().msecsSinceStartOfDay());

}

void GLView::rxParam0Val(double val) { paramVal[0] = val; }
void GLView::rxParam1Val(double val) { paramVal[1] = val; }
void GLView::rxParam2Val(double val) { paramVal[2] = val; }
void GLView::rxParam3Val(double val) { paramVal[3] = val; }
void GLView::rxParam4Val(double val) { paramVal[4] = val; }
void GLView::rxParam5Val(double val) { paramVal[5] = val; }
void GLView::rxParam0Toggle(bool state) { paramActive[0] = state; }
void GLView::rxParam1Toggle(bool state) { paramActive[1] = state; }
void GLView::rxParam2Toggle(bool state) { paramActive[2] = state; }
void GLView::rxParam3Toggle(bool state) { paramActive[3] = state; }
void GLView::rxParam4Toggle(bool state) { paramActive[4] = state; }
void GLView::rxParam5Toggle(bool state) { paramActive[5] = state; }

void GLView::rxUpdateParams()
{
   for (int i{0}; i < DataLayer::NUM_PARAM; ++i) {
      if (paramActive[i]) {
      datalayer.updateParameters(fbSelect.selected, i, paramVal[i]); }
   }
   update();
}
