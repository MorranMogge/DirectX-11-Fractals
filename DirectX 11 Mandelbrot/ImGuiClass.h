#pragma once

#include "FractalEngineStructs.h"
#include <Windows.h>
#include <d3d11.h>

class ImGuiClass
{
public:
	ImGuiClass();
	~ImGuiClass();
	bool initiateImGui(HWND& window, ID3D11Device* device, ID3D11DeviceContext* immediateContext);
	bool setSrvInfo(ID3D11ShaderResourceView* srv, fractalsetinfo& info);

	void renderMandelbrot();

private:
	fractalsetinfo* renderingInfo;

	//Data containing picture of mandelbrot/other fractal set
	ID3D11ShaderResourceView* mandelSrv;
};