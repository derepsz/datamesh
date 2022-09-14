#ifndef DATALAYER_H
#define DATALAYER_H
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLFunctions_4_5_Core>
#include <QObject>

#include "mesh.h"

class DataLayer : public QObject, protected QOpenGLFunctions_4_5_Core
{
      Q_OBJECT
   public:
      explicit DataLayer(QObject *parent = nullptr);
      void initShaderStorage(uint numFacets);
      void onLoadSession();
      void updateParameters(QVector<uint> &selected, int &param_num, float &val);
      void initNeighbors(const Mesh &mesh);
      bool isBusy();

      QVector<GLuint>         param_ssbos;
      QVector<QVector<float>> parameters;

      GLuint                  adj_ssbo;
      QVector<GLint>          neighbors;

      static constexpr int    NUM_PARAM {6};
      static constexpr int    NUM_ADJACENT {3};
      static constexpr int    NULL_ID {std::numeric_limits<qint32>::max()};
      static constexpr int    ADJ_SSBO_LOC{6};

   private:
      bool is_busy;
};

#endif // DATALAYER_H
