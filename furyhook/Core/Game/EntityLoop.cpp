#include "EntityLoop.hpp"

#include <unordered_map>
#include <unordered_set>
#include <algorithm>

inline Vector3 GetTransformPosition(uint64_t transform)
{
    if (!transform)
        return Vector3();

    uint64_t transform_internal = driver::read<uint64_t>(transform + 0x10);
    if (!transform_internal)
        return Vector3();

    UnityEngine_TransformAccessReadOnly accessReadOnly = driver::read<UnityEngine_TransformAccessReadOnly>(transform_internal + 0x38);
    if (!accessReadOnly.TransformData || accessReadOnly.Index < 0 || accessReadOnly.Index > 255)
        return Vector3();

    UnityEngine_TransformData transformData = driver::read<UnityEngine_TransformData>(accessReadOnly.TransformData + 0x18);
    if (!transformData.TransformArray || !transformData.TransformIndices)
        return Vector3();

    size_t matrixCount = accessReadOnly.Index + 1;
    size_t sizeMatricesBuf = sizeof(Matrix3x4) * matrixCount;
    size_t sizeIndicesBuf = sizeof(int) * matrixCount;

    std::vector<uint8_t> matricesBuf(sizeMatricesBuf);
    std::vector<uint8_t> indicesBuf(sizeIndicesBuf);

    if (!driver::readsize(transformData.TransformArray, matricesBuf.data(), sizeMatricesBuf) ||
        !driver::readsize(transformData.TransformIndices, indicesBuf.data(), sizeIndicesBuf))
        return Vector3();

    __m128 result = *reinterpret_cast<__m128*>(matricesBuf.data() + 0x30 * accessReadOnly.Index);
    int transformIndex = *reinterpret_cast<int*>(indicesBuf.data() + 0x4 * accessReadOnly.Index);

    __m128 mulVec0 = { -2.f, 2.f, -2.f, 0.f };
    __m128 mulVec1 = { 2.f, -2.f, -2.f, 0.f };
    __m128 mulVec2 = { -2.f, -2.f, 2.f, 0.f };

    while (transformIndex >= 0 && transformIndex * sizeof(Matrix3x4) < sizeMatricesBuf)
    {
        auto* matrixPtr = reinterpret_cast<Matrix3x4*>(matricesBuf.data() + transformIndex * sizeof(Matrix3x4));
        Matrix3x4& m = *matrixPtr;

        __m128 xxxx = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*) & m._21, 0x00));
        __m128 yyyy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*) & m._21, 0x55));
        __m128 zwxy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*) & m._21, 0x8E));
        __m128 wzyw = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*) & m._21, 0xDB));
        __m128 zzzz = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*) & m._21, 0xAA));
        __m128 yxwy = _mm_castsi128_ps(_mm_shuffle_epi32(*(__m128i*) & m._21, 0x71));
        __m128 tmp7 = _mm_mul_ps(*(__m128*) & m._31, result);

        result = _mm_add_ps(
            _mm_add_ps(
                _mm_add_ps(
                    _mm_mul_ps(
                        _mm_sub_ps(
                            _mm_mul_ps(_mm_mul_ps(xxxx, mulVec1), zwxy),
                            _mm_mul_ps(_mm_mul_ps(yyyy, mulVec2), wzyw)),
                        _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0xAA))),
                    _mm_mul_ps(
                        _mm_sub_ps(
                            _mm_mul_ps(_mm_mul_ps(zzzz, mulVec2), wzyw),
                            _mm_mul_ps(_mm_mul_ps(xxxx, mulVec0), yxwy)),
                        _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x55)))),
                _mm_add_ps(
                    _mm_mul_ps(
                        _mm_sub_ps(
                            _mm_mul_ps(_mm_mul_ps(yyyy, mulVec0), yxwy),
                            _mm_mul_ps(_mm_mul_ps(zzzz, mulVec1), zwxy)),
                        _mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(tmp7), 0x00))),
                    tmp7)),
            *(__m128*) & m._11);

        if (transformIndex * 4 + 4 > sizeIndicesBuf)
            break;

        transformIndex = *reinterpret_cast<int*>(indicesBuf.data() + 0x4 * transformIndex);
    }

    return Vector3{ result.m128_f32[0], result.m128_f32[1], result.m128_f32[2] };
}

inline Vector3 GetBoneLocation(uintptr_t player, int boneIndex)
{
    if (!player || boneIndex < 0 || boneIndex > 255)
        return Vector3();

    uint64_t model = driver::read<uint64_t>(player + 0xC8);
    if (!model)
        return Vector3();

    uint64_t boneList = driver::read<uint64_t>(model + 0x50);
    if (!boneList)
        return Vector3();

    uint64_t bone = driver::read<uint64_t>(boneList + 0x20 + boneIndex * 0x8);
    if (!bone)
        return Vector3();

    return GetTransformPosition(bone);
}



