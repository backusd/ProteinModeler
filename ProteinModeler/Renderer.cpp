#include "pch.h"
#include "Renderer.h"

using DirectX::XMFLOAT3;

Renderer::Renderer(std::shared_ptr<DeviceResources> deviceResources) :
    m_deviceResources(deviceResources),
    m_initialized(false),
    m_gameResourcesLoaded(false),
    m_viewport(CD3D11_VIEWPORT(0.0f, 0.0f, 100.0f, 100.0f)), // Assign dummy values for the viewport - this will be updated when the UI is created and triggers ViewportGrid_SizeChanged
    m_camera(nullptr)
{
    m_camera = std::make_unique<Camera>(m_viewport);

    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();

    CreateBoxPipelineConfig();
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

void Renderer::CreateBoxPipelineConfig()
{
    // Shaders
    std::unique_ptr<PixelShader> ps = std::make_unique<PixelShader>(m_deviceResources, L"BoxPixelShader.cso");
    std::unique_ptr<VertexShader> vs = std::make_unique<VertexShader>(m_deviceResources, L"BoxVertexShader.cso");

    // Input Layout
    std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
    inputElements.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
    std::unique_ptr<InputLayout> il = std::make_unique<InputLayout>(m_deviceResources, inputElements, vs.get());

    // Create Rasterizer State
    D3D11_RASTERIZER_DESC rasterDesc; // Fill with default values for now
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID; // D3D11_FILL_WIREFRAME; // ;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;
    std::unique_ptr<RasterizerState> rs = std::make_unique<RasterizerState>(m_deviceResources, rasterDesc);

    // Blend State
    D3D11_BLEND_DESC blendDesc;
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.IndependentBlendEnable = false;
    blendDesc.RenderTarget[0].BlendEnable = false;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    std::unique_ptr<BlendState> bs = std::make_unique<BlendState>(m_deviceResources, blendDesc);

    // Depth Stencil State
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.StencilEnable = false;
    depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    std::unique_ptr<DepthStencilState> dss = std::make_unique<DepthStencilState>(m_deviceResources, depthStencilDesc);

    // VS Buffers --------------
    std::unique_ptr<ConstantBufferArray> vsCBA = std::make_unique<ConstantBufferArray>(m_deviceResources);

    // Buffer #1: WorldViewProjection - Will be updated by Scene once per frame
    std::shared_ptr<ConstantBuffer> vsWorldViewProjectionBuffer = std::make_shared<ConstantBuffer>(m_deviceResources);
    vsWorldViewProjectionBuffer->CreateBuffer<WorldViewProjectionMatrix>(D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0u, 0u);
    //vsWorldViewProjectionBuffer->CreateBuffer<WorldViewProjectionMatrix>(D3D11_USAGE_DEFAULT, 0, 0u, 0u);

    vsCBA->AddBuffer(vsWorldViewProjectionBuffer);

    // Pipeline Configuration 
    std::unique_ptr<PipelineConfig> config = std::make_unique<PipelineConfig>(m_deviceResources,
        std::move(vs),
        std::move(ps),
        std::move(il),
        std::move(rs),
        std::move(bs),
        std::move(dss),
        std::move(vsCBA),
        nullptr
    );
    config->SetTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    // -------------------------------------------------
    // Mesh Set
    float x = 1.0f;
    float y = 1.0f;
    float z = 1.0f;
    std::vector<BoxVertex> vertices(8);
    vertices[0].Position = XMFLOAT3(x, y, z);
    vertices[1].Position = XMFLOAT3(-x, y, z);
    vertices[2].Position = XMFLOAT3(x, -y, z);
    vertices[3].Position = XMFLOAT3(x, y, -z);
    vertices[4].Position = XMFLOAT3(-x, -y, z);
    vertices[5].Position = XMFLOAT3(-x, y, -z);
    vertices[6].Position = XMFLOAT3(x, -y, -z);
    vertices[7].Position = XMFLOAT3(-x, -y, -z);

    std::vector<std::uint16_t> indices(24);

    // draw the square with all positive x
    indices[0] = 0;
    indices[1] = 3;
    indices[2] = 3;
    indices[3] = 6;
    indices[4] = 6;
    indices[5] = 2;
    indices[6] = 2;
    indices[7] = 0;

    // draw the square with all negative x
    indices[8] = 1;
    indices[9] = 5;
    indices[10] = 5;
    indices[11] = 7;
    indices[12] = 7;
    indices[13] = 4;
    indices[14] = 4;
    indices[15] = 1;

    // draw the four lines that connect positive x with negative x
    indices[16] = 0;
    indices[17] = 1;
    indices[18] = 3;
    indices[19] = 5;
    indices[20] = 6;
    indices[21] = 7;
    indices[22] = 2;
    indices[23] = 4;

    std::unique_ptr<MeshSet<BoxVertex>> ms = std::make_unique<MeshSet<BoxVertex>>(m_deviceResources);
    MeshInstance mi = ms->AddMesh(vertices, indices);
    ms->Finalize();

    // RenderObjectList ----------------------------------------------------------------------------

    XMFLOAT3 scaling = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
    const XMFLOAT3* translation = &m_translation;

    std::vector<RenderObjectList> objectLists;
    objectLists.emplace_back(m_deviceResources, mi);
    objectLists.back().AddRenderObject(scaling, translation, 0u);
    objectLists.back().SetBufferUpdateCallback([this](const RenderObjectList* renderObjectList, size_t, size_t)
        {
            using namespace DirectX;

            auto context = renderObjectList->GetDeviceResources()->GetD3DDeviceContext();

            D3D11_MAPPED_SUBRESOURCE ms;
            ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));

            Camera* camera = m_camera.get();
            XMMATRIX viewProj = camera->ViewMatrix() * camera->ProjectionMatrix();

            const std::vector<DirectX::XMFLOAT4X4>& worldMatrices = renderObjectList->GetWorldMatrices();
            WINRT_ASSERT(worldMatrices.size() == 1); // There should be exactly 1 world matrix for 1 simulation box

            XMMATRIX worldViewProjection = XMMatrixTranspose(XMLoadFloat4x4(&worldMatrices[0]) * viewProj);

            // Update the World-View-Projection buffer at VS slot 0 ------------------------------------------------------            
            winrt::com_ptr<ID3D11Buffer> buffer = nullptr;

            // TODO: Wrap this in THROW_INFO_ONLY macro
            context->VSGetConstantBuffers(0, 1, buffer.put());

            winrt::check_hresult(
                context->Map(buffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)
            );
            memcpy(ms.pData, &worldViewProjection, sizeof(XMMATRIX));

            // TODO: Wrap this in THROW_INFO_ONLY macro
            context->Unmap(buffer.get(), 0);
        }
    );

    m_configsAndObjectLists.push_back(std::make_tuple(std::move(config), std::move(ms), objectLists));
}

