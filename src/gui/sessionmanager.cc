#include "sessionmanager.h"
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

SessionManager::SessionManager(Mesh& m, DataLayer& dl, FbSelect& fb, QObject* parent)
   : QObject(parent)
   , mesh(m)
   , datalayer(dl)
   , fbSelect(fb)
{

}

void SessionManager::saveSession(QString path)
{
   QFile file(path);
   if (!file.open(QIODevice::WriteOnly)) {
      QMessageBox::information(
               nullptr, tr("Unable to create file"), file.errorString());
      return;
   }
   QTextStream stream(&file);

   stream << "# BEGIN_MESH\n";
   { for (const auto& vert : mesh.verts) {
         stream << "mv" << " "
                << QString::number(vert.x) << " "
                << QString::number(vert.y) << " "
                << QString::number(vert.z);
         Qt::endl(stream);
      }

      stream << "mi";
      for (const auto& idx : mesh.idxs) {
         stream << " " << QString::number(idx);
      }
      Qt::endl(stream);

      if (!mesh.norms.isEmpty()) {
         for (const auto& norm : mesh.norms) {
            stream << "mn" << " "
                   << QString::number(norm.x) << " "
                   << QString::number(norm.y) << " "
                   << QString::number(norm.z);
            Qt::endl(stream);
         }
      }
      if (!mesh.texcoords.isEmpty()) {
         for (const auto& texc : mesh.texcoords) {
            stream << "mt" << " "
                   << QString::number(texc.u) << " "
                   << QString::number(texc.v);
            Qt::endl(stream);
         }
      }
   }
   stream << "# END_MESH\n\n";


   stream << "# BEGIN_DATALAYER\n";
   {
      for (int i{0}; i < datalayer.NUM_PARAM; ++i) {
         stream << "p" + QString::number(i) << " ";
         for (const auto & param : datalayer.parameters[i]){
            stream << param << " ";
         }
         stream << "\n";
      }

      stream << "nn" << " ";
      for (const auto& neb : datalayer.neighbors) {
         stream << neb << " ";
      }
      Qt::endl(stream);

   }
   stream << "# END_DATALAYER\n\n";


   stream << "# BEGIN_FACETGROUPS\n";
   {
      for (auto pair {fbSelect.savedSelections.begin()};
           pair != fbSelect.savedSelections.end(); ++pair) {
         stream << "fg" << " " << pair.key();
         for (const auto& faceid : pair.value()) {
            stream << " " << QString::number(faceid);
         }
         Qt::endl(stream);
      }
   }
   stream << "# END_FACETGROUPS\n\n";


   stream << "# BEGIN_VERTEXSHADER\n";
   {
      stream << vertShader << "\n";
   }
   stream << "# END_VERTEXSHADER\n";

   stream << "# BEGIN_FRAGMENTSHADER\n";
   {
      stream << fragShader << "\n";
   }
   stream << "# END_FRAGMENTSHADER\n\n";

   stream << "# BEGIN_COMPUTESHADER\n";
   {
      stream << compShader << "\n";
   }
   stream << "# END_COMPUTESHADER\n";

}

void SessionManager::loadSession(QString path)
{

   QFile file(path);
   if (!file.open(QIODevice::ReadOnly)) {
      QMessageBox::information(
               nullptr, tr("Unable to open file"), file.errorString());
      return;
   }
   QTextStream stream(&file);

   mesh.verts.clear();
   mesh.norms.clear();
   mesh.texcoords.clear();
   mesh.idxs.clear();
   datalayer.parameters.clear();
   fbSelect.savedSelections.clear();

   datalayer.parameters.resize(datalayer.NUM_PARAM);

   vec3 temp_vec3;
   vec2 temp_vec2;

   while (!stream.atEnd()) {
      auto tokens = stream.readLine().split(" ");
      if (tokens[0] == "") { continue; }
      else if (tokens[0] == "mv") {
         temp_vec3.x = tokens[1].toFloat();
         temp_vec3.y = tokens[2].toFloat();
         temp_vec3.z = tokens[3].toFloat();
         mesh.verts.push_back(temp_vec3);
         continue;
      }
      else if (tokens[0] == "mi") {
         tokens.remove(0);
         mesh.idxs.reserve(tokens.size());
         for (const auto& idx : tokens) {
            mesh.idxs.push_back(idx.toInt());
         }
         continue;
      }
      else if (tokens[0] == "mn"){
         temp_vec3.x = tokens[1].toFloat();
         temp_vec3.y = tokens[2].toFloat();
         temp_vec3.z = tokens[3].toFloat();
         mesh.norms.push_back(temp_vec3);
         continue;
      }
      else if (tokens[0] == "mt"){
         temp_vec2.u = tokens[1].toFloat();
         temp_vec2.v = tokens[2].toFloat();
         mesh.texcoords.push_back(temp_vec2);
         continue;
      }
      else if (tokens[0][0] == 'p' && tokens[0][1].isDigit()){
         int p_idx = tokens[0].sliced(1,1).toInt();
         tokens.remove(0);
         datalayer.parameters[p_idx].reserve(tokens.size());
         for (const auto& param : tokens) {
            datalayer.parameters[p_idx].push_back(param.toFloat());
         }
         continue;
      }
      else if (tokens[0] == "nn") {
         tokens.remove(0);
         datalayer.neighbors.reserve(tokens.size());
         for (const auto& neb : tokens) {
            datalayer.neighbors.push_back(neb.toInt());
         }
      }
      else if (tokens[0] == "fg"){
         tokens.remove(0);
         QString name = tokens[0];
         tokens.remove(0);
         QVector<uint> selected{};
         selected.reserve(tokens.size());
         for (const auto& faceid : tokens) {
            selected.push_back(faceid.toUInt());
         }
         fbSelect.savedSelections.insert(name, selected);
         continue;
      }
      else if (tokens[1] == "BEGIN_VERTEXSHADER") {
         QString shaderText {""};
         QString shaderLine {stream.readLine()};
         do {
            shaderText += "\n" + shaderLine;
            shaderLine = stream.readLine();
         } while (shaderLine != "# END_VERTEXSHADER");
         vertShader = shaderText;
      }
      else if (tokens[1] == "BEGIN_FRAGMENTSHADER") {
         QString shaderText {""};
         QString shaderLine {stream.readLine()};
         do {
            shaderText += "\n" + shaderLine;
            shaderLine = stream.readLine();
         } while (shaderLine != "# END_FRAGMENTSHADER");
         fragShader = shaderText;
      }
      else if (tokens[1] == "BEGIN_COMPUTESHADER") {
         QString shaderText {""};
         QString shaderLine {stream.readLine()};
         do {
            shaderText += "\n" + shaderLine;
            shaderLine = stream.readLine();
         } while (shaderLine != "# END_COMPUTESHADER");
         compShader = shaderText;
      }
   }
   mesh.num_facets = mesh.idxs.size() / 3;
}
