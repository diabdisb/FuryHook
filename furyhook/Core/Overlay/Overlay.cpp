#include  "../imports.h"
#include  <thread>
#include "../config.hpp"

#include  <imgui_internal.h>
#include "../../font.h"
#include "../../image.h"
#include <imgui_freetype.h>



#include "../../notification.h"




namespace pic
{
	ID3D11ShaderResourceView* background = nullptr;

	ID3D11ShaderResourceView* aimbot = nullptr;
	ID3D11ShaderResourceView* visuals = nullptr;
	ID3D11ShaderResourceView* skins = nullptr;
	ID3D11ShaderResourceView* settings = nullptr;

	ID3D11ShaderResourceView* combo_widget = nullptr;
	ID3D11ShaderResourceView* input_widget = nullptr;

	ID3D11ShaderResourceView* menu_settings_icon = nullptr;

	ID3D11ShaderResourceView* circle_success = nullptr;
	ID3D11ShaderResourceView* circle_error = nullptr;

}

	
auto get_process_wnd(uint32_t pid) -> HWND
{
	std::pair<HWND, uint32_t> params = { 0, pid };
	BOOL bResult = EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
		auto pParams = (std::pair<HWND, uint32_t>*)(lParam);
		uint32_t processId = 0;

		if (GetWindowThreadProcessId(hwnd, reinterpret_cast<LPDWORD>(&processId)) && processId == pParams->second) {
			SetLastError((uint32_t)-1);
			pParams->first = hwnd;
			return FALSE;
		}

		return TRUE;

		}, (LPARAM)&params);

	if (!bResult && GetLastError() == -1 && params.first)
		return params.first;

	return NULL;
}


auto set_window_target() -> void {
	while (true) {
		GameWnd = get_process_wnd(driver::detail::pid);
		if (GameWnd) {
			ZeroMemory(&GameRect, sizeof(GameRect));
			GetClientRect(GameWnd, &GameRect); // Use GetClientRect instead of GetWindowRect
			DWORD dwStyle = GetWindowLong(GameWnd, GWL_STYLE);
			if (dwStyle & WS_BORDER) {
				GameRect.top += 32;
				Height -= 39;
			}

			// Ensuring that the calculated position and size match the client area
			ScreenCenterX2 = GameRect.right / 2;
			ScreenCenterY2 = GameRect.bottom / 2;

			// Adjust the overlay window position and size based on the game client area
			MoveWindow(my_wnd, GameRect.left, GameRect.top, GameRect.right, GameRect.bottom, true);
		}
	}
}

auto setup_window() -> void
{
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)set_window_target, 0, 0, 0);
	WNDCLASSEXA wcex = {
		sizeof(WNDCLASSEXA),
		0,
		DefWindowProcA,
		0,
		0,
		nullptr,
		LoadIcon(nullptr, IDI_APPLICATION),
		LoadCursor(nullptr, IDC_ARROW),
		nullptr,
		nullptr,
		_("Windows Explorer"),
		LoadIcon(nullptr, IDI_APPLICATION)
	};

	RECT Rect;
	GetWindowRect(GetDesktopWindow(), &Rect);

	RegisterClassExA(&wcex);

	my_wnd = CreateWindowExA(NULL, _("Windows Explorer"), _("Windows Explorer"), WS_POPUP, Rect.left, Rect.top, Rect.right, Rect.bottom, NULL, NULL, wcex.hInstance, NULL);
	SetWindowLong(my_wnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
	SetLayeredWindowAttributes(my_wnd, RGB(0, 0, 0), 255, LWA_ALPHA);

	MARGINS margin = { -1 };
	DwmExtendFrameIntoClientArea(my_wnd, &margin);

	ShowWindow(my_wnd, SW_SHOW);
	UpdateWindow(my_wnd);
}
void ApplyModernStyle() {
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowPadding = ImVec2(15, 15);
	style.WindowRounding = 5.0f;
	style.FramePadding = ImVec2(5, 5);
	style.FrameRounding = 4.0f;
	style.ItemSpacing = ImVec2(12, 8);
	style.ItemInnerSpacing = ImVec2(8, 6);
	style.IndentSpacing = 25.0f;
	style.ScrollbarSize = 15.0f;
	style.ScrollbarRounding = 9.0f;
	style.GrabMinSize = 5.0f;
	style.GrabRounding = 3.0f;
	style.TabRounding = 4.0f;
	style.ChildRounding = 4.0f;

	ImVec4* colors = style.Colors;
	colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 0.95f); // Slightly transparent
	colors[ImGuiCol_ChildBg] = ImVec4(0.07f, 0.07f, 0.09f, 0.95f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 0.95f);
	colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f); // Checkbox check color
	colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f); // Collapsing header
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f); // Tab background
	colors[ImGuiCol_TabHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f); // Tab hover
	colors[ImGuiCol_TabActive] = ImVec4(0.18f, 0.17f, 0.20f, 1.00f); // Active tab background
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.18f, 0.17f, 0.20f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
}


