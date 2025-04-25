#pragma once

#undef min


inline void DrawLine(const ImVec2& x, const ImVec2 y, ImU32 color, int width)
{
    //ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x.x + 1, x.y + 1), ImVec2(y.x + 1, y.y + 1), ImColor(25, 25, 25, 255), width);
    //ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x.x - 1, x.y - 1), ImVec2(y.x - 1, y.y - 1), ImColor(25, 25, 25, 255), width);
    ImGui::GetBackgroundDrawList()->AddLine(x, y, color, width);
}
inline auto Draw2DBox(float x, float y, float w, float h, ImColor color, int thickness) -> void
{
    DrawLine(ImVec2(x, y), ImVec2(x + w, y), color, thickness); // top 
    DrawLine(ImVec2(x, y - 1.3f), ImVec2(x, y + h + thickness), color, thickness); // left
    DrawLine(ImVec2(x + w, y - 1.3f), ImVec2(x + w, y + h + thickness), color, thickness);  // right
    DrawLine(ImVec2(x, y + h), ImVec2(x + w, y + h), color, thickness);   // bottom 
}

inline void DrawCorneredBox(int X, int Y, int W, int H, ImColor color, int thickness)
{
    float lineW = (W / 3);
    float lineH = (H / 3);
    DrawLine(ImVec2(X, Y), ImVec2(X, Y + lineH), color, thickness);
    DrawLine(ImVec2(X, Y), ImVec2(X + lineW, Y), color, thickness);
    DrawLine(ImVec2(X + W - lineW, Y), ImVec2(X + W, Y), color, thickness);
    DrawLine(ImVec2(X + W, Y), ImVec2(X + W, Y + lineH), color, thickness);
    DrawLine(ImVec2(X, Y + H - lineH), ImVec2(X, Y + H), color, thickness);
    DrawLine(ImVec2(X, Y + H), ImVec2(X + lineW, Y + H), color, thickness);
    DrawLine(ImVec2(X + W - lineW, Y + H), ImVec2(X + W, Y + H), color, thickness);
    DrawLine(ImVec2(X + W, Y + H - lineH), ImVec2(X + W, Y + H), color, thickness);
}


inline void DrawString(float fontSize, int x, int y, ImColor color, bool bCenter, bool stroke, const char* pText, ImDrawList* drawList = ImGui::GetBackgroundDrawList(), ...)
{
    va_list va_alist;
    char buf[1024] = { 0 };
    va_start(va_alist, pText);
    _vsnprintf_s(buf, sizeof(buf), pText, va_alist);
    va_end(va_alist);
    std::string text = buf;
    if (bCenter)
    {
        float offsetX = 0.0f;
        float offsetY = 0.0f;

        ImFont* font = main_font;
        ImVec2 textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, text.c_str());
        offsetX = textSize.x * 0.5f;
        offsetY = textSize.y;
        x -= offsetX;
        y -= offsetY;
    }
    if (stroke)
    {
        drawList->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 1, y + 1), ImColor(25, 25, 25, 255), text.c_str());
        drawList->AddText(ImGui::GetFont(), fontSize, ImVec2(x - 1, y - 1), ImColor(25, 25, 25, 255), text.c_str());
        drawList->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 1, y - 1), ImColor(25, 25, 25, 255), text.c_str());
        drawList->AddText(ImGui::GetFont(), fontSize, ImVec2(x - 1, y + 1), ImColor(25, 25, 25, 255), text.c_str());
    }
    drawList->AddText(ImGui::GetFont(), fontSize, ImVec2(x, y), color, text.c_str());
}

