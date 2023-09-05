#pragma once

#include "hmm/HandmadeMath.h"

namespace flip {

const float kPi = HMM_PI32;
const float k2Pi = HMM_PI32 * 2;
const float kPi_2 = HMM_PI32 / 2;

const HMM_Vec3 kUnitX = {1, 0, 0};
const HMM_Vec3 kUnitY = {0, 1, 0};
const HMM_Vec3 kUnitZ = {0, 0, 1};

const HMM_Mat2 kIdentity2 = HMM_M2D(1);
const HMM_Mat3 kIdentity3 = HMM_M3D(1);
const HMM_Mat4 kIdentity4 = HMM_M4D(1);
}  // namespace flip