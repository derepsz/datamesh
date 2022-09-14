#include "shaderhandler.h"

ShaderHandler::ShaderHandler(QObject *parent)
   : QObject{parent}
{
}


void ShaderHandler::init()
{
   initializeOpenGLFunctions();
   shader.addShaderFromSourceFile(
            QOpenGLShader::Vertex,
            "://shaders/default.vert");
   shader.addShaderFromSourceFile(
            QOpenGLShader::Fragment,
            "://shaders/default.frag");
   shader.link();
   initComputeShader();

}

void ShaderHandler::initSelectShader()
{
   selectShader.addShaderFromSourceFile(
            QOpenGLShader::Vertex,
            "://shaders/default.vert");
   selectShader.addShaderFromSourceFile(
            QOpenGLShader::Fragment,
            "://shaders/default.frag");
   selectShader.link();
}

void ShaderHandler::initComputeShader()
{
   compute.addShaderFromSourceFile(
            QOpenGLShader::Compute,
            "://shaders/default.comp");
   qDebug() << compute.link();}


void ShaderHandler::setFragShader(QString frag_shader)
{
   fragShaderText = frag_shader;
   reloadDrawShaders();
}

void ShaderHandler::setVertShader(QString vert_shader)
{
   vertShaderText = vert_shader;
   reloadDrawShaders();
}

void ShaderHandler::setCompShader(QString comp_shader)
{
   compShaderText = comp_shader;
   reloadComputeShader();
}

QString ShaderHandler::getLog()
{
   return shaderLog;
}

void ShaderHandler::dispatch()
{
   compute.bind();
   glDispatchCompute(20, 1, 1);
   glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
   compute.release();
}

void ShaderHandler::reloadDrawShaders()
{
   shaderLog.clear();
   const QString vert_shader = vertShaderText;
   const QString frag_shader = fragShaderText;
   shader.removeAllShaders();
   if (!shader.addShaderFromSourceCode(QOpenGLShader::Vertex, vert_shader)) {
      shaderLog += shader.log();
   }
   if (!shader.addShaderFromSourceCode(QOpenGLShader::Fragment, frag_shader)) {
      shaderLog += shader.log();
   }
   if (!shader.link()) {
      shaderLog += shader.log();
   }
}

void ShaderHandler::reloadComputeShader()
{
   shaderLog.clear();
   const QString comp_shader = compShaderText;
   compute.removeAllShaders();
   if (!compute.addShaderFromSourceCode(QOpenGLShader::Compute, comp_shader)) {
      shaderLog += compute.log();
   }
   if (!compute.link()) {
      shaderLog += compute.log();
   }
}
