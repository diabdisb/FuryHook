#include "aimbot.hpp"
#include   "../imports.h"
#include <map>
#include "../config.hpp"


uint64_t GetHeldItem(uint64_t playeraddress) {
    auto player_inventory_encrypted = driver::read<uint64_t>(playeraddress + offset::PlayerInventory);
   // std::cout << "[DEBUG] player_inventory_encrypted: 0x" << std::hex << player_inventory_encrypted << std::dec << "\n";


    auto player_inventory = decryptions::decrypt_inventory_pointer(player_inventory_encrypted);
    // std::cout << "[DEBUG] player_inventory: 0x" << std::hex << player_inventory << std::dec << "\n";


    auto inventory_belt = driver::read<uint64_t>(player_inventory + offset::Belt);
    // std::cout << "[DEBUG] inventory_belt: 0x" << std::hex << inventory_belt << std::dec << "\n";


    auto belt_contents_list = driver::read<uint64_t>(inventory_belt + offset::ItemList);
    //  std::cout << "[DEBUG] belt_contents_list: 0x" << std::hex << belt_contents_list << std::dec << "\n";


    auto belt_contents = driver::read<uint64_t>(belt_contents_list + 0x10);
    // std::cout << "[DEBUG] belt_contents: 0x" << std::hex << belt_contents << std::dec << "\n";


    auto active_item_id = driver::read<uint64_t>(playeraddress + offset::ClActiveItem);
    //  std::cout << "[DEBUG] active_item_id (encrypted): 0x" << std::hex << active_item_id << std::dec << "\n";

    auto decrypted_item_id = decryptions::DecryptClActiveItem(active_item_id);
    // std::cout << "[DEBUG] decrypted_item_id: 0x" << std::hex << decrypted_item_id << std::dec << "\n";
   //  if (!decrypted_item_id)
      //   return nullptr;

    for (int i = 0; i < 6; ++i) {
        auto current_item = driver::read<uint64_t>(belt_contents + 0x20 + (i * 8));
        //      std::cout << "[DEBUG] Item slot " << i << " | current_item: 0x" << std::hex << current_item << std::dec << "\n";
        if (!current_item)
            continue;

        auto current_item_id = driver::read<uint64_t>(current_item + offset::ItemId);
        //   std::cout << "         -> current_item_id: 0x" << std::hex << current_item_id << std::dec << "\n";
        if (!current_item_id)
            continue;

        if (decrypted_item_id == current_item_id) {
            //        std::cout << "[DEBUG] -> Match found! Returning item in slot " << i << "\n";
            return current_item;
        }
    }

    //   std::cout << "[DEBUG] No matching held item found in belt.\n";
      // return nullptr;
}

std::string get_item_name(uint64_t held)
{
    uint64_t unk = driver::read<uint64_t>(held + offset::ItemDefinition); // public ItemDefinition info;
    if (!unk)
    {
        //       std::cout << "[get_item_name] Failed to read ItemDefinition from: 0x" << std::hex << held << std::dec << std::endl;
        return std::string();
    }

    //  std::cout << "[get_item_name] ItemDefinition address: 0x" << std::hex << unk << std::dec << std::endl;

    unk = driver::read<uint64_t>(unk + 0x28); // public string shortname
    if (!unk)
    {
        //       std::cout << "[get_item_name] Failed to read shortname pointer from ItemDefinition." << std::endl;
        return std::string();
    }

    //   std::cout << "[get_item_name] Shortname pointer address: 0x" << std::hex << unk << std::dec << std::endl;

    std::wstring item_name = driver::read_wstring(unk + 0x14, 64);
    std::string final_name(item_name.begin(), item_name.end());

    //   std::cout << "[get_item_name] Item shortname: " << final_name << std::endl;

    return final_name;
}

