#pragma once
#include "../imports.h"

inline int64_t GetClothingItemsList(uint64_t address) {
    uint64_t Inventory = decryptions::decrypt_inventory_pointer(driver::read<uint64_t>(address + offset::PlayerInventory));

    LogDebug("[ClothingList] Inventory: 0x" + std::to_string(Inventory));

    uint64_t Belt = driver::read<uint64_t>(Inventory + offset::Wear);
    if (!Belt) {
        LogDebug("[ClothingList] Belt NULL");
        return 0;
    }
    LogDebug("[ClothingList] Belt: 0x" + std::to_string(Belt));

    uint64_t ItemList = driver::read<uint64_t>(Belt + offset::ItemList);
    if (!ItemList) {
        LogDebug("[ClothingList] ItemList NULL");
        return 0;
    }
    LogDebug("[ClothingList] ItemList: 0x" + std::to_string(ItemList));

    uint64_t LastItemList = driver::read<uint64_t>(ItemList + 0x10);
    if (!LastItemList) {
        LogDebug("[ClothingList] LastItemList NULL");
        return 0;
    }
    LogDebug("[ClothingList] LastItemList: 0x" + std::to_string(LastItemList));

    return LastItemList;
}

inline Cloth GetClothingItems(uint64_t ItemsList, int Slot) {


    constexpr uint64_t SlotOffsets[] = { 0x20, 0x28, 0x30, 0x38, 0x40, 0x48, 0x50 };
    if ((Slot + 1) < 1 || (Slot + 1) > 7) {
        LogDebug("[ClothingItem] Invalid Slot: " + std::to_string(Slot));
        return Cloth();
    }

    uint64_t SlotItem = driver::read<uint64_t>(ItemsList + SlotOffsets[(Slot + 1) - 1]);
    if (!SlotItem) {
        LogDebug("[ClothingItem] SlotItem NULL at slot " + std::to_string(Slot));
        return Cloth();
    }

    uint64_t Item1Info = driver::read<uint64_t>(SlotItem + offset::ItemDefinition);
    if (!Item1Info) {
        LogDebug("[ClothingItem] Item1Info NULL");
        return Cloth();
    }

    uint64_t ItemDisplayName = driver::read<uint64_t>(Item1Info + 0x28);
    if (!ItemDisplayName) {
        LogDebug("[ClothingItem] DisplayName NULL");
        return Cloth();
    }

    float health = driver::read<float>(SlotItem + offset::Health);
    float MAXHEALTH = driver::read<float>(SlotItem + offset::MaxHealth);

    std::wstring Name = driver::read_wstring(ItemDisplayName + 0x14, 128);
    std::string output(Name.begin(), Name.end());

    LogDebug("[ClothingItem] Slot " + std::to_string(Slot) + ": " + output + " (HP: " + std::to_string((int)health) + "/" + std::to_string((int)MAXHEALTH) + ")");

    return { output, health, MAXHEALTH };
}

inline uint64_t GetItemsList(uint64_t address) {
    uint64_t Inventory = decryptions::decrypt_inventory_pointer(driver::read<uint64_t>(address + offset::PlayerInventory));

    LogDebug("[BeltList] Inventory: 0x" + std::to_string(Inventory));

    uint64_t Belt = driver::read<uint64_t>(Inventory + offset::Belt);
    if (!Belt) {
        LogDebug("[BeltList] Belt NULL");
        return 0;
    }
    LogDebug("[BeltList] Belt: 0x" + std::to_string(Belt));

    uint64_t ItemList = driver::read<uint64_t>(Belt + offset::ItemList);

    LogDebug("[BeltList] ItemList: 0x" + std::to_string(ItemList));

    uint64_t LastItemList = driver::read<uint64_t>(ItemList + 0x10);
    if (!LastItemList) {
        LogDebug("[BeltList] LastItemList NULL");
        return 0;
    }
    LogDebug("[BeltList] LastItemList: 0x" + std::to_string(LastItemList));

    return LastItemList;
}

inline Belt GetBeltItems(uint64_t ItemsList, int Slot) {
    if (!ItemsList) {
        LogDebug("[BeltItem] ItemsList NULL");
        return Belt();
    }

    uint64_t SlotItem = driver::read<uint64_t>(ItemsList + ((Slot) * 8) + 0x20);
    if (!SlotItem) {
        //   LogDebug("[BeltItem] SlotItem NULL at slot " + std::to_string(Slot));
        return Belt();
    }

    uint64_t Item1Info = driver::read<uint64_t>(SlotItem + offset::ItemDefinition);
    if (!Item1Info) {
        LogDebug("[BeltItem] Item1Info NULL");
        return Belt();
    }

    uint64_t ItemDisplayName = driver::read<uint64_t>(Item1Info + 0x28);
    if (!ItemDisplayName) {
        LogDebug("[BeltItem] DisplayName NULL");
        return Belt();
    }

    int ItemAmmo = driver::read<int>(SlotItem + offset::Amount);
    int WeaponAmmo = driver::read<int>(SlotItem + 0xa8);
    float health = driver::read<float>(SlotItem + offset::Health);
    float MAXHEALTH = driver::read<float>(SlotItem + offset::MaxHealth);

    std::wstring Name = driver::read_wstring(ItemDisplayName + 0x14, 128);
    std::string output(Name.begin(), Name.end());

    LogDebug("[BeltItem] Slot " + std::to_string(Slot) + ": " + output +
        " Ammo: " + std::to_string(ItemAmmo) + "/" + std::to_string(WeaponAmmo) +
        " HP: " + std::to_string((int)health) + "/" + std::to_string((int)MAXHEALTH));

    return { output, ItemAmmo, WeaponAmmo, health, MAXHEALTH };
}
