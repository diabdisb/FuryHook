
#pragma once

#define  IMGUI_DEFINE_MATH_OPERATORS
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include <windows.h>

#include <map>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

#include "../Overlay/hashes.hpp"
#include <d3d11.h>
#pragma comment( lib, "D3DX11.lib" )

using namespace ImGui;
using namespace std;

#define vec4( r, g, b, a ) ImColor( r / 255.f, g / 255.f, b / 255.f, a )

class c_tab {
public:
    const char* icon, * name;
};

namespace ui {

    inline ImVec2 window_size = ImVec2(600, 520);
    inline float anim = 0.f, anim_destination = 1.f;
    inline int cur_tab = 0, new_tab = 0;
    inline bool should_change_tab = false;
    inline vector< c_tab > tabs = { { ICON_FA_CROSSHAIRS, "Aimbot" }, { ICON_FA_EYE, "Visuals" }, { ICON_FA_GLOBE, "Online" }, { ICON_FA_USER, "Self" }, { ICON_FA_GEAR, "Settings" }, { ICON_FA_FOLDER, "Configs" }, { ICON_FA_CODE, "Lua scripts" } };

    inline ID3D11ShaderResourceView* avatar = nullptr;
    inline std::string username = "neverlose";

    inline void handle_anims() {

        ui::anim = ImLerp(ui::anim, ui::anim_destination, 0.5f);

        if (!should_change_tab)
            return;

        if (anim < anim_destination + 0.01f)
            cur_tab = new_tab, anim_destination = 1.f, should_change_tab = false;
    }

    void ImRotateStart();
    ImVec2 ImRotationCenter();
    void ImRotateEnd(float rad, ImVec2 center = ImRotationCenter());

    void render_circle_for_horizontal_bar(ImVec2 pos, ImColor color, float alpha);

    bool tab(const char* icon, bool cur);

    void begin_child(const char* name, ImVec2 size);
    void end_child();

    bool selectable(const char* label, bool selected, ImGuiSelectableFlags flags, const ImVec2& size_arg);
    bool selectable(const char* label, bool* p_selected, ImGuiSelectableFlags flags = 0, const ImVec2& size_arg = ImVec2(0, 0));

    void input(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags = 0);

    inline float accent_c[4] = { 128 / 255.f, 184 / 255.f, 208 / 255.f, 1.f };
    inline ImColor get_accent_color(float a = 1.f) {

        return ImColor(accent_c[0], accent_c[1], accent_c[2], a);
    }
}
