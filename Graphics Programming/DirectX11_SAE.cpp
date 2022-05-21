#include "DirectX11_SAE.h"

// HRT
double countsPerSecond = 0;
__int64 CounterStart = 0;
int frameCount = 0;
int fps = 0;
__int64 frameTimeOld = 0;
double frameTime;

// Matrix Buffers
struct cbPerObject
{
	XMMATRIX WVP; 
	XMMATRIX World;
};
cbPerObject cbpMatte; // Matrix Buffer für
cbPerObject cbpShiny; // die verschiedenen
cbPerObject cbpRough; // Würfel

struct Light
{
	Light() 
	{
		ZeroMemory(this, sizeof(Light));
	}
	XMFLOAT3 direction;
	float intensity;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
};
Light light;

struct cbPerFrame
{
	Light light;
};
cbPerFrame cbpF;

struct Vertex
{
	Vertex() {}
	Vertex(float x, float y, float z,
		   float u, float v,
		   float nx, float ny, float nz) :
		   pos(x,y,z), texCoord(u,v), normal(nx,ny,nz){}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
	XMFLOAT3 normal;
};

D3D11_INPUT_ELEMENT_DESC layout[] = 
{
	{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
    {"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0},
	{"NORMAL",0,  DXGI_FORMAT_R32G32B32_FLOAT,0,20,D3D11_INPUT_PER_VERTEX_DATA,0},
};
UINT numElements = ARRAYSIZE(layout);

int WINAPI WinMain(HINSTANCE hInstance,    //Main windows function
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd)
{
	//Initialize Window//
	if (!InitializeWindow(hInstance, nShowCmd, Width, Height, true))
	{
	
		MessageBox(0, L"Window Init - Failed",
			L"Error", MB_OK);
		return 0;
	}

	//Initialize Direct3D//
	if (!InitDirect3DApp(hInstance))
	{

		MessageBox(0, L"D3D Init - Failed",
			L"Error", MB_OK);
		return 0;
	}

	//Initialize Direct3D//
	if (!Init())
	{

		MessageBox(0, L" Init - Failed",
			L"Error", MB_OK);
		return 0;
	}

	messageloop();    

	ReleaseObjects();

	return 0;
}

bool InitDirect3DApp(HINSTANCE hInstance)
{
	DXGI_MODE_DESC bufferDESC;
	ZeroMemory(&bufferDESC, sizeof(bufferDESC));

	bufferDESC.Height = Height;
	bufferDESC.Width = Width;
	bufferDESC.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDESC.RefreshRate.Numerator = 60;
	bufferDESC.RefreshRate.Denominator = 1;
	bufferDESC.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	bufferDESC.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	
	DXGI_SWAP_CHAIN_DESC swapChainDESC;
	ZeroMemory(&swapChainDESC, sizeof(swapChainDESC));

	swapChainDESC.BufferCount = 1;
	swapChainDESC.BufferDesc = bufferDESC;
	swapChainDESC.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDESC.Flags = NULL;
	swapChainDESC.OutputWindow = hwnd;
	swapChainDESC.SampleDesc.Count = 1;
	swapChainDESC.SampleDesc.Quality = 0;
	swapChainDESC.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDESC.Windowed = TRUE;

	// --------- Create Swapchain
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, 
		NULL, NULL,
		NULL, NULL, D3D11_SDK_VERSION, 
		&swapChainDESC, &swapChain, &d3dev, NULL, &d3devcon);

	ID3D11Texture2D* BackBuffer;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), 
		                   (void**)&BackBuffer);

	d3dev->CreateRenderTargetView(BackBuffer, NULL, &renderTargetView);
	BackBuffer->Release();

	// -------- DEPTH STENCIL VIEW

	D3D11_TEXTURE2D_DESC depthStencilDESC;

	depthStencilDESC.ArraySize = 1;
	depthStencilDESC.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDESC.CPUAccessFlags = 0;
	depthStencilDESC.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDESC.Height = Height;
	depthStencilDESC.MipLevels = 1;
	depthStencilDESC.MiscFlags = 0;
	depthStencilDESC.SampleDesc.Count = 1;
	depthStencilDESC.SampleDesc.Quality = 0;
	depthStencilDESC.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDESC.Width = Width;

	d3dev->CreateTexture2D(&depthStencilDESC, NULL, &depthStencilBuffer);
	d3dev->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);

	d3devcon->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

	return true;
}

