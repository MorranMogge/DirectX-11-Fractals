#include "ImGuiClass.h"
#include "imGui\imconfig.h"
#include "imGui\imgui.h"
#include "imGui\imgui_impl_dx11.h"
#include "imGui\imgui_internal.h"
#include "imGui\imstb_rectpack.h"
#include "imGui\imstb_textedit.h"
#include "imGui\imstb_truetype.h"
#include "imGui\imgui_impl_win32.h"
#include <string>
#include "InputInterpreter.h"

ImGuiClass::ImGuiClass()
	:renderingInfo(nullptr), mandelSrv(nullptr)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
}

ImGuiClass::~ImGuiClass()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

bool ImGuiClass::initiateImGui(HWND& window, ID3D11Device* device, ID3D11DeviceContext* immediateContext)
{
	if (!ImGui_ImplWin32_Init(window))
		return false;
	if (!ImGui_ImplDX11_Init(device, immediateContext))
		return false;

	return true;
}

bool ImGuiClass::setSrvInfo(ID3D11ShaderResourceView* srv, fractalsetinfo& info)
{
	this->mandelSrv = srv;
	this->renderingInfo = &info;

	return this->renderingInfo != nullptr;
}

void ImGuiClass::renderMandelbrot()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	{
		static const char* items[]{"32x32", "64x64", "128x128", "256x256" , "512x512", "1024x1024", "2048x2048" };
		static const char* sets[]{ "Mandelbrot set", "Julia set", "Own Function"};

		static int selectedRes = 5;
		static int selectedSet = 0;
		char function[64] = "";

		static float zoomX = 0.f;
		static float zoomY = 0.f;

		std::string resolution = "";


		bool begun = ImGui::Begin("Fractal settings");
		if (begun)
		{
			//ImGui::Text("Pointer = %p", mandelSrv);
			ImGui::Text("Size = %d x %d", renderingInfo->resWidth, renderingInfo->resHeight);
			ImGui::Text("Calculating the set took %f seconds", renderingInfo->calcMandelTime);
			ImGui::Text("Updating the texture took %f seconds", renderingInfo->updateTextureTime);
			ImGui::Combo("Fractal resolution", &selectedRes, items, (int)std::size(items));

			if (renderingInfo->resolution != selectedRes)
			{
				for (int i = 0; i < strlen(items[selectedRes]); i++)
				{
					if (items[selectedRes][i] == 'x')
						break;
					else
						resolution += items[selectedRes][i];
				}

				renderingInfo->resWidth = std::stoi(resolution);
				renderingInfo->resHeight = std::stoi(resolution);
				renderingInfo->resolution = selectedRes;
				renderingInfo->updateTexture = true;
			}

			ImGui::SliderInt("Max iterations", &renderingInfo->maxIterations, 1, 1000);
			ImGui::Combo("Sets", &selectedSet, sets, (int)std::size(sets));
			
			if (renderingInfo->set != selectedSet)
			{
				if (selectedSet == 0)
				{
					renderingInfo->rV = 0;
					renderingInfo->iV = 0;
				}
				renderingInfo->set = selectedSet;
			}

			if (selectedSet == 1)
			{
				ImGui::SliderFloat("Real Value", &renderingInfo->rV, -2, 2);
				ImGui::SliderFloat("Imaginary Value", &renderingInfo->iV, -2, 2);
				if (ImGui::Button("Reset constants value"))
				{
					renderingInfo->rV = 0;
					renderingInfo->iV = 0;
				}
				
			}
			else if (selectedSet == 2)
			{
				std::string txt = "";
				if (!renderingInfo->swapZandC)
					txt = "Mandelbrot version";
				else
					txt = "Julia set";

				ImGui::Text("Current function %s", renderingInfo->currentFunction.c_str());
				if (ImGui::InputText("Function", function, sizeof(function)))
					renderingInfo->currentFunction = function;
				ImGui::Text("Current version: %s", txt.c_str());

				if (ImGui::Button("Swap version"))
					renderingInfo->swapZandC = !renderingInfo->swapZandC;

				if (renderingInfo->swapZandC)
				{
					ImGui::SliderFloat("Real Value", &renderingInfo->rV, -2, 2);
					ImGui::SliderFloat("Imaginary Value", &renderingInfo->iV, -2, 2);
					if (ImGui::Button("Reset constants value"))
					{
						renderingInfo->rV = 0;
						renderingInfo->iV = 0;
					}
				}

				if (ImGui::Button("Test function"))
				{
					renderingInfo->doNewFractal = 1;
				}

			}

			if (renderingInfo->zooming.size() == 32)
				ImGui::Text("Max zoom limit has been reached");
			else if (ImGui::Button("Zoom"))
			{
				renderingInfo->zooming.push_back(zoom());
				renderingInfo->zooming.back().zoomValue = renderingInfo->zooming[renderingInfo->zooming.size() - 2].zoomValue * 2;
				renderingInfo->zooming.back().x = zoomX;
				renderingInfo->zooming.back().y = zoomY;
				renderingInfo->maxIterations--;
			}
			
			if ((ImGui::Button("Unzoom") && renderingInfo->zooming.size() > 1))
			{
				renderingInfo->zooming.pop_back();
				renderingInfo->maxIterations--;

			}

			ImGui::SliderFloat("x", &renderingInfo->offsetX, -2.f, 2.f);
			ImGui::SliderFloat("y", &renderingInfo->offsetY, -2.f, 2.f);

			if (ImGui::Button("Add colour") && renderingInfo->colours.size() < 10)
				renderingInfo->colours.push_back({0, 0, 0});

			for (int i = 0; i < renderingInfo->colours.size(); i++)
			{
				char plsFixThis[24];
				strcpy_s(plsFixThis, sizeof(plsFixThis), std::to_string(i).c_str());
				ImGui::ColorEdit3(plsFixThis, &renderingInfo->colours[i].colour[0]);
			}


			
			if (ImGui::Button("Remove colour") && renderingInfo->colours.size() > 0)
				renderingInfo->colours.pop_back();

			//No longer used, rip image rendering dx11 quad rendering is now my best friend
			//ImGui::Image((void*)(intptr_t)mandelSrv, ImVec2(renderingInfo->resWidth, renderingInfo->resHeight));
		}
		ImGui::End();
	}
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
