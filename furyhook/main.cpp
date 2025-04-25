
#include  "Core/imports.h"
#include <mutex>
#include <unordered_map>
#include <map>
#include  "Core/Game/aimbot.hpp"
#include "Core/Game/helperfuncs.hpp"
#include "Core/config.hpp"

GameInfo globalCache;




uint64_t cameraSetup(uint64_t gameAssemblyBase) {
	uint64_t camera_manager = driver::read<uint64_t>(gameAssemblyBase + offset::MainCamera);
	if (!camera_manager)
	{
		return 0;
	}

	uint64_t camera_static = driver::read<uint64_t>(camera_manager + 0xb8);
	if (!camera_static)
	{
		return 0;
	}

	uint64_t camera_object = driver::read<uint64_t>(camera_static + 0x30);
	if (!camera_object)
	{
		return 0;
	}

	uint64_t camera = driver::read<uint64_t>(camera_object + 0x10);
	if (!camera)
	{
		return 0;
	}

	return camera;
}


static GameInfo StartCache()
{
    std::cout << "[INFO] Starting cache initialization...\n";

    if (!GameAssembly) {
        std::cerr << "[Error] GameAssembly is null!\n";
        return globalCache;
    }
    std::cout << "[DEBUG] GameAssembly = 0x" << std::hex << GameAssembly << std::dec << "\n";

    globalCache.BaseNetworkable = driver::read<uintptr_t>(GameAssembly + offset::BaseNetworkable);
    std::cout << "[DEBUG] BaseNetworkable @ 0x" << std::hex << (GameAssembly + offset::BaseNetworkable)
        << " = 0x" << globalCache.BaseNetworkable << std::dec << "\n";
    if (!globalCache.BaseNetworkable) {
        std::cerr << "[Error] Failed to read BaseNetworkable!\n";
        return globalCache;
    }

    globalCache.StaticBaseNet = driver::read<uint64_t>(globalCache.BaseNetworkable + 0xB8);
    std::cout << "[DEBUG] StaticBaseNet @ 0x" << std::hex << (globalCache.BaseNetworkable + 0xB8)
        << " = 0x" << globalCache.StaticBaseNet << std::dec << "\n";
    if (!globalCache.StaticBaseNet) {
        std::cerr << "[Error] Failed to read StaticBaseNet!\n";
        return globalCache;
    }

    globalCache.WrapperPtr = driver::read<uint64_t>(globalCache.StaticBaseNet + offset::BaseNetworkable1);
    std::cout << "[DEBUG] WrapperPtr @ 0x" << std::hex << (globalCache.StaticBaseNet + offset::BaseNetworkable1)
        << " = 0x" << globalCache.WrapperPtr << std::dec << "\n";
    if (!globalCache.WrapperPtr) {
        std::cerr << "[Error] Failed to read WrapperPtr!\n";
        return globalCache;
    }

    globalCache.Wrapper = decryptions::DecryptBaseNetworkable(globalCache.WrapperPtr);
    std::cout << "[DEBUG] Wrapper (decrypted) = 0x" << std::hex << globalCache.Wrapper << std::dec << "\n";
    if (!globalCache.Wrapper) {
        std::cerr << "[Error] Failed to decrypt Wrapper!\n";
        return globalCache;
    }

    globalCache.ParentStatic = driver::read<uint64_t>(globalCache.Wrapper + offset::BaseNetworkable2);
    std::cout << "[DEBUG] ParentStatic @ 0x" << std::hex << (globalCache.Wrapper + offset::BaseNetworkable2)
        << " = 0x" << globalCache.ParentStatic << std::dec << "\n";
    if (!globalCache.ParentStatic) {
        std::cerr << "[Error] Failed to read ParentStatic!\n";
        return globalCache;
    }

    globalCache.ParentClass = decryptions::DecryptNetworkableList(globalCache.ParentStatic);
    std::cout << "[DEBUG] ParentClass (decrypted) = 0x" << std::hex << globalCache.ParentClass << std::dec << "\n";
    if (!globalCache.ParentClass) {
        std::cerr << "[Error] Failed to decrypt ParentClass!\n";
        return globalCache;
    }

    globalCache.ObjectDictionary = driver::read<uint64_t>(globalCache.ParentClass + 0x18);
    std::cout << "[DEBUG] ObjectDictionary @ 0x" << std::hex << (globalCache.ParentClass + offset::BaseNetworkable3)
        << " = 0x" << globalCache.ObjectDictionary << std::dec << "\n";
    if (!globalCache.ObjectDictionary) {
        std::cerr << "[Error] Failed to read ObjectDictionary!\n";
        return globalCache;
    }



    globalCache.EntityListBase = driver::read<uint64_t>(globalCache.ObjectDictionary + 0x18);
    std::cout << "[DEBUG] EntityListBase @ 0x" << std::hex << (globalCache.ObjectDictionary + 0x18)
        << " = 0x" << globalCache.EntityListBase << std::dec << "\n";
    if (!globalCache.EntityListBase) {
        std::cerr << "[Error] Failed to read EntityListBase!\n";
        return globalCache;
    }
    globalCache.Localplayer  = driver::read<uint64_t>(globalCache.EntityListBase + 0x20 + (0 * 0x8));

    globalCache.Camera = cameraSetup(GameAssembly);
    std::cout << "[DEBUG] Camera setup complete.\n";

    std::cout << "[INFO] Cache initialization complete.\n";
    return globalCache;
}










