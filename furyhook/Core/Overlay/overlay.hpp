#pragma once
#include <Windows.h>  
#include <iostream>
#include <d3d11.h>
#include <dwmapi.h>
#include <chrono>


//#include "../../memory/camm.h"


#include "skycrypt.hpp"

#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>
#include "../../main.h"

inline ID3D11Device* p_device = nullptr;
inline ID3D11DeviceContext* d3d_device_ctx = nullptr;
inline IDXGISwapChain* d3d_swap_chain = nullptr;
inline ID3D11RenderTargetView* d3d_render_target = nullptr;

inline MSG messager = { NULL };
inline HWND my_wnd = NULL;
inline RECT GameRect = { NULL };

inline HWND GameWnd = NULL;

inline DWORD ScreenCenterX2;
inline DWORD ScreenCenterY2;

static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;

static ULONG Width = GetSystemMetrics(SM_CXSCREEN);
static ULONG Height = GetSystemMetrics(SM_CYSCREEN);


inline float scale = 1.0f;
inline const float ImageSize = 30;
inline const float ImageSpace = 70;

inline ImFont* main_font = nullptr;

#include <deque>
#include <string>

namespace font
{
	inline ImFont* inter_bold = nullptr;
	inline ImFont* inter_default = nullptr;
	inline ImFont* icon = nullptr;
}

inline std::deque<std::string> debug_logs;
constexpr size_t max_log_size = 15;

inline void LogDebug(const std::string& msg) {
    if (debug_logs.size() >= max_log_size)
        debug_logs.pop_front();
    debug_logs.push_back(msg);
}

auto get_process_wnd(uint32_t pid) -> HWND;
auto set_window_target() -> void;
auto setup_window() -> void;
HRESULT directx_init();
void CreateRenderTarget();
void CleanupRenderTarget();
WPARAM render_loop();