uint64_t GetBaseProjectile(uint64_t address) {
    uint64_t unk = driver::read<uint64_t>(address + offset::HeldItem); // public HeldItem
    if (!unk)
        return 0;

    return unk;
}
uint64_t GetPrimaryMagazine(uint64_t address) {
    uint64_t unk = driver::read<uint64_t>(address + offset::PrimaryMagazine); // public HeldItem
    if (!unk)
        return 0;

    return unk;
}
std::map<std::wstring, float> bullets =
{
    {  std::wstring(_(L"ammo.rifle")), 1.0f },
    {  std::wstring(_(L"ammo.rifle.hv")), 1.2f },
    {  std::wstring(_(L"ammo.rifle.explosive")), 0.49f },
    {  std::wstring(_(L"ammo.rifle.incendiary")), 0.55f },
    {  std::wstring(_(L"ammo.pistol")), 1.0f },
    {  std::wstring(_(L"ammo.pistol.hv")), 1.33333f },
    {  std::wstring(_(L"ammo.pistol.fire")), 0.75f },
    {  std::wstring(_(L"arrow.wooden")), 1.0f },
    {  std::wstring(_(L"arrow.hv")), 1.6f },
    {  std::wstring(_(L"arrow.fire")), 0.8f },
    {  std::wstring(_(L"arrow.bone")), 0.9f },
    {  std::wstring(_(L"ammo.handmade.shell")), 1.0f },
    {  std::wstring(_(L"ammo.shotgun.slug")), 2.25f },
    {  std::wstring(_(L"ammo.shotgun.fire")), 1.0f },
    {  std::wstring(_(L"ammo.shotgun")), 2.25f },
    {  std::wstring(_(L"ammo.nailgun.nails")), 1.f }
};