void ReleaseObjects()
{
	swapChain->Release();
	d3dev->Release();
	d3devcon->Release();
	VertexBuffer->Release();
	SkyboxBuffer->Release();
	IndexBuffer->Release();
	MVS->Release();
	GVS->Release();
	RVS->Release();
	MVS_Buffer->Release();
	GVS_Buffer->Release();
	RVS_Buffer->Release();
	MPS->Release();
	GPS->Release();
	RPS->Release();
	MPS_Buffer->Release();
	GPS_Buffer->Release();
	RPS_Buffer->Release();
	VertexLayout->Release();
	depthStencilView->Release();
	depthStencilBuffer->Release();
	cbPerObjectBuffer->Release();
	cbPerObjectBufferShiny->Release();
	cbPerObjectBufferRough->Release();
	cbPerFrameBuffer->Release();
	Solid->Release();
	MatteCubeTexture->Release();
	SkyboxCubeTexture->Release();
	GlossyCubeTexture->Release();
	RoughCubeTexture->Release();
	RoughCubeNormal->Release();
	CubeTextureSamplerState->Release();
	CubeNormalSamplerState->Release();
}

bool Init()
{
	D3DX11CompileFromFile(L"Matte_Shader.hlsl", 0, 0, "VS", "vs_4_0", 0, 0, 0, &MVS_Buffer, 0, 0);
	D3DX11CompileFromFile(L"Matte_Shader.hlsl", 0, 0, "PS", "ps_4_0", 0, 0, 0, &MPS_Buffer, 0, 0);
	d3dev->CreateVertexShader(MVS_Buffer->GetBufferPointer(), MVS_Buffer->GetBufferSize(), NULL, &MVS);
	d3dev->CreatePixelShader(MPS_Buffer->GetBufferPointer(), MPS_Buffer->GetBufferSize(), NULL, &MPS);

	D3DX11CompileFromFile(L"Glossy_Shader.hlsl", 0, 0, "VS", "vs_4_0", 0, 0, 0, &GVS_Buffer, 0, 0);
	D3DX11CompileFromFile(L"Glossy_Shader.hlsl", 0, 0, "PS", "ps_4_0", 0, 0, 0, &GPS_Buffer, 0, 0);
	d3dev->CreateVertexShader(GVS_Buffer->GetBufferPointer(), GVS_Buffer->GetBufferSize(), NULL, &GVS);
	d3dev->CreatePixelShader(GPS_Buffer->GetBufferPointer(), GPS_Buffer->GetBufferSize(), NULL, &GPS);

	D3DX11CompileFromFile(L"Rough_Shader.hlsl", 0, 0, "VS", "vs_4_0", 0, 0, 0, &RVS_Buffer, 0, 0);
	D3DX11CompileFromFile(L"Rough_Shader.hlsl", 0, 0, "PS", "ps_4_0", 0, 0, 0, &RPS_Buffer, 0, 0);
	d3dev->CreateVertexShader(RVS_Buffer->GetBufferPointer(), RVS_Buffer->GetBufferSize(), NULL, &RVS);
	d3dev->CreatePixelShader(RPS_Buffer->GetBufferPointer(), RPS_Buffer->GetBufferSize(), NULL, &RPS);
	
	// Würfel Vertices
	Vertex v[] =
	{
		// Front Face
		Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f,  0, 0, -1.0f),
		Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f,  0, 0, -1.0f),
		Vertex(1.0f,   1.0f, -1.0f, 1.0f, 0.0f,  0, 0, -1.0f),
		Vertex(1.0f,  -1.0f, -1.0f, 1.0f, 1.0f,  0, 0, -1.0f),

		// Back Face
		Vertex(-1.0f, -1.0f, 1.0f, 1.0f, 1.0f,	0, 0, 1.0f),
		Vertex(1.0f, -1.0f, 1.0f, 0.0f, 1.0f,	0, 0, 1.0f),
		Vertex(1.0f,  1.0f, 1.0f, 0.0f, 0.0f,	0, 0, 1.0f),
		Vertex(-1.0f,  1.0f, 1.0f, 1.0f, 0.0f,	0, 0, 1.0f),

		// Top Face
		Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0, 1.0f, 0),
		Vertex(-1.0f, 1.0f,  1.0f, 0.0f, 0.0f, 0, 1.0f, 0),
		Vertex(1.0f, 1.0f,  1.0f, 1.0f, 0.0f,  0, 1.0f, 0),
		Vertex(1.0f, 1.0f, -1.0f, 1.0f, 1.0f,  0, 1.0f, 0),

		// Bottom Face
		Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f,	   0, -1.0f, 0),
		Vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 	   0, -1.0f, 0),
		Vertex(1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 	   0, -1.0f, 0),
		Vertex(-1.0f, -1.0f,  1.0f, 1.0f, 0.0f,	   0, -1.0f, 0),

		// Left Face
		Vertex(-1.0f, -1.0f,  1.0f, 0.0f, 1.0f, -1.0f, 0, 0),
		Vertex(-1.0f,  1.0f,  1.0f, 0.0f, 0.0f, -1.0f, 0, 0),
		Vertex(-1.0f,  1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0, 0),
		Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 0, 0),

		// Right Face
		Vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0, 0),
		Vertex(1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0, 0),
		Vertex(1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 1.0f, 0, 0),
		Vertex(1.0f, -1.0f,  1.0f, 1.0f, 1.0f, 1.0f, 0, 0),
	};

	// Skybox Vertices
	Vertex skybox[] =
	{
		// Front Face
		Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f,  0, 0, -1.0f),
		Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f,  0, 0, -1.0f),
		Vertex(1.0f,   1.0f, -1.0f, 1.0f, 0.0f,  0, 0, -1.0f),
		Vertex(1.0f,  -1.0f, -1.0f, 1.0f, 1.0f,  0, 0, -1.0f),

		// Back Face
		Vertex(-1.0f, -1.0f, 1.0f, 1.0f, 1.0f,	0, 0, -1.0f),
		Vertex(1.0f, -1.0f, 1.0f, 0.0f, 1.0f,	0, 0, -1.0f),
		Vertex(1.0f,  1.0f, 1.0f, 0.0f, 0.0f,	0, 0, -1.0f),
		Vertex(-1.0f,  1.0f, 1.0f, 1.0f, 0.0f,	0, 0, -1.0f),

		// Top Face
		Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0, 0, -1),
		Vertex(-1.0f, 1.0f,  1.0f, 0.0f, 0.0f, 0, 0, -1),
		Vertex(1.0f, 1.0f,  1.0f, 1.0f, 0.0f,  0, 0, -1),
		Vertex(1.0f, 1.0f, -1.0f, 1.0f, 1.0f,  0, 0, -1),

		// Bottom Face
		Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f,	   0, 0, -1),
		Vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 	   0, 0, -1),
		Vertex(1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 	   0, 0, -1),
		Vertex(-1.0f, -1.0f,  1.0f, 1.0f, 0.0f,	   0, 0, -1),

		// Left Face
		Vertex(-1.0f, -1.0f,  1.0f, 0.0f, 1.0f, 0, 0, -1),
		Vertex(-1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 0, 0, -1),
		Vertex(-1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 0, 0, -1),
		Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0, 0, -1),

		// Right Face
		Vertex(1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0, 0, -1),
		Vertex(1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0, 0, -1),
		Vertex(1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0, 0, -1),
		Vertex(1.0f, -1.0f,  1.0f, 1.0f, 1.0f, 0, 0, -1),
	};

	// Würfel Vertices
	DWORD indices[] = {
		// Front Face
		0,  1,  2,
		0,  2,  3,

		// Back Face
		4,  5,  6,
		4,  6,  7,

		// Top Face
		8,  9, 10,
		8, 10, 11,

		// Bottom Face
		12, 13, 14,
		12, 14, 15,

		// Left Face
		16, 17, 18,
		16, 18, 19,

		// Right Face
		20, 21, 22,
		20, 22, 23
	};

	// Skybox Vertices
	DWORD SkyboxIndices[] = {
		// Front Face
		0,  2,  1,
		0,  3,  2,

		// Back Face
		4,  6,  5,
		4,  7,  6,

		// Top Face
		8,  10, 9,
		8, 11, 10,

		// Bottom Face
		12, 14, 13,
		12, 15, 14,

		// Left Face
		16, 18, 17,
		16, 19, 18,

		// Right Face
		20, 22, 21,
		20, 23, 22
	};
	
	// ---------------------- VERTEX BUFFER

	D3D11_BUFFER_DESC vertexBufferDESC;
	ZeroMemory(&vertexBufferDESC, sizeof(vertexBufferDESC));

	vertexBufferDESC.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDESC.ByteWidth = sizeof(Vertex) * 24;
	vertexBufferDESC.CPUAccessFlags = 0;
	vertexBufferDESC.MiscFlags = 0;
	vertexBufferDESC.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = v;
	d3dev->CreateBuffer(&vertexBufferDESC, &vertexBufferData, &VertexBuffer);

	vertexBufferData.pSysMem = skybox;
	d3dev->CreateBuffer(&vertexBufferDESC, &vertexBufferData, &SkyboxBuffer);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	d3devcon->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);

	// --------------------- INDEX BUFFER

	D3D11_BUFFER_DESC indexBufferDESC;
	ZeroMemory(&indexBufferDESC, sizeof(indexBufferDESC));

	indexBufferDESC.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDESC.ByteWidth = sizeof(DWORD) * 3 * 12;
	indexBufferDESC.CPUAccessFlags = 0;
	indexBufferDESC.MiscFlags = 0;
	indexBufferDESC.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA indexBufferData;
	ZeroMemory(&indexBufferData, sizeof(indexBufferData));
	indexBufferData.pSysMem = indices;
	d3dev->CreateBuffer(&indexBufferDESC, &indexBufferData, &IndexBuffer);

	indexBufferData.pSysMem = SkyboxIndices;
	d3dev->CreateBuffer(&indexBufferDESC, &indexBufferData, &SkyboxIndexBuffer);

	d3devcon->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	d3dev->CreateInputLayout(layout, numElements,
		GVS_Buffer->GetBufferPointer(), GVS_Buffer->GetBufferSize(),
		&VertexLayout);

	d3devcon->IASetInputLayout(VertexLayout);
	d3devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(viewport));

	viewport.Height = Height;
	viewport.Width = Width;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;

	d3devcon->RSSetViewports(1, &viewport);

	// Laden der Texturen
	D3DX11CreateShaderResourceViewFromFile(d3dev, L"Glossy_Tex.jpg", NULL, NULL, &GlossyCubeTexture, NULL);
	D3DX11CreateShaderResourceViewFromFile(d3dev, L"Brown_Matte.jpg", NULL, NULL, &MatteCubeTexture, NULL);
	D3DX11CreateShaderResourceViewFromFile(d3dev, L"Normal_Map.tif",  NULL, NULL, &RoughCubeNormal, NULL);
	D3DX11CreateShaderResourceViewFromFile(d3dev, L"Rough_Texture.tif", NULL, NULL, &RoughCubeTexture, NULL);
	D3DX11CreateShaderResourceViewFromFile(d3dev, L"Skybox.jpg", NULL, NULL, &SkyboxCubeTexture, NULL);

	D3D11_SAMPLER_DESC samplerDESC;
	ZeroMemory(&samplerDESC, sizeof(samplerDESC));

	samplerDESC.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDESC.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDESC.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDESC.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDESC.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDESC.MinLOD = 0;
	samplerDESC.MaxLOD = D3D11_FLOAT32_MAX;

	d3dev->CreateSamplerState(&samplerDESC, &CubeTextureSamplerState);
	d3dev->CreateSamplerState(&samplerDESC, &CubeNormalSamplerState);

	// Erstellen der Constant Buffers
	// Für jeden Würfel ( Vertex Shader )

	// Matt Würfel
	D3D11_BUFFER_DESC cbDESC;
	ZeroMemory(&cbDESC, sizeof(cbDESC));

	cbDESC.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDESC.ByteWidth = sizeof(cbPerObject);
	cbDESC.CPUAccessFlags = 0;
	cbDESC.MiscFlags = 0;
	cbDESC.Usage = D3D11_USAGE_DEFAULT;

	d3dev->CreateBuffer(&cbDESC, NULL, &cbPerObjectBuffer);

	// Glänzender Würfel
	D3D11_BUFFER_DESC cbDESC_Shiny;
	ZeroMemory(&cbDESC_Shiny, sizeof(cbDESC_Shiny));

	cbDESC_Shiny.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDESC_Shiny.ByteWidth = sizeof(cbPerObject);
	cbDESC_Shiny.CPUAccessFlags = 0;
	cbDESC_Shiny.MiscFlags = 0;
	cbDESC_Shiny.Usage = D3D11_USAGE_DEFAULT;

	d3dev->CreateBuffer(&cbDESC_Shiny, NULL, &cbPerObjectBufferShiny);

	//Rauer Würfel
	D3D11_BUFFER_DESC cbDESC_Rough;
	ZeroMemory(&cbDESC_Rough, sizeof(cbDESC_Rough));

	cbDESC_Rough.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDESC_Rough.ByteWidth = sizeof(cbPerObject);
	cbDESC_Rough.CPUAccessFlags = 0;
	cbDESC_Rough.MiscFlags = 0;
	cbDESC_Rough.Usage = D3D11_USAGE_DEFAULT;

	d3dev->CreateBuffer(&cbDESC_Rough, NULL, &cbPerObjectBufferRough);
	   
	// Pro Frame ( Pixel Shader )
	ZeroMemory(&cbDESC, sizeof(cbDESC));

	cbDESC.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDESC.ByteWidth = sizeof(cbPerFrame);
	cbDESC.CPUAccessFlags = 0;
	cbDESC.MiscFlags = 0;
	cbDESC.Usage = D3D11_USAGE_DEFAULT;

	d3dev->CreateBuffer(&cbDESC, NULL, &cbPerFrameBuffer);

	// ---Kamera

	camPos = XMVectorSet(0, 0, -7, 0);
	camTarget = XMVectorSet(0, 0, 1, 0);
	camUp = XMVectorSet(0, 1, 0, 0);

	View = XMMatrixLookAtLH(camPos, camTarget, camUp);
	
	Projection = XMMatrixPerspectiveFovLH(0.5f*3.141f, (float)Width / (float)Height, 1, 1000);

	// Rasterizieren

	D3D11_RASTERIZER_DESC sldDESC;
	ZeroMemory(&sldDESC, sizeof(sldDESC));

	sldDESC.FillMode = D3D11_FILL_SOLID;
	sldDESC.CullMode = D3D11_CULL_BACK;
	d3dev->CreateRasterizerState(&sldDESC, &Solid);

	// Licht Einstellungen

	light.direction = XMFLOAT3(0, 0, -1);
	light.ambient =  XMFLOAT4(.25, .25, .25, 1);
	light.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	light.intensity = 2.0f;
	cbpF.light = light;
	return true;
}

