#include "ScriptHook.h"

ID3D11Device* pDevice;
ID3D11DeviceContext* pContext;
IDXGISwapChain* pSwapChain;
ID3D11RenderTargetView* pRenderTargetView;

ScriptHook::ScriptHook()
{

}


ScriptHook::~ScriptHook()
{

}

BOOL CALLBACK EnumWindowCallback(HWND hWnd, LPARAM lParam)
{
	ProcessWindowData& wndData = *(ProcessWindowData*)lParam;
	unsigned long lProcessId = 0;
	::GetWindowThreadProcessId(hWnd, &lProcessId);
	if ((wndData.lProcessId != lProcessId) || (::GetWindow(hWnd, GW_OWNER) != (HWND)0) || !::IsWindowVisible(hWnd))
		return 1;
	wndData.hWnd = hWnd;
	return 0;
}

HWND GetMainWindowHwnd(unsigned long lProcessId)
{
	ProcessWindowData wndData;
	wndData.hWnd = 0;
	wndData.lProcessId = lProcessId;
	::EnumWindows(EnumWindowCallback, (LPARAM)&wndData);
	return wndData.hWnd;
}

void ScriptHook::Initialize()
{
	HWND hWnd = GetMainWindowHwnd(GetCurrentProcessId());
	RECT rect;
	GetClientRect(hWnd, &rect);

	// ������
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
	scd.BufferCount = 1;														//	����˫����
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;							//	������ɫ��ʽ��ʹ��8λRGBA
	scd.BufferDesc.Width = rect.right - rect.left;
	scd.BufferDesc.Height = rect.bottom - rect.top;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;						//	���ű�
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;		//	ɨ����
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;							//	��ȾĿ�����
	scd.OutputWindow = hWnd;													//	ָ���������
	scd.SampleDesc.Count = 1;													//	�ز���
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;							//	����ģʽ�л�
	scd.BufferDesc.RefreshRate.Numerator = 144;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.Windowed = !(GetWindowLong(hWnd, GWL_STYLE) & WS_POPUP);				//	�Ƿ�ȫ��
	scd.SampleDesc.Quality = 0;													//	�����ȼ�
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;									//	��������

	D3D_FEATURE_LEVEL featrueLevel = D3D_FEATURE_LEVEL_11_1;
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &featrueLevel, 1, D3D11_SDK_VERSION, &scd, &pSwapChain, &pDevice, NULL, &pContext);

	DWORD_PTR* pSwapChainVT = NULL;
	DWORD_PTR* pDeviceVT = NULL;
	DWORD_PTR* pContextVT = NULL;
	pSwapChainVT = (DWORD_PTR *)pSwapChain;
	// pDeviceVT = (DWORD_PTR*)pDevice;
	// pContextVT = (DWORD_PTR*)pContext;

	pSwapChainVT = (DWORD_PTR *)pSwapChainVT[0];
	Hooks::oPresent = (tD3D11Present)pSwapChainVT[8];

	Renderer::GetInstance()->Initialize();
	Renderer::GetInstance()->SetStyle();
	Input::GetInstance()->StartThread();

	HookFunction((PVOID *)&Hooks::oPresent, (PVOID)&Hooks::hkD3D11Present);
}

void ScriptHook::Release()
{
	Input::GetInstance()->StopThread();
	UnHookFunction((PVOID *)&Hooks::oPresent, (PVOID)&Hooks::hkD3D11Present);
}

void ScriptHook::HookFunction(PVOID * oFunction, PVOID pDetour)
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(oFunction, pDetour);
	DetourTransactionCommit();
}

void ScriptHook::UnHookFunction(PVOID * oFunction, PVOID pDetour)
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(oFunction, pDetour);
	DetourTransactionCommit();
}

namespace Hooks
{
	tD3D11Present oPresent = NULL;
	bool bOnce = false;

	HRESULT __stdcall hkD3D11Present(IDXGISwapChain* pSwapChain, UINT SysInterval, UINT Flags)
	{
		if (!bOnce)
		{
			//��һ�λ����
			//�õ���Ϸ����
			HWND hWindow = GetMainWindowHwnd(GetCurrentProcessId());
			if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void **)(&pDevice))))
			{
				pSwapChain->GetDevice(__uuidof(ID3D11Device), (void **)(&pDevice));
				pDevice->GetImmediateContext(&pContext);
			}

			ID3D11Texture2D* renderTargetTexture = nullptr;
			//��ȡ�󻺳�����ַ
			if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&renderTargetTexture)))
			{
				//����Ŀ����ͼ
				pDevice->CreateRenderTargetView(renderTargetTexture, NULL, &pRenderTargetView);
				//�ͷź󻺳�
				renderTargetTexture->Release();
			}

			//��ʼ��ImGUI
			ImGui_ImplDX11_Init(hWindow, pDevice, pContext);
			ImGui_ImplDX11_CreateDeviceObjects();

			ImGui::StyleColorsDark();

			bOnce = true;
		}

		//��ͣ�Ļ��������
		ImGui_ImplDX11_NewFrame();

		DrawD3DMenuMain();

		ImGui::Render();
		pContext->OMSetRenderTargets(1, &pRenderTargetView, NULL);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		return oPresent(pSwapChain, SysInterval, Flags);
	}
}