inline void playerinfo(std::string targetName, Cloth clothing[7], Belt belt[7], const std::string& highlightName)
{
    // Check if all belt and clothing slots are empty
    bool allEmpty = true;
    for (int i = 0; i < 7; ++i) {
        if (!belt[i].Name.empty() || !clothing[i].Name.empty()) {
            allEmpty = false;
            break;
        }
    }
    if (allEmpty) return;

    ImDrawList* drawList = ImGui::GetBackgroundDrawList();

    // Adjust positions to center content better
    const float panelWidth = 500.0f * scale;
    const float panelHeight = 250.0f * scale;

    // Create properly centered panel
    ImVec2 panelStart = ImVec2(
        ImGui::GetIO().DisplaySize.x * 0.02f,  // ~2% from the left edge
        ImGui::GetIO().DisplaySize.y * 0.02f   // ~2% from the top edge
    );
    ImVec2 panelEnd = ImVec2(panelStart.x + panelWidth, panelStart.y + panelHeight);

    // Modern color scheme
    ImColor bgColor(18, 18, 24, 220);
    ImColor headerColor(30, 30, 40, 240);
    ImColor accentColor(65, 105, 225, 255);
    ImColor textColor(240, 240, 245, 255);
    ImColor healthFillColor(87, 218, 127, 255);
    ImColor healthBgColor(87, 218, 127, 60);
    ImColor itemBorderColor(70, 70, 90, 220);
    ImColor highlightColor(65, 105, 225, 180);

    const float cornerRadius = 8.0f * scale;

    // Main panel background with rounded corners
    drawList->AddRectFilled(panelStart, panelEnd, bgColor, cornerRadius);

    // Header bar with rounded top corners
    drawList->AddRectFilled(
        panelStart,
        ImVec2(panelEnd.x, panelStart.y + (30 * scale)),
        headerColor, cornerRadius);

    // Add a subtle accent line under the header
    drawList->AddRectFilled(
        ImVec2(panelStart.x, panelStart.y + (30 * scale)),
        ImVec2(panelEnd.x, panelStart.y + (32 * scale)),
        accentColor);

    // "PLAYER INFO" header text - centered
    float headerTextWidth = ImGui::CalcTextSize("PLAYER INFO: %s", targetName.c_str()).x * (24 * scale / ImGui::GetFontSize());

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "PLAYER INFO: %s", targetName.c_str());

    DrawString(24 * scale,
        panelStart.x + (panelWidth - headerTextWidth) / 4,
        panelStart.y + 30 * scale,
        textColor, false, true, buffer);

    // Player name - properly positioned below header


    const float scaledImageSize = ImageSize * scale;
    const float scaledImageSpace = ImageSpace * scale;

    // === CLOTHING SLOTS - REPOSITIONED ===
    {
        float sectionY = panelStart.y + 80 * scale;
        ImVec2 rectStart = { panelStart.x + 20 * scale, sectionY };
        ImVec2 rectEnd = { panelEnd.x - 20 * scale, sectionY + 2 * scaledImageSize };

        // Add a section label above the clothing slots - centered
        float labelWidth = ImGui::CalcTextSize("CLOTHING").x * (22 * scale / ImGui::GetFontSize());
        DrawString(22 * scale,
            panelStart.x + (panelWidth - labelWidth) / 2,
            sectionY - 25 * scale,
            textColor, false, true, "CLOTHING");

        // Add subtle background to the entire clothing section
        drawList->AddRectFilled(
            ImVec2(rectStart.x - 5 * scale, rectStart.y - 5 * scale),
            ImVec2(rectEnd.x + 5 * scale, rectEnd.y + 5 * scale),
            ImColor(30, 30, 40, 120), cornerRadius);

        // Calculate space for each slot
        float availableWidth = rectEnd.x - rectStart.x;
        float slotWidth = availableWidth / 7; // For 7 clothing slots

        for (int i = 0; i < 7; ++i) {
            ImVec2 drawStart = { rectStart.x + i * slotWidth, rectStart.y };
            ImVec2 drawEnd = { drawStart.x + slotWidth - 2 * scale, rectEnd.y };

            const auto& item = clothing[i];
            const auto& texture = texture_cache.get_texture(item.Name.c_str());

            // Slot background
            drawList->AddRectFilled(drawStart, drawEnd, ImColor(40, 40, 50, 180), cornerRadius * 0.5f);

            if (texture.texture) {
                // Item image
                drawList->AddImage((void*)texture.texture,
                    ImVec2(drawStart.x + 4 * scale, drawStart.y + 4 * scale),
                    ImVec2(drawEnd.x - 4 * scale, drawEnd.y - 4 * scale));

                // Health bar
                if (item.Health > 0 && item.MaxHealth > 0.001f) {
                    float healthRatio = std::min(item.Health / item.MaxHealth, 1.0f);
                    const float barHeight = 8 * scale;
                    const float barPadding = 4 * scale;

                    ImVec2 barStart = ImVec2(drawStart.x + barPadding, drawEnd.y - barHeight - barPadding);
                    ImVec2 barEnd = ImVec2(drawEnd.x - barPadding, drawEnd.y - barPadding);

                    drawList->AddRectFilled(barStart, barEnd, healthBgColor, barHeight * 0.5f);

                    ImVec2 fillEnd = barEnd;
                    fillEnd.x = barStart.x + (barEnd.x - barStart.x) * healthRatio;
                    drawList->AddRectFilled(barStart, fillEnd, healthFillColor, barHeight * 0.5f);
                }
            }

            drawList->AddRect(drawStart, drawEnd, itemBorderColor, cornerRadius * 0.5f);
        }
    }

    // === BELT SLOTS - REPOSITIONED ===
    {
        float sectionY = panelStart.y + 180 * scale;
        ImVec2 rectStart = { panelStart.x + 20 * scale, sectionY };
        ImVec2 rectEnd = { panelEnd.x - 20 * scale, sectionY + 2 * scaledImageSize };

        // Add a section label above the belt slots - centered
        float labelWidth = ImGui::CalcTextSize("INVENTORY").x * (22 * scale / ImGui::GetFontSize());
        DrawString(22 * scale,
            panelStart.x + (panelWidth - labelWidth) / 2,
            sectionY - 25 * scale,
            textColor, false, true, "INVENTORY");

        // Add subtle background to the entire belt section
        drawList->AddRectFilled(
            ImVec2(rectStart.x - 5 * scale, rectStart.y - 5 * scale),
            ImVec2(rectEnd.x + 5 * scale, rectEnd.y + 5 * scale),
            ImColor(30, 30, 40, 120), cornerRadius);

        // Calculate space for each slot
        float availableWidth = rectEnd.x - rectStart.x;
        float slotWidth = availableWidth / 6; // For 6 belt slots

        for (int i = 0; i < 6; ++i) {
            ImVec2 drawStart = { rectStart.x + i * slotWidth, rectStart.y };
            ImVec2 drawEnd = { drawStart.x + slotWidth - 2 * scale, rectEnd.y };

            const auto& item = belt[i];
            const auto& texture = texture_cache.get_texture(item.Name.c_str());
            bool isHighlight = (!highlightName.empty() && item.Name == highlightName);

            // Slot background
            drawList->AddRectFilled(drawStart, drawEnd,
                isHighlight ? highlightColor : ImColor(40, 40, 50, 180),
                cornerRadius * 0.5f);

            if (texture.texture) {
                // Item image with padding
                drawList->AddImage((void*)texture.texture,
                    ImVec2(drawStart.x + 4 * scale, drawStart.y + 4 * scale),
                    ImVec2(drawEnd.x - 4 * scale, drawEnd.y - 4 * scale));

                // Health bar
                if (item.Health > 0 && item.MaxHealth > 0.001f) {
                    float healthRatio = std::min(item.Health / item.MaxHealth, 1.0f);
                    const float barHeight = 8 * scale;
                    const float barPadding = 4 * scale;

                    ImVec2 barStart = ImVec2(drawStart.x + barPadding, drawEnd.y - barHeight - barPadding);
                    ImVec2 barEnd = ImVec2(drawEnd.x - barPadding, drawEnd.y - barPadding);

                    drawList->AddRectFilled(barStart, barEnd, healthBgColor, barHeight * 0.5f);

                    ImVec2 fillEnd = barEnd;
                    fillEnd.x = barStart.x + (barEnd.x - barStart.x) * healthRatio;
                    drawList->AddRectFilled(barStart, fillEnd, healthFillColor, barHeight * 0.5f);
                }

                // Item count with improved visibility
                if (item.Count > 1) {
                    // Background pill for the count 
                    ImVec2 countPos = ImVec2(drawEnd.x - 20 * scale, drawStart.y + 6 * scale);
                    float textWidth = ImGui::CalcTextSize(("x" + std::to_string((int)item.Count)).c_str()).x * (18 * scale / ImGui::GetFontSize());
                    float pillWidth = textWidth + 12 * scale;

                    drawList->AddRectFilled(
                        ImVec2(countPos.x - pillWidth, countPos.y),
                        ImVec2(countPos.x, countPos.y + 22 * scale),
                        ImColor(0, 0, 0, 180), 11 * scale);

                    // Count text
                    DrawString(18 * scale, countPos.x - pillWidth / 2 - textWidth / 2, countPos.y + 3 * scale,
                        ImColor(255, 255, 255), false, false,
                        ("x" + std::to_string((int)item.Count)).c_str());
                }
            }

            // Item border
            drawList->AddRect(drawStart, drawEnd, itemBorderColor, cornerRadius * 0.5f);

            // Highlight effect
            if (isHighlight) {
                static float pulseTime = 0.0f;
                pulseTime += ImGui::GetIO().DeltaTime;
                float pulse = 0.5f + 0.5f * sin(pulseTime * 4.0f);

                ImColor pulseColor(
                    65 + static_cast<int>(30 * pulse),
                    105 + static_cast<int>(30 * pulse),
                    225,
                    120 + static_cast<int>(60 * pulse)
                );

                drawList->AddRect(
                    ImVec2(drawStart.x - 2 * scale, drawStart.y - 2 * scale),
                    ImVec2(drawEnd.x + 2 * scale, drawEnd.y + 2 * scale),
                    pulseColor, cornerRadius * 0.7f, 0, 3 * scale);
            }
        }
    }
}