#pragma once

#include "Geometry.h"

class Frustum {
public:
    enum CornerID {
        NearLowerLeft, NearUpperLeft, NearLowerRight, NearUpperRight,
        FarLowerLeft, FarUpperLeft, FarLowerRight, FarUpperRight,
        NumCorners
    };

    enum PlaneID {
        NearPlane, FarPlane, LeftPlane, RightPlane, TopPlane, BottomPlane,
        NumPlanes
    };

    Frustum() = default;
    explicit Frustum(const Matrix4x4& projectionMatrix);

    Math::AABB CalcAABB() const;
    float CalcNearClip() const;
    float CalcFarClip() const;

    Frustum Transform(const Quaternion& rotate, const Vector3& translate) const;
    Frustum Transform(const Matrix4x4& matrix) const;

    const Vector3& GetCorner(CornerID id) const { return corners_[id]; }
    const Math::Plane& GetPlane(PlaneID id) const { return planes_[id]; }

private:
    Vector3 corners_[NumCorners]; // 頂点
    Math::Plane planes_[NumPlanes];     // 平面
};