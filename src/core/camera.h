#ifndef CAMERA_H
#define CAMERA_H

#include <QMatrix4x4>

class Camera
{
   public:
      Camera();

      QMatrix4x4  view_matrix;
      QMatrix4x4  projection_matrix;

      void        rotate(float dx, float dy);
      void        translate(float dx, float dy);
      void        setAspectRatio(int w, int h);
      void        setRadius(int scroll);
      QVector3D   getPosition();

   private:
      void        updateArcBall();

      QVector3D   position;
      QVector3D   lookat_target;
      QVector3D   up_vector;

      float       radius;
      float       azimuth_rad;
      float       polar_rad;

      QMatrix4x4  translation_matrix;
};

#endif // CAMERA_H