void Update(double deltaTime)
{
	// Drehung und Verschiebung der Würfel
	static float angle = 0;
	Matte_Cube = XMMatrixIdentity();
	Glossy_Cube = XMMatrixIdentity();
	Rough_Cube = XMMatrixIdentity();
	Skybox = XMMatrixIdentity();

	angle += 1.0f * deltaTime; if (angle > 6.282) angle = 0;
	
	// Rotation
	RotY = XMMatrixRotationAxis(XMVectorSet(-1, -1, 0, 0), angle);

	// Matt Würfel
	Matte_Cube = RotY;

	// Glänzender Würfel
	Translate = XMMatrixTranslation(-5, 0, 0);
	Glossy_Cube = RotY * Translate;

	// Rauer Würfel
	Translate = XMMatrixTranslation(5, 0, 0);
	Rough_Cube = RotY *  Translate;

	// Skybox
	Translate = XMMatrixTranslation(0, 0, 0);
	Scale = XMMatrixScaling(10, 10, 10);
	Skybox = Scale * Translate;
}

void Draw(double deltaTime)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	D3DXCOLOR backgroundColor(1, 0, 0, 1);
	
	d3devcon->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
	d3devcon->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	d3devcon->ClearRenderTargetView(renderTargetView, backgroundColor);
	d3devcon->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

	//Matt Würfel
	d3devcon->VSSetShader(MVS, 0, 0);
	d3devcon->PSSetShader(MPS, 0, 0);

	WVP = Matte_Cube * View * Projection;
	cbpMatte.WVP = XMMatrixTranspose(WVP);
	cbpMatte.World = XMMatrixTranspose(Glossy_Cube);

	d3devcon->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbpMatte, 0, 0);
	d3devcon->UpdateSubresource(cbPerFrameBuffer, 0, NULL, &cbpF, 0, 0);
	d3devcon->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
	d3devcon->PSSetConstantBuffers(0, 1, &cbPerFrameBuffer);
	d3devcon->RSSetState(Solid);
	d3devcon->PSSetShaderResources(0, 1, &MatteCubeTexture);

	d3devcon->DrawIndexed(36, 0, 0);

	//Glänzender Würfel
	d3devcon->VSSetShader(GVS, 0, 0);
	d3devcon->PSSetShader(GPS, 0, 0);

	WVP = Glossy_Cube * View * Projection;
	cbpMatte.WVP = XMMatrixTranspose(WVP);
	cbpMatte.World = XMMatrixTranspose(Glossy_Cube);
	   
	d3devcon->UpdateSubresource(cbPerObjectBufferShiny, 0, NULL, &cbpMatte, 0, 0);
	d3devcon->UpdateSubresource(cbPerFrameBuffer, 0, NULL, &cbpF, 0, 0);
	d3devcon->VSSetConstantBuffers(0, 1, &cbPerObjectBufferShiny);
	d3devcon->PSSetConstantBuffers(0, 1, &cbPerFrameBuffer);
	d3devcon->RSSetState(Solid);
	d3devcon->PSSetShaderResources(0, 1, &GlossyCubeTexture);
	d3devcon->DrawIndexed(36, 0, 0);
	   	 
	//Rauer Würfel
	d3devcon->VSSetShader(RVS, 0, 0);
	d3devcon->PSSetShader(RPS, 0, 0);

	WVP = Rough_Cube * View * Projection;
	cbpMatte.WVP = XMMatrixTranspose(WVP);
	cbpMatte.World = XMMatrixTranspose(Rough_Cube);
	
	d3devcon->UpdateSubresource(cbPerObjectBufferRough, 0, NULL, &cbpMatte, 0, 0);
	d3devcon->UpdateSubresource(cbPerFrameBuffer, 0, NULL, &cbpF, 0, 0);
	d3devcon->VSSetConstantBuffers(0, 1, &cbPerObjectBufferRough);
	d3devcon->PSSetConstantBuffers(0, 1, &cbPerFrameBuffer);
	d3devcon->RSSetState(Solid);
	d3devcon->PSSetShaderResources(0, 1, &RoughCubeTexture);
	d3devcon->PSSetShaderResources(1, 1, &RoughCubeNormal);
	d3devcon->PSSetSamplers(0, 1, &CubeTextureSamplerState);
	d3devcon->PSSetSamplers(1, 1, &CubeNormalSamplerState);
	d3devcon->DrawIndexed(36, 0, 0);

	//Skybox
	d3devcon->IASetVertexBuffers(0, 1, &SkyboxBuffer, &stride, &offset);
	d3devcon->IASetIndexBuffer(SkyboxIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	d3devcon->VSSetShader(MVS, 0, 0);
	d3devcon->PSSetShader(MPS, 0, 0);

	WVP = Skybox * View * Projection;
	cbpMatte.WVP = XMMatrixTranspose(WVP);
	cbpMatte.World = XMMatrixTranspose(Skybox);

	d3devcon->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbpMatte, 0, 0);
	d3devcon->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
	d3devcon->RSSetState(Solid);
	d3devcon->PSSetShaderResources(0, 1, &SkyboxCubeTexture);
	d3devcon->PSSetSamplers(0, 1, &CubeTextureSamplerState);
	d3devcon->DrawIndexed(36, 0, 0);
	 	  
	swapChain->Present(0, 0);
}
//  Erstellen des Fensters
bool InitializeWindow(HINSTANCE hInstance, 
	int ShowWnd,
	int width, int height,
	bool windowed)
{
	WNDCLASSEX wc;    

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;    
	wc.cbClsExtra = NULL;   
	wc.cbWndExtra = NULL;    
	wc.hInstance = hInstance;   
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);   
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);    
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);  
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);  
	wc.lpszMenuName = NULL;    
	wc.lpszClassName = WndClassName;    
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO); 

	if (!RegisterClassEx(&wc))  
	{
		
		MessageBox(NULL, L"Error registering class",
			L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	hwnd = CreateWindowEx(
		NULL,   
		WndClassName,   
		L"DirectX!",    
		WS_OVERLAPPEDWINDOW,    
		CW_USEDEFAULT, CW_USEDEFAULT,   
		width,   
		height, 
		NULL,   
		NULL,   
		hInstance, 
		NULL   
	);

	if (!hwnd)  
	{
		MessageBox(NULL, L"Error creating window",
			L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	ShowWindow(hwnd, ShowWnd);   
	UpdateWindow(hwnd);   

	return true;  
}

int messageloop() {    

	MSG msg;						
	ZeroMemory(&msg, sizeof(MSG));	

	while (true)					
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)    
				break;    
			TranslateMessage(&msg);  

			DispatchMessage(&msg);
		}
		else 
		{    
			frameCount++;
			if (GetTime()> 1.0f)
			{
				fps = frameCount;
				frameCount = 0;
				StartTimer();
			}
			frameTime = GetFrameTime();
			Update(frameTime);
			Draw(frameTime);
		}
	}

	return (int)msg.wParam;        
}