void render()
{
    ImGui::Begin("HiddenOverlayWindow", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);
   
    ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
    const ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    const ImVec2 screenCenter(displaySize.x * 0.5f, displaySize.y * 0.5f);
    const ImVec2 screenPositions[] = {
        { screenCenter.x, displaySize.y }, // Bottom
        { screenCenter.x, 0.0f },         // Top
        screenCenter                      // Center
    };
    const ViewMatrix viewmatrix = globalCache.viewmatrix; // Cache viewmatrix
    ImFont* font = ImGui::GetFont(); // Cache font for text rendering

    // Precompute colors
    const ImU32 colors[] = {
        ImGui::ColorConvertFloat4ToU32(g_Config.vFovCircleColor),
        ImGui::ColorConvertFloat4ToU32(g_Config.vPlayerNameColor),
        ImGui::ColorConvertFloat4ToU32(g_Config.vBoneColor),
        ImGui::ColorConvertFloat4ToU32(g_Config.vBoxColor),
        ImGui::ColorConvertFloat4ToU32(g_Config.vSnaplineColor)
    };
    enum { COLOR_FOV = 0, COLOR_NAME, COLOR_BONE, COLOR_BOX, COLOR_SNAPLINE };

    // Get local player position
    Vector3 localPos{};
    if (globalCache.Localplayer) {
        const uint64_t playerModel = driver::read<uint64_t>(globalCache.Localplayer + offset::PlayerModel);
        if (playerModel) {
            localPos = driver::read<Vector3>(playerModel + offset::Position);
        }
    }

    // Copy players and prefabs with minimal locking
    std::vector<BasePlayer> playersCopy;
    std::vector<PrefabEntry> prefabCacheCopy;
    {
        std::scoped_lock lock(playersMutex, prefabMutex);
        playersCopy = Players;
        if (g_Config.bDrawPrefabs) prefabCacheCopy = prefabCache;
    }

    // Draw FOV circle
    if (g_Config.bDrawFovCircle && g_Config.bEnableAimbot) {
        draw_list->AddCircle(screenCenter, g_Config.fAimbotMaxFov, colors[COLOR_FOV], 32, 1.5f); // Reduced segments
    }

    // Find closest player
    BasePlayer* closestPlayer = nullptr;
    float closestFov = 100.0f;
    float lastDynamicFovMultiplier = 1.0f;
    for (const auto& player : playersCopy) {
        if (player.bones.size() <= 9) continue;
        Vector3 targetBonePos = player.bones[9];
        auto screenPos = WorldToScreen(targetBonePos, viewmatrix);
        if (screenPos.x == -1 && screenPos.y == -1) continue;

        float playerDistance = localPos.distance(targetBonePos);
        float dynamicFovMultiplier = playerDistance > 200.0f ? 0.7f :
            playerDistance > 100.0f ? 0.85f :
            playerDistance > 7.0f ? 1.0f :
            playerDistance > 5.0f ? 1.4f : 1.5f;

        float dx = screenPos.x - screenCenter.x;
        float dy = screenPos.y - screenCenter.y;
        float fov = std::sqrt(dx * dx + dy * dy) / dynamicFovMultiplier;

        if (fov < closestFov) {
            closestFov = fov;
            closestPlayer = const_cast<BasePlayer*>(&player);
            lastDynamicFovMultiplier = dynamicFovMultiplier;
        }
    }

    // Prefab rendering
    for (const auto& prefab : prefabCacheCopy)
    {
        bool shouldDraw = false;
        bool isDroppedItem = (prefab.type == PrefabType::DroppedItem);

        // Check if this type is enabled in config
        if (isDroppedItem && g_Config.bDrawDroppedItems) {
            shouldDraw = true;
        }
        else if (!isDroppedItem && g_Config.bDrawOtherPrefabs) {
            shouldDraw = true;
        }

        if (!shouldDraw) continue; // Skip if this type is disabled

        // --- Common: WorldToScreen ---
        auto screenPos = WorldToScreen(prefab.location, viewmatrix);
        if (screenPos.x < 0 && screenPos.y < 0) continue; // Check screen bounds properly if needed

        // --- Icon Rendering (Only for 'Other' prefabs and if enabled) ---
        bool drawIcon = !isDroppedItem && g_Config.bDrawPrefabIcons;
        ImVec2 iconBottomPos = screenPos.ToImVec2();

        if (drawIcon) {
            const c_texture& texture = texture_cache.get_texture(prefab.PrefabName); // Use name from entry
            if (texture.texture) {
                const ImVec2 texSize(g_Config.fPrefabIconSize, g_Config.fPrefabIconSize);
                // Position icon centered horizontally, with its *bottom* edge slightly above screenPos
                const ImVec2 texPos(screenPos.x - texSize.x * 0.5f, screenPos.y - texSize.y - 2.0f); // Icon above point
                draw_list->AddImage(reinterpret_cast<ImTextureID>(texture.texture), texPos, ImVec2(texPos.x + texSize.x, texPos.y + texSize.y));
                // Update text anchor point to be below the icon

                // later:
                iconBottomPos = ImVec2(screenPos.x, texPos.y + texSize.y);
            }
        }

        // --- Text Rendering (If enabled) ---
        if (g_Config.bDrawPrefabText) {
            ImGui::PushFont(font::inter_default); // Use your default ESP font

            // Use configured font size (could be different for types if needed)
            float fontSize = g_Config.iPrefabTextFontSize;
            // if (isDroppedItem && g_Config.bUseDifferentDroppedSize) {
            //     fontSize = g_Config.iDroppedItemTextSize;
            // }

            // Calculate font scale based on the base size of the loaded font
            if (font && font->FontSize > 0) {
                ImGui::SetWindowFontScale(fontSize / font->FontSize);
            }

            // Use the color stored in the prefab entry
            ImU32 textColor = prefab.color;
            // Optional: Override color via config
            // if (isDroppedItem && g_Config.bOverrideDroppedColor) {
            //     textColor = ImGui::ColorConvertFloat4ToU32(g_Config.vDroppedItemColor);
            // }

            ImVec2 textSize = ImGui::CalcTextSize(prefab.PrefabName.c_str());
            // Position text centered horizontally, below the icon (if drawn) or the screen point
            ImVec2 textPos(iconBottomPos.x - textSize.x * 0.5f, iconBottomPos.y + 2.0f); // Text below point/icon

            // Add text using the determined color and position
            DrawString(15, textPos.x ,textPos.y , textColor,  1, 1, prefab.PrefabName.c_str());

            // Reset font scale and pop font
            ImGui::SetWindowFontScale(1.0f); // Reset scale before popping
            ImGui::PopFont();
        }
    } // End prefab loop


    // Player rendering
    struct ScreenBone { ImVec2 pos; bool valid; };
    for (const auto& player : playersCopy) {
        if (player.bones.size() < 17) continue;
        Vector3 centralPos = player.bones[10]; // Pelvis
        auto screenPos = WorldToScreen(centralPos, viewmatrix);
        if (screenPos.x == -1 && screenPos.y == -1) continue;

        // Cache screen positions for bones
        std::vector<ScreenBone> screenBones(player.bones.size());
        for (size_t i = 0; i < player.bones.size(); ++i) {
            auto pos = WorldToScreen(player.bones[i], viewmatrix);
            screenBones[i] = { ImVec2(pos.x, pos.y), pos.x != -1 && pos.y != -1 };
        }

        // Draw player name
   

        // Show closest player info
        if (g_Config.bShowClosestPlayerInfo && (&player == closestPlayer)) {
            Cloth clothSlots[7];
            Belt beltSlots[6];
            for (int i = 0; i < 7; ++i) {
                clothSlots[i] = GetClothingItems(GetClothingItemsList(player.address), i);
                if (i < 6) beltSlots[i] = GetBeltItems(GetItemsList(player.address), i);
            }
            playerinfo(player.name.c_str(), clothSlots, beltSlots, "");
        }

        // Calculate box dimensions
        if (!screenBones[0].valid || !screenBones[15].valid || !screenBones[16].valid) continue;
        auto middleFeet = (player.bones[15] + player.bones[16]) / 2.0f;
        auto screenFeet = WorldToScreen(middleFeet, viewmatrix);
        if (screenFeet.x == -1) continue;

        float boxHeight = std::abs(screenBones[0].pos.y - screenFeet.y);
        float boxPadding = boxHeight * 0.1f;
        float boxTopY = screenBones[0].pos.y - boxPadding;
        float boxBottomY = screenFeet.y + boxPadding;
        boxHeight = boxBottomY - boxTopY;
        float boxWidth = boxHeight / 2.0f;
        float boxLeftX = screenFeet.x - boxWidth / 2.0f;

        if (g_Config.bDrawPlayerNames && screenBones[0].valid) {
            ImGui::PushFont(font);
            ImGui::SetWindowFontScale(g_Config.iPlayerNameFontSize / font->FontSize);
            ImVec2 textSize = ImGui::CalcTextSize(player.name.c_str());
            ImVec2 namePos(screenFeet.ToImVec2().x - textSize.x * 0.5f, screenFeet.ToImVec2().y - 15);
            DrawString( 13 , namePos.x , namePos.y, colors[COLOR_NAME],1, 1,  player.name.c_str());
            ImGui::PopFont();
        }
        // Draw snapline
        if (g_Config.bDrawSnapline && screenBones[0].valid) {
            draw_list->AddLine(screenPositions[g_Config.iSnaplineSource], screenBones[0].pos, colors[COLOR_SNAPLINE], g_Config.fSnaplineThickness);
        }

        // Draw player box
        if (g_Config.bDrawPlayerBox) {
            switch (g_Config.iBoxType) {
            case 1: // Normal Box
                draw_list->AddRect(ImVec2(boxLeftX, boxTopY), ImVec2(boxLeftX + boxWidth, boxBottomY), colors[COLOR_BOX], 0.0f, ImDrawFlags_RoundCornersAll, g_Config.fBoxThickness);
                break;
            case 2: // Cornered Box
                DrawCorneredBox(boxLeftX, boxTopY, boxWidth, boxHeight, colors[COLOR_BOX], g_Config.fBoxThickness);
                break;
            case 3: // Filled Box
                ImU32 topColor = ImGui::ColorConvertFloat4ToU32(ImVec4(g_Config.vBoxColor.x, g_Config.vBoxColor.y, g_Config.vBoxColor.z, g_Config.vBoxColor.w * 0.1f));
                ImU32 bottomColor = ImGui::ColorConvertFloat4ToU32(ImVec4(g_Config.vBoxColor.x, g_Config.vBoxColor.y, g_Config.vBoxColor.z, g_Config.vBoxColor.w * 0.5f));
                draw_list->AddRectFilledMultiColor(
                    ImVec2(boxLeftX, boxTopY), ImVec2(boxLeftX + boxWidth, boxBottomY),
                    topColor, topColor, bottomColor, bottomColor);
                draw_list->AddRect(ImVec2(boxLeftX, boxTopY), ImVec2(boxLeftX + boxWidth, boxBottomY), colors[COLOR_BOX], 0.0f, ImDrawFlags_RoundCornersAll, g_Config.fBoxThickness);
                break;
            }
        }

        // Draw player bones
        if (g_Config.bDrawPlayerBones) {
            auto draw_bone_line = [&](int a, int b) {
                if (a >= screenBones.size() || b >= screenBones.size() || !screenBones[a].valid || !screenBones[b].valid) return;
                draw_list->AddLine(screenBones[a].pos, screenBones[b].pos, colors[COLOR_BONE], g_Config.fBoneThickness);
                };
            draw_bone_line(0, 2); draw_bone_line(2, 9); draw_bone_line(9, 10);
            draw_bone_line(2, 3); draw_bone_line(3, 5); draw_bone_line(5, 7);
            draw_bone_line(2, 4); draw_bone_line(4, 6); draw_bone_line(6, 8);
            draw_bone_line(10, 11); draw_bone_line(11, 13); draw_bone_line(13, 15);
            draw_bone_line(10, 12); draw_bone_line(12, 14); draw_bone_line(14, 16);
        }
    }
}

