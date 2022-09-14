#ifndef GLVIEW_H
#define GLVIEW_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_5_Core>
#include <QPointer>
#include <QList>
#include <QMouseEvent>
#include <QOpenGLDebugLogger>
#include <QPainter>
#include <QTime>
#include <QTimer>

#include "camera.h"
#include "materialhandler.h"
#include "shaderwatcher.h"
#include "shaderhandler.h"
#include "mesh.h"
#include "fbselect.h"
#include "datalayer.h"
#include "sessionmanager.h"

class GLView : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
      Q_OBJECT

   public:
      GLView(QWidget *parent = nullptr);
      ~GLView();
      void saveSession();
      void loadSession();
      void getGlError();

      SessionManager       sessionManager;

   protected:
      void initializeGL() override;
      void resizeGL(int w, int h) override;
      void paintGL() override;

      void mousePressEvent(QMouseEvent* event) override;
      void mouseReleaseEvent(QMouseEvent* event) override;
      void mouseMoveEvent(QMouseEvent* event) override;
      void wheelEvent(QWheelEvent *event) override;

   public slots:
      void rxVertShaderString(QString);
      void rxFragShaderString(QString);
      void rxCompShaderString(QString);
      void rxNewSession(QString);
      void rxTriggerShaderWatcher();
      void rxTriggerCompute();

      void rxSaveSelection(QString);
      void rxLoadSelection(QString);
      void rxBuildConnectivity();

      void rxParam0Val(double);
      void rxParam1Val(double);
      void rxParam2Val(double);
      void rxParam3Val(double);
      void rxParam4Val(double);
      void rxParam5Val(double);
      void rxParam0Toggle(bool);
      void rxParam1Toggle(bool);
      void rxParam2Toggle(bool);
      void rxParam3Toggle(bool);
      void rxParam4Toggle(bool);
      void rxParam5Toggle(bool);
      void rxUpdateParams();

   signals:
      void txStatusMessage(QString);
      void txShaderProgramReady();
      void txDebugMessage(QString);
      void txSavedSelectionsOnLoad(QString);
      void txNewSession();
      void txSessionLoaded();

   private:
      static constexpr int COMPUTE_FREQ = 1000;
      static constexpr int DBG_SSBO_LOC{7};


      bool loadMesh(QString);
      void updateUniforms();

      QVector<float>       paramVal{};
      QVector<bool>        paramActive{};

      ShaderHandler        shaderProgram{};
      Mesh                 mesh{};
      MaterialHandler      material{};
      DataLayer            datalayer{};
      Camera               camera{};
      QMatrix4x4           mvp_matrix{};

      FbSelect             fbSelect{mesh};
      QOpenGLDebugLogger   debugLogger{};
      QString              lastDebugMessage{};
      bool                 debugLoggerActive{true};
      int                  selected_id{};
      bool                 solo_selected{false};
      QPoint               mouse_last_pos{};
      bool                 box_select_active{false};

      ShaderWatcher        shaderWatcher{};
      QTime                clock{};
      int                  timeLast{0};

};

#endif // GLVIEW_H