// Beenden des Programms
LRESULT CALLBACK WndProc(HWND hwnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (msg)    
	{

	case WM_KEYDOWN:  
		   
		if (wParam == VK_ESCAPE) {
			if (MessageBox(0, L"Are you sure you want to exit?",
				L"Really POC?", MB_YESNO | MB_ICONQUESTION) == IDYES)

				
				DestroyWindow(hwnd);
		}
		return 0;

	case WM_DESTROY:   
		PostQuitMessage(0);
		return 0;
	}
	
	return DefWindowProc(hwnd,
		msg,
		wParam,
		lParam);
}

void StartTimer() 
{
	LARGE_INTEGER frequencyCount;
	QueryPerformanceFrequency(&frequencyCount);
	countsPerSecond = double(frequencyCount.QuadPart);
	QueryPerformanceCounter(&frequencyCount);
	CounterStart = frequencyCount.QuadPart;
}
double GetTime() 
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	return double(currentTime.QuadPart - CounterStart) / countsPerSecond;

}
double GetFrameTime()
{
	LARGE_INTEGER currentTime;
	__int64 tickCount;
	QueryPerformanceCounter(&currentTime);

	tickCount = currentTime.QuadPart - frameTimeOld;
	frameTimeOld = currentTime.QuadPart;

	if (tickCount < 0) tickCount = 0;

	return double(tickCount) / countsPerSecond;
}