HRESULT directx_init() {
	std::cout << "Initializing DirectX..." << std::endl;

	HRESULT hr = S_OK;

	// Validate window handle
	if (!my_wnd || !IsWindow(my_wnd)) {
		std::cerr << "Invalid window handle!" << std::endl;
		return E_INVALIDARG;
	}

	// Create the swap chain description
	DXGI_SWAP_CHAIN_DESC swap_chain_desc = {};
	ZeroMemory(&swap_chain_desc, sizeof(swap_chain_desc));

	swap_chain_desc.BufferCount = 2;
	swap_chain_desc.BufferDesc.Width = Width;
	swap_chain_desc.BufferDesc.Height = Height;
	swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_desc.OutputWindow = my_wnd;
	swap_chain_desc.SampleDesc.Count = 1;
	swap_chain_desc.SampleDesc.Quality = 0;
	swap_chain_desc.Windowed = TRUE;
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Create the Direct3D device and swap chain
	D3D_FEATURE_LEVEL d3d_feature_lvl;
	const D3D_FEATURE_LEVEL d3d_feature_array[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };

	hr = D3D11CreateDeviceAndSwapChain(
		nullptr,                              // Adapter (default)
		D3D_DRIVER_TYPE_HARDWARE,            // Driver type
		nullptr,                              // Software rasterizer
		0,                                    // Flags
		d3d_feature_array,                    // Feature levels
		_countof(d3d_feature_array),          // Number of feature levels
		D3D11_SDK_VERSION,                    // SDK version
		&swap_chain_desc,                     // Swap chain description
		&d3d_swap_chain,                      // Swap chain
		&p_device,                            // Device
		&d3d_feature_lvl,                     // Feature level
		&d3d_device_ctx                       // Device context
	);
	if (FAILED(hr)) {
		std::cerr << "Failed to create Direct3D device and swap chain. HRESULT: " << std::hex << hr << std::endl;
		return hr;
	}

	std::cout << "Direct3D device and swap chain created successfully." << std::endl;

	// Create the render target view
	ID3D11Texture2D* back_buffer = nullptr;
	hr = d3d_swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
	if (FAILED(hr)) {
		std::cerr << "Failed to get back buffer. HRESULT: " << std::hex << hr << std::endl;
		return hr;
	}

	hr = p_device->CreateRenderTargetView(back_buffer, nullptr, &d3d_render_target);
	back_buffer->Release(); // Release back buffer immediately after use
	if (FAILED(hr)) {
		std::cerr << "Failed to create render target view. HRESULT: " << std::hex << hr << std::endl;
		return hr;
	}

	// Initialize ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable keyboard controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable gamepad controls

	io.IniFilename = nullptr; // Disable ImGui INI file saving
	io.Fonts->AddFontDefault();

	ImFontConfig fontConfig;
	//main_font = io.Fonts->AddFontFromFileTTF(_("C:\\Windows\\Fonts\\Arial.ttf"), 14.0f, &fontConfig, io.Fonts->GetGlyphRangesCyrillic());
	// Build font atlas
	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
	if (!ImGui_ImplWin32_Init(my_wnd)) {
		std::cerr << "Failed to initialize ImGui Win32 backend." << std::endl;
		return E_FAIL;
	}
	if (!ImGui_ImplDX11_Init(p_device, d3d_device_ctx)) {
		std::cerr << "Failed to initialize ImGui DX11 backend." << std::endl;
		return E_FAIL;
	}

	ImFontConfig cfg;
	cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_ForceAutoHint | ImGuiFreeTypeBuilderFlags_LightHinting | ImGuiFreeTypeBuilderFlags_LoadColor;
	// Load fonts with proper error handling
	font::inter_default = io.Fonts->AddFontFromMemoryTTF(inter_medium, sizeof(inter_medium), 17.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
	font::inter_bold = io.Fonts->AddFontFromMemoryTTF(inter_bold, sizeof(inter_bold), 17.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
	main_font = io.Fonts->AddFontFromMemoryTTF(inter_medium, sizeof(inter_medium), 17.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
	//font::icon = io.Fonts->AddFontFromMemoryTTF(icon_font, sizeof(icon_font), 15.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());

	ImGui::StyleColorsDark();

	D3DX11_IMAGE_LOAD_INFO info; ID3DX11ThreadPump* pump{ nullptr };
	if (pic::background == nullptr) D3DX11CreateShaderResourceViewFromMemory(p_device, background, sizeof(background), &info, pump, &pic::background, 0);

	if (pic::aimbot == nullptr) D3DX11CreateShaderResourceViewFromMemory(p_device, aimbot, sizeof(aimbot), &info, pump, &pic::aimbot, 0);
	if (pic::visuals == nullptr) D3DX11CreateShaderResourceViewFromMemory(p_device, visuals, sizeof(visuals), &info, pump, &pic::visuals, 0);
	if (pic::skins == nullptr) D3DX11CreateShaderResourceViewFromMemory(p_device, skins, sizeof(skins), &info, pump, &pic::skins, 0);
	if (pic::settings == nullptr) D3DX11CreateShaderResourceViewFromMemory(p_device, settings, sizeof(settings), &info, pump, &pic::settings, 0);

	if (pic::combo_widget == nullptr) D3DX11CreateShaderResourceViewFromMemory(p_device, combo_widget, sizeof(combo_widget), &info, pump, &pic::combo_widget, 0);
	if (pic::input_widget == nullptr) D3DX11CreateShaderResourceViewFromMemory(p_device, input_widget, sizeof(input_widget), &info, pump, &pic::input_widget, 0);

//	if (pic::menu_settings_icon == nullptr) D3DX11CreateShaderResourceViewFromMemory(p_device, menu_settings_icon, sizeof(menu_settings_icon), &info, pump, &pic::menu_settings_icon, 0);

	if (pic::circle_success == nullptr) D3DX11CreateShaderResourceViewFromMemory(p_device, circle_success, sizeof(circle_success), &info, pump, &pic::circle_success, 0);
	if (pic::circle_error == nullptr) D3DX11CreateShaderResourceViewFromMemory(p_device, circle_error, sizeof(circle_error), &info, pump, &pic::circle_error, 0);
	// Load the avatar texture
	//ApplyModernStyle();

	std::cout << "DirectX initialization completed successfully." << std::endl;

	return S_OK; // Return success
}

void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	d3d_swap_chain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	p_device->CreateRenderTargetView(pBackBuffer, nullptr, &d3d_render_target);
	pBackBuffer->Release();
}

void CleanupRenderTarget()
{
	if (d3d_render_target) { d3d_render_target->Release(); d3d_render_target = nullptr; }
}




template<typename T>
int FindIndex(const std::vector<T>& vec, const T& value) {
	for (int i = 0; i < vec.size(); ++i) {
		if (vec[i] == value) {
			return i;
		}
	}
	return -1; // Not found
}

// --- Menu Styling (Optional - Apply once at startup or push/pop here) ---



const char* GetKeyName(int vk_code) {
	// Use static storage for the map to initialize it only once
	static std::unordered_map<int, const char*> keyNames;
	if (keyNames.empty()) {
		// Mouse Buttons
		keyNames[VK_LBUTTON] = "LMB";
		keyNames[VK_RBUTTON] = "RMB";
		keyNames[VK_MBUTTON] = "MMB";
		keyNames[VK_XBUTTON1] = "Mouse 4";
		keyNames[VK_XBUTTON2] = "Mouse 5";
		// Control Keys
		keyNames[VK_BACK] = "Backspace";
		keyNames[VK_TAB] = "Tab";
		keyNames[VK_RETURN] = "Enter";
		keyNames[VK_SHIFT] = "Shift"; // Generic Shift
		keyNames[VK_LSHIFT] = "LShift";
		keyNames[VK_RSHIFT] = "RShift";
		keyNames[VK_CONTROL] = "Ctrl"; // Generic Ctrl
		keyNames[VK_LCONTROL] = "LCtrl";
		keyNames[VK_RCONTROL] = "RCtrl";
		keyNames[VK_MENU] = "Alt"; // Generic Alt (VK_MENU)
		keyNames[VK_LMENU] = "LAlt";
		keyNames[VK_RMENU] = "RAlt";
		keyNames[VK_PAUSE] = "Pause";
		keyNames[VK_CAPITAL] = "Caps Lock";
		keyNames[VK_ESCAPE] = "Esc";
		keyNames[VK_SPACE] = "Space";
		keyNames[VK_PRIOR] = "Page Up";
		keyNames[VK_NEXT] = "Page Down";
		keyNames[VK_END] = "End";
		keyNames[VK_HOME] = "Home";
		keyNames[VK_LEFT] = "Left Arrow";
		keyNames[VK_UP] = "Up Arrow";
		keyNames[VK_RIGHT] = "Right Arrow";
		keyNames[VK_DOWN] = "Down Arrow";
		keyNames[VK_SELECT] = "Select";
		keyNames[VK_PRINT] = "Print";
		keyNames[VK_EXECUTE] = "Execute";
		keyNames[VK_SNAPSHOT] = "PrtSc";
		keyNames[VK_INSERT] = "Insert";
		keyNames[VK_DELETE] = "Delete";
		keyNames[VK_HELP] = "Help";
		// 0-9 Keys (top row)
		for (int i = 0; i <= 9; ++i) {
			static char buf[2] = "0"; // Static buffer for single digit
			buf[0] = '0' + i;
			keyNames[0x30 + i] = _strdup(buf); // Use strdup for dynamic allocation if needed, or manage static buffers carefully
		}
		// A-Z Keys
		for (char c = 'A'; c <= 'Z'; ++c) {
			static char buf[2] = "A";
			buf[0] = c;
			keyNames[c] = _strdup(buf);
		}
		// Numpad Keys
		keyNames[VK_NUMPAD0] = "Num 0";
		keyNames[VK_NUMPAD1] = "Num 1";
		keyNames[VK_NUMPAD2] = "Num 2";
		keyNames[VK_NUMPAD3] = "Num 3";
		keyNames[VK_NUMPAD4] = "Num 4";
		keyNames[VK_NUMPAD5] = "Num 5";
		keyNames[VK_NUMPAD6] = "Num 6";
		keyNames[VK_NUMPAD7] = "Num 7";
		keyNames[VK_NUMPAD8] = "Num 8";
		keyNames[VK_NUMPAD9] = "Num 9";
		keyNames[VK_MULTIPLY] = "Num *";
		keyNames[VK_ADD] = "Num +";
		keyNames[VK_SEPARATOR] = "Num Sep";
		keyNames[VK_SUBTRACT] = "Num -";
		keyNames[VK_DECIMAL] = "Num .";
		keyNames[VK_DIVIDE] = "Num /";
		// Function Keys
		for (int i = 1; i <= 12; ++i) {
			static char buf[4] = "F1"; // Static buffer for F keys
			snprintf(buf, sizeof(buf), "F%d", i);
			keyNames[VK_F1 + i - 1] = _strdup(buf);
		}
		// Other Keys
		keyNames[VK_NUMLOCK] = "Num Lock";
		keyNames[VK_SCROLL] = "Scroll Lock";
		keyNames[VK_OEM_1] = "; :";
		keyNames[VK_OEM_PLUS] = "+ =";
		keyNames[VK_OEM_COMMA] = ", <";
		keyNames[VK_OEM_MINUS] = "- _";
		keyNames[VK_OEM_PERIOD] = ". >";
		keyNames[VK_OEM_2] = "/ ?";
		keyNames[VK_OEM_3] = "` ~";
		keyNames[VK_OEM_4] = "[ {";
		keyNames[VK_OEM_5] = "\\ |";
		keyNames[VK_OEM_6] = "] }";
		keyNames[VK_OEM_7] = "' \"";
	}

	auto it = keyNames.find(vk_code);
	if (it != keyNames.end()) {
		return it->second;
	}

	// Fallback for unknown keys
	static char unknownBuf[16];
	snprintf(unknownBuf, sizeof(unknownBuf), "VK [0x%X]", vk_code);
	return unknownBuf;
}

const char* GetKeyName(int vk_code);

// Keybind widget function
// label: Text displayed next to the keybind button (e.g., "Aimbot Key")
// key_code: Pointer to the integer variable storing the VK code
void KeyBind(const char* label, int* key_code)
{
	// Use a unique ID based on the label to allow multiple KeyBind widgets
	ImGui::PushID(label);

	// Static variable to track if this specific widget is waiting for input
	// This works because PushID creates a unique context
	static bool isWaitingForKey = false;

	std::string buttonText;
	if (isWaitingForKey) {
		buttonText = "..."; // Indicate waiting state
	}
	else {
		buttonText = GetKeyName(*key_code);
	}

	// Display the label and the button showing the current key or waiting state
	ImGui::Text("%s:", label);
	ImGui::SameLine();
	if (ImGui::Button(buttonText.c_str(), ImVec2(100, 0))) { // Fixed width button
		isWaitingForKey = true;
		// Set keyboard focus to this widget to potentially help capture, though not strictly necessary with GetAsyncKeyState
		// ImGui::SetKeyboardFocusHere();
	}

	// Capture logic runs only when waiting
	if (isWaitingForKey)
	{
		// Optional: Add visual feedback while waiting (e.g., change style)
		// ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);

		// Iterate through possible key codes
		// Mouse buttons (1-6), Keyboard keys (8-254 typically)
		for (int vk = VK_LBUTTON; vk <= VK_OEM_CLEAR /*0xFE*/; ++vk)
		{
			// Skip invalid VK codes in the range if necessary (e.g., 0x07 is undefined)
			if (vk == 7 || (vk > VK_XBUTTON2 && vk < VK_BACK)) continue;

			// Check if the key is currently pressed down
			// Using 0x8000 check means it captures the key while it's held
			if (GetAsyncKeyState(vk) & 0x8000)
			{
				// Special handling for the click that initiated the wait
				// If the left mouse button is pressed AND the button was just activated this frame, ignore it.
				if (vk == VK_LBUTTON && ImGui::IsItemActivated()) {
					continue; // Ignore the click that started the binding process
				}

				// Assign the captured key
				*key_code = vk;
				isWaitingForKey = false; // Stop waiting
				break; // Exit loop once a key is captured
			}
		}

		// Optional: Check for ESC key press specifically to cancel waiting
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
			isWaitingForKey = false; // Cancel without changing key
		}

		// Optional: Pop style if pushed
		// ImGui::PopStyleColor();
	}

	ImGui::PopID();
}
void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)"); // Display a small question mark
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f); // Wrap text nicely
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

