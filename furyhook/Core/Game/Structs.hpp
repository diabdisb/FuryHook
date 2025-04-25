#pragma once
#include <mutex>
#include <unordered_map>
#include "../Overlay/skycrypt.hpp"
#include <d3dx11tex.h>
#include <TlHelp32.h>
#include <functional>
#include "../Overlay/overlay.hpp"

#pragma comment(lib, "d3dx11.lib")

enum BoneList : int {
    head = 47,
    eye = 49,
    neck = 46,
    l_upperarm = 24,
    r_upperarm = 55,
    l_forearm = 25,
    r_forearm = 56,
    l_hand = 26,
    r_hand = 57,
    spine4 = 22,
    pelvis = 19,
    l_hip = 1,
    r_hip = 13,
    l_knee = 2,
    r_knee = 14,
    l_foot = 3,
    r_foot = 15,
};

struct unity_string
{
    char pad[0x10];
    int length;
    wchar_t buffer[128 + 1];
};


struct Matrix3x4 {
    float _11, _12, _13, _14;
    float _21, _22, _23, _24;
    float _31, _32, _33, _34;
};

struct UnityEngine_TransformAccessReadOnly
{
    uint64_t TransformData;
    uint32_t Index;
};

struct UnityEngine_TransformData
{
    uint64_t TransformArray;
    uint64_t TransformIndices;
};

enum class EntityCategory {
    Player,
    Dynamic,
    Static
};

struct EntityInfo {
    std::string name;
    ImColor* color;
    bool* esp;
    bool* radar;
    EntityCategory category;
};
enum LifeState {
    Alive,
    Dead
};
class BaseEntity {
public:
    uint64_t address = 0;
    uint64_t base_object = 0;
    uint64_t entity_object = 0;
    uint64_t entity_class = 0;
    uint64_t entity_transform = 0;
    uint64_t entity_visual_state = 0;
    uint64_t prefab_id;
    float distance;
    std::string dropped_name;
    Vector3 position;
    bool position_initialized = false;

    EntityInfo entity_info;
};
class BasePlayer : public BaseEntity {
public:
    uint32_t flags;
    uint32_t team;
    LifeState life_state;
    uint64_t model;
    uint64_t name_ptr;
    int name_length;
    std::string name;
    uint64_t skinned_multi_meshes;
    uint64_t renderer_lists;
    uint64_t skinned_list;
    std::vector<Vector3> bones;
    std::vector<Vector3> previous_bones; // Add this to store previous frame's bones
    bool has_previous_bones = false;     // Track if we have previous bone data

    bool is_dead = false;
    bool is_sleeping = false;
    bool is_wounded;

    uint64_t inventory_encrypted;
    uint64_t active_item_id;
    uint64_t current_active_item_id = 0;
    uint64_t inventory;
    uint64_t inventory_belt;
    uint64_t belt_contents_list;
    uint64_t belt_contents;
    std::vector<uint64_t> item_slots = std::vector<uint64_t>(6, 0);
    std::vector<uint64_t> item_ids = std::vector<uint64_t>(6, 0);
    uint64_t held_item_ptr;
    uintptr_t item_definition;
    uintptr_t item_display_name;
    uintptr_t item_display_name_english;
    uint64_t item_length;
    std::string held_item;

    //std::shared_ptr<Transform> transformPtr = nullptr;
};
inline std::mutex playersMutex;
inline std::vector<BasePlayer> Players;

struct GameInfo {
    uintptr_t BaseNetworkable = 0;
    uintptr_t StaticBaseNet = 0;
    uintptr_t WrapperPtr = 0;
    uintptr_t Wrapper = 0;
    uintptr_t ParentStatic = 0;
    uintptr_t ParentClass = 0;
    uintptr_t ObjectDictionary = 0;
    uintptr_t EntityListBase = 0;
    uintptr_t Camera = 0;
    uintptr_t Localplayer = 0;
    ViewMatrix viewmatrix;
    bool Error = false;
    int Errorcode = 0x0;

    GameInfo() = default;
};

extern GameInfo globalCache;

enum features
{
    Spiderman,
    TimeChanger,
    FovChanger,
    NoRecoil,
    NoSpread,
    Automatic,
    SuperEoka,
    SuperMeele,
    AdminFlag,
    Aimbot,
    Silent,
    Aimline,
    AimFov,
    Crosshair,
    Predictions,
    HandChams,
    IgnoreVisible,
    IgnoreWounded,
    IgnoreSleeper,
    IgnoreNpc,

