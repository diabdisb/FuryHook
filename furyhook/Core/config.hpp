#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <windows.h>
#include <algorithm>
#include <imgui.h>

// #include "Game/math.hpp"
#include "Game/structs.hpp"


inline ImU32 Vec4ToU32(const ImVec4& color) {
    return ImGui::ColorConvertFloat4ToU32(color);
}

struct Config {
    // --- General ---
    std::string configFileName = "render_config.cfg";

    // --- FOV Circle (Visual Only - Uses Aimbot FOV value now) ---
    bool bDrawFovCircle = true;
    // float fFovRadius = 100.0f; // REMOVED - Use fAimbotMaxFov instead
    ImVec4 vFovCircleColor = ImVec4(0.65f, 1.0f, 1.0f, 1.0f);
    // float fFovCircleThickness = 1.5f; // REMOVED (or keep if you want separate thickness control)

    // --- Aimbot Settings ---
    bool bEnableAimbot = true;
    int iAimbotKey = VK_XBUTTON1;
    float fAimbotMaxFov = 100.0f; // *** THIS VALUE NOW CONTROLS BOTH AIMBOT AND VISUAL FOV ***
    int iAimbotHitbox = 0;
    bool bClosestBoneTargeting = false;
    bool bEnablePrediction = true;
    bool bEnableSilentAim = false;
    int iSmoothingAmount = 10;
    int iHitChancePercent = 100;

    // --- Player ESP ---
    bool bDrawPlayerNames = true;
    ImVec4 vPlayerNameColor = ImVec4(0.65f, 1.0f, 1.0f, 1.0f);
    int iPlayerNameFontSize = 13;
    bool bDrawPlayerBones = true;
    ImVec4 vBoneColor = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
    float fBoneThickness = 1.5f;
    bool bDrawPlayerBox = true;
    int iBoxType = 2;
    ImVec4 vBoxColor = ImVec4(1.0f, 0.41f, 0.71f, 0.33f);
    float fBoxThickness = 1.0f;
    bool bDrawSnapline = true;
    ImVec4 vSnaplineColor = ImVec4(0.65f, 1.0f, 1.0f, 1.0f);
    float fSnaplineThickness = 1.5f;
    int iSnaplineSource = 0;
    bool bShowClosestPlayerInfo = true;

    // --- Prefab/World ESP ---
    bool bDrawPrefabs = true;
    bool bDrawOtherPrefabs = true;      // Toggle for ores, collectibles etc.
    bool bDrawDroppedItems = true;      // Toggle for dropped items
    bool bDrawPrefabIcons = false;       // General toggle for icons (applies to 'Other' type)
    float fPrefabIconSize = 24.0f;      // Icon size
    bool bDrawPrefabText = true;        // General toggle for text
    float iPrefabTextFontSize = 12.0f;  // Font size for prefab text (can be float)

