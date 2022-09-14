#include "datalayer.h"
#include <limits>
#include <QProgressDialog>
#include <QCoreApplication>

DataLayer::DataLayer(QObject *parent)
   : QObject{parent}
   , param_ssbos(NUM_PARAM)
   , parameters(NUM_PARAM)
   , is_busy(false)
{
}

void DataLayer::initShaderStorage(uint numFacets)
{
   initializeOpenGLFunctions();
   for (int i{0}; i < NUM_PARAM; ++i) {
      parameters[i].resize(numFacets);
      parameters[i].fill(0.5f);
      glGenBuffers(1, &param_ssbos[i]);
      glBindBuffer(GL_SHADER_STORAGE_BUFFER, param_ssbos[i]);
      glBindBufferBase(GL_SHADER_STORAGE_BUFFER, i, param_ssbos[i]);
      glBufferData(
               GL_SHADER_STORAGE_BUFFER,
               parameters[i].size() * sizeof(float),
               parameters[i].data(),
               GL_STATIC_DRAW);
      glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
   }
   glGenBuffers(1, &adj_ssbo);
   glBindBuffer(GL_SHADER_STORAGE_BUFFER, adj_ssbo);
   glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ADJ_SSBO_LOC, adj_ssbo);
   glBufferData(
            GL_SHADER_STORAGE_BUFFER,
            neighbors.size() * sizeof(GLint),
            neighbors.data(),
            GL_DYNAMIC_COPY);
   glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void DataLayer::onLoadSession()
{
   initializeOpenGLFunctions();
   for (int i{0}; i < NUM_PARAM; ++i) {
      glGenBuffers(1, &param_ssbos[i]);
      glBindBuffer(GL_SHADER_STORAGE_BUFFER, param_ssbos[i]);
      glBufferData(
               GL_SHADER_STORAGE_BUFFER,
               parameters[i].size() * sizeof(float),
               parameters[i].data(),
               GL_STATIC_DRAW);
      glBindBufferBase(GL_SHADER_STORAGE_BUFFER, i, param_ssbos[i]);
      glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
   }
}

void DataLayer::updateParameters(QVector<uint> &selected, int &param_num, float &val)
{
   glBindBuffer(GL_SHADER_STORAGE_BUFFER, param_ssbos[param_num]);
   for (auto faceId : selected) {
      parameters[param_num][faceId] = val;
      glBufferSubData(
               GL_SHADER_STORAGE_BUFFER,
               faceId * sizeof(float),
               sizeof(float),
               &val);
   }
   glBindBufferBase(GL_SHADER_STORAGE_BUFFER, param_num, param_ssbos[param_num]);
   glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void DataLayer::initNeighbors(const Mesh &mesh)
{
   is_busy = true;
   neighbors.clear();
   neighbors.reserve(mesh.num_facets*3);

   QVector<int> face_ids{};
   face_ids.reserve(mesh.num_facets);
   for (uint i{0}; i < mesh.num_facets; ++i) {
      face_ids.push_back(i);
   }


   int end_count = mesh.num_facets;
   int progress_counter {0};
   int progress_increment = end_count / 100 - 1;
   int next_update_count {progress_increment};

   QProgressDialog progress("Building facet connectivity array...", "Abort", 0, end_count);
   progress.setWindowTitle("Computering");
   progress.setMinimumDuration(2000);
   progress.setWindowModality(Qt::WindowModal);
   progress.setValue(0);
   progress.show();
   QCoreApplication::processEvents();
   auto mesh_verts_ptr = mesh.verts.data();
   auto mesh_idxs_ptr =  mesh.idxs.data();
   QVector<GLint> new_nebs;
   for (const auto& node_id : face_ids) {

      ++progress_counter;
      if (progress_counter >= next_update_count || progress_counter >= end_count) {
         progress.setValue(progress_counter);
         next_update_count += progress_increment;
      }
      if (progress.wasCanceled()) {
         break;
      }

      QVector<vec3> node {
         mesh_verts_ptr[mesh_idxs_ptr[node_id*3 + 0]],
               mesh_verts_ptr[mesh_idxs_ptr[node_id*3 + 1]],
               mesh_verts_ptr[mesh_idxs_ptr[node_id*3 + 2]],
      };

      new_nebs.clear();
      new_nebs.reserve(face_ids.size());
      for (const auto& suspect_id : face_ids) {



         if (suspect_id == node_id) { continue; }
         else {
            QVector<vec3> suspect {
               mesh_verts_ptr[mesh_idxs_ptr[suspect_id*3 + 0]],
                     mesh_verts_ptr[mesh_idxs_ptr[suspect_id*3 + 1]],
                     mesh_verts_ptr[mesh_idxs_ptr[suspect_id*3 + 2]],
            };
            int shared_verts {0};
            for (vec3 &vert : node) {
               if ( (vert == suspect[0])
                    || (vert == suspect[1])
                    || (vert == suspect[2])
                    ) {
                  ++shared_verts;
               }
               if (shared_verts == 2) {
                  new_nebs.push_back(suspect_id);
                  break;
               }
            }
            if (new_nebs.size() == NUM_ADJACENT) {
               break;
            }
         }
      }
      if (new_nebs.size() != NUM_ADJACENT) {
         for (int idx = new_nebs.size(); idx < NUM_ADJACENT; ++idx) {
            new_nebs.push_back(NULL_ID);
         }
      }
      for (const auto& neb : new_nebs) {
         neighbors.push_back(neb);
      }
   }

   glBindBuffer(GL_SHADER_STORAGE_BUFFER, adj_ssbo);
   glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ADJ_SSBO_LOC, adj_ssbo);
   glBufferData(
            GL_SHADER_STORAGE_BUFFER,
            neighbors.size() * sizeof(GLint),
            neighbors.data(),
            GL_DYNAMIC_COPY);
   glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
   is_busy = false;
}

bool DataLayer::isBusy()
{
   return is_busy;
}