float ProjectileSpeed_Normal(float dist)
{

    auto HELD = GetHeldItem(globalCache.Localplayer);
    const float default_speed = 300.0f;



    std::string HandWeapon = get_item_name(HELD);
    if (HandWeapon.empty())
    {

        return default_speed;
    }



    uint64_t baseProjectile = GetBaseProjectile(HELD);


    uint64_t primary_magazine = GetPrimaryMagazine(baseProjectile);
    if (!primary_magazine)
    {

        return default_speed;
    }



    uint64_t Ammo = driver::read<uint64_t>(primary_magazine + 0x20);
    if (!Ammo)
    {

        return default_speed;
    }



    uint64_t Unk3 = driver::read<uint64_t>(Ammo + 0x28);
    if (!Unk3)
    {

        return default_speed;
    }

    std::wstring magazine_shortname = driver::read_wstring(Unk3 + 0x14, 64);
    if (magazine_shortname.empty())
    {

        return default_speed;
    }

    if (bullets.find(magazine_shortname) == bullets.end())
    {

        return default_speed;
    }

    const float bullet_multiplier = bullets[magazine_shortname];


    auto print_and_return = [](const char* weaponName, float value, float dist) -> float {

        return value;
        };
    if (HandWeapon == std::string(_("rifle.ak")) || HandWeapon == std::string(_("rifle.ak.ice")) || HandWeapon == std::string(_("rifle.ak.diver"))) {
        if (dist <= 0.f) return print_and_return("AK", 375.9f * bullet_multiplier, dist);
        else if (dist <= 150.f) return print_and_return("AK", 386.f * bullet_multiplier, dist);
        else if (dist <= 200.f) return print_and_return("AK", 390.f * bullet_multiplier, dist);
        else if (dist <= 250.f) return print_and_return("AK", 400.f * bullet_multiplier, dist);
        else if (dist <= 300.0f) return print_and_return("AK", 410.f * bullet_multiplier, dist);
    }

    if (HandWeapon == std::string(_("hmlmg"))) return ("HMLMG", 500 * bullet_multiplier);
    if (HandWeapon == std::string(_("rifle.lr300"))) return ("LR300", 340.f * bullet_multiplier);
    if (HandWeapon == std::string(_("rifle.bolt"))) return ("Bolt", 579.f * bullet_multiplier);
    if (HandWeapon == std::string(_("rifle.l96"))) return ("L96", 1126.0f * bullet_multiplier);
    if (HandWeapon == std::string(_("rifle.m39"))) return ("M39", 445.f * bullet_multiplier);
    if (HandWeapon == std::string(_("rifle.semiauto")) || HandWeapon == std::string(_("rifle.sks"))) return ("Semi/SKS", 358.0f * bullet_multiplier);
    if (HandWeapon == std::string(_("lmg.m249"))) return ("M249", 450.f * bullet_multiplier);
    if (HandWeapon == std::string(_("smg.thompson")))
    {
        if (dist <= 180.f)
        {
            return 270.f * bullet_multiplier;
        }
        else
            if (dist <= 204.f)
            {
                return 250.f * bullet_multiplier;
            }
            else
                if (dist <= 250.f)
                {
                    return 245.f * bullet_multiplier;
                }
                else
                    if (dist <= 350.f)
                    {
                        return 230.f * bullet_multiplier;
                    }
    }							                              //FULL FIXLEDIM
    if (HandWeapon == std::string(_("smg.2")))
    {
        if (dist <= 160.f)
        {
            return 220.f * bullet_multiplier;
        }
        else
            if (dist <= 200.f)
            {
                return 195.f * bullet_multiplier;
            }
    }
    if (HandWeapon == std::string(_("smg.mp5"))) return 240.f * bullet_multiplier;  //220
    if (HandWeapon == std::string(_("pistol.prototype17"))) //FULL FIXLEDIM
    {
        if (dist <= 110.f)
        {
            return 300.f * bullet_multiplier;
        }
        else
            if (dist <= 169.f)
            {
                return 290.f * bullet_multiplier;
            }
            else
                if (dist <= 170.f)
                {
                    return 280.f * bullet_multiplier;
                }
                else
                    if (dist <= 180.f)
                    {
                        return 275.f * bullet_multiplier;
                    }
                    else
                        if (dist <= 190.f)
                        {
                            return 270.f * bullet_multiplier;
                        }
                        else
                            if (dist <= 200.f)
                            {
                                return 265.f * bullet_multiplier;
                            }
    }
    if (HandWeapon == std::string(_("pistol.python"))) return 300.f * bullet_multiplier;
    if (HandWeapon == std::string(_("pistol.semiauto"))) return 300.f * bullet_multiplier;
    if (HandWeapon == std::string(_("pistol.revolver"))) return 270.f * bullet_multiplier;
    if (HandWeapon == std::string(_("pistol.m92"))) return 300.f * bullet_multiplier;
    if (HandWeapon == std::string(_("pistol.eoka"))) return 90.f * bullet_multiplier;
    if (HandWeapon == std::string(_("pistol.nailgun")))
    {
        if (dist <= 60.f)
        {
            return 59.f * bullet_multiplier;
        }
        else
            if (dist <= 85.f)
            {
                return 58.3f * bullet_multiplier;
            }
            else
                return 57.8f * bullet_multiplier;
    }
    if (HandWeapon == std::string(_("crossbow"))) {
        if (dist <= 83.f)
        {
            return 90.f * bullet_multiplier;
        }
        else
            if (dist <= 100.f)
            {
                return 88.f * bullet_multiplier;
            }
            else
                if (dist <= 150.f)
                {
                    return 86.f * bullet_multiplier;
                }
                else
                    return 86.f * bullet_multiplier;
    }
    if (HandWeapon == std::string(_("bow.compound")))
    {
        if (dist <= 90.f)
        {
            return 120.f * bullet_multiplier;
        }
        else
            if (dist <= 150.F)
            {
                return  115.76f * bullet_multiplier;
            }
            else
                return  115.5f * bullet_multiplier;

    }
    if (HandWeapon == std::string(_("bow.hunting")) || HandWeapon == std::string(_("legacy bow"))) {
        if (dist <= 41.f)
        {
            return 60.f * bullet_multiplier;
        }
        else
            if (dist <= 82.f)
            {
                return  58.f * bullet_multiplier;
            }
            else
                if (dist <= 102.f)
                {
                    return  57.5f * bullet_multiplier;
                }
                else
                    if (dist <= 112.f)
                    {
                        return  57.3f * bullet_multiplier;
                    }
                    else
                        if (dist <= 127.f)
                        {
                            return  57.f * bullet_multiplier;
                        }
                        else
                            if (dist <= 146.f)
                            {
                                return  56.5f * bullet_multiplier;
                            }
                            else
                                if (dist <= 153.f)
                                {
                                    return  56.3f * bullet_multiplier;
                                }
                                else
                                    if (dist <= 163.f)
                                    {
                                        return  56.f * bullet_multiplier;
                                    }
                                    else
                                        if (dist <= 172.f)
                                        {
                                            return  55.7f * bullet_multiplier;
                                        }
                                        else
                                            if (dist <= 178.f)
                                            {
                                                return  55.5f * bullet_multiplier;
                                            }
                                            else
                                                if (dist <= 184.f)
                                                {
                                                    return  55.3f * bullet_multiplier;
                                                }
                                                else
                                                    if (dist <= 189.f)
                                                    {
                                                        return  55.1f * bullet_multiplier;
                                                    }
                                                    else
                                                        if (dist <= 196.f)
                                                        {
                                                            return  54.9f * bullet_multiplier;
                                                        }
                                                        else
                                                            if (dist <= 201.f)
                                                            {
                                                                return  54.7f * bullet_multiplier;
                                                            }
                                                            else
                                                                if (dist <= 206.f)
                                                                {
                                                                    return  54.5f * bullet_multiplier;
                                                                }
                                                                else
                                                                    if (dist <= 210.f)
                                                                    {
                                                                        return  54.3f * bullet_multiplier;
                                                                    }
                                                                    else
                                                                        if (dist <= 215.f)
                                                                        {
                                                                            return 54.1f * bullet_multiplier;
                                                                        }
                                                                        else
                                                                            if (dist <= 220.f)
                                                                            {
                                                                                return  53.9f * bullet_multiplier;
                                                                            }
                                                                            else
                                                                                if (dist <= 225.1f)
                                                                                {
                                                                                    return 53.7f * bullet_multiplier;
                                                                                }
                                                                                else
                                                                                    if (dist <= 230.1f)
                                                                                    {
                                                                                        return  53.5f * bullet_multiplier;
                                                                                    }
                                                                                    else
                                                                                        if (dist <= 233.1f)
                                                                                        {
                                                                                            return  53.3f * bullet_multiplier;
                                                                                        }
                                                                                        else
                                                                                            if (dist <= 237.1f)
                                                                                            {
                                                                                                return  53.1f * bullet_multiplier;
                                                                                            }
                                                                                            else
                                                                                                if (dist <= 241.1f)
                                                                                                {
                                                                                                    return  52.9f * bullet_multiplier;
                                                                                                }
                                                                                                else
                                                                                                    if (dist <= 244.1f)
                                                                                                    {
                                                                                                        return  52.7f * bullet_multiplier;
                                                                                                    }
                                                                                                    else
                                                                                                        if (dist <= 248.1f)
                                                                                                        {
                                                                                                            return  52.5f * bullet_multiplier;
                                                                                                        }
                                                                                                        else
                                                                                                            if (dist <= 252.1f)
                                                                                                            {
                                                                                                                return  52.3f * bullet_multiplier;
                                                                                                            }
                                                                                                            else
                                                                                                                if (dist <= 255.1f)
                                                                                                                {
                                                                                                                    return  52.1f * bullet_multiplier;
                                                                                                                }
                                                                                                                else
                                                                                                                    if (dist <= 500.f)
                                                                                                                    {
                                                                                                                        return 50.f * bullet_multiplier;
                                                                                                                    }
    }
    if (HandWeapon == std::string(_("shotgun.pump"))) return 100.0f * bullet_multiplier;
    if (HandWeapon == std::string(_("shotgun.spas12"))) return 100.0f * bullet_multiplier;
    if (HandWeapon == std::string(_("shotgun.waterpipe"))) return 100.0f * bullet_multiplier;
    if (HandWeapon == std::string(_("shotgun.double"))) return 100.0f * bullet_multiplier;

    return default_speed;
}