inline std::string GetName(uint64_t Player)
{
    unity_string* t2 = driver::read<unity_string*>((uint64_t)Player + offset::DisplayName);
    if (!t2)
        return std::string();

    unity_string ustr = driver::read<unity_string>((uint64_t)t2);
    if (!ustr.buffer)
        return std::string();

    std::wstring tmpe(ustr.buffer);
    if (tmpe.empty())
        return std::string();

    std::wstring filteredText;
    for (char c : tmpe) {
        if ((c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9') || c == L' ' || c == L'_') {
            filteredText += c;
        }
        else {
            filteredText += L'.';
        }
    }

    return std::string(filteredText.begin(), filteredText.end());
}

struct Vector3Hash {
    size_t operator()(const Vector3& v) const {
        // Simple hash function for Vector3 - adjust precision as needed 
        return std::hash<int>()(int(v.x * 10)) ^
            std::hash<int>()(int(v.y * 10)) ^
            std::hash<int>()(int(v.z * 10));
    }
};

// Helper function for distance calculation
float Vector3Distance(const Vector3& a, const Vector3& b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float dz = a.z - b.z;
    return sqrt(dx * dx + dy * dy + dz * dz);
}



auto EntityThread() -> void
{
    // Define constants
    const float MAX_PREFAB_DISTANCE = 300.0f;
    const int PREFAB_TIMEOUT_MS = 100; // Increased timeout for debugging
    const float PREFAB_MATCH_DISTANCE = 2.0f; // Increased distance tolerance for matching

    printf("[INFO] EntityThread started.\n"); // Indicate thread start

    for (;;)
    {
        std::vector<PrefabEntry> currentPrefabs; // Prefabs found in this frame

        if (!globalCache.ObjectDictionary) {
            printf("[ERROR] EntityThread: ObjectDictionary is null!\n");
            std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Longer sleep on critical error
            continue;
        }

        // Read entity list size safely
        uint32_t entitySize = 0;
        if (globalCache.EntityListBase) { // Check if EntityListBase itself is valid
            entitySize = driver::read<uint32_t>(globalCache.ObjectDictionary + 0x10); // Offset for size might vary, double check this!
            // printf("[DEBUG] EntityThread: Found entity list size: %u\n", entitySize); // Optional: Check size
            if (entitySize > 20000) { // Sanity check for unreasonable size
                printf("[WARN] EntityThread: Unusually large entity size: %u. Skipping frame.\n", entitySize);
                entitySize = 0; // Prevent huge loop
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
        }
        else {
            printf("[ERROR] EntityThread: EntityListBase is null!\n");
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }


        std::vector<BasePlayer> tempPlayers;

        Vector3 localPlayerPos = { 0,0,0 };
        bool localPlayerValid = false;
        if (globalCache.Localplayer) {
            const uint64_t playerModel = driver::read<uint64_t>(globalCache.Localplayer + offset::PlayerModel);
            if (playerModel) {
                localPlayerPos = driver::read<Vector3>(playerModel + offset::Position);
                // Basic sanity check for position
                if (localPlayerPos.x != 0 || localPlayerPos.y != 0 || localPlayerPos.z != 0) {
                    localPlayerValid = true;
                }
                else {
                    // printf("[DEBUG] EntityThread: LocalPlayer position is (0,0,0).\n");
                }
            }
            else {
                // printf("[DEBUG] EntityThread: LocalPlayer PlayerModel is null.\n");
            }
        }
        else {
            // printf("[DEBUG] EntityThread: LocalPlayer is null.\n");
        }

        for (uint32_t i = 0; i < entitySize; ++i)
        {
            uint64_t entityAddress = driver::read<uint64_t>(globalCache.EntityListBase + 0x20 + (i * 0x08));
            if (!entityAddress) continue;

            const uint64_t entityObject = driver::read<uint64_t>(entityAddress + 0x10);
            if (!entityObject) continue;
            const uint64_t gameObject = driver::read<uint64_t>(entityObject + 0x30);
            if (!gameObject) continue;
            const uint64_t namePtr = driver::read<uint64_t>(gameObject + 0x60);
            if (!namePtr) continue;
            std::string rawName = driver::ReadChar(namePtr); // Read the raw name once
           // if (rawName.empty()) continue; // Skip if name reading failed

         //   printf("[VERBOSE] Entity %u: Addr=0x%llX, Name='%s'\n", i, entityAddress, rawName.c_str()); // Very verbose, uncomment if needed

            // --- Check for Other Prefabs (Ores, Collectibles, etc.) ---
            if (const PrefabInfo* prefabInfo = GetPrefabInfo(rawName)) {
                // printf("[DEBUG] EntityThread: Matched PrefabInfo for raw name: %s -> %s\n", rawName.c_str(), prefabInfo->displayName.c_str()); // Check if GetPrefabInfo works
                uint64_t transform = driver::readchain<uint64_t>(entityObject, { 0x30, 0x30, 0x8, 0x38 }); // Simplified chain? Verify offset
                if (!transform) {
                    // printf("[DEBUG] EntityThread: Prefab '%s' - Null transform\n", rawName.c_str());
                    continue;
                }
                Vector3 prefabpos = driver::read<Vector3>(transform + 0x90); // Position offset

                float distance = localPlayerValid ? Vector3Distance(localPlayerPos, prefabpos) : 0.0f;
                // printf("[DEBUG] EntityThread: Found 'Other' Prefab: %s (%s) at %.1f, %.1f, %.1f (Dist: %.1f)\n",
                //        rawName.c_str(), prefabInfo->displayName.c_str(), prefabpos.x, prefabpos.y, prefabpos.z, distance); // More detailed print

                if (localPlayerValid && distance > MAX_PREFAB_DISTANCE) {
                    // printf("[DEBUG] EntityThread: Prefab '%s' too far (%.1f > %.1f)\n", prefabInfo->displayName.c_str(), distance, MAX_PREFAB_DISTANCE);
                    continue; // Skip if too far
                }

         //       printf("[DEBUG] EntityThread: Adding 'Other' Prefab to current frame: %s\n", prefabInfo->displayName.c_str()); // <-- Key Print
                currentPrefabs.push_back({
                    prefabInfo->displayName, // Use clean name from PrefabInfo
                    prefabpos,
                    prefabInfo->color,       // Use color from PrefabInfo
                    std::chrono::steady_clock::now(),
                    PrefabType::Other        // *** SET TYPE ***
                    });
                continue; // Skip to next entity if identified as a known prefab
            }

            // --- Check for Dropped Items ---
            // Use a more robust check if needed, e.g., check components or tags if possible
            if (rawName.find("(world)") != std::string::npos || rawName.find("dropped_item.prefab") != std::string::npos) // Added common prefab name
            {
                // printf("[DEBUG] EntityThread: Potential Dropped Item found: %s\n", rawName.c_str()); // Check if condition is met
                uint64_t transform = driver::readchain<uint64_t>(entityObject, { 0x30, 0x8, 0x38 }); // Simplified chain? Verify offset
                if (!transform) {
                    // printf("[DEBUG] EntityThread: Dropped '%s' - Null transform\n", rawName.c_str());
                    continue;
                }
                Vector3 prefabpos = driver::read<Vector3>(transform + 0x90); // Position offset

                float distance = localPlayerValid ? Vector3Distance(localPlayerPos, prefabpos) : 0.0f;
                // printf("[DEBUG] EntityThread: Found Dropped Item Raw: %s at %.1f, %.1f, %.1f (Dist: %.1f)\n",
                //       rawName.c_str(), prefabpos.x, prefabpos.y, prefabpos.z, distance); // More detailed print

                if (localPlayerValid && distance > MAX_PREFAB_DISTANCE) {
                    // printf("[DEBUG] EntityThread: Dropped '%s' too far (%.1f > %.1f)\n", rawName.c_str(), distance, MAX_PREFAB_DISTANCE);
                    continue; // Skip if too far
                }

                std::string filteredName = FilterDroppedItem(rawName);
                if (!filteredName.empty())
                {
            //        printf("[DEBUG] EntityThread: Adding 'DroppedItem' Prefab to current frame: %s (Filtered from %s)\n", filteredName.c_str(), rawName.c_str()); // <-- Key Print
                    currentPrefabs.push_back({
                        filteredName,             // Use filtered name
                        prefabpos,
                        IM_COL32(200, 200, 200, 255), // Use a distinct color for dropped items (e.g., light grey)
                        std::chrono::steady_clock::now(),
                        PrefabType::DroppedItem   // *** SET TYPE ***
                        });
                }
                else {
                    // printf("[DEBUG] EntityThread: Dropped '%s' filtered to empty.\n", rawName.c_str());
                }
                continue; // Skip to next entity if identified as a dropped item
            }

            // --- Check for Players ---
            if (rawName.find("player.prefab") != std::string::npos) // Example: include scientists
            {
              //   printf("[DEBUG] EntityThread: Found Player/Scientist: %s\n", rawName.c_str()); // Check if condition is met
                BasePlayer entity; // Create player struct only if it's a player
                entity.address = entityAddress;

                const uint64_t playerModel = driver::read<uint64_t>(entity.address + offset::PlayerModel);
                if (!playerModel) {
                    // printf("[DEBUG] EntityThread: Player '%s' - Null PlayerModel\n", rawName.c_str());
                    continue;
                }
                entity.position = driver::read<Vector3>(playerModel + offset::Position);
                entity.name = GetName(entity.address); // Ensure GetName works

                // Optional: Add bone reading back if needed, ensure GetBoneLocation works
              
                static const std::vector<int> boneIds = {
                    BoneList::head, BoneList::eye, BoneList::neck,
                    BoneList::l_upperarm, BoneList::r_upperarm,
                    BoneList::l_forearm, BoneList::r_forearm,
                    BoneList::l_hand, BoneList::r_hand,
                    BoneList::spine4, BoneList::pelvis,
                    BoneList::l_hip, BoneList::r_hip,
                    BoneList::l_knee, BoneList::r_knee,
                    BoneList::l_foot, BoneList::r_foot
                };

                entity.bones.reserve(boneIds.size());
                for (int idx = 0; idx < boneIds.size(); idx++)
                {
                    int boneId = boneIds[idx];
                    Vector3 bone_pos = GetBoneLocation(entity.address, boneId);
                    entity.bones.push_back(bone_pos);
                }

                tempPlayers.push_back(std::move(entity));
            }
        } // End entity loop

        // --- Update Global Caches ---
        {
            std::scoped_lock lock(playersMutex);
            // printf("[DEBUG] EntityThread: Updating Players cache. Old size: %zu, New size: %zu\n", Players.size(), tempPlayers.size());
            Players = std::move(tempPlayers);
        }

        {
            std::scoped_lock lock(prefabMutex);
            // printf("[DEBUG] EntityThread: Prefab Update Start: Cache Size=%zu, Current Frame Found=%zu\n", prefabCache.size(), currentPrefabs.size()); // <-- Key Print

            auto now = std::chrono::steady_clock::now();
            std::vector<bool> currentHandled(currentPrefabs.size(), false);
            size_t removed_count = 0;
            size_t updated_count = 0;

            // Iterate existing cache to update timestamps or remove expired
            prefabCache.erase(
                std::remove_if(prefabCache.begin(), prefabCache.end(),
                    [&](PrefabEntry& existing) {
                        bool updated_this_entry = false;
                        // Try to find a match in current frame's prefabs
                        for (size_t i = 0; i < currentPrefabs.size(); ++i) {
                            if (currentHandled[i]) continue; // Already matched

                            // Match based on type and position proximity
                            if (existing.type == currentPrefabs[i].type &&
                                Vector3Distance(existing.location, currentPrefabs[i].location) < PREFAB_MATCH_DISTANCE) // Use constant
                            {
                                // printf("[VERBOSE] EntityThread: Updating existing prefab '%s'\n", existing.PrefabName.c_str());
                                existing.lastSeen = now; // Update timestamp
                                existing.location = currentPrefabs[i].location; // Update position slightly
                                existing.PrefabName = currentPrefabs[i].PrefabName; // Update name (e.g., dropped item count)
                                existing.color = currentPrefabs[i].color; // Update color if needed
                                currentHandled[i] = true; // Mark as handled
                                updated_this_entry = true;
                                updated_count++;
                                break; // Found match for this existing entry
                            }
                        }
                        // If not updated, check if expired
                        if (!updated_this_entry) {
                            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - existing.lastSeen).count();
                            if (elapsed > PREFAB_TIMEOUT_MS) { // Use constant
                                // printf("[VERBOSE] EntityThread: Removing expired prefab '%s' (elapsed: %lld ms)\n", existing.PrefabName.c_str(), elapsed);
                                removed_count++;
                                return true; // Mark for removal if expired
                            }
                        }
                        return false; // Keep if updated or not expired
                    }),
                prefabCache.end()
            );

            // Add new prefabs from current frame that weren't matched
            size_t added_count = 0;
            for (size_t i = 0; i < currentPrefabs.size(); ++i) {
                if (!currentHandled[i]) {
                    // printf("[VERBOSE] EntityThread: Adding new prefab to cache: %s\n", currentPrefabs[i].PrefabName.c_str());
                    prefabCache.push_back(currentPrefabs[i]); // Add new entry
                    added_count++;
                }
            }
          //  printf("[DEBUG] EntityThread: Prefab Update End: Cache Size=%zu (Added %zu, Updated %zu, Removed %zu)\n", prefabCache.size(), added_count, updated_count, removed_count); // <-- Key Print
        } // End prefabMutex scope

        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Slightly longer sleep might be okay
    } // End infinite loop

    printf("[INFO] EntityThread finished (should not happen in infinite loop).\n");
}