// --- The Menu Function ---
void DrawMenu(bool* p_open) // Pass a pointer to control visibility
{
	if (!p_open || !*p_open) return;

	// --- Animation Setup (Simple Fade In) ---
	static float menuAlpha = 0.0f;
	static bool wasOpen = false;
	bool isOpen = *p_open;

	if (isOpen && !wasOpen) {
		menuAlpha = 0.0f; // Start fade in
	}

	if (isOpen) {
		menuAlpha += ImGui::GetIO().DeltaTime * 4.0f; // Adjust speed as needed
		menuAlpha = std::clamp(menuAlpha, 0.0f, 1.0f); // Use std::clamp
	}
	else {
		menuAlpha = 0.0f; // Reset when closed
	}
	wasOpen = isOpen;

	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, menuAlpha); // Apply fade

	// --- Menu Window ---
	// Increased default size slightly
	ImGui::SetNextWindowSize(ImVec2(650, 500), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("FuryHook", p_open, ImGuiWindowFlags_NoCollapse /* | ImGuiWindowFlags_NoResize */)) {

		// --- Tab Bar ---
		if (ImGui::BeginTabBar("MainTabs")) {

			// == Visuals Tab ==
			if (ImGui::BeginTabItem("Visuals")) {
				ImGui::Text("Player ESP");
				ImGui::Separator();
				ImGui::Checkbox("Draw Names", &g_Config.bDrawPlayerNames);
				ImGui::SameLine(200); ImGui::ColorEdit4("##NameColor", &g_Config.vPlayerNameColor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
				ImGui::Checkbox("Draw Bones", &g_Config.bDrawPlayerBones);
				ImGui::SameLine(200); ImGui::ColorEdit4("##BoneColor", &g_Config.vBoneColor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
				ImGui::Checkbox("Draw Box", &g_Config.bDrawPlayerBox);
				ImGui::SameLine(200); ImGui::ColorEdit4("##BoxColor", &g_Config.vBoxColor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);

				// Combo box for Box Type (only if Draw Box is enabled)
				if (g_Config.bDrawPlayerBox) {
					ImGui::Indent();
					const char* boxTypes[] = { "None", "Normal", "Corner", "Filled" };
					ImGui::SetNextItemWidth(120); // Adjusted width slightly
					ImGui::Combo("Box Type", &g_Config.iBoxType, boxTypes, IM_ARRAYSIZE(boxTypes));
					ImGui::Unindent();
				}

				ImGui::Checkbox("Draw Snapline", &g_Config.bDrawSnapline);
				ImGui::SameLine(200); ImGui::ColorEdit4("##SnapColor", &g_Config.vSnaplineColor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar);
				if (g_Config.bDrawSnapline) {
					ImGui::Indent();
					const char* snapSources[] = { "Bottom Center", "Top Center" }; // Assuming 0 and 1 map correctly
					ImGui::SetNextItemWidth(120);
					ImGui::Combo("Snapline Source", &g_Config.iSnaplineSource, snapSources, IM_ARRAYSIZE(snapSources));
					ImGui::Unindent();
				}

				ImGui::Checkbox("Show Closest Player Info", &g_Config.bShowClosestPlayerInfo);

				ImGui::Dummy(ImVec2(0, 10)); // Spacing
				ImGui::Text("World ESP");
				ImGui::Separator();
				ImGui::Checkbox("Draw Prefabs", &g_Config.bDrawPrefabs);
				if (g_Config.bDrawPrefabs) {
					ImGui::Indent();
					ImGui::Checkbox("Draw Prefab Text", &g_Config.bDrawPrefabText);
					ImGui::SliderFloat("Icon Size", &g_Config.fPrefabIconSize, 16.0f, 64.0f, "%.0f");
					ImGui::Unindent();
				}


				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Aimbot")) {
				ImGui::Checkbox("Enable Aimbot", &g_Config.bEnableAimbot);
				ImGui::Separator();

				if (g_Config.bEnableAimbot) {
					ImGui::Columns(2, nullptr, false);

					// --- Column 1: Activation & Targeting ---
					ImGui::Text("Activation & FOV");
					ImGui::Separator();
					KeyBind("Aimbot Key", &g_Config.iAimbotKey);
					// *** THIS SLIDER NOW CONTROLS BOTH ***
					ImGui::SliderFloat("Aimbot/Visual FOV", &g_Config.fAimbotMaxFov, 1.0f, 500.0f, "%.0f px");
					ImGui::Checkbox("Draw Visual FOV Circle", &g_Config.bDrawFovCircle);
					if (g_Config.bDrawFovCircle) {
						ImGui::Indent();
						// ImGui::SliderFloat("Visual FOV Radius", &g_Config.fFovRadius, 10.0f, 500.0f, "%.0f px"); // REMOVED
						ImGui::ColorEdit4("##FovColor", &g_Config.vFovCircleColor.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar); // Keep color edit
						ImGui::Unindent();
					}

					ImGui::Dummy(ImVec2(0, 10));
					ImGui::Text("Targeting");
					ImGui::Separator();
					ImGui::Checkbox("Target Closest Bone", &g_Config.bClosestBoneTargeting);
					if (!g_Config.bClosestBoneTargeting) {
						ImGui::Indent();
						const char* hitboxes[] = { "Head", "Spine/Chest", "Pelvis" };
						ImGui::SetNextItemWidth(120);
						ImGui::Combo("Target Hitbox", &g_Config.iAimbotHitbox, hitboxes, IM_ARRAYSIZE(hitboxes));
						ImGui::Unindent();
					}

					ImGui::NextColumn();

					// --- Column 2: Behavior ---
					ImGui::Text("Behavior");
					ImGui::Separator();
					ImGui::Checkbox("Enable Prediction", &g_Config.bEnablePrediction);
					ImGui::Checkbox("Enable Silent Aim", &g_Config.bEnableSilentAim);
					if (g_Config.bEnableSilentAim) {
						ImGui::SliderInt("Hit Chance %% (Silent)", &g_Config.iHitChancePercent, 0, 100);
						ImGui::SameLine(); HelpMarker("Percentage chance for silent aim to activate per frame.");
					}
					else {
						ImGui::SliderInt("Smoothing (Normal)", &g_Config.iSmoothingAmount, 1, 100);
						ImGui::SameLine(); HelpMarker("Aim speed adjustment for normal aimbot.\n1 = Instant\n100 = Very Slow");
					}

					ImGui::Columns(1);
					ImGui::Separator();
				}
				else {
					ImGui::TextDisabled("Aimbot is disabled.");
				}
				ImGui::EndTabItem();
			} // End Aimbot Tab

			// == Misc Tab ==
			if (ImGui::BeginTabItem("Misc")) {
				ImGui::Text("Miscellaneous Settings");
				ImGui::Separator();
				// Add any other settings that don't fit elsewhere
				// Example:
				// ImGui::Checkbox("Enable Feature X", &g_Config.bEnableFeatureX);

				ImGui::Text("Performance Note:");
				ImGui::TextWrapped("Disabling complex visuals like skeletons or reducing FOV can improve performance.");

				ImGui::EndTabItem();
			}


			// == Config Tab ==
			if (ImGui::BeginTabItem("Config")) {
				ImGui::Text("Configuration Management");
				ImGui::Separator();

				if (ImGui::Button("Save Config", ImVec2(120, 30))) {
					g_Config.Save();
					// Optional: Add feedback like a temporary text message
				}
				ImGui::SameLine();
				if (ImGui::Button("Load Config", ImVec2(120, 30))) {
					g_Config.Load();
					// Optional: Add feedback
				}

				ImGui::Dummy(ImVec2(0, 20));
				ImGui::TextWrapped("Current config file: %s", g_Config.configFileName.c_str());
				ImGui::TextWrapped("Settings are saved/loaded automatically based on changes in other tabs if 'Save Config' is used.");


				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}
	ImGui::End(); // End Menu Window

	ImGui::PopStyleVar(); // Pop fade alpha
}


const char* cheat_name = "FuryHook";

void menu2(bool menu) // Renamed from menu2_filled back to menu2
{
	if (!menu) return; // Exit early if menu is not shown

	DWORD picker_flags = ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview;

	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec2 originalItemSpacing = style->ItemSpacing;
	float originalWindowRounding = style->WindowRounding;
	float originalChildRounding = style->ChildRounding;
	float originalFrameRounding = style->FrameRounding;
	float originalGrabRounding = style->GrabRounding;
	ImVec2 originalWindowPadding = style->WindowPadding;
	float originalScrollbarSize = style->ScrollbarSize;
	float originalWindowBorderSize = style->WindowBorderSize;


	// --- Apply custom style settings from menu2 ---
	style->WindowPadding = ImVec2(0, 0);             // From menu2
	style->WindowBorderSize = 0;                    // From menu2
	style->ItemSpacing = ImVec2(20, 20);            // Main spacing (menu2)
	style->ScrollbarSize = 8.f;                     // From menu2
	style->WindowRounding = c::bg::rounding;        // From menu2 (using c:: namespace)
	style->ChildRounding = c::child::rounding;      // From menu2 (using c:: namespace)
	// Keep Frame/Grab rounding from menu2 for consistency, adjust if needed
	style->FrameRounding = 3.0f;
	style->GrabRounding = 3.0f;


	// --- Accent Color Setup (from rendermenu, slightly different from menu2's original) ---
	static float color[4] = { 0.f, 1.f, 0.f, 1.f }; // Default green
	// Update global accent color based on the picker
	// If c::accent should drive the picker, initialize color[] from c::accent once
	// static bool initialized_color = false;
	// if (!initialized_color) {
	//     color[0] = c::accent.x; color[1] = c::accent.y; color[2] = c::accent.z; color[3] = c::accent.w;
	//     initialized_color = true;
	// }
	c::accent = { color[0], color[1], color[2], 1.f }; // Update global c::accent


	// --- Main Window Setup ---
	ImGui::SetNextWindowSize(c::bg::size);
	ImGui::Begin("FuryHookMain", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus);
	{
		const ImVec2& pos = ImGui::GetWindowPos();
		const ImVec2& spacing = style->ItemSpacing; // Use the currently set style spacing (20, 20)
		const ImVec2& region = ImGui::GetContentRegionMax();
		ImDrawList* draw_list = ImGui::GetBackgroundDrawList(); // Use background draw list for elements behind widgets

		// --- Background Drawing (from menu2, slight adjustments maybe needed based on rendermenu's exact look) ---
		// Main background
		draw_list->AddRectFilled(pos, pos + region, ImGui::GetColorU32(c::bg::background), c::bg::rounding);

		// Top-left logo area (Pos: top-left corner + spacing, Size: 180 width, 62 height + vertical spacing)
		ImVec2 logoAreaTopLeft = pos + spacing;
		ImVec2 logoAreaBottomRight = pos + ImVec2(180, 62 + spacing.y);
		draw_list->AddRectFilled(logoAreaTopLeft, logoAreaBottomRight, ImGui::GetColorU32(c::child::background), c::child::rounding);
		draw_list->AddRect(logoAreaTopLeft, logoAreaBottomRight, ImGui::GetColorU32(c::child::outline), c::child::rounding);

		// Left sidebar area (Below logo area)
		ImVec2 sidebarAreaTopLeft = pos + ImVec2(spacing.x, 62 + spacing.y * 2); // Start below logo + one spacing gap
		ImVec2 sidebarAreaBottomRight = pos + ImVec2(180, region.y - spacing.y); // End at bottom - spacing gap
		draw_list->AddRectFilled(sidebarAreaTopLeft, sidebarAreaBottomRight, ImGui::GetColorU32(c::child::background), c::child::rounding);
		draw_list->AddRect(sidebarAreaTopLeft, sidebarAreaBottomRight, ImGui::GetColorU32(c::child::outline), c::child::rounding);

		// --- Top Bar Area (Right of Logo, from rendermenu, omitted in original menu2) ---
		// Let's add it back as per rendermenu drawing logic
		float topBarStartX = 180 + spacing.x;
		float settingsIconAreaWidth = 62 + spacing.x * 2; // Width needed for the settings icon box
		float topBarEndX = region.x - settingsIconAreaWidth + spacing.x; // End before the settings icon box starts
		ImVec2 topBarAreaTopLeft = pos + ImVec2(topBarStartX, spacing.y);
		ImVec2 topBarAreaBottomRight = pos + ImVec2(topBarEndX, 62 + spacing.y);
		// Uncomment these lines if you want the top bar rectangle like in rendermenu
		// draw_list->AddRectFilled(topBarAreaTopLeft, topBarAreaBottomRight, ImGui::GetColorU32(c::child::background), c::child::rounding);
		// draw_list->AddRect(topBarAreaTopLeft, topBarAreaBottomRight, ImGui::GetColorU32(c::child::outline), c::child::rounding);


		// Top-right settings icon area (from rendermenu drawing logic)

		// --- End Background Drawing ---


		// --- Logo Text (Using rendermenu's GlowText centered in the logo box) ---
		ImFont* logoFont = font::inter_bold ? font::inter_bold : ImGui::GetFont(); // Use bold font if available
		// Calculate center position within the logo box (180 x 62)
		// The box starts at pos + spacing
		ImVec2 logoBoxCenter = pos + spacing + ImVec2(180 / 2.0f, (62 + spacing.y) / 2.0f);
		ImVec2 logoTextSize = ImGui::CalcTextSize(cheat_name); // Use standard CalcTextSize
		ImVec2 logoTextPos = logoBoxCenter - logoTextSize * 0.5f;
		// Use standard AddText or your custom GlowText
		// ImGui::GlowText(logoFont, 17.f, cheat_name, logoTextPos, ImGui::GetColorU32(c::accent)); // Using rendermenu's style
		// Standard text alternative:
		ImGui::PushFont(logoFont);
		ImGui::GetWindowDrawList()->AddText(logoTextPos, ImGui::GetColorU32(c::accent), cheat_name);
		ImGui::PopFont();
		// --- End Logo Text ---

		ImGui::PushFont(font::inter_default);
		// --- Left Sidebar Tabs ---
		static int current_tab = 0; // Default to first tab (Aimbot based on rendermenu's order)

		// Position cursor below logo area, inside the sidebar area
		ImGui::SetCursorPos(ImVec2(spacing.x * 2, 62 + spacing.y * 2)); // Start below logo box + spacing
		ImGui::BeginGroup();
		{
			ImVec2 tab_button_size = ImVec2(180 - spacing.x * 3, 35); // Size from rendermenu
			float original_spacing_y = style->ItemSpacing.y;
			style->ItemSpacing.y = 10; // Vertical spacing between tab buttons (adjust as needed)

			// Using rendermenu's tab order and icons/labels
			if (ImGui::Tabs(0 == current_tab, pic::aimbot, "Aimbot", tab_button_size, NULL)) current_tab = 0;
			if (ImGui::Tabs(1 == current_tab, pic::visuals, "Visuals", tab_button_size, NULL)) current_tab = 1;
			if (ImGui::Tabs(2 == current_tab, pic::settings, "Settings", tab_button_size, NULL)) current_tab = 2; // Use pic::settings
			// Exit tab removed to match menu2 structure, can be added back if needed
			// if (ImGui::Tabs(3 == current_tab, NULL, "Exit", tab_button_size, NULL)) exit(0);

			style->ItemSpacing.y = original_spacing_y; // Restore original item spacing Y
		}
		ImGui::EndGroup();
		// --- End Left Sidebar Tabs ---


		// --- Main Content Area (2x2 Grid) ---
		ImVec2 content_start_pos = ImVec2(180 + spacing.x, spacing.y); // Start content right of sidebar, below top padding

		// Calculate available space for the 2x2 grid (using menu2's logic)
		float availableWidth = region.x - content_start_pos.x - spacing.x; // Width right of sidebar minus right padding
		float availableHeight = region.y - content_start_pos.y - spacing.y; // Height below top padding minus bottom padding

		// Calculate child size for a 2x2 grid (menu2's robust calculation)
		ImVec2 childSize = ImVec2(
			ImMax(10.0f, (availableWidth - spacing.x) / 2.0f),  // Subtract gap before dividing
			ImMax(10.0f, (availableHeight - spacing.y) / 2.0f) // Subtract gap before dividing
		);

		// --- Set cursor for the start of the content area ---
		ImGui::SetCursorPos(content_start_pos);

		// Use standard ImGui spacing within children, or keep menu2's inner spacing
		float innerSpacingX = 8.0f;
		float innerSpacingY = 8.0f;
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(innerSpacingX, innerSpacingY));

		// --- Render Content Based on Selected Tab ---
		if (current_tab == 0) // Aimbot Tab (Content from rendermenu)
		{
			// --- Row 1 ---
			ImGui::BeginGroup(); // Group for top-left child
			ImGui::CustomBeginChild("Aimbot Main", childSize); // Renamed child window
			{
				ImGui::Text("Aimbot"); ImGui::Separator(); // Title
				ImGui::Checkbox("Enable Aimbot", &g_Config.bEnableAimbot);
		
				ImGui::Checkbox("Enable Silent Aim", &g_Config.bEnableSilentAim);
				
				//ImGui::Checkbox("Team Check", &globals::aimbot_teamcheck); // Simplified label
				//ImGui::Checkbox("Knock Check", &globals::aimbot_knockchecck); // Simplified label
				//ImGui::Checkbox("Visibility Check", &globals::aimbotvisibilitycheck);
				ImGui::Checkbox("Random bones", &g_Config.bClosestBoneTargeting);
				ImGui::Checkbox("Aimbot prediction", &g_Config.bEnablePrediction);

			}
			ImGui::CustomEndChild();
			ImGui::EndGroup();

			ImGui::SameLine(0, spacing.x); // Horizontal gap

			ImGui::BeginGroup(); // Group for top-right child
			ImGui::CustomBeginChild("Aimbot Settings", childSize); // Renamed child window
			{
				ImGui::Text("Settings & Targeting"); ImGui::Separator(); // Title
				ImGui::Checkbox("Enable FOV Circle", &g_Config.bDrawFovCircle); // Using enableFOV from rendermenu
			//	ImGui::Checkbox("Enable Aim Trace", &globals::showtracers); // Using showtracers
				static int aimbot_keybind_mode = 0; // Dummy mode for Keybind widget
				ImGui::Keybind("Aimbot Key", &g_Config.iAimbotKey, &aimbot_keybind_mode, false); // Use Keybind from rendermenu

				if (!g_Config.bClosestBoneTargeting) {
					ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.8f); // Adjust width
					// Ensure globals::hitbox_names is populated correctly
					const char* hitboxes[] = { "Head", "Spine/Chest", "Pelvis" };
					ImGui::Combo("Target Hitbox", &g_Config.iAimbotHitbox, hitboxes, IM_ARRAYSIZE(hitboxes));
				}
				else {
					ImGui::TextDisabled("Targeting: Random Bone");
				}
			}
			ImGui::CustomEndChild();
			ImGui::EndGroup();

			// --- Row 2 ---
			// Set cursor position for the second row
			float secondRowY = content_start_pos.y + childSize.y + spacing.y;
			ImGui::SetCursorPos(ImVec2(content_start_pos.x, secondRowY));

			ImGui::BeginGroup(); // Group for bottom-left child
			ImGui::CustomBeginChild("Aimbot Sliders", childSize); // Renamed child window
			{
				ImGui::Text("Adjustments"); ImGui::Separator(); // Title
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.8f);
				if (g_Config.bEnableSilentAim) {
					ImGui::SliderInt("Hit Chance %% (Silent)", &g_Config.iHitChancePercent, 0, 100);
					ImGui::SameLine(); HelpMarker("Percentage chance for silent aim to activate per frame.");
				}
				else {
					ImGui::SliderInt("Smoothing (Normal)", &g_Config.iSmoothingAmount, 1, 25);
					ImGui::SameLine(); HelpMarker("Aim speed adjustment for normal aimbot.\n1 = Instant\n100 = Very Slow");
				}
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.8f);
			//	ImGui::SliderFloat("Max Distance", &globals::aimbotdistance, 0.f, 500.f, "%.0f m"); // Adjusted label/range
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.8f);
				ImGui::SliderFloat("FOV Size", &g_Config.fAimbotMaxFov, 1, 360, "%.0f px"); // Adjusted label
			}
			ImGui::CustomEndChild();
			ImGui::EndGroup();

			ImGui::SameLine(0, spacing.x); // Horizontal gap

			ImGui::BeginGroup(); // Group for bottom-right child (Empty or add more controls)
			// This child was empty in rendermenu's aimbot tab structure after rearranging
			ImGui::CustomBeginChild("Aimbot Extra", childSize);
			{
				ImGui::Text("Misc"); ImGui::Separator(); // Title
				// Add other aimbot related toggles or info here if needed
				ImGui::TextDisabled("...");
			}
			ImGui::CustomEndChild();
			ImGui::EndGroup();
		}
		else if (current_tab == 1) // Visuals Tab (Content from rendermenu)
		{
			// --- Row 1 ---
			ImGui::BeginGroup(); // Group for top-left child
			ImGui::CustomBeginChild("ESP General", childSize);
			{
				ImGui::Text("ESP Toggles"); ImGui::Separator();
			//	ImGui::Checkbox("Visibility Check", &globals::espvisibilitycheck);
			//	ImGui::Checkbox("Team Check", &globals::espteamcheck);
			//	ImGui::Checkbox("Knock Check", &globals::knockcheck); // ESP knock check
		
			}
			ImGui::CustomEndChild();
			ImGui::EndGroup();

			ImGui::SameLine(0, spacing.x); // Horizontal gap

			ImGui::BeginGroup(); // Group for top-right child
			ImGui::CustomBeginChild("ESP Player", childSize);
			{
				ImGui::Text("Player Visuals"); ImGui::Separator();
				
				ImGui::Checkbox("Draw Box", &g_Config.bDrawPlayerBox);
				ImGui::ColorEdit4("##BoxColor", &g_Config.vBoxColor.x, picker_flags);
				if (g_Config.bDrawPlayerBox) {
					
					const char* boxTypes[] = { "Normal", "Corner", "Filled" }; int currentBoxIndex = g_Config.iBoxType - 1;
					if (currentBoxIndex < 0 || currentBoxIndex >= IM_ARRAYSIZE(boxTypes)) { currentBoxIndex = 0; }
					ImGui::SetNextItemWidth(120);
					if (ImGui::Combo("Box Type", &currentBoxIndex, boxTypes, IM_ARRAYSIZE(boxTypes))) { g_Config.iBoxType = currentBoxIndex + 1; }
					
				}

			//	ImGui::Checkbox("Draw Distance", &globals::drawplayerdistance);
				ImGui::Checkbox("Draw skeleton", &g_Config.bDrawPlayerBones);
				ImGui::Checkbox("Draw Snapline", &g_Config.bDrawSnapline); 
				ImGui::ColorEdit4("##SnapColor", &g_Config.vSnaplineColor.x, picker_flags);
				if (g_Config.bDrawSnapline) {
					
					const char* snapSources[] = { "Bottom Center", "Top Center" }; ImGui::SetNextItemWidth(120);
					ImGui::Combo("Snapline Source", &g_Config.iSnaplineSource, snapSources, IM_ARRAYSIZE(snapSources));
				
				}
				// Add box type combo, snapline options etc. here if desired, similar to original menu2
			}
			ImGui::CustomEndChild();
			ImGui::EndGroup();

			// --- Row 2 ---
			float secondRowY = content_start_pos.y + childSize.y + spacing.y;
			ImGui::SetCursorPos(ImVec2(content_start_pos.x, secondRowY));

			ImGui::BeginGroup(); // Group for bottom-left child
			ImGui::CustomBeginChild("ESP Sliders", childSize);
			{
				ImGui::Text("Distance Limits"); ImGui::Separator();
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.8f);
		//		ImGui::SliderFloat("Skeleton Max Dist", &globals::playerskeletondistance, 1.0f, 1000.0f, "%.0f m"); // Adjusted label/range
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.8f);
		//		ImGui::SliderFloat("Box Max Dist", &globals::playerboxdistance, 1.0f, 1000.0f, "%.0f m"); // Adjusted label/range
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.8f);
		//		ImGui::SliderFloat("Health Max Dist", &globals::playerhealthdistance, 1.0f, 1000.0f, "%.0f m"); // Adjusted label/range
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.8f);
		//		ImGui::SliderFloat("Distance Max Dist", &globals::distancedistance, 1.0f, 1000.0f, "%.0f m"); // Renamed slider
			}
			ImGui::CustomEndChild();
			ImGui::EndGroup();

			ImGui::SameLine(0, spacing.x); // Horizontal gap

			ImGui::BeginGroup(); // Group for bottom-right child
			ImGui::CustomBeginChild("World ESP Settings", childSize);
			{
				ImGui::Text("World ESP"); ImGui::Separator();
				if (g_Config.bDrawPrefabs) {
					ImGui::Checkbox("Draw Prefab Text", &g_Config.bDrawPrefabText);
					ImGui::SliderFloat("Icon Size", &g_Config.fPrefabIconSize, 16.0f, 64.0f, "%.0f");


					// Add item filtering options here if needed (from rendermenu's commented section)
					// Example:
					// ImGui::Checkbox("Ground Item ESP", &globals::drawcontainer_item); // If you have this var
					// ImGui::SliderFloat("Item Price Threshold", &globals::g_item_price, 100.0f, 100000.0f, "%.0f");
					// ImGui::SliderFloat("Ground Item Show Distance", &globals::g_grounditemdistance, 5.0f, 100.0f, "%.0f");
					// ImGui::SliderFloat("Container Show Distance", &globals::g_containerdistance, 5.0f, 100.0f, "%.0f");
					ImGui::TextDisabled("Item filtering options..."); // Placeholder
				}
				
				ImGui::Checkbox("Draw Dropped Items", &g_Config.bDrawDroppedItems);
				if (g_Config.bDrawDroppedItems)
				{

				}
				else {
					ImGui::TextDisabled("Item ESP is disabled.");
				}

				// Placeholder for Prefabs from original menu2, if still desired
				// ImGui::Separator();
				// ImGui::Checkbox("Draw Prefabs", &g_Config.bDrawPrefabs); // Need to decide if using g_Config or globals
			}
			ImGui::CustomEndChild();
			ImGui::EndGroup();
		}
		else if (current_tab == 2) // Settings Tab (Content from rendermenu)
		{
			// Using only top row for settings as in rendermenu example
			ImGui::BeginGroup(); // Group for top-left child
			ImGui::CustomBeginChild("Configuration & Settings", childSize);
			{
				ImGui::Text("Configuration"); ImGui::Separator();
				// Use buttons similar to rendermenu
				ImVec2 buttonSize = ImVec2(ImGui::GetContentRegionAvail().x * 0.45f, 30); // Responsive button size
				if (ImGui::Button("Save Settings", buttonSize)) {
					g_Config.Save();
					// ImGui::Notification({ ImGuiToastType_Success, 4000, "Settings saved." }); // Use your notification system
				}
				ImGui::SameLine();
				if (ImGui::Button("Load Settings", buttonSize)) {
					g_Config.Load();
					// ImGui::Notification({ ImGuiToastType_Success, 4000, "Settings loaded." }); // Use your notification system
				}
				ImGui::Dummy(ImVec2(0, 10)); // Spacing

				ImGui::Separator();
				ImGui::Text("Appearance & Misc"); ImGui::Separator();

			//	ImGui::Checkbox("Streamproof", &globals::streamproof); // Streamproof toggle

				ImGui::Text("Accent Color:"); ImGui::SameLine();
				// Use the color picker associated with static color[] variable
				if (ImGui::ColorEdit4("##AccentColor", color, picker_flags)) {
					c::accent = { color[0], color[1], color[2], 1.f }; // Update global color on change
				}

				// Display config file name if available (like menu2)
				// ImGui::TextWrapped("Current config file: %s", g_Config.configFileName.c_str()); // Needs g_Config setup
			}
			ImGui::CustomEndChild();
			ImGui::EndGroup();

			ImGui::SameLine(0, spacing.x); // Horizontal gap

			ImGui::BeginGroup(); // Group for top-right child (Can be used for logs, info, etc.)
			ImGui::CustomBeginChild("Information", childSize);
			{
				ImGui::Text("Info"); ImGui::Separator();
				ImGui::Text("Cheat Name: %s", cheat_name);
				ImGui::Text("Status: Connected"); // Example info
				ImGui::Dummy(ImVec2(0, 10));
				ImGui::TextWrapped("Performance Note: Disabling complex visuals or reducing distances can improve FPS."); // From menu2
			}
			ImGui::CustomEndChild();
			ImGui::EndGroup();

			// Bottom row is empty in this settings layout
		}

		// Pop inner style
		ImGui::PopStyleVar(); // ItemSpacing

	}
	ImGui::End(); // End main window "FuryHookMain"

	// Restore original style settings
	style->ItemSpacing = originalItemSpacing;
	style->WindowRounding = originalWindowRounding;
	style->ChildRounding = originalChildRounding;
	style->FrameRounding = originalFrameRounding;
	style->GrabRounding = originalGrabRounding;
	style->WindowPadding = originalWindowPadding;
	style->ScrollbarSize = originalScrollbarSize;
	style->WindowBorderSize = originalWindowBorderSize;


	// Render notifications if used
	ImGui::RenderNotifications();
}