    // --- Simple Load/Save ---
    void Save() {
        std::ofstream outFile(configFileName);
        if (!outFile) return;

        // FOV Circle (Visual Only)
        outFile << "bDrawFovCircle=" << bDrawFovCircle << std::endl;
        // outFile << "fFovRadius=" << fFovRadius << std::endl; // REMOVED
        outFile << "vFovCircleColor=" << vFovCircleColor.x << "," << vFovCircleColor.y << "," << vFovCircleColor.z << "," << vFovCircleColor.w << std::endl;
        // outFile << "fFovCircleThickness=" << fFovCircleThickness << std::endl; // REMOVED

        // Aimbot
        outFile << "bEnableAimbot=" << bEnableAimbot << std::endl;
        outFile << "iAimbotKey=" << iAimbotKey << std::endl;
        outFile << "fAimbotMaxFov=" << fAimbotMaxFov << std::endl; // Keep this
        outFile << "iAimbotHitbox=" << iAimbotHitbox << std::endl;
        outFile << "bClosestBoneTargeting=" << bClosestBoneTargeting << std::endl;
        outFile << "bEnablePrediction=" << bEnablePrediction << std::endl;
        outFile << "bEnableSilentAim=" << bEnableSilentAim << std::endl;
        outFile << "iSmoothingAmount=" << iSmoothingAmount << std::endl;
        outFile << "iHitChancePercent=" << iHitChancePercent << std::endl;

        // Player ESP ... (rest of save remains the same)
        outFile << "bDrawPlayerNames=" << bDrawPlayerNames << std::endl;
        outFile << "vPlayerNameColor=" << vPlayerNameColor.x << "," << vPlayerNameColor.y << "," << vPlayerNameColor.z << "," << vPlayerNameColor.w << std::endl;
        outFile << "iPlayerNameFontSize=" << iPlayerNameFontSize << std::endl;
        outFile << "bDrawPlayerBones=" << bDrawPlayerBones << std::endl;
        outFile << "vBoneColor=" << vBoneColor.x << "," << vBoneColor.y << "," << vBoneColor.z << "," << vBoneColor.w << std::endl;
        outFile << "fBoneThickness=" << fBoneThickness << std::endl;
        outFile << "bDrawPlayerBox=" << bDrawPlayerBox << std::endl;
        outFile << "iBoxType=" << iBoxType << std::endl;
        outFile << "vBoxColor=" << vBoxColor.x << "," << vBoxColor.y << "," << vBoxColor.z << "," << vBoxColor.w << std::endl;
        outFile << "fBoxThickness=" << fBoxThickness << std::endl;
        outFile << "bDrawSnapline=" << bDrawSnapline << std::endl;
        outFile << "vSnaplineColor=" << vSnaplineColor.x << "," << vSnaplineColor.y << "," << vSnaplineColor.z << "," << vSnaplineColor.w << std::endl;
        outFile << "fSnaplineThickness=" << fSnaplineThickness << std::endl;
        outFile << "iSnaplineSource=" << iSnaplineSource << std::endl;
        outFile << "bShowClosestPlayerInfo=" << bShowClosestPlayerInfo << std::endl;

        // Prefab ESP ... (rest of save remains the same)
        outFile << "bDrawPrefabs=" << bDrawPrefabs << std::endl;
        outFile << "fPrefabIconSize=" << fPrefabIconSize << std::endl;
        outFile << "bDrawPrefabText=" << bDrawPrefabText << std::endl;
        outFile << "iPrefabTextFontSize=" << iPrefabTextFontSize << std::endl;
    }

