#include "pch.h"
#include "Renderer.h"

using namespace DirectX;

Renderer::Renderer(std::shared_ptr<DeviceResources> deviceResources, Simulation* simulation) :
    m_deviceResources(deviceResources),
    m_simulation(simulation),
    m_initialized(false),
    m_gameResourcesLoaded(false),
    m_viewport(CD3D11_VIEWPORT(0.0f, 0.0f, 100.0f, 100.0f)), // Assign dummy values for the viewport - this will be updated when the UI is created and triggers ViewportGrid_SizeChanged
    m_camera(nullptr)
{
    WINRT_ASSERT(simulation != nullptr);

    m_camera = std::make_unique<Camera>(m_viewport);

    CreateDeviceDependentResources();
    CreateWindowSizeDependentResources();

    m_materials = std::make_unique<MaterialsArray>();
    CreateMaterials();

    CreateMainPipelineConfig();
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

void Renderer::CreateMaterials()
{
    // Load the data
    for (unsigned int iii = 0; iii < NUM_MATERIALS; ++iii)
    {
        m_materials->materials[iii].DiffuseAlbedo = DirectX::XMFLOAT4(0.5f, 0.2f, 0.1f, 1.0f);
        m_materials->materials[iii].FresnelR0 = DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f);
        m_materials->materials[iii].Shininess = 0.2f;
    }
}