//void display_fps() {
//	static int frame_count = 0;
//	static float fps = 0.0f;
//	static auto last_time = std::chrono::high_resolution_clock::now();
//
//	// Increment frame count
//	frame_count++;
//
//	// Get the current time
//	auto now = std::chrono::high_resolution_clock::now();
//	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time);
//
//	// Update FPS every second
//	if (elapsed.count() >= 1000) {
//		fps = frame_count / (elapsed.count() / 1000.0f);
//		frame_count = 0;
//		last_time = now;
//	}
//
//	// Render FPS using ImGui
//	ImGui::SetNextWindowPos(ImVec2(10, 10)); // Top-left corner with 10px margin
//	ImGui::SetNextWindowBgAlpha(0.5f);       // Semi-transparent background
//
//	if (ImGui::Begin("FPS Counter", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
//		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
//		ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav)) {
//		ImGui::Text("FPS: %.1f", fps);
//	}
//	ImGui::End();
//}

WPARAM render_loop() {
	std::cout << _("Starting...") << std::endl;

	const int target_fps = 1250;
	const auto target_frame_time = std::chrono::microseconds(1000000 / target_fps);

	ZeroMemory(&messager, sizeof(MSG));
	std::cout << _("Starting render loop...") << std::endl;

	ImGuiIO& io = ImGui::GetIO();
	POINT p;
	const float clear_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	static bool show_menu = true; // Menu starts visible

	for (;;) {
		auto frame_start = std::chrono::high_resolution_clock::now();

		while (PeekMessage(&messager, nullptr, 0, 0, PM_REMOVE)) {
			if (messager.message == WM_QUIT) {
				goto cleanup;
			}
			TranslateMessage(&messager);
			DispatchMessage(&messager);
		}

		// --- Toggle Menu Visibility & Click-Through ---
		if (GetAsyncKeyState(VK_INSERT) & 1) { // Check for rising edge
			show_menu = !show_menu;
			io.MouseDrawCursor = show_menu; // Toggle ImGui cursor visibility

			// --- Toggle WS_EX_TRANSPARENT ---
			LONG_PTR currentExStyle = GetWindowLongPtr(my_wnd, GWL_EXSTYLE);
			if (show_menu) {
				// Menu is now SHOWN: Remove TRANSPARENT style to capture clicks
				SetWindowLongPtr(my_wnd, GWL_EXSTYLE, currentExStyle & ~WS_EX_TRANSPARENT);
				// Optional: Bring overlay to front if needed for focus
				// SetForegroundWindow(my_wnd);
			}
			else {
				// Menu is now HIDDEN: Add TRANSPARENT style for click-through
				SetWindowLongPtr(my_wnd, GWL_EXSTYLE, currentExStyle | WS_EX_TRANSPARENT);
				// Optional: Give focus back to game
				// if (GameWnd) SetForegroundWindow(GameWnd);
			}
		}
		// --- End Toggle Logic ---


		// --- REMOVE Old Window Positioning Logic ---
		HWND hwnd_active = GetForegroundWindow();
		if (hwnd_active == GameWnd) {
			HWND hwndtest = GetWindow(hwnd_active, GW_HWNDPREV);
			SetWindowPos(my_wnd, hwndtest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); // <-- REMOVE THIS
		}
		// --- End Removal ---


		// Update mouse position for ImGui
		if (GetCursorPos(&p)) {
			ScreenToClient(my_wnd, &p);
			io.MousePos = ImVec2(static_cast<float>(p.x), static_cast<float>(p.y));
		}

		// Update mouse buttons ONLY if the menu is shown (clicks should pass through otherwise)
		io.MouseDown[0] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 && show_menu;
		io.MouseDown[1] = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0 && show_menu;
		io.MouseDown[2] = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0 && show_menu;


		// ImGui frame setup
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// --- Draw ESP/Visuals ---
		// This uses ImGui::GetBackgroundDrawList()
		render();


		// --- Draw FPS Counter ---
		// Ensure this doesn't interfere if it also uses Begin/End
	

		// --- Draw the Menu ---
		// Crucially, call DrawMenu. It will internally check if *p_open (show_menu) is true.
		// Even if show_menu is false, calling ImGui::Begin/End inside DrawMenu (which returns false
		// when *p_open is false) might be enough to keep the pipeline happy.
		// If DrawMenu *only* draws when *p_open is true, we might need a dummy window.
		//DrawMenu(&show_menu); // <-- KEEP THIS CALL
		menu2(show_menu);
		// --- Alternative: Dummy Window if DrawMenu does nothing when hidden ---
		// If DrawMenu is structured like: if (!*p_open) return;
		// then you might need this when show_menu is false:
		// if (!show_menu) {
		//     ImGui::Begin("BackgroundWorker", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs);
		//     ImGui::End();
		// }
		// --- End Alternative ---


		// Render ImGui (Compiles draw lists from render(), display_fps(), DrawMenu())
		ImGui::Render();

		// Direct3D rendering
		d3d_device_ctx->OMSetRenderTargets(1, &d3d_render_target, nullptr);
		d3d_device_ctx->ClearRenderTargetView(d3d_render_target, clear_color);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // Renders the compiled data
		d3d_swap_chain->Present(0, 0);

		// ... (frame timing end, busy-wait) ...
	}

cleanup:
	// --- Save Config ---
	printf(xorstr_("[~] Saving configuration...\n"));
	g_Config.Save();
	printf(xorstr_("[+] Configuration saved to '%s'\n"), g_Config.configFileName.c_str());
	// -------------------

	printf(xorstr_("[~] Shutting down ImGui and DirectX...\n"));
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	if (d3d_render_target) { d3d_render_target->Release(); d3d_render_target = nullptr; }
	if (d3d_swap_chain) { d3d_swap_chain->Release(); d3d_swap_chain = nullptr; }
	if (d3d_device_ctx) { d3d_device_ctx->Release(); d3d_device_ctx = nullptr; }
	if (p_device) { p_device->Release(); p_device = nullptr; }
	printf(xorstr_("[+] DirectX resources released.\n"));

	DestroyWindow(my_wnd);
	printf(xorstr_("[+] Window destroyed.\n"));
	std::cout << _("Render loop ended.") << std::endl;
	return messager.wParam;
}