    NameEsp,
    PlayerInfo,
    PlayerEsp,
    Chams,
    CornerBox,
    NormalBox,
    FillBox,
    SkeletonEsp,
    ScientistEsp,
    ShowSleepers,
    TeamEsp,
    Wounded,
    StoneEsp,
    SulfurEsp,
    MetalEsp,
    HempEsp,
    MilitaryCrateEsp,
    DroppedItems,
    DroppedItemsImage,
    SupplyDropEsp,
    AutoTurretEsp,
    ShotgunTrapEsp,
    FlameTurretEsp,
    BearTrapEsp,
    LandMineEsp,
    SamSiteEsp,
    ToolCupboardEsp,
    CarEsp,
    BoatEsp,
    BoarEsp,
    BearEsp,
    ChickenEsp,
    StagEsp,
    HorseEsp,
    WolfEsp,
    NormalCrate,
    EliteCrate,
    PatrolHeli,
    MinicopterEsp,
    ShelterEsp,
    SmallBoxEsp,
    LargeBoxEsp,
    FurnaceEsp,
    MushroomEsp,
    CollectableStoneEsp,
    CollectableMetalEsp,
    CollectableSulfurEsp,
    CollectableWoodEsp,
    CornEsp,
    PumpkinEsp,
    LootBarrelEsp,
    CorpseEsp,
    OffScreenArrows,
};

struct sPrefabList
{
    std::string PrefabName;
    Vector3 location;
    std::chrono::steady_clock::time_point lastSeen;
    ImU32 color;
};

struct PrefabInfo {
    std::string displayName;
    ImU32 color;
};
enum class PrefabType {
    Other,        // Ores, collectibles, crates, etc.
    DroppedItem   // Items dropped on the ground "(world)"
};

struct PrefabEntry {
    std::string PrefabName;       // Display name (filtered for dropped items)
    Vector3 location;             // World position
    ImU32 color;                  // Color associated with the prefab type
    std::chrono::steady_clock::time_point lastSeen; // For expiration
    PrefabType type;              // The type of this prefab entry
    // Add any other relevant data if needed
};
// Replace your foundPrefabs vector with this
inline std::vector<PrefabEntry> prefabCache;
inline std::mutex prefabMutex;

struct StringHash {
    std::size_t operator()(const std::string& str) const {
        return std::hash<std::string>()(str);
    }
};

