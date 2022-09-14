#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <QElapsedTimer>
#include <QStringTokenizer>

#include <QFile>
#include <QOpenGLFunctions_4_5_Core>
#include <QFuture>
#include <QMessageBox>
#include <QProgressDialog>

#include "global.h"
#include "mesh.h"

Mesh::Mesh(QObject *parent)
   : QObject{parent}
   , model_matrix()
   , vbo_pos(QOpenGLBuffer::VertexBuffer)
   , vbo_norm(QOpenGLBuffer::VertexBuffer)
   , vbo_uv(QOpenGLBuffer::VertexBuffer)
   , ebo(QOpenGLBuffer::IndexBuffer)
   , verts_counted {false}
{
}

void Mesh::destroy() {
   vbo_pos.destroy();
   vbo_norm.destroy();
   ebo.destroy();
   vao.destroy();
}


void Mesh::draw(QOpenGLShaderProgram &shader)
{
   shader.bind();
   vao.bind();
   glDrawElements(GL_TRIANGLES, GLsizei(idxs.size()), GL_UNSIGNED_INT, 0);
   shader.release();
}


bool Mesh::hasMesh()
{
   if(verts.size() > 0) {
      return true;
   } else {
      return false;
   }
}


void Mesh::initBuffers(QOpenGLShaderProgram& shader)
{
   initializeOpenGLFunctions();
   shader.bind();

   if (vao.isCreated()){
      destroy();
   }

   vao.create();
   vao.bind();

   vbo_pos.create();
   vbo_pos.bind();
   vbo_pos.setUsagePattern(QOpenGLBuffer::StaticDraw);
   vbo_pos.allocate(&verts[0], int(verts.size() * sizeof(vec3)));
   shader.enableAttributeArray(0);
   shader.setAttributeBuffer(0, GL_FLOAT, 0, 3);

   vbo_norm.create();
   vbo_norm.bind();
   vbo_norm.setUsagePattern(QOpenGLBuffer::StaticDraw);
   vbo_norm.allocate(&norms[0], int(norms.size() * sizeof(vec3)));
   shader.enableAttributeArray(1);
   shader.setAttributeBuffer(1, GL_FLOAT, 0, 3);

   vbo_uv.create();
   vbo_uv.bind();
   vbo_uv.setUsagePattern(QOpenGLBuffer::StaticDraw);
   vbo_uv.allocate(&texcoords[0], int(texcoords.size() * sizeof(vec2)));
   shader.enableAttributeArray(2);
   shader.setAttributeBuffer(2, GL_FLOAT, 0, 2);

   ebo.create();
   ebo.bind();
   ebo.setUsagePattern(QOpenGLBuffer::StaticDraw);
   ebo.allocate(&idxs[0], int(idxs.size() * sizeof(GLuint)));
   if (int er = glGetError())
   {
      qWarning() << "OPENGL ERROR: " << er;
   }
   vao.release();
   shader.release();
}

bool Mesh::getNumVertsToLoad(QString path) {

   QFile file(path);
      if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
      {
         verts_counted = false;
         return false;
      }
   QTextStream stream(&file);
   QString line{};

   uint num_verts {0};
   num_lines_to_read = 0;
   num_verts = 0;
   while (!stream.atEnd()){
      ++num_lines_to_read;
      stream.readLineInto(&line);
      QString flag = line.split(" ")[0];
      if ( flag == "v") {
         ++num_verts;
      }
   }
   // TODO: Get timing estimate for warning
   QMessageBox msgBox;
   msgBox.setWindowTitle("OBJ Info");
   msgBox.setText("OBJ contains " + QString::number(num_verts) + " vertices.");
   msgBox.setInformativeText("Process mesh?");
   msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
   msgBox.setDefaultButton(QMessageBox::Yes);
   int input = msgBox.exec();
   if (input == QMessageBox::No) {
      emit txMeshStatusMessage("Session build canceled by user.");
      verts_counted = false;
      return false;
   } else {
      num_verts_to_load = num_verts;
      verts_counted = true;
      return true;
   }
}

bool Mesh::loadMesh(QString file_path)
{

   if (!verts_counted) { getNumVertsToLoad(file_path);}
   QFile file(file_path);
      if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
      {
         verts_counted = false;
         return false;
      }

   int num_verts = num_verts_to_load;

   verts.clear();
   texcoords.clear();
   norms.clear();
   idxs.clear();

   file.open(QIODevice::ReadOnly | QIODevice::Text);
   QTextStream stream(&file);
   QString line{};

   QVector<vec3> temp_verts{};
   QVector<vec3> temp_norms{};
   QVector<vec2> temp_texcoords {};
   QHash<QString, int> existing_faces{};
   auto pair_iter = existing_faces.begin();
   QVector<QString> vert_idxs;
   int idx_count { 0 };

   temp_verts.reserve(num_verts);
   temp_norms.reserve(num_verts);
   temp_texcoords.reserve(num_verts);

   verts.reserve(num_verts);
   norms.reserve(num_verts);
   texcoords.reserve(num_verts);
   idxs.reserve(num_verts);

   QVector<QString> tokens;

   QProgressDialog progress("Parsing OBJ...", "Abort", 0, num_lines_to_read);
   progress.setMinimumDuration(2000);
   progress.setWindowModality(Qt::WindowModal);
   progress.setValue(0);
   int line_counter {0};
   int progress_increment = num_lines_to_read / 100 -1;
   int next_update_count {progress_increment};

   while (!stream.atEnd())
   {
      ++line_counter;
      if (line_counter >= next_update_count || line_counter >= num_lines_to_read) {
         progress.setValue(line_counter);
         next_update_count += progress_increment;
      }
      if (progress.wasCanceled()) {
          break;
      }

      stream.readLineInto(&line);
      tokens = line.split(" ");

      if (tokens[0] == "v")
      {
         vec3 vert;
         vert.x = tokens[1].toFloat();
         vert.y = tokens[2].toFloat();
         vert.z = tokens[3].toFloat();
         temp_verts.push_back(vert);
      }


      else if (tokens[0] == "vn")
      {
         vec3 norm;
         norm.x = tokens[1].toFloat();
         norm.y = tokens[2].toFloat();
         norm.z = tokens[3].toFloat();
         temp_norms.push_back(norm);
      }
      else if (tokens[0] == "vt")
      {
         vec2 uv;
         uv.u = tokens[1].toFloat();
         uv.v = tokens[2].toFloat();
         temp_texcoords.push_back(uv);
      }

      else if (tokens[0] == "f")
      {
         tokens.remove(0);
         for (const auto &token : tokens)
         {
            bool duplicate {false};
            for (; pair_iter != existing_faces.end(); ++pair_iter)
            {
               if (token == pair_iter.key())
               {
                  idxs.push_back(pair_iter.value());
                  duplicate = true;
                  break;
               }
            }
            if (!duplicate){
               existing_faces.insert(token, idx_count);
               vert_idxs = token.split("/");
               verts.push_back(temp_verts[vert_idxs[0].toInt()-1]);
               if (vert_idxs[1] != "")
               {
                  texcoords.push_back(temp_texcoords[vert_idxs[1].toInt()-1]);
               }
               if (vert_idxs[2] != "")
               {
                  norms.push_back(temp_norms[vert_idxs[2].toInt()-1]);
               }
               idxs.push_back(idx_count++);
            }
         }
      }
   }
   num_facets = idxs.size() / 3;
   verts_counted = false;
   progress.setValue(num_lines_to_read);
   return true;
}
