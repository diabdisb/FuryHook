#include "ui.hpp"

int rotation_start_index;
void ui::ImRotateStart()
{
    rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
}

ImVec2 ui::ImRotationCenter()
{
    ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX); // bounds

    const auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
    for (int i = rotation_start_index; i < buf.Size; i++)
        l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

    return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2); // or use _ClipRectStack?
}

void ui::ImRotateEnd(float rad, ImVec2 center)
{
    float s = sin(rad), c = cos(rad);
    center = ImRotate(center, s, c) - center;

    auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
    for (int i = rotation_start_index; i < buf.Size; i++)
        buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
}

void ui::render_circle_for_horizontal_bar(ImVec2 pos, ImColor color, float alpha) {

    auto draw = GetWindowDrawList();
    draw->AddCircleFilled(pos, 6, ImColor(color.Value.x, color.Value.y, color.Value.z, alpha * GetStyle().Alpha));
}

bool ui::tab(const char* icon, bool cur) {

    auto window = GetCurrentWindow();
    auto id = window->GetID(std::string(icon).append(".TAB").c_str());

    auto pos = window->DC.CursorPos;
    auto draw = window->DrawList;

    ImVec2 size = ImVec2(55, 44);
    ImRect bb = ImRect(pos, pos + size);
    ItemAdd(bb, id);
    ItemSize(bb, GetStyle().FramePadding.y);

    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);

    static std::unordered_map< ImGuiID, float > values;
    auto value = values.find(id);
    if (value == values.end()) {

        values.insert({ id, 0.f });
        value = values.find(id);
    }

    value->second = ImLerp(value->second, (cur ? 1.f : hovered ? 0.5f : 0.f), 0.05f);

    draw->AddRectFilled(bb.Min, bb.Max, GetColorU32(ImGuiCol_Text, 0.02f * value->second));

    static float line_pos = 0.f;
    line_pos = ImLerp(line_pos, cur ? bb.Min.y - window->Pos.y : line_pos, 0.1f);
    draw->AddRectFilled(ImVec2(bb.Min.x, window->Pos.y + line_pos), ImVec2(bb.Min.x + 3, window->Pos.y + line_pos + size.y), ui::get_accent_color(GetStyle().Alpha));

    draw->AddText(bb.GetCenter() - CalcTextSize(icon) / 2, ui::get_accent_color(GetStyle().Alpha * (value->second > 0.25f ? value->second : 0.25f)), icon);

    return pressed;
}

void ui::begin_child(const char* name, ImVec2 size) {

    ImGuiWindow* window = GetCurrentWindow();
    ImVec2 pos = window->DC.CursorPos;

    BeginChild(std::string(name).append(".main").c_str(), size, false, ImGuiWindowFlags_NoScrollbar);

    GetWindowDrawList()->AddRectFilled(pos, pos + size, vec4(12, 15, 16, ui::anim), GetStyle().WindowRounding);
    GetWindowDrawList()->AddText(GetIO().Fonts->Fonts[1], GetIO().Fonts->Fonts[1]->FontSize, pos + ImVec2(12, 13), ui::get_accent_color(ui::anim), name, FindRenderedTextEnd(name));

    PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 12 });
    SetCursorPosY(26);
    BeginChild(name, { size.x, size.y - 26 }, 0, ImGuiWindowFlags_AlwaysUseWindowPadding);

    SetCursorPosX(12);
    BeginGroup();

    PushStyleVar(ImGuiStyleVar_Alpha, ui::anim);
    PushStyleVar(ImGuiStyleVar_ItemSpacing, { 8, 9 });
}

void ui::end_child() {

    PopStyleVar(3);
    EndGroup();
    EndChild();
    EndChild();
}

struct selectable_t {

    float checkbox, box;
};

