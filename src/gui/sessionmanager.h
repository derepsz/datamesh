#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include "datalayer.h"
#include "fbselect.h"
#include "mesh.h"
#include <QObject>
#include <QPointer>

class SessionManager : public QObject
{
      Q_OBJECT
   public:
      explicit SessionManager(Mesh &m, DataLayer &dl, FbSelect &fb, QObject *parent = nullptr);
//      explicit SessionManager(QObject *parent = nullptr);

      QString fragShader;
      QString vertShader;
      QString compShader;

   public slots:
      void saveSession(QString path);
      void loadSession(QString path);

   signals:
      void txSessionManagerStatus(QString);

   private:
      Mesh&                mesh;
      DataLayer&           datalayer;
      FbSelect&            fbSelect;
};

#endif // SESSIONMANAGER_H
