#include <QFileDialog>
#include <QGroupBox>
#include <QInputDialog>
#include <QMessageBox>

#include "helpdialog.h"
#include "mainwindow.h"
#include "readme.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
   : QMainWindow(parent)
   , ui(new Ui::MainWindow)
   , defaultVert(getDefaultVertString())
   , defaultFrag(getDefaultFragString())
   , defaultComp(getDefaultCompString())

{
   ui->setupUi(this);

   ui->vertTextEdit->setPlainText(defaultVert);
   ui->fragTextEdit->setPlainText(defaultFrag);
   ui->compTextEdit->setPlainText(defaultComp);
   QObject::connect(
            ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(rxSelectionListDoubleClicked(QListWidgetItem*)));
   QObject::connect(
            ui->addNewFg, SIGNAL(clicked()),
            this, SLOT(on_saveSelection_triggered()));
   QObject::connect(
            ui->openGLWidget, SIGNAL(txNewSession()),
            this, SLOT(resetUI()));
   QObject::connect(
            ui->openGLWidget, SIGNAL(txSessionLoaded()),
            this, SLOT(rxSessionLoaded()));

   QObject::connect(ui->p0toggle, SIGNAL(toggled(bool)), ui->openGLWidget, SLOT(rxParam0Toggle(bool)));
   QObject::connect(ui->p1toggle, SIGNAL(toggled(bool)), ui->openGLWidget, SLOT(rxParam1Toggle(bool)));
   QObject::connect(ui->p2toggle, SIGNAL(toggled(bool)), ui->openGLWidget, SLOT(rxParam2Toggle(bool)));
   QObject::connect(ui->p3toggle, SIGNAL(toggled(bool)), ui->openGLWidget, SLOT(rxParam3Toggle(bool)));
   QObject::connect(ui->p4toggle, SIGNAL(toggled(bool)), ui->openGLWidget, SLOT(rxParam4Toggle(bool)));
   QObject::connect(ui->p5toggle, SIGNAL(toggled(bool)), ui->openGLWidget, SLOT(rxParam5Toggle(bool)));

   QFile ss("://common/main.css");
   ss.open(QFile::ReadOnly);
   setStyleSheet(ss.readAll());
}

MainWindow::~MainWindow()
{
   delete ui;
}

void MainWindow::rxShaderProgramReady()
{
   ui->openGLWidget->rxVertShaderString(defaultVert);
   ui->openGLWidget->rxFragShaderString(defaultFrag);
   ui->openGLWidget->rxCompShaderString(defaultComp);

}

void MainWindow::rxStatusMessage(QString msg)
{
   statusBar()->showMessage(msg, STATUS_MSG_TIMEOUT);
}

void MainWindow::onPressUpdateFragShader()
{
   QString fragShaderString = ui->fragTextEdit->toPlainText();
   emit txFragShaderString(fragShaderString);
}

void MainWindow::onPressUpdateVertShader()
{
   QString vertShaderString = ui->vertTextEdit->toPlainText();
   emit txVertShaderString(vertShaderString);
}

void MainWindow::onPressUpdateCompShader()
{
   QString compShaderString = ui->compTextEdit->toPlainText();
   emit txCompShaderString(compShaderString);
}

void MainWindow::rxSavedSelectionsOnLoad(QString name)
{
   ui->listWidget->addItem(name);
}

void MainWindow::rxSelectionListDoubleClicked(QListWidgetItem* item)
{
   QString savedSelectionName = item->text();
   emit txLoadSelection(savedSelectionName);
   statusBar()->showMessage("Selection loaded", STATUS_MSG_TIMEOUT);
}

void MainWindow::rxSessionLoaded()
{
   resetUI();
   QString loadedVert {ui->openGLWidget->sessionManager.vertShader};
   QString loadedFrag {ui->openGLWidget->sessionManager.fragShader};
   QString loadedComp {ui->openGLWidget->sessionManager.compShader};

   ui->vertTextEdit->setPlainText(loadedVert);
   ui->fragTextEdit->setPlainText(loadedFrag);
   ui->compTextEdit->setPlainText(loadedComp);
}

void MainWindow::on_loadSelection_triggered()
{
   if (ui->listWidget->selectedItems().size() != 0){
      QString savedSelectionName = ui->listWidget->currentItem()->text();
      emit txLoadSelection(savedSelectionName);
      statusBar()->showMessage("Selection loaded", STATUS_MSG_TIMEOUT);
   } else {
      statusBar()->showMessage("<<ERROR>> Must select name in list", STATUS_MSG_TIMEOUT);
   }
}


void MainWindow::on_saveSelection_triggered()
{
   bool ok;
   QString name = QInputDialog::getText(this, tr("Save Current Selection"),
                                        tr("Name Selection:"), QLineEdit::Normal,
                                        "", &ok);
   if (ok && !name.isEmpty()) {
      ui->listWidget->addItem(name);
      emit txSaveSelection(name);
      statusBar()->showMessage("Selection saved", STATUS_MSG_TIMEOUT);
   } else {
      statusBar()->showMessage("<<ERROR>> Must name selection", STATUS_MSG_TIMEOUT);
   }
}

QString MainWindow::getDefaultVertString()
{
   QFile f("://shaders/default.vert");
   f.open(QIODevice::ReadOnly);
   QTextStream in(&f);
   return in.readAll();
}

QString MainWindow::getDefaultFragString()
{
   QFile f("://shaders/default.frag");
   f.open(QIODevice::ReadOnly);
   QTextStream in(&f);
   return in.readAll();
}

QString MainWindow::getDefaultCompString()
{
   QFile f("://shaders/default.comp");
   f.open(QIODevice::ReadOnly);
   QTextStream in(&f);
   return in.readAll();
}