bool ui::selectable(const char* label, bool selected, ImGuiSelectableFlags flags, const ImVec2& size_arg) {

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    // Submit label or explicit size to ItemSize(), whereas ItemAdd() will submit a larger/spanning rectangle.
    ImGuiID id = window->GetID(label);
    ImVec2 label_size = CalcTextSize(label, NULL, true);
    ImVec2 size(size_arg.x != 0.0f ? size_arg.x : label_size.x, size_arg.y != 0.0f ? size_arg.y : label_size.y);
    ImVec2 pos = window->DC.CursorPos;
    pos.y += window->DC.CurrLineTextBaseOffset;
    ItemSize(size, 0.0f);

    // Fill horizontal space
    // We don't support (size < 0.0f) in Selectable() because the ItemSpacing extension would make explicitly right-aligned sizes not visibly match other widgets.
    const bool span_all_columns = (flags & ImGuiSelectableFlags_SpanAllColumns) != 0;
    const float min_x = span_all_columns ? window->ParentWorkRect.Min.x : pos.x;
    const float max_x = span_all_columns ? window->ParentWorkRect.Max.x : window->WorkRect.Max.x;
    if (size_arg.x == 0.0f || (flags & ImGuiSelectableFlags_SpanAvailWidth))
        size.x = ImMax(label_size.x, max_x - min_x);

    // Text stays at the submission position, but bounding box may be extended on both sides
    const ImVec2 text_min = pos;
    const ImVec2 text_max(min_x + size.x, pos.y + size.y);

    // Selectables are meant to be tightly packed together with no click-gap, so we extend their box to cover spacing between selectable.
    ImRect bb(min_x, pos.y, text_max.x, text_max.y);
    if ((flags & ImGuiSelectableFlags_NoPadWithHalfSpacing) == 0)
    {
        const float spacing_x = span_all_columns ? 0.0f : style.ItemSpacing.x;
        const float spacing_y = style.ItemSpacing.y;
        const float spacing_L = IM_FLOOR(spacing_x * 0.50f);
        const float spacing_U = IM_FLOOR(spacing_y * 0.50f);
        bb.Min.x -= spacing_L;
        bb.Min.y -= spacing_U;
        bb.Max.x += (spacing_x - spacing_L);
        bb.Max.y += (spacing_y - spacing_U);
    }
    //if (g.IO.KeyCtrl) { GetForegroundDrawList()->AddRect(bb.Min, bb.Max, IM_COL32(0, 255, 0, 255)); }

    // Modify ClipRect for the ItemAdd(), faster than doing a PushColumnsBackground/PushTableBackground for every Selectable..
    const float backup_clip_rect_min_x = window->ClipRect.Min.x;
    const float backup_clip_rect_max_x = window->ClipRect.Max.x;
    if (span_all_columns)
    {
        window->ClipRect.Min.x = window->ParentWorkRect.Min.x;
        window->ClipRect.Max.x = window->ParentWorkRect.Max.x;
    }

    const bool disabled_item = (flags & ImGuiSelectableFlags_Disabled) != 0;
    const bool item_add = ItemAdd(bb, id, NULL, disabled_item ? ImGuiItemFlags_Disabled : ImGuiItemFlags_None);
    if (span_all_columns)
    {
        window->ClipRect.Min.x = backup_clip_rect_min_x;
        window->ClipRect.Max.x = backup_clip_rect_max_x;
    }

    if (!item_add)
        return false;

    const bool disabled_global = (g.CurrentItemFlags & ImGuiItemFlags_Disabled) != 0;
    if (disabled_item && !disabled_global) // Only testing this as an optimization
        BeginDisabled();

    // FIXME: We can standardize the behavior of those two, we could also keep the fast path of override ClipRect + full push on render only,
    // which would be advantageous since most selectable are not selected.
    if (span_all_columns && window->DC.CurrentColumns)
        PushColumnsBackground();
    else if (span_all_columns && g.CurrentTable)
        TablePushBackgroundChannel();

    // We use NoHoldingActiveID on menus so user can click and _hold_ on a menu then drag to browse child entries
    ImGuiButtonFlags button_flags = 0;
    if (flags & ImGuiSelectableFlags_NoHoldingActiveID) { button_flags |= ImGuiButtonFlags_NoHoldingActiveId; }
    if (flags & ImGuiSelectableFlags_NoSetKeyOwner) { button_flags |= ImGuiButtonFlags_NoSetKeyOwner; }
    if (flags & ImGuiSelectableFlags_SelectOnClick) { button_flags |= ImGuiButtonFlags_PressedOnClick; }
    if (flags & ImGuiSelectableFlags_SelectOnRelease) { button_flags |= ImGuiButtonFlags_PressedOnRelease; }
    if (flags & ImGuiSelectableFlags_AllowDoubleClick) { button_flags |= ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnDoubleClick; }
   // if ((flags & ImGuiSelectableFlags_AllowOverlap) || (g.LastItemData.InFlags & ImGuiItemFlags_AllowOverlap)) { button_flags |= ImGuiButtonFlags_AllowOverlap; }

    const bool was_selected = selected;
    bool hovered, held;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held, button_flags);

    // Auto-select when moved into
    // - This will be more fully fleshed in the range-select branch
    // - This is not exposed as it won't nicely work with some user side handling of shift/control
    // - We cannot do 'if (g.NavJustMovedToId != id) { selected = false; pressed = was_selected; }' for two reasons
    //   - (1) it would require focus scope to be set, need exposing PushFocusScope() or equivalent (e.g. BeginSelection() calling PushFocusScope())
    //   - (2) usage will fail with clipped items
    //   The multi-select API aim to fix those issues, e.g. may be replaced with a BeginSelection() API.
    if ((flags & ImGuiSelectableFlags_SelectOnNav) && g.NavJustMovedToId != 0 && g.NavJustMovedToFocusScopeId == g.CurrentFocusScopeId)
        if (g.NavJustMovedToId == id)
            selected = pressed = true;

    // Update NavId when clicking or when Hovering (this doesn't happen on most widgets), so navigation can be resumed with gamepad/keyboard
    if (pressed || (hovered && (flags & ImGuiSelectableFlags_SetNavIdOnHover)))
    {
        if (!g.NavDisableMouseHover && g.NavWindow == window && g.NavLayer == window->DC.NavLayerCurrent)
        {
            SetNavID(id, window->DC.NavLayerCurrent, g.CurrentFocusScopeId, WindowRectAbsToRel(window, bb)); // (bb == NavRect)
            g.NavDisableHighlight = true;
        }
    }
    if (pressed)
        MarkItemEdited(id);

    // In this branch, Selectable() cannot toggle the selection so this will never trigger.
    if (selected != was_selected) //-V547
        g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_ToggledSelection;

    static std::unordered_map< ImGuiID, selectable_t > values;
    auto value = values.find(id);
    if (value == values.end()) {

        values.insert({ id, { 0.f, 0.f } });
        value = values.find(id);
    }

    value->second.checkbox = ImLerp(value->second.checkbox, (float)selected, 0.09f);
    value->second.box = ImLerp(value->second.box, (selected ? 1.f : hovered ? 0.5f : 0.f), 0.05f);

    // Render
    RenderFrame(bb.Min, bb.Max, GetColorU32(ImGuiCol_Text, 0.05f * value->second.box));

    if (span_all_columns && window->DC.CurrentColumns)
        PopColumnsBackground();
    else if (span_all_columns && g.CurrentTable)
        TablePopBackgroundChannel();

    ImVec2 checkbox_size = ImVec2(14, 14);
    window->DrawList->AddRectFilled(ImVec2(bb.Min.x + style.ItemInnerSpacing.x + 6.f, bb.GetCenter().y - checkbox_size.y / 2), ImVec2(bb.Min.x + style.ItemInnerSpacing.x + 6.f + checkbox_size.x, bb.GetCenter().y + checkbox_size.y / 2), vec4(12, 15, 16, style.Alpha), 2.f);
    window->DrawList->AddRect(ImVec2(bb.Min.x + style.ItemInnerSpacing.x + 6.f, bb.GetCenter().y - checkbox_size.y / 2), ImVec2(bb.Min.x + style.ItemInnerSpacing.x + 6.f + checkbox_size.x, bb.GetCenter().y + checkbox_size.y / 2), ImColor(1.f, 1.f, 1.f, 0.07f * style.Alpha), 2.f);
    window->DrawList->AddRectFilled(ImVec2(bb.Min.x + style.ItemInnerSpacing.x + 6.f, bb.GetCenter().y - checkbox_size.y / 2), ImVec2(bb.Min.x + style.ItemInnerSpacing.x + 6.f + checkbox_size.x, bb.GetCenter().y + checkbox_size.y / 2), ui::get_accent_color(style.Alpha * value->second.checkbox), 2.f);
    RenderCheckMark(window->DrawList, ImRect(bb.Min.x + style.ItemInnerSpacing.x + 6.f, bb.GetCenter().y - checkbox_size.y / 2, bb.Min.x + style.ItemInnerSpacing.x + 6.f + checkbox_size.x, bb.GetCenter().y + checkbox_size.y / 2).GetCenter() - ImVec2(6.f / 2.f, 6.f / 2.f), GetColorU32(ImGuiCol_WindowBg, value->second.checkbox), 6.f);

    PushStyleColor(ImGuiCol_Text, GetColorU32(ImGuiCol_Text, value->second.box > 0.5f ? value->second.box : 0.5f));
    RenderText(ImVec2(bb.Min.x + style.ItemInnerSpacing.x + 25.f, bb.GetCenter().y - label_size.y / 2.f), label, FindRenderedTextEnd(label));
    PopStyleColor();

    // Automatically close popups
    if (pressed && (window->Flags & ImGuiWindowFlags_Popup) && !(flags & ImGuiSelectableFlags_DontClosePopups) && !(g.LastItemData.InFlags & ImGuiItemFlags_SelectableDontClosePopup))
        CloseCurrentPopup();

    if (disabled_item && !disabled_global)
        EndDisabled();

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
    return pressed; //-V1020
}

