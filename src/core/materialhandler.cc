#include "materialhandler.h"

MaterialHandler::MaterialHandler(QObject *parent)
   : QObject{parent}
{

}

MaterialHandler::~MaterialHandler()
{
   for (int i = 0; i < NUM_TEX; ++i)
      delete texture[i];
}

void MaterialHandler::init()
{
   texture[0] = new QOpenGLTexture(QImage(QString("E:/UnrealEngine/Materials/alien-metal1-bl/alien-metal_albedo.png")).mirrored());
   texture[1] = new QOpenGLTexture(QImage(QString("E:/UnrealEngine/Materials/alien-metal1-bl/alien-metal_ao.png")).mirrored());
   texture[2] = new QOpenGLTexture(QImage(QString("E:/UnrealEngine/Materials/alien-metal1-bl/alien-metal_metallic.png")).mirrored());
   texture[3] = new QOpenGLTexture(QImage(QString("E:/UnrealEngine/Materials/alien-metal1-bl/alien-metal_normal-ogl.png")).mirrored());
   texture[4] = new QOpenGLTexture(QImage(QString("E:/UnrealEngine/Materials/alien-metal1-bl/alien-metal_roughness.png")).mirrored());

}
