#include <cstdint>

namespace offset
{ 
	inline uint64_t Il2CppGetHandle = 0xBF45AD0;
	inline uint64_t BaseNetworkable = 0xBCA9B50;
	inline uint64_t BaseNetworkable1 = 0x10;
	inline uint64_t BaseNetworkable2 = 0x10;
	inline uint64_t BaseNetworkable3 = 0x18;
	inline uint64_t MainCamera = 0xBC94C10; // 0x30
	inline uint64_t MainCamera1 = 0x30;
	inline uint64_t TOD_Sky = 0xBCA0848;
	inline uint64_t SingletonComponent = 0xBC40470;
	inline uint64_t ConsoleSystem = 0xBCCB550;
	inline uint64_t OcclusionCulling = 0xBCF9138;
	inline uint64_t FacepunchInput = 0xBCCFE60; // 0x48;

	// BasePlayer
	inline uint64_t ClActiveItem = 0x458;
	inline uint64_t PlayerEyes = 0x648;
	inline uint64_t PlayerInventory = 0x590;
	inline uint64_t CurrrentTeam = 0x428;
	inline uint64_t BaseMovement = 0x300;
	inline uint64_t PlayerModel = 0x408;
	inline uint64_t PlayerFlags = 0x550;
	inline uint64_t DisplayName = 0x558;
	inline uint64_t PlayerInput = 0x328;
	inline uint64_t Lifestate = 0x234;
	inline uint64_t Position = 0x1D8;
	inline uint64_t NewPosition = 0x454;
	inline uint64_t Velocity = 0x1FC;

	// PlayerModel
	inline uint64_t SkinnedMultiMesh = 0x3C8;

	// SkinnedMultiMesh
	inline uint64_t RendererList = 0x58;

	// PlayerInventory
	inline uint64_t Belt = 0x38;
	inline uint64_t Wear = 0x60;

	// %8e84899c790cfa0b6ba4cc05d3f5871f0146c0e1 (ItemContainer)
	inline uint64_t ItemList = 0x60;

	// %e1ac09b7f113111b9b80a112c6358b07ced328b8 (Item)
	inline uint64_t Health = 0x48;
	inline uint64_t MaxHealth = 0x70;
	inline uint64_t Amount = 0x78;
	inline uint64_t ItemDefinition = 0xA8;
	inline uint64_t HeldItem = 0x18;
	inline uint64_t ItemId = 0x28;

	// BaseProjectile
	inline uint64_t RecoilProperties = 0x340;
	inline uint64_t PrimaryMagazine = 0x318;

	// RecoilProperties
	inline uint64_t NewRecoilOverride = 0x80;
	inline uint64_t RecoilYawMin = 0x18;
	inline uint64_t RecoilYawMax = 0x1C;
	inline uint64_t RecoilPitchMin = 0x20;
	inline uint64_t RecoilPitchMax = 0x24;

	// HeldEntity
	inline uint64_t ViewModel = 0x218;

	// Input.Button
	inline uint64_t IsDown = 0x28;
	inline uint64_t Name = 0x20;
	inline uint64_t Binds = 0x30;
	inline uint64_t Code = 0x1C;
}