#ifndef SHADERWATCHER_H
#define SHADERWATCHER_H

#include <QObject>
#include <QOpenGLFunctions_4_5_Core>
#include "global.h"

class ShaderWatcher : public QObject, protected QOpenGLFunctions_4_5_Core
{
      Q_OBJECT
   public:
      explicit ShaderWatcher(QObject *parent = nullptr);

      void initDebugSSBO();
      void printDebugSSBO();

   signals:
      void txDebugMessage(QString);

   private:
      GLuint               debugSSBO;

      static constexpr int DBG_SSBO_LOC{7};
      static constexpr int NUM_WATCHES {8};

      struct ShaderWatchBlock {
         int      ints     [NUM_WATCHES];
         float    floats   [NUM_WATCHES];
         vec4     vec4s    [NUM_WATCHES];
      };

};

#endif // SHADERWATCHER_H
