#pragma once
#include <d3d11.h>
#include <array>


struct Vertex
{
	float pos[3]{ 0 };
	float n[3]{ 0 };
	float uv[2]{ 0 };

	Vertex(const std::array<float, 3>& position, const std::array<float, 3>& normal, const std::array<float, 2>& inUv)
	{
		for (int i = 0; i < 3; i++)
		{
			pos[i] = position[i];
			n[i] = normal[i];
			if (i < 2)
				uv[i] = inUv[i];
		}
	}
};

bool setPipeline(ID3D11Device* device, ID3D11DeviceContext* immediateContext);