Vector3 Prediction(Vector3 LP_Pos, Vector3 Velocity, Vector3 BonePos) {
    const float Dist = LP_Pos.distance(BonePos);
  //  std::cout << "[DEBUG] Prediction - Distance to target: " << Dist << "\n";

    if (Dist > 0.001f) {
        const float BulletTime = Dist / ProjectileSpeed_Normal(Dist);
        //  std::cout << "[DEBUG] Prediction - Bullet time: " << BulletTime << "\n";

        const Vector3 vel = Velocity;
        // std::cout << "[DEBUG] Prediction - Target velocity: x=" << vel.x << " y=" << vel.y << " z=" << vel.z << "\n";

        Vector3 PredictVel;
        PredictVel.x = vel.x * BulletTime * 0.75f;
        PredictVel.y = vel.y * BulletTime * 0.75f;
        PredictVel.z = vel.z * BulletTime * 0.75f;
        // std::cout << "[DEBUG] Prediction - Predicted velocity: x=" << PredictVel.x << " y=" << PredictVel.y << " z=" << PredictVel.z << "\n";

        BonePos.x += PredictVel.x;
        BonePos.y += PredictVel.y;
        BonePos.z += PredictVel.z;
        BonePos.y += (4.905 * BulletTime * BulletTime);

        //  std::cout << "[DEBUG] Prediction - Predicted bone position: x=" << BonePos.x << " y=" << BonePos.y << " z=" << BonePos.z << "\n";
    }

    return BonePos;
}
class PlayerEyes
{
public:
    void SetSilent(Vector3 TargetAngles)
    {


        Vector4 Quat = ToQuat(Vector3(TargetAngles.x, TargetAngles.y, 0.0f));

   

        if (Quat.empty()) {
   
            return;
        }

        driver::write<Vector4>((uint64_t)this + 0x50, Quat);


    }
};

