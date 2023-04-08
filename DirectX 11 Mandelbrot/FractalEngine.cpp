#include "FractalEngine.h"
#include "D3D11Helper.h"
#include <complex>

FractalEngine::FractalEngine(HWND& window, uint32_t WIDTH, uint32_t HEIGHT)
    :WIDTH(WIDTH), HEIGHT(HEIGHT), currentMaxIterations(100)
{
 
    if (!SetupD3D11(WIDTH, HEIGHT, window, m_device, m_immediateContext, m_swapChain, m_rtv, m_dsTexture, m_dsView, m_viewport))
        std::cerr << "Error creating m_mandelSrv\n";

    renderingInfo.pixels.reserve(2048 * 2048 * 4); //Amount of data needed for picture

    //Expands the array to 2048x2048 size (max size) 
    for (int i = 0; i < 2048 * 2048 * 4; i++)
    {
        renderingInfo.pixels.push_back(0);
        renderingInfo.pixels[i] = (uint8_t)(rand() % 256);
    }


    renderingInfo.nrOfColours = 4;
    renderingInfo.maxIterations = 100;
    renderingInfo.calcMandelTime = 0;
    renderingInfo.updateTextureTime = 0;
    renderingInfo.resWidth = WIDTH;
    renderingInfo.resHeight = HEIGHT;
    renderingInfo.updateTexture = false;

    if (!setupSrv())
        std::cerr << "Error creating m_mandelSrv\n";
    if (!imGui.initiateImGui(window, m_device, m_immediateContext))
        std::cerr << "Error initiating imGui class\n";
    if (!imGui.setSrvInfo(m_mandelbrotSrv, renderingInfo))
        std::cerr << "Error setting texture info for imGui class\n";

    m_immediateContext->RSSetViewports(1, &m_viewport);
    m_immediateContext->OMSetRenderTargets(1, &m_rtv, m_dsView);
    setPipeline(m_device, m_immediateContext);
}

FractalEngine::~FractalEngine()
{
	//Clearing up the mess
	m_device->Release();
	m_immediateContext->Release();
	m_swapChain->Release();
	m_rtv->Release();
	m_dsView->Release();
    m_mandelbrotSrv->Release();
    m_mandelbrotTexture->Release();
	m_dsTexture->Release();
}

void FractalEngine::run()
{
    calcMandelbrot();
    updateSrv();

	MSG msg = {};

	while (msg.message != WM_QUIT && !GetAsyncKeyState(VK_ESCAPE))
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

        this->render();
        this->update();
	}
}

void FractalEngine::calc()
{
    switch (renderingInfo.set)
    {
    case 0:
        this->calcMandelbrot();
        break;
    case 1:
        this->calcMandelbrot();
        break;
    default:
        break;
    }
}

void FractalEngine::calcMandelbrot()
{

    currentMaxIterations = renderingInfo.maxIterations;

    int count = 0;
    int r = 0;
    int g = 0;
    int b = 0;
    int iterations = 0;
    set = renderingInfo.set;

    //Used in order to zoom in on the set
    double currentWidth = renderingInfo.resWidth;
    double currentHeight = renderingInfo.resHeight;
    double offset[2] = { 0, 0 };

    z1 = renderingInfo.z1;
    z2 = renderingInfo.z2;


    //Start timer
    start = std::chrono::system_clock::now();
    
    for (uint32_t i = 0; i < currentWidth; i++)
    {
        for (uint32_t j = 0; j < currentHeight; j++)
        {
            //This next line tells us what position (in the coordinate system) the current pixel is at
            std::complex<double> c((double)j / currentWidth * 4.0 - 2.0, (double)i / currentHeight * 4.0 - 2.0);
            std::complex<double> z(z1, z2); //When mandelbrot is active these values are 0, 0

            //If it is the julia set, then we need to make c constant
            if (set == 1) {
                std::complex<double> temp = c;
                c = z;
                z = temp;
            }

            //Now it is time to check if the number is within the mandelbrot set, max iterations controls how accurate the result is
            iterations = 0;
            while (abs(z) < 2 && iterations < currentMaxIterations) {
                z = z * z + c;
                iterations++;
            }

            this->colourPixel(iterations, count);
        }
    }

    //Save the time it took to calculate the whole set
    renderingInfo.calcMandelTime = ((std::chrono::duration<float>)(std::chrono::system_clock::now() - start)).count();
}