inline std::unordered_map<std::string, PrefabInfo, StringHash> prefabMap{
    {"assets/bundled/prefabs/autospawn/resource/ores/stone-ore.prefab", {"Stone Ore", IM_COL32(153, 153, 153, 255)}},
    {"assets/bundled/prefabs/autospawn/resource/ores_sand/stone-ore.prefab", {"Stone Ore", IM_COL32(153, 153, 153, 255)}},
    {"assets/bundled/prefabs/autospawn/resource/ores_snow/stone-ore.prefab", {"Stone Ore", IM_COL32(153, 153, 153, 255)}},

    {"assets/bundled/prefabs/autospawn/resource/ores/sulfur-ore.prefab", {"Sulfur Ore", IM_COL32(255, 255, 0, 255)}},
    {"assets/bundled/prefabs/autospawn/resource/ores_sand/sulfur-ore.prefab", {"Sulfur Ore", IM_COL32(255, 255, 0, 255)}},
    {"assets/bundled/prefabs/autospawn/resource/ores_snow/sulfur-ore.prefab", {"Sulfur Ore", IM_COL32(255, 255, 0, 255)}},

    {"assets/bundled/prefabs/autospawn/resource/ores/metal-ore.prefab", {"Metal Ore", IM_COL32(191, 191, 217, 255)}},
    {"assets/bundled/prefabs/autospawn/resource/ores_sand/metal-ore.prefab", {"Metal Ore", IM_COL32(191, 191, 217, 255)}},
    {"assets/bundled/prefabs/autospawn/resource/ores_snow/metal-ore.prefab", {"Metal Ore", IM_COL32(191, 191, 217, 255)}},

    {"assets/bundled/prefabs/autospawn/collectable/hemp/hemp-collectable.prefab", {"Hemp", IM_COL32(51, 204, 51, 255)}},

    {"assets/bundled/prefabs/radtown/crate_normal.prefab", {"Military Chest", IM_COL32(153, 230, 153, 255)}},
    {"assets/bundled/prefabs/radtown/crate_normal_2.prefab", {"Normal Crate", IM_COL32(128, 204, 128, 255)}},
    {"assets/bundled/prefabs/radtown/crate_elite.prefab", {"Elite Crate", IM_COL32(0, 255, 255, 255)}},
    {"assets/bundled/prefabs/radtown/loot_barrel_1.prefab", {"Loot Barrel", IM_COL32(102, 102, 153, 255)}},
    {"assets/bundled/prefabs/radtown/loot_barrel_2.prefab", {"Loot Barrel", IM_COL32(102, 102, 153, 255)}},

    {"assets/prefabs/misc/supply drop/supply_drop.prefab", {"Supply Drop", IM_COL32(77, 153, 255, 255)}},

    {"assets/prefabs/npc/autoturret/autoturret_deployed.prefab", {"Auto Turret", IM_COL32(255, 0, 0, 255)}},
    {"assets/prefabs/deployable/single shot trap/guntrap.deployed.prefab", {"ShotGun Trap", IM_COL32(230, 51, 51, 255)}},
    {"assets/prefabs/npc/flame turret/flameturret.deployed.prefab", {"Flame Turret", IM_COL32(255, 77, 0, 255)}},
    {"assets/prefabs/deployable/bear trap/beartrap.prefab", {"Bear Trap", IM_COL32(204, 51, 51, 255)}},
    {"assets/prefabs/deployable/landmine/landmine.prefab", {"Landmine", IM_COL32(230, 26, 26, 255)}},
    {"assets/prefabs/npc/sam_site_turret/sam_site_turret_deployed.prefab", {"Sam Site", IM_COL32(255, 128, 0, 255)}},

    {"assets/prefabs/deployable/tool cupboard/cupboard.tool.deployed.prefab", {"Tool Cupboard", IM_COL32(204, 179, 153, 255)}},

    {"assets/content/vehicles/modularcar/2module_car_spawned.entity.prefab", {"2 Slot Car", IM_COL32(77, 77, 230, 255)}},
    {"assets/content/vehicles/modularcar/3module_car_spawned.entity.prefab", {"3 Slot Car", IM_COL32(77, 77, 230, 255)}},
    {"assets/content/vehicles/modularcar/4module_car_spawned.entity.prefab", {"4 Slot Car", IM_COL32(77, 77, 230, 255)}},

    {"assets/content/vehicles/boats/rowboat/rowboat.prefab", {"Boat", IM_COL32(26, 153, 230, 255)}},

    {"assets/rust.ai/agents/boar/boar.prefab", {"Boar", IM_COL32(179, 128, 102, 255)}},
    {"assets/rust.ai/agents/bear/bear.prefab", {"Bear", IM_COL32(102, 77, 51, 255)}},
    {"assets/rust.ai/agents/chicken/chicken.prefab", {"Chicken", IM_COL32(255, 255, 204, 255)}},
    {"assets/rust.ai/agents/stag/stag.prefab", {"Stag", IM_COL32(153, 102, 77, 255)}},
    {"assets/rust.ai/agents/bear/polarbear.prefab", {"Bear", IM_COL32(230, 230, 255, 255)}},
    {"assets/rust.ai/agents/wolf/wolf.prefab", {"Wolf", IM_COL32(128, 128, 128, 255)}},
    {"assets/rust.ai/agents/horse/horse.prefab", {"Horse", IM_COL32(128, 102, 77, 255)}},

    {"assets/prefabs/npc/patrol helicopter/patrolhelicopter.prefab", {"Patrol Helicopter", IM_COL32(230, 51, 51, 255)}},
    {"assets/content/vehicles/minicopter/minicopter.entity.prefab", {"Minicopter", IM_COL32(51, 204, 255, 255)}},

    {"assets/prefabs/building/legacy.shelter.wood/legacy.shelter.wood.deployed.prefab", {"Shelter", IM_COL32(153, 102, 51, 255)}},

    {"assets/prefabs/deployable/woodenbox/woodbox_deployed.prefab", {"Small Box", IM_COL32(153, 102, 77, 255)}},
    {"assets/prefabs/deployable/large wood storage/box.wooden.large.prefab", {"Large Box", IM_COL32(153, 77, 51, 255)}},
    {"assets/prefabs/deployable/furnace/furnace.prefab", {"Furnace", IM_COL32(255, 128, 0, 255)}},

    {"assets/bundled/prefabs/autospawn/collectable/mushrooms/mushroom-cluster-5.prefab", {"Mushroom", IM_COL32(204, 102, 255, 255)}},
    {"assets/bundled/prefabs/autospawn/collectable/mushrooms/mushroom-cluster-6.prefab", {"Mushroom", IM_COL32(204, 102, 255, 255)}},

    {"assets/bundled/prefabs/autospawn/collectable/stone/stone-collectable.prefab", {"Stone", IM_COL32(153, 153, 153, 255)}},
    {"assets/bundled/prefabs/autospawn/collectable/stone/sulfur-collectable.prefab", {"Sulfur", IM_COL32(255, 255, 0, 255)}},
    {"assets/bundled/prefabs/autospawn/collectable/stone/metal-collectable.prefab", {"Metal", IM_COL32(191, 191, 217, 255)}},
    {"assets/bundled/prefabs/autospawn/collectable/wood/wood-collectable.prefab", {"Wood", IM_COL32(102, 51, 26, 255)}},
    {"assets/bundled/prefabs/autospawn/collectable/corn/corn-collectable.prefab", {"Corn", IM_COL32(255, 230, 51, 255)}},
    {"assets/bundled/prefabs/autospawn/collectable/pumpkin/pumpkin-collectable.prefab", {"Pumpkin", IM_COL32(255, 153, 51, 255)}},

    {"assets/prefabs/player/player_corpse.prefab", {"Corpse", IM_COL32(179, 51, 51, 255)}},
    {"assets/prefabs/misc/item drop/item_drop_backpack.prefab", {"Backpack", IM_COL32(128, 51, 204, 255)}}
};