void MainWindow::on_loadFragShader_triggered()
{
   QString file_path = QFileDialog::getOpenFileName(
                          this,
                          tr("Load Fragment Shader"), "",
                          tr("Frag GLSL(*.*)"));
   QFile file(file_path);
   if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      statusBar()->showMessage("<<ERROR>> Unable to open file", STATUS_MSG_TIMEOUT);
      return;
   }

   QTextStream in(&file);
   QString fileContent = in.readAll();
   ui->fragTextEdit->setPlainText(fileContent);
   emit txFragShaderString(fileContent);
   statusBar()->showMessage("Loaded fragment shader from " + file_path, STATUS_MSG_TIMEOUT);
}


void MainWindow::on_loadVertShader_triggered()
{
   QString file_path = QFileDialog::getOpenFileName(
                          this,
                          tr("Load Vertex Shader"), "",
                          tr("Vert GLSL(*.*)"));
   QFile file(file_path);
   if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      statusBar()->showMessage("<<ERROR>> Unable to open file", STATUS_MSG_TIMEOUT);
      return;
   }

   QTextStream in(&file);
   QString fileContent = in.readAll();
   ui->vertTextEdit->setPlainText(fileContent);
   emit txVertShaderString(fileContent);
   statusBar()->showMessage("Loaded vertex shader from " + file_path, STATUS_MSG_TIMEOUT);
}

void MainWindow::on_saveSession_triggered()
{
   ui->openGLWidget->saveSession();
}


void MainWindow::on_loadSession_triggered()
{
   ui->openGLWidget->loadSession();
}


void MainWindow::on_newSession_triggered()
{
   QString file_path = QFileDialog::getOpenFileName(
                          this,
                          tr("Open Triangulated OBJ"), "",
                          tr("Wavefront .obj(*.obj);"));
   if (file_path.isEmpty()) {
      return;
   } else {
      emit txNewSession(file_path);
   }
}

void MainWindow::on_buildConnectivity_triggered()
{
   ui->openGLWidget->rxBuildConnectivity();
}

void MainWindow::showCookbook() {

}


void MainWindow::on_shaderCookbook_triggered()
{
   QFile f("://readme/shadercookbook.txt");
   f.open(QIODevice::ReadOnly);
   QString cbText = QTextStream(&f).readAll();
   f.close();

   QPointer<HelpDialog> cookBook = new HelpDialog("", cbText, {600, 800});
   cookBook->setWindowTitle("Shader Cookbook");
   cookBook->setModal(false);
   cookBook->show();
}


void MainWindow::on_displayAbout_triggered()
{
   QString aboutLabel{
      "University of Washington\n"
      "Department of Electrical and Computer Engineering\n\n"

      "Derek Repsch, MSEE, Developer\n"
      "Colin Reinhard, PhD, Faculty\n\n"

      "Spring - Summer 2022"
   };

   QPointer<HelpDialog> about = new HelpDialog(aboutLabel, "", {200, 100});
   about->setWindowTitle("About");
   about->setModal(true);
   about->show();
}


void MainWindow::on_readMe_triggered()
{
   QPointer<ReadMe> readme = new ReadMe();
   readme->show();
}



void MainWindow::on_displayBindingsHelp_triggered()
{
   QString bindingsLabel{
      "# MOUSE\n"
      "[ Left ]\t On Mesh -> Select\n"
      "[ Left ]\t On Background -> Clear selection\n"
      "[ + Drag ]\t Box Select\n"
      "[ + Shift ]\t Add to selection\n"
      "[ + Ctrl ]\t Remove from selection\n"
      "[ Middle ]\n"
      "[ + Drag ]\t Rotate view\n"
      "[ + Shift ]\t Pan model\n"
      "[ Scroll ]\t Zoom\n"

      "\n# KEYBOARD\n"
      "[ Alt ]\n"
      "[ + X ]\t\t Dispatch Compute Shader\n"
      "[ + S ]\t\t Print Shader Watcher\n"
      "[ + D ]\t\t Print glError\n"
   };

   QPointer<HelpDialog> bindings = new HelpDialog(bindingsLabel, "", {200, 100});
   bindings->setWindowTitle("Bindings");
   bindings->setModal(false);
   bindings->show();

}





void MainWindow::on_shaderWatch_triggered()
{
   ui->openGLWidget->rxTriggerShaderWatcher();
}


void MainWindow::on_dispatchCompute_triggered()
{
   ui->openGLWidget->rxTriggerCompute();

}


void MainWindow::on_GetGlError_triggered()
{
   ui->openGLWidget->getGlError();

}


void MainWindow::on_clearDebug_triggered()
{
   ui->debugTextEdit->clear();
}

void MainWindow::resetUI()
{
   ui->vertTextEdit->setPlainText(defaultVert);
   ui->fragTextEdit->setPlainText(defaultFrag);
   ui->compTextEdit->setPlainText(defaultComp);
   ui->listWidget->clear();
   ui->p0val->setValue(0.0);
   ui->p1val->setValue(0.0);
   ui->p2val->setValue(0.0);
   ui->p3val->setValue(0.0);
   ui->p4val->setValue(0.0);
   ui->p5val->setValue(0.0);
   ui->p0name->setText("PARAM0");
   ui->p1name->setText("PARAM1");
   ui->p2name->setText("PARAM2");
   ui->p3name->setText("PARAM3");
   ui->p4name->setText("PARAM4");
   ui->p5name->setText("PARAM5");
   QVector<QPointer<QRadioButton>> toggles {
      ui->p0toggle,
      ui->p1toggle,
      ui->p2toggle,
      ui->p3toggle,
      ui->p4toggle,
      ui->p5toggle
   };
   for (const auto& tog : toggles) {
      if (tog->isChecked()) {
         tog->toggle();
      }
   }
}

