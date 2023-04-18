#pragma once

#include "WindowHelper.h"
#include "FractalEngineStructs.h"
#include "ImGuiClass.h"
#include <iostream>
#include <d3d11.h>
#include <chrono>
#include <array>
#include <DirectXMath.h>
#include "SetupPipeline.h"

class FractalEngine
{
public:
	FractalEngine(HWND& window, uint32_t WIDTH, uint32_t HEIGHT);
	~FractalEngine();

	void run();

private:

	std::chrono::time_point<std::chrono::system_clock> start;

	fractalsetinfo renderingInfo;
	ImGuiClass imGui;

	//DirectX 11 stuff we need
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_immediateContext;
	IDXGISwapChain* m_swapChain;
	ID3D11RenderTargetView* m_rtv;
	ID3D11DepthStencilView* m_dsView;
	D3D11_VIEWPORT m_viewport;
	
	ID3D11Texture2D* m_dsTexture;

	////What we see on the screen
	ID3D11ShaderResourceView* m_mandelbrotSrv;
	ID3D11Texture2D* m_mandelbrotTexture;

	uint32_t WIDTH;
	uint32_t HEIGHT;

	float rV = 0;
	float iV = 0;

	int set = 5;

	int currentMaxIterations;

	void calc();
	void calcMandelbrot();
	void calcOwnFunction();
	bool setupSrv();
	void updateSrv();
	void colourPixel(int iterations, int& count);

	void render();
	void update();
};