void Renderer::CreateMainPipelineConfig()
{
    // Shaders
    std::unique_ptr<PixelShader> ps = std::make_unique<PixelShader>(m_deviceResources, L"PixelShaderInstanced.cso");
    std::unique_ptr<VertexShader> vs = std::make_unique<VertexShader>(m_deviceResources, L"VertexShaderInstanced.cso");

    // Input Layout
    std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
    inputElements.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,                            0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
    inputElements.push_back({ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 });
    // Instance Data ---------------------------------------------
    inputElements.push_back({ "MATERIAL_INDEX", 0, DXGI_FORMAT_R32_UINT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 });
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

    // Buffer #1: PassConstants - Will be updated by Scene once per rendering pass
    std::shared_ptr<ConstantBuffer<PassConstants>> vsPassConstantsBuffer = 
        std::make_shared<ConstantBuffer<PassConstants>>(m_deviceResources, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0u, 0u);

    m_vsPerPassConstantsBuffers.push_back(vsPassConstantsBuffer); // The Scene must keep track of this buffer because it is responsible for updating it
    vsCBA->AddBuffer(vsPassConstantsBuffer);

    // Buffer #2: WorldMatrixInstances - Will be updated by each RenderObjectList before each Draw call
    std::shared_ptr<ConstantBuffer<WorldMatrixInstances>> vsWorldMatrixInstancesBuffer = 
        std::make_shared<ConstantBuffer<WorldMatrixInstances>>(m_deviceResources, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0u, 0u);

    vsCBA->AddBuffer(vsWorldMatrixInstancesBuffer);

    // PS Buffers --------------
    std::unique_ptr<ConstantBufferArray> psCBA = std::make_unique<ConstantBufferArray>(m_deviceResources);

    // Buffer #1: PassConstants - Will be updated by Scene once per rendering pass
    std::shared_ptr<ConstantBuffer<PassConstants>> psPassConstantsBuffer 
        = std::make_shared<ConstantBuffer<PassConstants>>(m_deviceResources, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0u, 0u);

    m_psPerPassConstantsBuffers.push_back(psPassConstantsBuffer); 
    psCBA->AddBuffer(psPassConstantsBuffer);

    // Buffer #2: MaterialsArray - Buffer with all materials that will not ever be updated
    WINRT_ASSERT(m_materials != nullptr); // Materials have not been created
    m_materialsBuffer = std::make_shared<ConstantBuffer<MaterialsArray>>(m_deviceResources, D3D11_USAGE_DEFAULT, 0u, 0u, 0u, m_materials.get());

    m_psPerPassConstantsBuffers.push_back(m_materialsBuffer);
    psCBA->AddBuffer(m_materialsBuffer);

    // Pipeline Configuration 
    std::unique_ptr<PipelineConfig> config = std::make_unique<PipelineConfig>(m_deviceResources,
        std::move(vs),
        std::move(ps),
        std::move(il),
        std::move(rs),
        std::move(bs),
        std::move(dss),
        std::move(vsCBA),
        std::move(psCBA)
    );

    // -------------------------------------------------
    // Mesh Set
    std::unique_ptr<MeshSet<Vertex>> ms = std::make_unique<MeshSet<Vertex>>(m_deviceResources);
    ms->SetVertexConversionFunction([](std::vector<GenericVertex> input) -> std::vector<Vertex>
        {
            std::vector<Vertex> output(input.size());
            for (unsigned int iii = 0; iii < input.size(); ++iii)
            {
                output[iii].Pos = input[iii].Position;
                output[iii].Normal = input[iii].Normal;
            }
            return output;
        }
    );
    MeshInstance mi = ms->AddGeosphere(1.0f, 3);
    ms->Finalize();

    // RenderObjectLists ----------------------------------------------------------------------------
    std::vector<DirectX::XMFLOAT3>& positions = m_simulation->Positions();
    //std::vector<DirectX::XMFLOAT3>& velocities = m_simulation->Velocities(); Not needed right now
    std::vector<Element>& elementTypes = m_simulation->ElementTypes();

    // NOTE: Template parameter specifies the data type used by the instance buffer
    std::unique_ptr<RenderObjectInstanced<unsigned int>> instancedObject = std::make_unique<RenderObjectInstanced<unsigned int>>(m_deviceResources, mi);

    float r;
    unsigned int elementType;
    for (unsigned int iii = 0; iii < positions.size(); ++iii)
    {
        elementType = static_cast<int>(elementTypes[iii]);
        r = AtomicRadii[elementType];
        instancedObject->AddInstance({ r, r, r }, &positions.data()[iii], elementType - 1); // must subtract one because Hydrogen is 1, but its material is at index 0, etc.
    }

    instancedObject->m_BufferUpdateFn = [](const RenderObjectInstanced<unsigned int>* instancedObject, size_t startIndex, size_t endIndex)
        {
            auto context = instancedObject->GetDeviceResources()->GetD3DDeviceContext();

            const std::vector<DirectX::XMFLOAT4X4>& worldMatrices = instancedObject->GetWorldMatrices();
            const std::vector<unsigned int>& materialIndices = instancedObject->GetMaterialIndices();

            D3D11_MAPPED_SUBRESOURCE ms;

            // Update the World buffers at VS slot 1 ------------------------------------------------------
            ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));
            winrt::com_ptr<ID3D11Buffer> buffer = nullptr;

            // TODO: Wrap this in a THROW_INFO_ONLY macro
            context->VSGetConstantBuffers(1, 1, buffer.put());
            winrt::check_hresult(
                context->Map(buffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)
            );
            // Only copy the elements between the start and end indices
            memcpy(ms.pData, &worldMatrices.data()[startIndex], sizeof(DirectX::XMFLOAT4X4) * (endIndex - startIndex + 1));
            // TODO: Wrap this in a THROW_INFO_ONLY macro
            context->Unmap(buffer.get(), 0);

            // --------------------------------------------------------------------------------------------
            // Update the instance buffer and then bind it to the IA

            winrt::com_ptr<ID3D11Buffer> instanceBuffer = instancedObject->GetInstanceBuffer();

            ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));

            winrt::check_hresult(
                context->Map(instanceBuffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)
            );
            // Only copy the elements between the start and end indices
            memcpy(ms.pData, &materialIndices.data()[startIndex], sizeof(unsigned int) * (endIndex - startIndex + 1));
            // TODO: Wrap this in a THROW_INFO_ONLY macro
            context->Unmap(instanceBuffer.get(), 0);
        };

    std::vector<std::unique_ptr<RenderableBase>> objects;
    objects.push_back(std::move(instancedObject));

    std::vector<MeshSetAndObjectList> meshSetAndObjectLists;
    meshSetAndObjectLists.push_back(std::make_tuple(std::move(ms), std::move(objects)));

    m_configsAndObjectLists.push_back(std::make_tuple(std::move(config), std::move(meshSetAndObjectLists)));
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
    std::shared_ptr<ConstantBuffer<WorldViewProjectionMatrix>> vsWorldViewProjectionBuffer = 
        std::make_shared<ConstantBuffer<WorldViewProjectionMatrix>>(m_deviceResources, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0u, 0u);

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

    XMFLOAT3 scaling = m_simulation->BoxScaling();
    const XMFLOAT3* translation = m_simulation->BoxTranslation();

    std::unique_ptr<RenderObject> object = std::make_unique<RenderObject>(m_deviceResources, mi, scaling, translation, 0u);
    object->m_BufferUpdateFn = [this](const RenderObject* object)
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        D3D11_MAPPED_SUBRESOURCE ms;
        ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));

        Camera* camera = m_camera.get();
        XMMATRIX viewProj = camera->ViewMatrix() * camera->ProjectionMatrix();

        // By default, the world matrices are computed PRE-transposed. This is done by default because the overwhelming majority of
        // the time, we will NOT be computing the World-View-Projection matrix on the CPU and instead be computing it in the VertexShader.
        // Therefore, we need to untranspose it before computing the final matrix.
        XMMATRIX worldViewProjection = XMMatrixTranspose(XMMatrixTranspose(object->WorldMatrix()) * viewProj);

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
    };

    std::vector<std::unique_ptr<RenderableBase>> objects;
    objects.push_back(std::move(object));

    std::vector<MeshSetAndObjectList> meshSetAndObjectLists;
    meshSetAndObjectLists.push_back(std::make_tuple(std::move(ms), std::move(objects)));

    m_configsAndObjectLists.push_back(std::make_tuple(std::move(config), std::move(meshSetAndObjectLists)));
}