bool FractalEngine::setupSrv()
{
    // Create texture
    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = renderingInfo.resWidth;
    desc.Height = renderingInfo.resHeight;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA subResource;
    subResource.pSysMem = renderingInfo.pixels.data();
    subResource.SysMemPitch = desc.Width * 4;
    subResource.SysMemSlicePitch = 0;
    HRESULT hr = m_device->CreateTexture2D(&desc, NULL, &m_mandelbrotTexture);

    if (FAILED(hr))
        return false;

    // Create texture view
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    hr = m_device->CreateShaderResourceView(m_mandelbrotTexture, &srvDesc, &m_mandelbrotSrv);

    if (FAILED(hr))
        return false;

    m_immediateContext->PSSetShaderResources(0, 1, &m_mandelbrotSrv);

	return true;
}

void FractalEngine::updateSrv()
{
    //If the resolution is changed
    if (renderingInfo.updateTexture) {
        //Get the new width and height
        
        //Begone
        m_mandelbrotSrv->Release();
        m_mandelbrotTexture->Release();

        //Recalculate the set to update after new resolution
        calc();
        setupSrv();

        //We don't really need this anymore
        imGui.setSrvInfo(m_mandelbrotSrv, renderingInfo);
    }

    
    //Start timer
    start = std::chrono::system_clock::now();

    //Map and unmap the resource to update the texture
    D3D11_MAPPED_SUBRESOURCE mapRes;
    ZeroMemory(&mapRes, sizeof(D3D11_MAPPED_SUBRESOURCE));
    HRESULT hr = m_immediateContext->Map(m_mandelbrotTexture, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mapRes);
    memcpy(mapRes.pData, renderingInfo.pixels.data(), sizeof(uint8_t)*renderingInfo.resWidth * renderingInfo.resHeight * 4);
    m_immediateContext->Unmap(m_mandelbrotTexture, 0);

    //End timer
    renderingInfo.updateTextureTime = ((std::chrono::duration<float>)(std::chrono::system_clock::now() - start)).count();
    
}

void FractalEngine::colourPixel(int iterations, int& count)
{
    int r, g, b;

    //When we have computed whether or not it is within the mandelbrot set, we can add colour
    if (iterations == currentMaxIterations) {
        r = 0;
        g = 0;
        b = 0;
    }
    else {
        double factor = (double)iterations / (double)currentMaxIterations;

        r = (int)(factor * 255.f);
        g = (int)(factor * 255.f);
        b = (int)(factor * 255.f);
    }

    renderingInfo.pixels[count++] = r;
    renderingInfo.pixels[count++] = g;
    renderingInfo.pixels[count++] = b;
    renderingInfo.pixels[count++] = 255; //0 is fully transparent, 255 is fully visable
}

void FractalEngine::render()
{
    
    static float clearColour[4]{ 0,0,0,0 };

    m_immediateContext->ClearRenderTargetView(m_rtv, clearColour);
    m_immediateContext->ClearDepthStencilView(m_dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

    //We only want to draw a quad (two triangles)
    m_immediateContext->Draw(6, 0);

    //Let imGui do imGui stuff
    imGui.renderMandelbrot();

    m_swapChain->Present(1, 0); //Limit the fps since we do not require uncapped framerate for this implementaion, also then my GPU doesn't have to suffer
}

void FractalEngine::update()
{
    if (renderingInfo.updateTexture || set != renderingInfo.set || currentMaxIterations != renderingInfo.maxIterations //All sets 
        || z1 != renderingInfo.z1 || z2 != renderingInfo.z2 //Julia set
        ) 
    {
        if (renderingInfo.updateTexture) {
            updateSrv();
            renderingInfo.updateTexture = false;
        }
        else {
            z1 = renderingInfo.z1;
            z2 = renderingInfo.z2;
            set = renderingInfo.set;
            calc();
            updateSrv();
        }

    }
}
