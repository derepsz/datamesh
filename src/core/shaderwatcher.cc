#include "shaderwatcher.h"

ShaderWatcher::ShaderWatcher(QObject *parent)
   : QObject{parent}
{

}

void ShaderWatcher::initDebugSSBO() {
   initializeOpenGLFunctions();
   glGenBuffers(1, &debugSSBO);
   glBindBuffer(GL_SHADER_STORAGE_BUFFER, debugSSBO);
   glBindBufferBase(GL_SHADER_STORAGE_BUFFER, DBG_SSBO_LOC, debugSSBO);
   glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ShaderWatchBlock), NULL, GL_DYNAMIC_COPY);
   glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}


void ShaderWatcher::printDebugSSBO() {
   glBindBuffer(GL_SHADER_STORAGE_BUFFER, debugSSBO);
   ShaderWatchBlock* ssbo_ptr = (ShaderWatchBlock*)glMapNamedBuffer(debugSSBO, GL_READ_ONLY);
   QString message;
   QTextStream stream(&message);
   stream << "\n<< SHADER WATCH >>";
   stream << "\n(int)\t";
   for (int i{0}; i < NUM_WATCHES; ++i) {
      stream << "[" << i << "] " << ssbo_ptr->ints[i] << "\t";
   }
   stream << "\n(float)\t";
   for (int i{0}; i < NUM_WATCHES; ++i) {
      stream << "[" << i << "] " << ssbo_ptr->floats[i] << "\t";
   }
   stream << "\n(vec3)\t";
   for (int i{0}; i < NUM_WATCHES; ++i) {
      stream << "[" << i << "] ("
             << ssbo_ptr->vec4s[i].x << ", "
             << ssbo_ptr->vec4s[i].y << ", "
             << ssbo_ptr->vec4s[i].z << ", "
             << ssbo_ptr->vec4s[i].w << ")" << "\t";
   }
   glUnmapNamedBuffer(debugSSBO);
   glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
   emit txDebugMessage(message);
}
