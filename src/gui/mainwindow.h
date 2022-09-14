#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QListWidgetItem>
#include <QMainWindow>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
      Q_OBJECT

   public:
      MainWindow(QWidget *parent = nullptr);
      ~MainWindow();

   protected:

   public slots:
      void rxShaderProgramReady();
      void rxStatusMessage(QString msg);
      void onPressUpdateFragShader();
      void onPressUpdateVertShader();
      void onPressUpdateCompShader();
      void rxSavedSelectionsOnLoad(QString);
      void rxSelectionListDoubleClicked(QListWidgetItem*);
      void rxSessionLoaded();
      void showCookbook();


   signals:
      void txFragShaderString(QString);
      void txVertShaderString(QString);
      void txCompShaderString(QString);
      void txNewSession(QString);
      void txSaveSelection(QString);
      void txLoadSelection(QString);

   private slots:
      void on_saveSelection_triggered();
      void on_loadSelection_triggered();
      void on_loadFragShader_triggered();
      void on_loadVertShader_triggered();
      void on_newSession_triggered();
      void on_saveSession_triggered();
      void on_loadSession_triggered();
      void on_buildConnectivity_triggered();
      void on_shaderCookbook_triggered();
      void on_shaderWatch_triggered();
      void on_dispatchCompute_triggered();
      void on_GetGlError_triggered();
      void on_clearDebug_triggered();
      void resetUI();

      void on_displayAbout_triggered();

      void on_displayBindingsHelp_triggered();

      void on_readMe_triggered();

   private:
      Ui::MainWindow *ui;
      QString getDefaultVertString();
      QString getDefaultFragString();
      QString getDefaultCompString();

      const QString           defaultVert;
      const QString           defaultFrag;
      const QString           defaultComp;

      static constexpr int    STATUS_MSG_TIMEOUT {10000};

};
#endif // MAINWINDOW_H
