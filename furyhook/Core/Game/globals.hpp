#pragma once

inline uint64_t GameAssembly;


namespace globals
{ 
	inline const int MaxFeatures = 100;
inline bool features[MaxFeatures];
inline bool FillBox = true;
inline bool CornerBox = false;
inline bool NormalBox = true;
inline int hitboxtype = 0;
inline float fov = 100;
inline bool draw_fov = true;
inline bool closest_bone = false;
inline Vector2 screen_center = { static_cast<float>(GetSystemMetrics(SM_CXSCREEN)) / 2, static_cast<float>(GetSystemMetrics(SM_CYSCREEN)) / 2 };

}
inline float dpi_scale = 1.0f;