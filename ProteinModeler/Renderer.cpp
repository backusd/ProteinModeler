#include "pch.h"
#include "Renderer.h"



Renderer::Renderer(std::shared_ptr<DeviceResources> deviceResources) :
    m_deviceResources(deviceResources),
    m_initialized(false),
    m_gameResourcesLoaded(false)
{
    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();
}

void Renderer::CreateDeviceDependentResources()
{
    m_gameResourcesLoaded = false;
}

void Renderer::CreateWindowSizeDependentResources()
{
    // TODO: Update the Projection matrix
}

void Renderer::ReleaseDeviceDependentResources()
{
    // TODO: Determine if there is anything necessary to put here
    // 
    //    // On device lost all the device resources are invalid.
    //    // Set the state of the renderer to not have a pointer to the
    //    // Simple3DGame object.  It will be reset as a part of the
    //    // game devices resources being recreated.
    //    m_game = nullptr;
    //    m_gameHud->ReleaseDeviceDependentResources();
}

void Renderer::Render()
{
    auto context = m_deviceResources->GetD3DDeviceContext();

    ID3D11RenderTargetView* const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };
    context->OMSetRenderTargets(1u, targets, m_deviceResources->GetDepthStencilView());

    float background[4] = { 0.3f, 0.0f, 0.6f, 1.0f };
    context->ClearRenderTargetView(m_deviceResources->GetBackBufferRenderTargetView(), background);
    context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

}