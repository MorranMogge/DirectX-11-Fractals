#include "SetupPipeline.h"
#include <DirectXMath.h>
#include <fstream>
#include <iostream>


bool setPipeline(ID3D11Device* device, ID3D11DeviceContext* immediateContext)
{
    //These do not need to exist anymore after the pipeline has been set up
    ID3D11InputLayout* inputLayout;
    ID3D11Buffer* vBuffer;
    ID3D11SamplerState* sampler;
    ID3D11RasterizerState* rasterizerState;
    ID3D11VertexShader* vShader;
    ID3D11PixelShader* pShader;


    std::string shaderData;
    std::ifstream reader;
    reader.open("../x64/Debug/vShader.cso", std::ios::binary | std::ios::ate);
    if (!reader.is_open())
    {
        std::cout << "Could not open vertex shader file!" << std::endl;
    }

    reader.seekg(0, std::ios::end);
    shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
    reader.seekg(0, std::ios::beg);

    shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

    if (FAILED(device->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &vShader)))
    {
        std::cerr << "Failed to create vertex shader!" << std::endl;
    }

    std::string vShaderByteCode = shaderData;
    shaderData.clear();
    reader.close();


    reader.open("../x64/Debug/pShader.cso", std::ios::binary | std::ios::ate);
    if (!reader.is_open())
    {
        std::cout << "Could not open pixel shader file!" << std::endl;
    }

    reader.seekg(0, std::ios::end);
    shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
    reader.seekg(0, std::ios::beg);

    shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

    if (FAILED(device->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pShader)))
    {
        std::cerr << "Failed to create pixel shader!" << std::endl;
    }

    D3D11_INPUT_ELEMENT_DESC inputDesc[3] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    HRESULT hr = device->CreateInputLayout(inputDesc, 3, vShaderByteCode.c_str(), vShaderByteCode.length(), &inputLayout);

    if (FAILED(hr))
        std::cerr << "Failed to create pixel shader!" << std::endl;

    Vertex quad[6] =
    {
        //First triangle
        { {-0.5f, 0.5f, 0.0f}, {0, 0, -0}, {0, 0} },
        { {0.5f, -0.5f, 0.0f}, {0, 0, -1}, {1, 1} },
        { {-0.5f, -0.5f, 0.0f}, {0, 0, -1}, {0, 1} },

        //Second triangle
        { {-0.5f, 0.5f, 0.0f}, {0, 0, -1}, {0, 0} },
        { {0.5f, 0.5f, 0.0f}, {0, 0, -1}, {1, 0} },
        { {0.5f, -0.5f, 0.0f}, {0, 0, -1}, {1, 1} }
    };

    D3D11_BUFFER_DESC bufferDesc;
    bufferDesc.ByteWidth = sizeof(quad);
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = quad;
    data.SysMemPitch = 0;
    data.SysMemSlicePitch = 0;

    hr = device->CreateBuffer(&bufferDesc, &data, &vBuffer);

    D3D11_SAMPLER_DESC desc;
    desc.Filter = D3D11_FILTER_ANISOTROPIC;
    desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    desc.MipLODBias = 0;
    desc.MaxAnisotropy = 16;
    desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 0;
    desc.MinLOD = 0;
    desc.MaxLOD = D3D11_FLOAT32_MAX;

    hr = device->CreateSamplerState(&desc, &sampler);

    D3D11_RASTERIZER_DESC descdefault;
    descdefault.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
    descdefault.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
    descdefault.FrontCounterClockwise = false;
    descdefault.DepthBias = 0;
    descdefault.DepthBiasClamp = 0.0f;
    descdefault.SlopeScaledDepthBias = 0.0f;
    descdefault.DepthClipEnable = true;
    descdefault.ScissorEnable = false;
    descdefault.MultisampleEnable = false;
    descdefault.AntialiasedLineEnable = false;

    hr = device->CreateRasterizerState(&descdefault, &rasterizerState);


    struct ConstantBuffer
    {
        DirectX::XMFLOAT4X4 viewProjMatrix;
    };

    // Create the constant buffer
    ID3D11Buffer* constantBuffer;
    ConstantBuffer cb;


    DirectX::XMVECTOR eyePos = DirectX::XMVectorSet(0.0f, 0.0f, -0.5f, 0.0f);
    DirectX::XMVECTOR focusPos = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    DirectX::XMVECTOR upDir = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    DirectX::XMMATRIX viewM = DirectX::XMMatrixLookAtLH(eyePos, focusPos, upDir) * DirectX::XMMatrixPerspectiveFovLH(0.25f * DirectX::XM_2PI, 1024.f / 1024.f, 0.1f, 100.0f);;
    viewM = XMMatrixTranspose(viewM);

    DirectX::XMStoreFloat4x4(&cb.viewProjMatrix, viewM);

    D3D11_BUFFER_DESC cbDesc;
    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.ByteWidth = sizeof(cb);
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = 0;
    cbDesc.MiscFlags = 0;
    cbDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA cbData;
    cbData.pSysMem = &cb;
    cbData.SysMemPitch = 0;
    cbData.SysMemSlicePitch = 0;

    if (FAILED(device->CreateBuffer(&cbDesc, &cbData, &constantBuffer)))
        std::cerr << "Error creating world view projection matrix constant buffer\n";


    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    immediateContext->IASetInputLayout(inputLayout);
    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    immediateContext->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);



    immediateContext->VSSetShader(vShader, nullptr, 0);
    immediateContext->PSSetShader(pShader, nullptr, 0);

    immediateContext->PSSetSamplers(0, 1, &sampler);
    immediateContext->VSSetConstantBuffers(0, 1, &constantBuffer);

    vShader->Release();
    pShader->Release();
    inputLayout->Release();
    sampler->Release();
    vBuffer->Release();
    rasterizerState->Release();
    constantBuffer->Release();

    return true;
}