inline const PrefabInfo* GetPrefabInfo(const std::string& prefabPath) {
    auto it = prefabMap.find(prefabPath);
    if (it != prefabMap.end())
        return &it->second;
    return nullptr;
}
class c_texture
{
public:
    ID3D11ShaderResourceView* texture = nullptr;
    std::string item_name = "";
};

inline std::string get_rust_client_path()
{
    std::string rust_path;

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return rust_path;

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(snapshot, &pe)) {
        do {
            if (_stricmp(pe.szExeFile, "RustClient.exe") == 0) {
                // We found the process. Now get full path without opening a handle (dirty trick):
                // Use QueryFullProcessImageName *if* you allow OpenProcess, otherwise skip
                DWORD pid = pe.th32ProcessID;
                HANDLE h = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
                if (h) {
                    char path[MAX_PATH];
                    DWORD size = MAX_PATH;
                    if (QueryFullProcessImageNameA(h, 0, path, &size))
                        rust_path = std::string(path);
                    CloseHandle(h);
                }
                break;
            }
        } while (Process32Next(snapshot, &pe));
    }

    CloseHandle(snapshot);

    // Trim to directory
    if (!rust_path.empty()) {
        size_t last = rust_path.find_last_of("\\/");
        if (last != std::string::npos)
            rust_path = rust_path.substr(0, last + 1); // Keep trailing slash
    }

    return rust_path;
}

class c_texture_cache
{
private:
    std::vector<c_texture> texture_cache{};
    std::string base_path = get_rust_client_path() + "Bundles\\items\\";

    ID3D11ShaderResourceView* create_texture(ID3D11Device* device, const std::string& name)
    {
        std::string image_path = base_path + name;

        ID3D11ShaderResourceView* tex = nullptr;

        if (D3DX11CreateShaderResourceViewFromFileA(device, image_path.c_str(), nullptr, nullptr, &tex, nullptr) != S_OK)
            return nullptr;

        return tex;
    }

public:
    c_texture get_texture(const std::string& item_name)
    {
        for (auto& texture : texture_cache)
        {
            if (texture.item_name == item_name)
                return texture;
        }

        c_texture ret;
        ret.item_name = item_name;
        ret.texture = create_texture(p_device, item_name + ".png");

        if (!ret.texture)
            return ret;

        texture_cache.push_back(ret);
        return ret;
    }
};

inline c_texture_cache texture_cache;

static inline char* memdup(const char* s, size_t n)
{
    char* t = (char*)malloc(n);
    memcpy(t, s, n);
    return t;
}
#define _memdup(object) memdup(object, sizeof(object))

inline std::string FilterDroppedItem(std::string input)
{
    size_t start = input.find(_memdup(_(" (")));
    size_t end = input.find(_memdup(_(")")));
    if (start != std::string::npos && end != std::string::npos)
        input.erase(start, end - start + 1);
    bool skip_item = false;
    std::string skip_keywords[] = { _memdup(_("torch")), _memdup(_("rock")), _memdup(_("asset")), _memdup(_("fire")), _memdup(_("dung")), _memdup(_("arrow")), _memdup(_("nail")), _memdup(_("movepoint")), _memdup(_("-")), _memdup(_("_")) };
    for (const std::string& keyword : skip_keywords) {
        if (input.find(keyword) != std::string::npos) {
            skip_item = true;
            break;
        }
    }
    if (skip_item)
        return (std::string)_("");

    return input;
}

struct Cloth
{
    std::string Name;
    float Health;
    float MaxHealth;
};

struct Matrix4x4 {
    float m[4][4];
};
struct Belt
{
    std::string Name;
    int Count;
    int WeaponAmmo;
    float Health;
    float MaxHealth;
};
