#pragma once
#include "../imports.h"

auto EntityThread() -> void;
inline Vector3 GetTransformPosition(uint64_t transform);
inline Vector3 GetBoneLocation(uintptr_t player, int boneIndex);