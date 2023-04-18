#include "SetupPipeline.h"
#include <DirectXMath.h>
#include <fstream>
#include <iostream>


bool createShaders(ID3D11Device* device, ID3D11DeviceContext* immediateContext, std::string& vShaderByteCode)
{

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

    vShaderByteCode = shaderData;
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

    immediateContext->VSSetShader(vShader, nullptr, 0);
    immediateContext->PSSetShader(pShader, nullptr, 0);

    vShader->Release();
    pShader->Release();

    return true;
}

bool createInputLayout(ID3D11Device* device, ID3D11DeviceContext* immediateContext, std::string& vShaderByteCode)
{
    ID3D11InputLayout* inputLayout;

    D3D11_INPUT_ELEMENT_DESC inputDesc[3] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    HRESULT hr = device->CreateInputLayout(inputDesc, 3, vShaderByteCode.c_str(), vShaderByteCode.length(), &inputLayout);

    if (FAILED(hr))
    {
        std::cerr << "Failed to create input layout\n";
        return false;
    }

    immediateContext->IASetInputLayout(inputLayout);
    inputLayout->Release();

    return true;
}

bool createVertexBuffer(ID3D11Device* device, ID3D11DeviceContext* immediateContext)
{
    ID3D11Buffer* vBuffer;


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

    HRESULT hr = device->CreateBuffer(&bufferDesc, &data, &vBuffer);
    if (FAILED(hr))
    {
        std::cerr << "Failed to create vertex buffer\n";
        return false;
    }


    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    immediateContext->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);
    vBuffer->Release();

    return true;
}

bool createCameraBuffer(ID3D11Device* device, ID3D11DeviceContext* immediateContext)
{
    struct ConstantBuffer
    {
        DirectX::XMFLOAT4X4 viewProjMatrix;
    };

    // Create the constant buffer for the camera
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
    {
        std::cerr << "Error creating world view projection matrix constant buffer\n";
        return false;
    }

    immediateContext->VSSetConstantBuffers(0, 1, &constantBuffer);
    constantBuffer->Release();


    return true;
}

bool createSampler(ID3D11Device* device, ID3D11DeviceContext* immediateContext)
{
    ID3D11SamplerState* sampler;

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

    HRESULT hr = device->CreateSamplerState(&desc, &sampler);

    if (FAILED(hr))
    {
        std::cerr << "Failed to create sampler state\n";
        return false;
    }

    immediateContext->PSSetSamplers(0, 1, &sampler);
    sampler->Release();

    return true;
}

bool setPipeline(ID3D11Device* device, ID3D11DeviceContext* immediateContext)
{
    
    std::string vShaderByteCode = "";
    
    //These do not need to exist anymore after the pipeline has been set up
    if (!createShaders(device, immediateContext, vShaderByteCode))
        return false;

    if (!createInputLayout(device, immediateContext, vShaderByteCode))
        return false;
    
    if (!createVertexBuffer(device, immediateContext))
        return false;
    
    if (!createCameraBuffer(device, immediateContext))
        return false;

    if (!createSampler(device, immediateContext))
        return false;

    //Now everything is set up
    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    return true;
}
