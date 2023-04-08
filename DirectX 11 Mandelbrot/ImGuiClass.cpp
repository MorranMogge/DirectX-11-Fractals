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
		static const char* sets[]{ "Mandelbrot set", "Julia set"};

		static int selectedRes = 5;
		static int selectedSet = 0;

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
					renderingInfo->z1 = 0;
					renderingInfo->z2 = 0;
				}
				renderingInfo->set = selectedSet;
			}
			if (selectedSet == 1)
			{
				ImGui::SliderFloat("C1", &renderingInfo->z1, -2, 2);
				ImGui::SliderFloat("C2", &renderingInfo->z2, -2, 2);
			}

			if (ImGui::Button("Add colour") && renderingInfo->colours.size() < 10)
				renderingInfo->colours.push_back({0, 0, 0});

			for (int i = 0; i < renderingInfo->colours.size(); i++)
			{
				/// I DO NOT KNOW WHY THIS SH*T DOES NOT WORK
				ImGui::ColorEdit3("", &renderingInfo->colours[i].colour[0]);
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
