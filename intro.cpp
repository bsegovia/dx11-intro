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

#define WINWIDTH 1280
#define WINHEIGHT 720
#define WINPOSX 200
#define WINPOSY 200

// from iq source code
static const int wavHeader[11] = {
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
static short music[MZK_NUMSAMPLESC+22];
#define PI 3.14159265359f

static float wave(float x) {
  auto const r = int(x/PI);
  return r % 2 ? 1.f : -1.f;
}

static void musicInit(short *buffer) {
  for (int i=0; i<MZK_NUMSAMPLES; i++) {
    const auto fl = wave(6.2831f*440.0f * (float)i/(float)MZK_RATE);
    const auto fr = wave(6.2831f*587.3f * (float)i/(float)MZK_RATE);
    buffer[2*i+0] = int(fl*32767.0f);
    buffer[2*i+1] = int(fr*32767.0f);
  }
}

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

static const D3D11_BUFFER_DESC Desc = {
  ((sizeof(MainConstantBuffer)+15) / 16) * 16,
  D3D11_USAGE_DYNAMIC,
  D3D11_BIND_CONSTANT_BUFFER,
  D3D11_CPU_ACCESS_WRITE,
  0,
  0
};

INLINE void GetDesktopResolution(int *horizontal, int *vertical) {
  RECT desktop;
  const HWND hDesktop = GetDesktopWindow();
  GetWindowRect(hDesktop, &desktop);
  *horizontal = desktop.right;
  *vertical = desktop.bottom;
}

#if !defined(NAKED_ENTRY)
// this is the main windows entry point ...
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
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
    ID3D11Device *pd3dDevice;
    IDXGISwapChain *pSwapChain;
    ID3D11DeviceContext *pImmediateContext;
    ID3D11Buffer *pcbFractal;
    ID3D11UnorderedAccessView *pComputeOutput;

    // timer global variables
    DWORD StartTime;

    // keep track if the game loop is still running
    BOOL BStartRunning = FALSE;
    // int w = 800, h = 600;
    // GetDesktopResolution(&w, &h);

    // the most simple window
    HWND hWnd = CreateWindow(L"edit", 0, WS_CAPTION | WS_POPUP | WS_VISIBLE, WINPOSX, WINPOSY, WINWIDTH, WINHEIGHT, 0, 0, 0, 0);
    // SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    // don't show the cursor
    // ShowCursor(FALSE);

    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS | DXGI_USAGE_SHADER_INPUT;
    swapChainDesc.OutputWindow = hWnd;

    D3D11CreateDeviceAndSwapChain(
      NULL,
      D3D_DRIVER_TYPE_HARDWARE,
      NULL,
      D3D11_CREATE_DEVICE_DEBUG,
      NULL,
      0,
      D3D11_SDK_VERSION,
      &swapChainDesc,
      &pSwapChain,
      &pd3dDevice,
      NULL,
      &pImmediateContext);

    // get access to the back buffer via a texture
    ID3D11Texture2D* pTexture;
    pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pTexture);

    // Create constant buffer
    pd3dDevice->CreateBuffer(&Desc, NULL, &pcbFractal);

    // create shader unordered access view on back buffer for compute shader to write into texture
    pd3dDevice->CreateUnorderedAccessView((ID3D11Resource*)pTexture, NULL, &pComputeOutput );

    // compile a compute shader
    ID3D11ComputeShader *pCompiledComputeShader;
    ID3DBlob *blob = NULL, *errmsg = NULL;
    HRESULT hr = D3DCompile(roadtohell_shader_h, sizeof(roadtohell_shader_h), NULL, NULL, NULL, "main", "cs_5_0", 0, 0, &blob, &errmsg);
#if defined(_DEBUG)
    if (hr != S_OK) {
      MessageBoxA(NULL, (LPCSTR) errmsg->GetBufferPointer(), "Error", MB_OK | MB_ICONERROR);
      MessageBoxA(NULL, "CreateComputerShader() failed", "Error", MB_OK | MB_ICONERROR);
    }
#endif
    pd3dDevice->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), NULL, &pCompiledComputeShader);
#if defined(_DEBUG)
    if (hr != S_OK)
      MessageBoxA(NULL, "CreateComputerShader() failed", "Error", MB_OK | MB_ICONERROR);
#endif

    // setup timer
    StartTime = GetTickCount();

    // set the game loop to running by default
#if defined(WELLBEHAVIOUR)
    MSG msg;
#endif
    musicInit(music);
    memcpy(music, wavHeader, sizeof(wavHeader));
    //sndPlaySound(LPCWSTR(&music), SND_ASYNC|SND_MEMORY);

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
      pImmediateContext->Map((ID3D11Resource*) pcbFractal, 0, D3D11_MAP_WRITE_DISCARD, 0,  &msr);

      const auto mc = (MainConstantBuffer*) msr.pData;
      mc->iResolution[0] = float(WINWIDTH);
      mc->iResolution[1] = float(WINHEIGHT);
      mc->iGlobalTime = (GetTickCount() - StartTime) / 1000.0f;
      pImmediateContext->Unmap((ID3D11Resource*) pcbFractal,0);
      pImmediateContext->CSSetShader(pCompiledComputeShader, NULL, 0 );
      pImmediateContext->CSSetUnorderedAccessViews(0, 1, &pComputeOutput, NULL );
      pImmediateContext->CSSetConstantBuffers(0, 1, &pcbFractal );
      pImmediateContext->Dispatch((WINWIDTH + 7) / 8, (WINHEIGHT + 7) / 8, 1 );
      pSwapChain->Present(0, 0 );
    }
    sndPlaySound(NULL, SND_ASYNC|SND_MEMORY);

    // release all D3D device related resources
#if defined(WELLBEHAVIOUR)
    pImmediateContext->ClearState();
    pd3dDevice->Release();
    pSwapChain->Release();	
    pTexture->Release();	
    pcbFractal->Release();
    pComputeOutput->Release();
#endif

#if !defined(NAKED_ENTRY)
    ExitProcess(0);
    return 0;
#else
  }

  ExitProcess(0);
#endif
}

