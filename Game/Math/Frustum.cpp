#include "Frustum.h"

using namespace Math;

Frustum::Frustum(const Matrix4x4& projectionMatrix) {
    const Vector3 basePoints[]{
        { -1.0f, -1.0f, 0.0f },
        { -1.0f,  1.0f, 0.0f },
        {  1.0f, -1.0f, 0.0f },
        {  1.0f,  1.0f, 0.0f },
        { -1.0f, -1.0f, 1.0f },
        { -1.0f,  1.0f, 1.0f },
        {  1.0f, -1.0f, 1.0f },
        {  1.0f,  1.0f, 1.0f },
    };

    Matrix4x4 projectionMatrixInverse = projectionMatrix.Inverse();

    for (int i = 0; i < NumCorners; ++i) {
        corners_[i] = projectionMatrixInverse.ApplyTransformWDivide(basePoints[i]);
    }

    planes_[NearPlane] = Plane(Triangle(corners_[NearLowerRight], corners_[NearUpperRight], corners_[NearUpperLeft]));
    planes_[FarPlane] = Plane(Triangle(corners_[FarLowerLeft], corners_[FarUpperLeft], corners_[FarUpperRight]));
    planes_[LeftPlane] = Plane(Triangle(corners_[NearLowerLeft], corners_[NearUpperLeft], corners_[FarUpperLeft]));
    planes_[RightPlane] = Plane(Triangle(corners_[FarLowerRight], corners_[FarUpperRight], corners_[NearUpperRight]));
    planes_[TopPlane] = Plane(Triangle(corners_[FarUpperLeft], corners_[NearUpperLeft], corners_[NearUpperRight]));
    planes_[BottomPlane] = Plane(Triangle(corners_[NearLowerLeft], corners_[FarLowerLeft], corners_[FarLowerRight]));
}

Math::AABB Frustum::CalcAABB() const {
    Math::AABB aabb(corners_[0]);
    for (int i = 1; i < NumCorners; ++i) {
        aabb.Merge(corners_[i]);
    }
    return aabb;
}

Frustum Frustum::Transform(const Quaternion& rotate, const Vector3& translate) const {
    return Transform(Matrix4x4::MakeAffineTransform(Vector3::one, rotate, translate));
}

Frustum Frustum::Transform(const Matrix4x4& matrix) const {
    Frustum frustum;
    for (int i = 0; i < NumCorners; ++i) {
        frustum.corners_[i] = corners_[i] * matrix;
    }
    for (int i = 0; i < NumPlanes; ++i) {
        Vector4 vPlane = Vector4(planes_[i].normal, planes_[i].distance) * matrix;
        frustum.planes_[i] = Plane(vPlane.GetXYZ(), vPlane.w);
    }
    return frustum;
}