void Renderer::Render()
{
    auto context = m_deviceResources->GetD3DDeviceContext();

    ID3D11RenderTargetView* const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };
    context->OMSetRenderTargets(1u, targets, m_deviceResources->GetDepthStencilView());

    float background[4] = { 0.3f, 0.0f, 0.6f, 1.0f };
    context->ClearRenderTargetView(m_deviceResources->GetBackBufferRenderTargetView(), background);
    context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // Set the viewport
    // TODO: Wrap this in THROW_INFO_ONLY macro
    context->RSSetViewports(1, &m_viewport);

    // Apply the pipeline config for each list of render objects, render each object, then move onto the next config
    for (auto& configAndObjectList : m_configsAndObjectLists)
    {
        std::unique_ptr<PipelineConfig>& config = std::get<0>(configAndObjectList);
        config->ApplyConfig();

        std::unique_ptr<MeshSetBase>& ms = std::get<1>(configAndObjectList);
        ms->BindToIA();

        std::vector<RenderObjectList>& objectLists = std::get<2>(configAndObjectList);
        for (unsigned int iii = 0; iii < objectLists.size(); ++iii)
        {
            objectLists[iii].Render();
        }
    }
}

void Renderer::SetViewport(float top, float left, float height, float width) noexcept
{
    m_viewport.TopLeftX = left;
    m_viewport.TopLeftY = top;
    m_viewport.Width = width;
    m_viewport.Height = height;

    m_camera->SetViewport(m_viewport);
}