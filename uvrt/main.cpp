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

bool CreateDeviceD3D(HWND hWnd)  //�ú������ڴ���DirectX�豸�ͽ�������
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

void CleanupDeviceD3D()//�ú�����������DirectX�豸�����Դ��
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()//�������������ڴ�����������ȾĿ�ꡣ
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget()//�������������ڴ�����������ȾĿ�ꡣ
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)//�ú�����Windows��Ϣ���������������¼���
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

	FreeConsole();  // �ͷſ���̨

	HWND GameHwnd = FindWindowA(NULL, "��������"); //��ȡ�ͻ����ھ��


	// Create application window
	   //ImGui_ImplWin32_EnableDpiAwareness();
	WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
	::RegisterClassExW(&wc);
//	HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX11 Example", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);
	HWND hwnd = ::CreateWindowExW(WS_EX_TOPMOST|WS_EX_LAYERED | WS_EX_TOOLWINDOW,wc.lpszClassName, L"Dear ImGui DirectX11 Example", WS_POPUP, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);
	                              //��imguiϵ���ö�|�������ں�imgui���ڷֲ�|������������ʾ                                       //ȥ��������������
	                                                     //Ϊ��͸��

	// �Ƴ��������Ȳ���Ҫ�Ĳ���
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

	// Setup Dear ImGui context �ⲿ�ֳ�ʼ����ImGui�⣬���������ġ�Win32ƽ̨�ĳ�ʼ���Լ���DirectX 11�ļ��ɡ�
	IMGUI_CHECKVERSION();              
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io; //(void)io; ����δʹ�õı��� io �ı���������
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls���ü��̵������ơ�
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls������Ϸ�ֱ��������ơ�

	// Setup Dear ImGui style ���ñ���ɫ
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);


	ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\msyh.ttc", 25.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
	//IM_ASSERT(font != nullptr);                     //����                                                //����ʾ����



	//��δ����Ŀ�����ô��ڵı�����ɫΪ��ɫ��ͬʱ����ɫ����Ϊ͸��ɫ��
	// ����������Ⱦ ImGui UI ʱ����ɫ���ֽ���Ϊ͸�������ڵ����ಿ�ֽ���ʾӦ�ó�������������ݡ�
	ImVec4 clear_color = ImVec4(ImColor(0,0,0)); //
	SetLayeredWindowAttributes(hwnd, ImColor(0, 0, 0), NULL, LWA_COLORKEY);
		                                                      //ʹ��crKey��Ϊ͸������ɫ��

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
		
		// ��� "��������" �����Ƿ����
		if (!IsWindow(GameHwnd))
		{

			done = true;  // ������ڲ����ڣ��˳���ѭ��
			break;
		}


		//��δ��봦���ڵ�����С���߼���
		// �� Windows ��Ϣ����ѭ���У���� g_ResizeWidth �� g_ResizeHeight ��Ϊ�㣬˵�����ڴ�С�����˸ı䡣
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

		//���ƴ�����
		static bool �򿪴��� = true;
		static bool ��ѡ�� = false;
		static int a = 0;
		static float b = 0;
		static float c = 0;
		static char butter[1024];


		if (�򿪴���)
		{
			ImGui::Begin(u8"myfirst ����", &�򿪴���);
			//u8��ʾ����

			ImGui::Text(u8"�������");//��ͨ�ı�
			ImGui::SameLine(); //���������ı�������ͬһ��
			ImGui::TextColored(ImColor(48, 152, 56, 255), u8"�������");//����ɫ���ı�
			ImGui::BulletText(u8"���Ҵ���");  //ǰ���СԲ���ı�

			if (ImGui::Button(u8"����")) { //����һ����ť
				MessageBoxA(NULL, "123", "done", NULL);
			}

			ImGui::Checkbox(u8"��ѡ��", &��ѡ��);
			if (��ѡ��)
			{
				printf("��ѡ��");  //��Ҫ��MessageBoxA  ��������
			}

			ImGui::RadioButton(u8"0", &a, 0);
			ImGui::RadioButton(u8"1", &a, 1);
			ImGui::RadioButton(u8"2", &a, 2);
			//ѡ���ĸ�a���Ǽ�  printf("%d", a);

			ImGui::DragFloat(u8"������", &b, 0, 0, 10.0f);//����b�ķ���ֵ��ȷ����С

			ImGui::SliderFloat(u8"������", &c, 0, 20.0f);//����c�ķ���ֵ��ȷ����С


			ImGui::InputText(u8"�����", butter, sizeof(butter));

			        //��ǰ
			ImGui::GetForegroundDrawList()->AddLine(ImVec2(100, 100), ImVec2(100, 300), ImColor(0, 245, 255, 255));
			       //����                  //������			
			ImGui::GetBackgroundDrawList()->AddRect(ImVec2(200, 200), ImVec2(400, 400), ImColor(0, 245, 255, 255), 15);
			                               //���ƾ���
			ImGui::GetForegroundDrawList()->AddCircle(ImVec2(1280 / 2, 800 / 2), 15, ImColor(0, 245, 255, 255),6);
			                               //����Բ                                                           //�oԲ���ӱߵ�
		
		    ImGui::GetForegroundDrawList()->AddText(ImVec2(100,100), ImColor(0,245,255,255), u8"�������");
			                               //�����ı�

			����ֱ��(ImVec2(100, 200), ImVec2(500, 300), ImColor(0, 235, 225, 255),30);


			ImGui::End();
		}



		//ˢ�´���
		RECT GameRect;
		GetClientRect(GameHwnd, &GameRect);//��ȡ�ͻ������ڴ�С
		POINT ClientD2D = { GameRect.left,GameRect.top };
		ClientToScreen(GameHwnd, & ClientD2D);//��imgui��������ͶӰ���ͻ���������
		MoveWindow(hwnd,ClientD2D.x,ClientD2D.y,GameRect.right,GameRect.bottom,true);//��֮�䶯 









		//--------------------------------------------------------
		// Rendering��Ⱦ
		ImGui::Render();
		const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		g_pSwapChain->Present(1, 0); // Present with vsync
		//g_pSwapChain->Present(0, 0); // Present without vsync
	
    }

	// Cleanup�����ͷŴ�����
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClassW(wc.lpszClassName, wc.hInstance);


}