#include "camera.h"

#include <QtMath>

Camera::Camera()
   : view_matrix()
   , projection_matrix()
   , position(0.0f, 4.0f, 8.0f)
   , lookat_target(0.0f, 0.0f, 0.0f)
   , up_vector(0.0f, 1.0f, 0.0f)
   , radius{8.0f}
   , azimuth_rad{0.5f}
   , polar_rad{1.2f}
   , translation_matrix()
{
   updateArcBall();
}

void Camera::setAspectRatio(int w, int h )
{
   projection_matrix.setToIdentity();
   projection_matrix.perspective(
            50.0f,
            static_cast<float>(w)/static_cast<float>(h),
            0.001f,
            100);
}

void Camera::rotate(float dx, float dy)
{
   float scale_rotation = 0.01f;
   azimuth_rad += -(dx * scale_rotation);
   polar_rad += -(dy * scale_rotation);
   polar_rad = qBound(0.01f, polar_rad, 3.14f);
   updateArcBall();
}

void Camera::translate(float dx, float dy)
{
   float scale_translation = 0.005f;
   translation_matrix.translate(dx*scale_translation, -dy*scale_translation);
   updateArcBall();
}

void Camera::setRadius(int scroll_angleDelta)
{
   float scale_scroll = 0.5f;
   float scroll_step = scroll_angleDelta / abs(scroll_angleDelta);
   radius += scroll_step * scale_scroll;
   updateArcBall();
}

QVector3D Camera::getPosition()
{
   return position;
}

void Camera::updateArcBall()
{
   position.setX(radius * qSin(azimuth_rad) * qSin(polar_rad));
   position.setY(radius * qCos(polar_rad));
   position.setZ(radius * qCos(azimuth_rad) * qSin(polar_rad));
   view_matrix.setToIdentity();
   view_matrix.lookAt(position, lookat_target, up_vector);
   view_matrix = translation_matrix * view_matrix;
}