void Renderer::Update(const Timer& timer)
{
    auto context = m_deviceResources->GetD3DDeviceContext();

    // Update the Camera ---------------------------------------------------------------------
    m_camera->Update(timer);

    // Update Pass Constants -----------------------------------------------------------------

    XMMATRIX view = m_camera->ViewMatrix();
    XMMATRIX proj = m_camera->ProjectionMatrix();

    XMVECTOR viewDet = DirectX::XMMatrixDeterminant(view);
    XMMATRIX invView = DirectX::XMMatrixInverse(&viewDet, view);

    XMVECTOR projDet = DirectX::XMMatrixDeterminant(proj);
    XMMATRIX invProj = DirectX::XMMatrixInverse(&projDet, proj);

    XMMATRIX viewProj = DirectX::XMMatrixMultiply(view, proj);
    XMVECTOR viewProjDet = DirectX::XMMatrixDeterminant(viewProj);
    XMMATRIX invViewProj = DirectX::XMMatrixInverse(&viewProjDet, viewProj);

    DirectX::XMStoreFloat4x4(&m_passConstants.View, DirectX::XMMatrixTranspose(view));
    DirectX::XMStoreFloat4x4(&m_passConstants.InvView, DirectX::XMMatrixTranspose(invView));
    DirectX::XMStoreFloat4x4(&m_passConstants.Proj, DirectX::XMMatrixTranspose(proj));
    DirectX::XMStoreFloat4x4(&m_passConstants.InvProj, DirectX::XMMatrixTranspose(invProj));
    DirectX::XMStoreFloat4x4(&m_passConstants.ViewProj, DirectX::XMMatrixTranspose(viewProj));
    DirectX::XMStoreFloat4x4(&m_passConstants.InvViewProj, DirectX::XMMatrixTranspose(invViewProj));
    m_passConstants.EyePosW = m_camera->Position();
    float width = m_deviceResources->GetRenderTargetSize().Width;
    float height = m_deviceResources->GetRenderTargetSize().Height;
    m_passConstants.RenderTargetSize = XMFLOAT2(width, height);
    m_passConstants.InvRenderTargetSize = XMFLOAT2(1.0f / width, 1.0f / height);
    m_passConstants.NearZ = 1.0f;
    m_passConstants.FarZ = 1000.0f;
    m_passConstants.TotalTime = static_cast<float>(timer.GetTotalSeconds());
    m_passConstants.DeltaTime = static_cast<float>(timer.GetElapsedSeconds());
    m_passConstants.AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };

    XMVECTOR lightDir = -MathHelper::SphericalToCartesian(1.0f, 1.25f * XM_PI, XM_PIDIV4);
    DirectX::XMStoreFloat3(&m_passConstants.Lights[0].Direction, lightDir);
    m_passConstants.Lights[0].Strength = { 1.0f, 1.0f, 0.9f };

    // Update the constant buffer with the new matrix values
    // VS
    D3D11_MAPPED_SUBRESOURCE ms;
    ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));

    winrt::check_hresult(
        context->Map(m_vsPerPassConstantsBuffers[0]->GetRawBufferPointer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)
    );
    memcpy(ms.pData, &m_passConstants, sizeof(PassConstants));
    // TODO: Wrap next line in THROW_INFO_ONLY macro
    context->Unmap(m_vsPerPassConstantsBuffers[0]->GetRawBufferPointer(), 0);

    // PS
    ZeroMemory(&ms, sizeof(D3D11_MAPPED_SUBRESOURCE));
    winrt::check_hresult(
        context->Map(m_psPerPassConstantsBuffers[0]->GetRawBufferPointer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &ms)
    );
    memcpy(ms.pData, &m_passConstants, sizeof(PassConstants));
    // TODO: Wrap next line in THROW_INFO_ONLY macro
    context->Unmap(m_psPerPassConstantsBuffers[0]->GetRawBufferPointer(), 0);

    for (auto& configAndObjectList : m_configsAndObjectLists)
    {
        // Iterate over vector of MeshSet & ObjectList tuple
        for (auto& meshSetAndObjectList : std::get<1>(configAndObjectList))
        {
            // List of RenderObjects
            std::vector<std::unique_ptr<RenderableBase>>& objectLists = std::get<1>(meshSetAndObjectList);
            for (unsigned int iii = 0; iii < objectLists.size(); ++iii)
            {
                objectLists[iii]->Update(timer);
            }
        }
    }
}

void Renderer::Render()
{
    auto context = m_deviceResources->GetD3DDeviceContext();

    ID3D11RenderTargetView* const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };
    context->OMSetRenderTargets(1u, targets, m_deviceResources->GetDepthStencilView());

    float background[4] = { 0.16f, 0.16f, 0.16f, 1.0f };
    context->ClearRenderTargetView(m_deviceResources->GetBackBufferRenderTargetView(), background);
    context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // Set the viewport
    // TODO: Wrap this in THROW_INFO_ONLY macro
    context->RSSetViewports(1, &m_viewport);

    for (auto& configAndObjectList : m_configsAndObjectLists)
    {
        // Pipeline config
        std::get<0>(configAndObjectList)->ApplyConfig();

        // Iterate over vector of MeshSet & ObjectList tuple
        for (auto& meshSetAndObjectList : std::get<1>(configAndObjectList))
        {
            // MeshSet
            std::get<0>(meshSetAndObjectList)->BindToIA();

            // List of RenderObjects
            std::vector<std::unique_ptr<RenderableBase>>& objectLists = std::get<1>(meshSetAndObjectList);
            for (unsigned int iii = 0; iii < objectLists.size(); ++iii)
            {
                objectLists[iii]->Render();
            }
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