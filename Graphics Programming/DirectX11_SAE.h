#pragma once

#include "resource.h"
//Headers//
#include <windows.h>
#include <d3d11.h>
#include <D3DX11.h>
#include <D3DX10.h>
#include <xnamath.h>
#include <dinput.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

//variables/constants//
LPCTSTR WndClassName = L"DXwin11";
HWND hwnd = NULL;

const int Width = 1024;
const int Height = 720;

// DirectX11

ID3D11Device* d3dev;
ID3D11DeviceContext* d3devcon;
IDXGISwapChain* swapChain;
ID3D11RenderTargetView* renderTargetView;
ID3D11DepthStencilView* depthStencilView;
ID3D11Texture2D* depthStencilBuffer;


// DirectX Vars
// Buffers
ID3D11Buffer* VertexBuffer;
ID3D11Buffer* SkyboxBuffer;
ID3D11Buffer* IndexBuffer;
ID3D11Buffer* SkyboxIndexBuffer;
ID3D10Blob* MVS_Buffer;
ID3D10Blob* GVS_Buffer;
ID3D10Blob* RVS_Buffer;
ID3D10Blob* MPS_Buffer;
ID3D10Blob* GPS_Buffer;
ID3D10Blob* RPS_Buffer;

// Vertex und Pixel Shader
ID3D11VertexShader* MVS;
ID3D11VertexShader* GVS;
ID3D11VertexShader* RVS;
ID3D11PixelShader* MPS;
ID3D11PixelShader* GPS;
ID3D11PixelShader* RPS;

// Layout
ID3D11InputLayout* VertexLayout;

// Texturen
ID3D11ShaderResourceView* MatteCubeTexture;
ID3D11ShaderResourceView* GlossyCubeTexture;
ID3D11ShaderResourceView* RoughCubeTexture;
ID3D11ShaderResourceView* RoughCubeNormal;
ID3D11ShaderResourceView* SkyboxCubeTexture;
ID3D11SamplerState* CubeTextureSamplerState;
ID3D11SamplerState* CubeNormalSamplerState;

// Objektansicht
ID3D11RasterizerState* Solid;

// Constant Buffers
ID3D11Buffer* cbPerObjectBuffer;
ID3D11Buffer* cbPerObjectBufferShiny;
ID3D11Buffer* cbPerObjectBufferRough;
ID3D11Buffer* cbPerFrameBuffer;

// Matrizen
XMMATRIX WVP;
XMMATRIX Matte_Cube;
XMMATRIX Glossy_Cube;
XMMATRIX Rough_Cube;
XMMATRIX Skybox;
XMMATRIX View;
XMMATRIX Projection;

// Kamera
XMVECTOR camPos;
XMVECTOR camTarget;
XMVECTOR camUp;
XMMATRIX camRot;

// Transform Komponenten
XMMATRIX Translate;
XMMATRIX Scale;
XMMATRIX RotY;

void StartTimer();
double GetTime();
double GetFrameTime();

//DX11 Functions//
bool InitDirect3DApp(HINSTANCE hInstance);
void ReleaseObjects();
bool Init();
void Update(double deltaTime);
void Draw(double deltaTime);
bool InitializeWindow(HINSTANCE hInstance,
	int ShowWnd,
	int width, int height,
	bool windowed);

int messageloop();

LRESULT CALLBACK WndProc(HWND hWnd,    //Windows callback procedure
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);