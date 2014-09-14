#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include <Windows.h>
#include <sal.h>
#include <mmsystem.h>
#include <rpcsal.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "sys.hpp"
#include "sound.hpp"
#include "roadtohell.shader.h"
#include "soundtrack.shader.h"

#define WINWIDTH 1280
#define WINHEIGHT 720
#define WINPOSX 200
#define WINPOSY 200

// from iq source code
static const int wav[11] = {
  0x46464952,
  MZK_NUMSAMPLES*2+36,
  0x45564157,
  0x20746D66,
  16,
  WAVE_FORMAT_PCM|(MZK_NUMCHANNELS<<16),
  MZK_RATE,
  MZK_RATE*MZK_NUMCHANNELS*sizeof(short),
  (MZK_NUMCHANNELS*sizeof(short))|((8*sizeof(short))<<16),
  0x61746164,
  MZK_NUMSAMPLES*sizeof(short)
};
static int music[11+MZK_NUMSAMPLES];
#define PI 3.14159265359f

struct MainConstantBuffer {
  float iResolution[2];
  float iGlobalTime;
};

static DXGI_SWAP_CHAIN_DESC swapChainDesc = {
  {
    WINWIDTH, WINHEIGHT, {60, 1},  DXGI_FORMAT_R8G8B8A8_UNORM,
    DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED, DXGI_MODE_SCALING_UNSPECIFIED
  },
  {1, 0},
  DXGI_USAGE_RENDER_TARGET_OUTPUT, 1, NULL, TRUE, DXGI_SWAP_EFFECT_SEQUENTIAL, 0
};

static const D3D11_BUFFER_DESC constantBufferDesc = {
  ((sizeof(MainConstantBuffer)+15) / 16) * 16,
  D3D11_USAGE_DYNAMIC,
  D3D11_BIND_CONSTANT_BUFFER,
  D3D11_CPU_ACCESS_WRITE,
  0,
  0
};

static const D3D11_BUFFER_DESC soundTrackBufferDesc = {
  MZK_NUMSAMPLES*sizeof(int),
  D3D11_USAGE_DEFAULT,
  D3D11_BIND_UNORDERED_ACCESS|D3D11_BIND_SHADER_RESOURCE,
  0,
  D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
  sizeof(int)
};

static const D3D11_BUFFER_DESC stagingBufferDesc = {
  MZK_NUMSAMPLES*sizeof(int),
  D3D11_USAGE_STAGING,
  0,
  D3D11_CPU_ACCESS_READ,
  0,
  sizeof(int)
};

INLINE void GetDesktopResolution(int &w, int &h) {
  RECT desktop;
  const auto hDesktop = GetDesktopWindow();
  GetWindowRect(hDesktop, &desktop);
  w = desktop.right;
  h = desktop.bottom;
}

static ID3D11ComputeShader *CreateShader(ID3D11Device *device, const char *source,
                                         size_t sz, const char *entry)
{
  ID3D11ComputeShader *computeShader;
  ID3DBlob *blob = NULL, *errmsg = NULL;
  const auto hr = D3DCompile(source, sz, NULL, NULL, NULL, entry,
                             "cs_5_0", 0, 0, &blob, &errmsg);
#if defined(_DEBUG)
  if (hr != S_OK) {
    MessageBoxA(NULL, (LPCSTR) errmsg->GetBufferPointer(), "Error", MB_OK | MB_ICONERROR);
    MessageBoxA(NULL, "CreateComputeShader() failed", "Error", MB_OK | MB_ICONERROR);
  }
#endif
  D3DRUN(device->CreateComputeShader(blob->GetBufferPointer(),
                                     blob->GetBufferSize(), NULL,
                                     &computeShader));
  return computeShader;
}

INLINE void MakeSoundTrack(ID3D11Device *device, ID3D11DeviceContext *immCtx) {
  ID3D11Buffer *soundTrackBuffer, *stagingBuffer;
  ID3D11UnorderedAccessView *unorderedAccessView;
  ID3D11ComputeShader *computeShader;
  computeShader = CreateShader(device, soundtrack_shader_h, sizeof(soundtrack_shader_h), "main");
  D3DRUN(device->CreateBuffer(&soundTrackBufferDesc, NULL, &soundTrackBuffer));
  D3DRUN(device->CreateBuffer(&stagingBufferDesc, NULL, &stagingBuffer));

  // Create staging buffer
  // This is used to read the results back to the CPU
  D3D11_BUFFER_DESC stagingBufferDesc = {0};
  stagingBufferDesc.ByteWidth = MZK_NUMSAMPLES * sizeof(int);
  stagingBufferDesc.Usage = D3D11_USAGE_STAGING;
  stagingBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
  stagingBufferDesc.StructureByteStride = sizeof(int);
  D3DRUN(device->CreateBuffer(&stagingBufferDesc, nullptr, &stagingBuffer));
  D3DRUN(device->CreateUnorderedAccessView((ID3D11Resource*)soundTrackBuffer, NULL, &unorderedAccessView));

  // Run the sound shader
  immCtx->CSSetShader(computeShader, NULL, 0);
  immCtx->CSSetUnorderedAccessViews(0, 1, &unorderedAccessView, NULL);
  immCtx->Dispatch((MZK_NUMSAMPLES+1023)/1024, 1, 1);

  // Download the data
  D3D11_MAPPED_SUBRESOURCE mappedResource = {0};
  immCtx->CopyResource(stagingBuffer, soundTrackBuffer);
  D3DRUN(immCtx->Map(stagingBuffer, 0, D3D11_MAP_READ, 0, &mappedResource));
  auto const src = (const int*)mappedResource.pData;
  for (int i = 0; i < 11; ++i) music[i] = wav[i];
  for (int i = 0; i < MZK_NUMSAMPLES; ++i) music[11+i] = src[i];
#if defined(WELLBEHAVIOUR)
  immCtx->Unmap((ID3D11Resource*)stagingBuffer, 0);
  stagingBuffer->Release();
  soundTrackBuffer->Release();
  unorderedAccessView->Release();
  computeShader->Release();
#endif
}