class PlayerInput
{
public:
    Vector3 GetVAngles()
    {
        Vector3 VAngles = driver::read<Vector3>((uint64_t)this + 0x44);
        if (!this && VAngles.empty())
            return Vector3();

        return VAngles;
    }
    void SetVAngles(Vector3 TargetAngles)
    {
        driver::write<Vector2>((uint64_t)this + 0x44, Vector2(TargetAngles.x, TargetAngles.y)); // private Vector3 bodyAngles;
    }
};









float distance(const Vector2& v1, const Vector2& v2) {
    float dx = v2.x - v1.x;
    float dy = v2.y - v1.y;
    return std::sqrt(dx * dx + dy * dy);
}

float NormalizeDeltaAngle(float delta) {
    while (delta > 180.0f) delta -= 360.0f;
    while (delta < -180.0f) delta += 360.0f;
    return delta;
}

// Deterministic hit chance function to replace random generation
// Deterministic hit chance function to replace random generation
uint64_t GetSilentAimSequenceValue() {
    // Use a combination of techniques for a non-random but variable pattern
    static uint64_t counter = 0;
    counter++;

    // Get current time for variation but in a predictable cycle
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);

    // Combine counter with time for a deterministic but variable sequence
    uint64_t sequence_value = (time.QuadPart + counter) % 100 + 1; // Value between 1-100

    return sequence_value;
}