    void Load() {
        std::ifstream inFile(configFileName);
        if (!inFile) return;

        std::string line;
        while (std::getline(inFile, line)) {
            std::stringstream ss(line);
            std::string key;
            if (std::getline(ss, key, '=')) {
                std::string value;
                std::getline(ss, value);

                try {
                    // FOV Circle
                    if (key == "bDrawFovCircle") bDrawFovCircle = std::stoi(value);
                    // else if (key == "fFovRadius") fFovRadius = std::stof(value); // REMOVED
                    else if (key == "vFovCircleColor") sscanf_s(value.c_str(), "%f,%f,%f,%f", &vFovCircleColor.x, &vFovCircleColor.y, &vFovCircleColor.z, &vFovCircleColor.w);
                    // else if (key == "fFovCircleThickness") fFovCircleThickness = std::stof(value); // REMOVED

                    // Aimbot
                    else if (key == "bEnableAimbot") bEnableAimbot = std::stoi(value);
                    else if (key == "iAimbotKey") iAimbotKey = std::stoi(value);
                    else if (key == "fAimbotMaxFov") fAimbotMaxFov = std::stof(value); // Keep this
                    else if (key == "iAimbotHitbox") iAimbotHitbox = std::stoi(value);
                    else if (key == "bClosestBoneTargeting") bClosestBoneTargeting = std::stoi(value);
                    else if (key == "bEnablePrediction") bEnablePrediction = std::stoi(value);
                    else if (key == "bEnableSilentAim") bEnableSilentAim = std::stoi(value);
                    else if (key == "iSmoothingAmount") {
                        iSmoothingAmount = std::stoi(value);
                        iSmoothingAmount = std::clamp(iSmoothingAmount, 1, 100);
                    }
                    else if (key == "iHitChancePercent") {
                        iHitChancePercent = std::stoi(value);
                        iHitChancePercent = std::clamp(iHitChancePercent, 0, 100);
                    }

                    // Player ESP ... (rest of load remains the same)
                    else if (key == "bDrawPlayerNames") bDrawPlayerNames = std::stoi(value);
                    else if (key == "vPlayerNameColor") sscanf_s(value.c_str(), "%f,%f,%f,%f", &vPlayerNameColor.x, &vPlayerNameColor.y, &vPlayerNameColor.z, &vPlayerNameColor.w);
                    else if (key == "iPlayerNameFontSize") iPlayerNameFontSize = std::stoi(value);
                    else if (key == "bDrawPlayerBones") bDrawPlayerBones = std::stoi(value);
                    else if (key == "vBoneColor") sscanf_s(value.c_str(), "%f,%f,%f,%f", &vBoneColor.x, &vBoneColor.y, &vBoneColor.z, &vBoneColor.w);
                    else if (key == "fBoneThickness") fBoneThickness = std::stof(value);
                    else if (key == "bDrawPlayerBox") bDrawPlayerBox = std::stoi(value);
                    else if (key == "iBoxType") iBoxType = std::stoi(value);
                    else if (key == "vBoxColor") sscanf_s(value.c_str(), "%f,%f,%f,%f", &vBoxColor.x, &vBoxColor.y, &vBoxColor.z, &vBoxColor.w);
                    else if (key == "fBoxThickness") fBoxThickness = std::stof(value);
                    else if (key == "bDrawSnapline") bDrawSnapline = std::stoi(value);
                    else if (key == "vSnaplineColor") sscanf_s(value.c_str(), "%f,%f,%f,%f", &vSnaplineColor.x, &vSnaplineColor.y, &vSnaplineColor.z, &vSnaplineColor.w);
                    else if (key == "fSnaplineThickness") fSnaplineThickness = std::stof(value);
                    else if (key == "iSnaplineSource") iSnaplineSource = std::stoi(value);
                    else if (key == "bShowClosestPlayerInfo") bShowClosestPlayerInfo = std::stoi(value);

                    // Prefab ESP ... (rest of load remains the same)
                    else if (key == "bDrawPrefabs") bDrawPrefabs = std::stoi(value);
                    else if (key == "fPrefabIconSize") fPrefabIconSize = std::stof(value);
                    else if (key == "bDrawPrefabText") bDrawPrefabText = std::stoi(value);
                    else if (key == "iPrefabTextFontSize") iPrefabTextFontSize = std::stoi(value);

                }
                catch (const std::exception& e) {
                    (void)e;
                }
            }
        }
    }
};

inline Config g_Config; // Ensure this is defined globally

const std::vector<int> validBones = {
    static_cast<int>(BoneList::head),
    // static_cast<int>(BoneList::eye), // Eyes might be less reliable targets
    static_cast<int>(BoneList::neck),
    static_cast<int>(BoneList::l_upperarm),
    static_cast<int>(BoneList::r_upperarm),
    static_cast<int>(BoneList::l_forearm),
    static_cast<int>(BoneList::r_forearm),
    static_cast<int>(BoneList::l_hand),
    static_cast<int>(BoneList::r_hand),
    static_cast<int>(BoneList::spine4),
    static_cast<int>(BoneList::pelvis),
    static_cast<int>(BoneList::l_hip),
    static_cast<int>(BoneList::r_hip),
    static_cast<int>(BoneList::l_knee),
    static_cast<int>(BoneList::r_knee),
    static_cast<int>(BoneList::l_foot),
    static_cast<int>(BoneList::r_foot)
};
