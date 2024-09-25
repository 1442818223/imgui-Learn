#define _CRT_SECURE_NO_WARNINGS 
#include"auto.h"



// Data
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();

bool CreateDeviceD3D(HWND hWnd)  //该函数用于创建DirectX设备和交换链。
{
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
		res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
	if (res != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()//该函数用于清理DirectX设备相关资源。
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()//这两个函数用于创建和清理渲染目标。
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget()//这两个函数用于创建和清理渲染目标。
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)//该函数是Windows消息处理函数，处理窗口事件。
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
			return 0;
		g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
		g_ResizeHeight = (UINT)HIWORD(lParam);
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

//int APIENTRY main(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
int main(int argc, char* argv[])
{

	FreeConsole();  // 释放控制台

	HWND GameHwnd = FindWindowA(NULL, "弹窗测试"); //获取客户窗口句柄


	// Create application window
	   //ImGui_ImplWin32_EnableDpiAwareness();
	WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
	::RegisterClassExW(&wc);
//	HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX11 Example", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);
	HWND hwnd = ::CreateWindowExW(WS_EX_TOPMOST|WS_EX_LAYERED | WS_EX_TOOLWINDOW,wc.lpszClassName, L"Dear ImGui DirectX11 Example", WS_POPUP, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);
	                              //让imgui系列置顶|背景窗口和imgui窗口分层|不在任务栏显示                                       //去除背景窗标题栏
	                                                     //为了透明

	// 移除标题栏等不必要的部分
	SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_CAPTION & ~WS_THICKFRAME & ~WS_MINIMIZEBOX & ~WS_MAXIMIZEBOX);
	SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

	// Initialize Direct3D
	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClassW(wc.lpszClassName, wc.hInstance);
		return 1;
	}

	// Show the window
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	// Setup Dear ImGui context 这部分初始化了ImGui库，包括上下文、Win32平台的初始化以及与DirectX 11的集成。
	IMGUI_CHECKVERSION();              
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io; //(void)io; 消除未使用的变量 io 的编译器警告
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls启用键盘导航控制。
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls启用游戏手柄导航控制。

	// Setup Dear ImGui style 设置背景色
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);


	ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msyh.ttc", 25.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
	//IM_ASSERT(font != nullptr);                     //字体                                                //能显示中文



	//这段代码的目的是让窗口的背景颜色为黑色，同时将黑色设置为透明色。
	// 这样，在渲染 ImGui UI 时，黑色部分将变为透明，窗口的其余部分将显示应用程序的主窗口内容。
	ImVec4 clear_color = ImVec4(ImColor(0,0,0)); //
	SetLayeredWindowAttributes(hwnd, ImColor(0, 0, 0), NULL, LWA_COLORKEY);
		                                                      //使用crKey作为透明度颜色。

	// Main loop
	bool done = false;
	while (!done)
	{
		// Poll and handle messages (inputs, window resize, etc.)
		// See the WndProc() function below for our to dispatch events to the Win32 backend.
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				done = true;
		}
		if (done)
			break;
		
		// 检查 "弹窗测试" 窗口是否存在
		if (!IsWindow(GameHwnd))
		{

			done = true;  // 如果窗口不存在，退出主循环
			break;
		}


		//这段代码处理窗口调整大小的逻辑。
		// 在 Windows 消息处理循环中，如果 g_ResizeWidth 和 g_ResizeHeight 不为零，说明窗口大小发生了改变。
		// Handle window resize (we don't resize directly in the WM_SIZE handler)
		if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
		{
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
			g_ResizeWidth = g_ResizeHeight = 0;
			CreateRenderTarget();
		}

		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		//----------------------------------------------------------------------------------------

		//绘制代码区
		static bool 打开窗口 = true;
		static bool 复选框 = false;
		static int a = 0;
		static float b = 0;
		static float c = 0;
		static char butter[1024];


		if (打开窗口)
		{
			ImGui::Begin(u8"myfirst 窗口", &打开窗口);
			//u8表示中文

			ImGui::Text(u8"世界你好");//普通文本
			ImGui::SameLine(); //让下语句的文本保持在同一行
			ImGui::TextColored(ImColor(48, 152, 56, 255), u8"世界你好");//加颜色的文本
			ImGui::BulletText(u8"等我打完");  //前面加小圆点文本

			if (ImGui::Button(u8"测试")) { //生成一个按钮
				MessageBoxA(NULL, "123", "done", NULL);
			}

			ImGui::Checkbox(u8"复选框", &复选框);
			if (复选框)
			{
				printf("已选中");  //不要用MessageBoxA  他会阻塞
			}

			ImGui::RadioButton(u8"0", &a, 0);
			ImGui::RadioButton(u8"1", &a, 1);
			ImGui::RadioButton(u8"2", &a, 2);
			//选中哪个a就是几  printf("%d", a);

			ImGui::DragFloat(u8"滑块条", &b, 0, 0, 10.0f);//根据b的返回值来确定大小

			ImGui::SliderFloat(u8"浮动条", &c, 0, 20.0f);//根据c的返回值来确定大小


			ImGui::InputText(u8"输入框", butter, sizeof(butter));

			        //当前
			ImGui::GetForegroundDrawList()->AddLine(ImVec2(100, 100), ImVec2(100, 300), ImColor(0, 245, 255, 255));
			       //背景                  //绘制线			
			ImGui::GetBackgroundDrawList()->AddRect(ImVec2(200, 200), ImVec2(400, 400), ImColor(0, 245, 255, 255), 15);
			                               //绘制矩形
			ImGui::GetForegroundDrawList()->AddCircle(ImVec2(1280 / 2, 800 / 2), 15, ImColor(0, 245, 255, 255),6);
			                               //绘制圆                                                           //給圆增加边的
		
		    ImGui::GetForegroundDrawList()->AddText(ImVec2(100,100), ImColor(0,245,255,255), u8"你好世界");
			                               //绘制文本

			绘制直线(ImVec2(100, 200), ImVec2(500, 300), ImColor(0, 235, 225, 255),30);


			ImGui::End();
		}



		//刷新窗口
		RECT GameRect;
		GetClientRect(GameHwnd, &GameRect);//获取客户区窗口大小
		POINT ClientD2D = { GameRect.left,GameRect.top };
		ClientToScreen(GameHwnd, & ClientD2D);//让imgui背景窗口投影到客户窗口上面
		MoveWindow(hwnd,ClientD2D.x,ClientD2D.y,GameRect.right,GameRect.bottom,true);//随之变动 









		//--------------------------------------------------------
		// Rendering渲染
		ImGui::Render();
		const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		g_pSwapChain->Present(1, 0); // Present with vsync
		//g_pSwapChain->Present(0, 0); // Present without vsync
	
    }

	// Cleanup清理释放代码区
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClassW(wc.lpszClassName, wc.hInstance);


}