auto aimbot() -> void
{
    // Local cache for the best target found in the loop iteration
    BasePlayer currentBestPlayer{};
    Vector3 currentBestBonePos{};
    Vector2 currentBestScreenPos{};
    float currentClosestPixelDist = FLT_MAX;
    bool foundTargetThisFrame = false;

    for (;;) {
        // --- Early exit if aimbot is disabled ---
        if (!g_Config.bEnableAimbot) {
            // Reset target info if disabled
            foundTargetThisFrame = false;
            currentClosestPixelDist = FLT_MAX;
            // Sleep longer if disabled to reduce CPU usage
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        // --- Get necessary data ---
        std::vector<BasePlayer> playersCopy;
        {
            std::scoped_lock lock(playersMutex);
            playersCopy = Players;
        }

        // Ensure local player and camera are valid before proceeding
        if (!globalCache.Localplayer || !globalCache.Camera) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Wait if cache isn't ready
            continue;
        }
        Vector3 localPos = driver::read<Vector3>(globalCache.Camera + 0x454); // Camera position might be better than player model pos for angle calcs
        if (localPos.empty()) { // Check if camera position read failed
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }


        int screen_width = GetSystemMetrics(SM_CXSCREEN);
        int screen_height = GetSystemMetrics(SM_CYSCREEN);
        Vector2 screenCenter = { static_cast<float>(screen_width) * 0.5f, static_cast<float>(screen_height) * 0.5f };

        // Reset best target for this frame's search
        float frameClosestPixelDist = FLT_MAX;
        BasePlayer frameBestPlayer{};
        Vector3 frameBestBonePos{};
        Vector2 frameBestScreenPos{};
        bool foundPlayerInFrame = false;

        // --- Iterate through players to find the best target ---
        for (const auto& player : playersCopy)
        {
            // Skip self or invalid players
            if (!player.address || player.address == globalCache.Localplayer) continue;
            // Add health check, team check, etc. if needed

            Vector3 targetBonePos{};
            Vector2 targetScreenPos{};
            float targetPixelDist = FLT_MAX;

            if (g_Config.bClosestBoneTargeting)
            {
                // Find the bone closest to the crosshair within FOV
                float closestBoneDistInFov = g_Config.fAimbotMaxFov; // Start with max allowed FOV
                Vector3 bestBoneForPlayer{};
                Vector2 bestScreenForPlayer{};
                bool foundValidBone = false;

                for (int boneId : validBones) {
                    Vector3 bone = GetBoneLocation(player.address, static_cast<BoneList>(boneId));
                    if (bone.empty()) continue; // Skip invalid bone locations

                    Vector2 screen_bone = WorldToScreen(bone, globalCache.viewmatrix);
                    // Basic visibility check (on screen)
                    if (screen_bone.x <= 0 || screen_bone.y <= 0 || screen_bone.x >= screen_width || screen_bone.y >= screen_height)
                        continue;

                    float pixel_distance = distance(screenCenter, screen_bone);

                    // Check if this bone is within FOV and closer than the current best *for this player*
                    if (pixel_distance < closestBoneDistInFov) {
                        closestBoneDistInFov = pixel_distance; // Update closest distance found so far for this player
                        bestBoneForPlayer = bone;
                        bestScreenForPlayer = screen_bone;
                        foundValidBone = true;
                    }
                }

                // If we found a valid bone for this player within FOV
                if (foundValidBone) {
                    targetBonePos = bestBoneForPlayer;
                    targetScreenPos = bestScreenForPlayer;
                    targetPixelDist = closestBoneDistInFov; // The distance of the best bone found
                }
                else {
                    continue; // No valid bone found for this player within FOV
                }
            }
            else // Use specific hitbox targeting
            {
                BoneList selectedHitbox;
                switch (g_Config.iAimbotHitbox) {
                case 1: selectedHitbox = BoneList::spine4; break;
                case 2: selectedHitbox = BoneList::pelvis; break;
                case 0:
                default: selectedHitbox = BoneList::head; break;
                }
                targetBonePos = GetBoneLocation(player.address, selectedHitbox);
                if (targetBonePos.empty()) continue; // Skip if target bone is invalid

                targetScreenPos = WorldToScreen(targetBonePos, globalCache.viewmatrix);
                // Basic visibility check (on screen)
                if (targetScreenPos.x <= 0 || targetScreenPos.y <= 0 || targetScreenPos.x >= screen_width || targetScreenPos.y >= screen_height)
                    continue;

                targetPixelDist = distance(screenCenter, targetScreenPos);

                // Check if the specific hitbox is outside the FOV
                if (targetPixelDist >= g_Config.fAimbotMaxFov) {
                    continue; // Skip player if target hitbox is outside FOV
                }
            }

            // --- Compare this player's target point with the overall best found so far ---
            // We already checked FOV implicitly or explicitly above
            if (targetPixelDist < frameClosestPixelDist)
            {
                frameClosestPixelDist = targetPixelDist;
                frameBestBonePos = targetBonePos;
                frameBestPlayer = player;
                frameBestScreenPos = targetScreenPos;
                foundPlayerInFrame = true;
            }
        } // End player loop

        // Update the persistent target info if a valid target was found this frame
        if (foundPlayerInFrame) {
            currentBestPlayer = frameBestPlayer;
            currentBestBonePos = frameBestBonePos;
            currentBestScreenPos = frameBestScreenPos;
            currentClosestPixelDist = frameClosestPixelDist;
            foundTargetThisFrame = true;
        }
        else {
            foundTargetThisFrame = false; // No target found or in FOV this frame
            currentClosestPixelDist = FLT_MAX; // Reset distance if no target
        }


        // --- Aimbot Activation Logic ---
        // Check if a valid target is locked and the aim key is pressed
        if (foundTargetThisFrame && (GetAsyncKeyState(g_Config.iAimbotKey) & 0x8000)) // Check if key is held down
        {
            // Ensure target bone position is valid before proceeding
            if (currentBestBonePos.empty()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                continue;
            }

            Vector3 aimPosition = currentBestBonePos;

            // --- Prediction ---
            if (g_Config.bEnablePrediction) {
                uint64_t modelPtr = driver::read<uint64_t>(currentBestPlayer.address + offset::PlayerModel);
                if (modelPtr) { // Check if model pointer is valid
                    Vector3 velocity = driver::read<Vector3>(modelPtr + offset::Velocity);
                    // Only predict if velocity is significant? (Optional)
                    // float speed = sqrt(velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z);
                    // if (speed > 0.1f) { // Example threshold
                    aimPosition = Prediction(localPos, velocity, currentBestBonePos);
                    // }
                }
            }

            // --- Calculate Angles ---
            Vector3 angles = CalcAngle(localPos, aimPosition);
            Normalize(angles.y, angles.x); // Ensure angles are within valid ranges

            // --- Set Angles (Normal or Silent) ---
            if (!isnan(angles.x) && !isnan(angles.y)) // Check for calculation errors
            {
                if (g_Config.bEnableSilentAim)
                {
                    // --- Silent Aim with Deterministic Hit Chance ---
                    uint64_t hitChanceValue = GetSilentAimSequenceValue(); // Get deterministic value instead of random

                    // Apply hit chance calculation - only apply silent aim if the hit chance succeeds
                    if (hitChanceValue <= g_Config.iHitChancePercent)
                    {
                        // Silent Aim: Modify server-side view angles (less visible client-side)
                        uint64_t eyesEnc = driver::read<uint64_t>(globalCache.Localplayer + offset::PlayerEyes);
                        auto eyesPtr = decryptions::DecryptPlayerEyes(eyesEnc); // Ensure decryption works
                        if (eyesPtr) {
                            reinterpret_cast<PlayerEyes*>(eyesPtr)->SetSilent(angles);
                        }
                    }
                    // If hit chance fails, do nothing this frame (maintains original behavior)
                }
                else
                {
                    // Normal Aim: Modify client-side input angles with smoothing
                    uint64_t inputPtr = driver::read<uint64_t>(globalCache.Localplayer + offset::PlayerInput);

                    if (inputPtr) {
                        PlayerInput* playerInput = reinterpret_cast<PlayerInput*>(inputPtr);

                        // Apply smoothing if enabled
                        if (g_Config.iSmoothingAmount > 1) {
                            Vector3 currentAngles = playerInput->GetVAngles();

                            // Calculate the difference, handling wrap-around
                            Vector3 deltaAngles = angles - currentAngles;
                            deltaAngles.y = NormalizeDeltaAngle(deltaAngles.y); // Normalize Yaw delta [-180, 180]
                            deltaAngles.x = NormalizeDeltaAngle(deltaAngles.x); // Normalize Pitch delta [-180, 180]

                            // Apply smoothing factor
                            float smoothDivisor = static_cast<float>(std::clamp(g_Config.iSmoothingAmount, 1, 100));
                            Vector3 smoothedAngles = currentAngles + (deltaAngles / smoothDivisor);

                            // Normalize the final smoothed angles before setting
                            Normalize(smoothedAngles.y, smoothedAngles.x);

                            // Set the smoothed angles
                            playerInput->SetVAngles(smoothedAngles);
                        }
                        else {
                            // No smoothing, set angles directly
                            playerInput->SetVAngles(angles);
                        }
                    }
                }
            }
        } // End aim key check

        // --- Sleep ---
        // Sleep for a short duration to control loop frequency and reduce CPU load
        std::this_thread::sleep_for(std::chrono::milliseconds(5)); // Adjust sleep time as needed

    } // End infinite loop
}