bool ui::selectable(const char* label, bool* p_selected, ImGuiSelectableFlags flags, const ImVec2& size_arg)
{
    if (ui::selectable(label, *p_selected, flags, size_arg))
    {
        *p_selected = !*p_selected;
        return true;
    }
    return false;
}

struct input_t {

    float icon;
    ImVec4 frame_color;
};

void ui::input(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags) {

    ImGuiWindow* window = GetCurrentWindow();
    ImGuiID id = window->GetID(label);

    auto pos = window->DC.CursorPos;

    ImVec2 label_size = CalcTextSize(label, 0, 1);
    ImVec2 icon_size = GetIO().Fonts->Fonts[2]->CalcTextSizeA(GetIO().Fonts->Fonts[2]->FontSize, FLT_MAX, 0.f, ICON_FA_PENCIL);

    ImVec2 size = ImVec2(GetWindowWidth() - 24, 27);

    ImRect bb(pos, pos + size + ImVec2(0, label_size.y + 5));
    ImRect frame_bb(pos + ImVec2(0, label_size.y + 5), pos + size + ImVec2(0, label_size.y + 5));
    ItemAdd(bb, id);
    ItemSize(size, GetStyle().FramePadding.y);

    static std::unordered_map < ImGuiID, input_t > values;
    auto value = values.find(id);
    if (value == values.end()) {

        values.insert({ id, { 0.f, ImVec4(18 / 255.f, 22 / 255.f, 23 / 255.f, 1.f) } });
        value = values.find(id);
    }

    RenderFrame(frame_bb.Min, frame_bb.Max, ImColor(value->second.frame_color.x, value->second.frame_color.y, value->second.frame_color.z, value->second.frame_color.w * GetStyle().Alpha), 0, 2.f);

    PushStyleColor(ImGuiCol_FrameBg, GetColorU32(ImGuiCol_FrameBg, 0.f));

    SetCursorPos(GetCursorPos() - ImVec2(-24, 5 + GetStyle().ItemSpacing.y));
    InputTextEx(std::string("##").append(label).append(".input").c_str(), NULL, buf, buf_size, ImVec2(size.x - 26, 20), flags);

    PopStyleColor();

    value->second.frame_color = ImLerp(value->second.frame_color, (IsItemActive() ? ImVec4(23 / 255.f, 27 / 255.f, 28 / 255.f, 1.f) : ImVec4(18 / 255.f, 22 / 255.f, 23 / 255.f, 1.f)), 0.07f);
    value->second.icon = ImLerp(value->second.icon, (IsItemActive() ? 1.f : IsItemHovered() ? 0.75f : 0.5f), 0.07f);

    window->DrawList->AddText(GetIO().Fonts->Fonts[2], GetIO().Fonts->Fonts[2]->FontSize, ImVec2(frame_bb.Min.x + GetStyle().ItemInnerSpacing.x + 6.f, frame_bb.GetCenter().y - icon_size.y / 2 - 2.f), GetColorU32(ImGuiCol_Text, value->second.icon), ICON_FA_PENCIL);
    window->DrawList->AddText(bb.Min, GetColorU32(ImGuiCol_Text), label);

    window->DC.CursorPos.y = bb.Max.y + GetStyle().ItemSpacing.y;

}