#if !defined(NAKED_ENTRY)
// this is the main windows entry point ...
int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/) {
#else
// Take away prolog and epilog, then put a minial prolog back manually with
// assembly below. The function never returns so no epilog is necessary.
__declspec(naked)  void __cdecl winmain() {
  // Prolog
  //__asm enter 0x10, 0;
  __asm {
    push ebp
    mov ebp, esp
    sub esp,__LOCAL_SIZE
  }

  // Extra scope to make compiler accept the __decalspec(naked) with local variables
  {
#endif
    // D3D 11 device variables
    ID3D11Device *device;
    IDXGISwapChain *swapChain;
    ID3D11DeviceContext *immCtx;
    ID3D11Buffer *constantBuffer;
    ID3D11UnorderedAccessView *unorderedAccessView;
    ID3D11ComputeShader *computeShader;

    // timer global variables
    DWORD StartTime;

    // keep track if the game loop is still running
    BOOL BStartRunning = FALSE;
    // int w = 800, h = 600;
    // GetDesktopResolution(&w, &h);

    // the most simple window
    const auto hWnd = CreateWindow(L"edit", 0, WS_CAPTION | WS_POPUP |
      WS_VISIBLE, WINPOSX, WINPOSY, WINWIDTH, WINHEIGHT, 0, 0, 0, 0);
    // SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    // don't show the cursor
    // ShowCursor(FALSE);

    swapChainDesc.BufferUsage =
      DXGI_USAGE_RENDER_TARGET_OUTPUT |
      DXGI_USAGE_UNORDERED_ACCESS |
      DXGI_USAGE_SHADER_INPUT;
    swapChainDesc.OutputWindow = hWnd;

    D3DRUN(D3D11CreateDeviceAndSwapChain(
      NULL,
      D3D_DRIVER_TYPE_HARDWARE,
      NULL,
      D3D11_CREATE_DEVICE_DEBUG,
      NULL,
      0,
      D3D11_SDK_VERSION,
      &swapChainDesc,
      &swapChain,
      &device,
      NULL,
      &immCtx));

    // Load the source track
    MakeSoundTrack(device, immCtx);

    // get access to the back buffer via a texture
    ID3D11Texture2D* texture;
    D3DRUN(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&texture));

    // Create constant buffer
    D3DRUN(device->CreateBuffer(&constantBufferDesc, NULL, &constantBuffer));

    // create shader unordered access view on back buffer for compute shader to write into texture
    D3DRUN(device->CreateUnorderedAccessView((ID3D11Resource*)texture, NULL, &unorderedAccessView));

    // load compute shader
    computeShader = CreateShader(device, roadtohell_shader_h, sizeof(roadtohell_shader_h), "main");

    // setup timer
    StartTime = GetTickCount();

    // set the game loop to running by default
#if defined(WELLBEHAVIOUR)
    MSG msg;
#endif
    sndPlaySound(LPCWSTR(&wav), SND_ASYNC|SND_MEMORY);

    while (!BStartRunning) {
#if defined(WELLBEHAVIOUR)
      // Just remove the message
      PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE);
#endif

      // go out of game loop and shutdown
      if (GetAsyncKeyState(VK_ESCAPE))
        BStartRunning = TRUE;

      // Fill constant buffer
      D3D11_MAPPED_SUBRESOURCE msr;
      immCtx->Map((ID3D11Resource*) constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0,  &msr);

      const auto mc = (MainConstantBuffer*) msr.pData;
      mc->iResolution[0] = float(WINWIDTH);
      mc->iResolution[1] = float(WINHEIGHT);
      mc->iGlobalTime = (GetTickCount() - StartTime) / 1000.0f;
      immCtx->Unmap((ID3D11Resource*) constantBuffer,0);
      immCtx->CSSetShader(computeShader, NULL, 0);
      immCtx->CSSetUnorderedAccessViews(0, 1, &unorderedAccessView, NULL);
      immCtx->CSSetConstantBuffers(0, 1, &constantBuffer);
      immCtx->Dispatch((WINWIDTH + 7) / 8, (WINHEIGHT + 7) / 8, 1);
      swapChain->Present(0, 0);
    }
    sndPlaySound(NULL, SND_ASYNC|SND_MEMORY);

    // release all D3D device related resources
#if defined(WELLBEHAVIOUR)
    immCtx->ClearState();
    device->Release();
    swapChain->Release();
    texture->Release();
    constantBuffer->Release();
    unorderedAccessView->Release();
#endif

#if !defined(NAKED_ENTRY)
    ExitProcess(0);
    return 0;
#else
  }

  ExitProcess(0);
#endif
}