auto matrix() -> void
{
    while (true)
    {
        globalCache.viewmatrix = driver::read<ViewMatrix>(globalCache.Camera + 0x30C);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
auto basic() -> void
{
    while (true)
    {
        globalCache = StartCache();
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}
int main()
{
    if (!memory::initialize())
    {
        printf(xorstr_("[-] memory init failed\n"));
        system(xorstr_("pause"));
        return 1;
    }

    memory::call<void>(xorstr_("kernel32.dll"), xorstr_("SetConsoleTitleA"), xorstr_("Skibidbobo"));

    if (!driver::load())
    {
        printf(xorstr_("[-] driver init failed\n"));
        system(xorstr_("pause"));
        return 1;
    }
    if (!driver::Mouse_innit()) {
        printf(xorstr_("[-] mouse init failed\n"));

    }




    while (!driver::detail::pid)
        driver::detail::pid = memory::get_pid(xorstr_("RustClient.exe"));

    driver::detail::base = driver::get_process_exe_base();
    if (!driver::detail::base)
    {
        printf(xorstr_("[-] base addr not found\n"));
        system(xorstr_("pause"));
        return 1;
    }
    std::cout << xorstr_("[+] Found Game Base ---> ") << xorstr_("0x") << std::hex << driver::detail::base << std::dec << std::endl;

    ULONG64 Dirbase = driver::GetDirBase();

    std::cout << xorstr_("[+] Found Game Dirbase ---> ") << xorstr_("0x") << std::hex << Dirbase << std::dec << std::endl;



    uintptr_t pebadd = driver::get_peb();
    printf("Peb addr 0x%llx \n", pebadd);
    //driver::PrintAllModules(pebadd);
    uintptr_t module = driver::GetModuleByName(pebadd, L"GameAssembly.dll");
    printf("Module 0x%llx \n", module);
    GameAssembly = module;
    uint64_t basenetworkable = driver::read<uint64_t>(module + 0xBCA9B50);
    printf("basenetworkable 0x%llx \n", basenetworkable);

    //auto apple = driver::read<uint64_t>(module);
    //printf("Module 0x%llx \n", apple);

    globalCache = StartCache();

    std::thread(basic).detach();
    std::thread(EntityThread).detach();
    std::thread(matrix).detach();
    std::thread(aimbot).detach();

    setup_window();
    directx_init();
    render_loop();
}