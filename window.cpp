////////////////////////////////////////////////////////////////////////
//
// Port of Jan Vlietnick's Julia 4D demo 
//
// by Wolfgang Engel 
//
// Last time modified: 12/31/2013 
//
///////////////////////////////////////////////////////////////////////
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include <Windows.h>
#include <sal.h>
#include <mmsystem.h>
#include <rpcsal.h>

//#define WELLBEHAVIOUR
//#define NAKED_ENTRY

#include <d3d11.h>
#include <d3dcompiler.h>
#include "qjulia.shader.h"

// define the size of the window
#define WINWIDTH 1280 
#define WINHEIGHT 720
#define WINPOSX 200 
#define WINPOSY 200

// from iq source code
#define MZK_DURATION    22
#define MZK_RATE        44100
#define MZK_NUMCHANNELS 2
#define MZK_NUMSAMPLES  (MZK_DURATION*MZK_RATE)
#define MZK_NUMSAMPLESC (MZK_NUMSAMPLES*MZK_NUMCHANNELS)
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

// These values are not magical, just the default values Marsaglia used.
// Any pair of unsigned integers should be fine.
static unsigned int m_w = 521288629;
//static unsigned int m_z = 362436069;
#define MZ ((36969 * (362436069 & 65535) + (362436069 >> 16)) << 16)

static void __forceinline SetSeed(unsigned int u)
{
  m_w = u;
}

// This is the heart of the generator.
// It uses George Marsaglia's MWC algorithm to produce an unsigned integer.
// See http://www.bobwheeler.com/statistics/Password/MarsagliaPost.txt
static __forceinline unsigned int GetUint()
{
  //	m_z = 36969 * (m_z & 65535) + (m_z >> 16);
  m_w = 18000 * (m_w & 65535) + (m_w >> 16);
  return (MZ) + m_w;
}

// Produce a uniform random sample from the interval (-1, 1).
// The method will not return either end point.
static __forceinline float GetUniform()
{
  // 0 <= u < 2^32
  unsigned int u = GetUint();
  // The magic number below is 1/(2^32 + 2).
  // The result is strictly between 0 and 1.
  return (u) * (float) 2.328306435454494e-10 * 2.0f;
}


static __forceinline void
Interpolate( float m[4], float t, float a[4], float b[4] )
{
  int i;
  for ( i = 0; i < 4; i++ )
    m[ i ] = ( 1.0f - t ) * a[ i ] + t * b[ i ];
}


float dt; // time increment depending on frame rendering time for same animation speed independent of rendering speed

static __forceinline void
UpdateMu( float t[4], float a[4], float b[4] )
{
  *t += 0.01f *dt;

  if ( *t >= 1.0f )
  {
    *t = 0.0f;

    a[ 0 ] = b[ 0 ];
    a[ 1 ] = b[ 1 ];
    a[ 2 ] = b[ 2 ];
    a[ 3 ] = b[ 3 ];

    b[ 0 ] = GetUniform();
    b[ 1 ] = GetUniform();
    b[ 2 ] = GetUniform();
    b[ 3 ] = GetUniform();
  }
}

static __forceinline void
RandomColor( float v[4] )
{
  do
  {
    v[ 0 ] = GetUniform();
    v[ 1 ] = GetUniform();
    v[ 2 ] = GetUniform();
  }
  while (v[0] < 0 && v[1] <0 && v[2]<0); // prevent black colors
  v[ 3 ] = 1.0f;
}

static __forceinline void
UpdateColor( float t[4], float a[4], float b[4] )
{
  *t += 0.01f *dt;

  if ( *t >= 1.0f )
  {
    *t = 0.0f;

    a[ 0 ] = b[ 0 ];
    a[ 1 ] = b[ 1 ];
    a[ 2 ] = b[ 2 ];
    a[ 3 ] = b[ 3 ];

    RandomColor(b);
  }
}

static __forceinline void GetDesktopResolution(int *horizontal, int *vertical)
{
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	*horizontal = desktop.right;
	*vertical = desktop.bottom;
}
#pragma function(memcpy)
#pragma function(memset)
__forceinline void* __cdecl memset(void* _Dst, _In_ int _Val, size_t _Size) {
	//slow, but probably small
	char *dst = (char*)_Dst;

	for (size_t x = 0; x < _Size; x++)
		*dst++ = _Val;

	return _Dst;
}

void* __cdecl memcpy(void* _Dst, const void* _Src, size_t _Size) {
	//slow, but probably small
	char *dst = (char*)_Dst;
	char *src = (char*)_Src;

	for (size_t x = 0; x < _Size; x++)
		*dst++ = *src++;

	return _Dst;
}
#if !defined(NAKED_ENTRY)
// this is the main windows entry point ... 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#else
  // Take away prolog and epilog, then put a minial prolog back manually with
  // assembly below. The function never returns so no epilog is necessary.
__declspec( naked )  void __cdecl winmain()

  {
    // Prolog
    //__asm enter 0x10, 0;
    __asm 
    {
      push ebp
      mov ebp, esp
      sub esp,__LOCAL_SIZE
    }

    { // Extra scope to make compiler accept the __decalspec(naked) with local variables

#endif
      // D3D 11 device variables
      ID3D11Device *pd3dDevice;
      IDXGISwapChain *pSwapChain;
      ID3D11DeviceContext *pImmediateContext;
      ID3D11Buffer* pcbFractal;
      ID3D11UnorderedAccessView*pComputeOutput;

      static D3D11_BUFFER_DESC Desc;

      static float Epsilon                    = 0.003f;
      static float ColorT                     = 0.0f;
      static float ColorA[4]                  = { 0.25f, 0.45f, 1.0f, 1.0f };
      static float ColorB[4]                  = { 0.25f, 0.45f, 1.0f, 1.0f };
      static float ColorC[4]                  = { 0.25f, 0.45f, 1.0f, 1.0f };

      static float MuT                        = 0.0f;
      static float MuA[4]                     = { -.278f, -.479f, 0.0f, 0.0f };
      static float MuB[4]                     = { 0.278f, 0.479f, 0.0f, 0.0f };
      static float MuC[4]                     = { -.278f, -.479f, -.231f, .235f };

      BOOL selfShadow = TRUE;
      float zoom = 1.0f;

      // timer global variables
      DWORD		StartTime;
      static DWORD CurrentTime;

      // keep track if the game loop is still running
      static BOOL BStartRunning;
      int w = 800, h = 600;

      //GetDesktopResolution(&w, &h);

      // the most simple window
      HWND hWnd = CreateWindow(L"edit", 0, WS_CAPTION | WS_POPUP | WS_VISIBLE, WINPOSX, WINPOSY, WINWIDTH, WINHEIGHT, 0, 0, 0, 0);
//      SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	  // don't show the cursor
      // ShowCursor(FALSE);

      const static DXGI_SWAP_CHAIN_DESC sd = {
        {WINWIDTH, WINHEIGHT, {60, 1},  DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED, DXGI_MODE_SCALING_UNSPECIFIED },
        {1, 0},
        DXGI_USAGE_RENDER_TARGET_OUTPUT, 1, NULL, TRUE, DXGI_SWAP_EFFECT_SEQUENTIAL, 0};

      //
      DXGI_SWAP_CHAIN_DESC temp;
      temp = sd;
      temp.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS | DXGI_USAGE_SHADER_INPUT;
      temp.OutputWindow = hWnd;

      D3D11CreateDeviceAndSwapChain(
          NULL,
          D3D_DRIVER_TYPE_HARDWARE,
          NULL, 
          D3D11_CREATE_DEVICE_DEBUG,
          NULL,
          0,
          D3D11_SDK_VERSION,
          &temp,
          &pSwapChain,
          &pd3dDevice,
          NULL,
          &pImmediateContext);


      DXGI_SWAP_CHAIN_DESC sdtemp;
      pSwapChain->GetDesc(&sdtemp);

      // get access to the back buffer via a texture
      ID3D11Texture2D* pTexture;
      pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pTexture);
      //
      // Create constant buffer
      //
      typedef struct
      {
        float diffuse[4]; // diffuse shading color
        float mu[4];    // quaternion julia parameter
        float epsilon;  // detail julia
        int c_width;      // view port size
        int c_height;
        int selfShadow;  // selfshadowing on or off 
        float orientation[4*4]; // rotation matrix
        float zoom;
      } MainConstantBuffer;

      Desc.Usage = D3D11_USAGE_DYNAMIC;
      Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
      Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
      Desc.ByteWidth = ((sizeof(MainConstantBuffer)+15) / 16) * 16; // must be multiple of 16 bytes
      pd3dDevice->CreateBuffer(&Desc, NULL, &pcbFractal);

      // create shader unordered access view on back buffer for compute shader to write into texture
      pd3dDevice->CreateUnorderedAccessView((ID3D11Resource*)pTexture, NULL, &pComputeOutput );

      //
      // compile a compute shader
      //
      ID3D11ComputeShader *pCompiledComputeShader;
      ID3DBlob *blob = NULL, *errmsg = NULL;
      HRESULT hr = D3DCompile(qjulia, sizeof(qjulia), NULL, NULL, NULL, "main", "cs_5_0", 0, 0, &blob, &errmsg);
#if defined(_DEBUG)
      if (hr != S_OK)
      {
       // MessageBoxA(NULL, errmsg->GetBufferPointer(), "Error", MB_OK | MB_ICONERROR);
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

      // seed the random number generator
      SetSeed((unsigned int)GetCurrentTime());

      // set the game loop to running by default
      MSG msg;
      musicInit(music);
      memcpy(music, wavHeader, sizeof(wavHeader));
      sndPlaySound(LPCWSTR(&music), SND_ASYNC|SND_MEMORY);

      while (!BStartRunning)
      {
#if defined(WELLBEHAVIOUR)
        // Just remove the message
        PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE);
#endif
        // Calculate the current demo time
        CurrentTime = GetTickCount() - StartTime;

        // go out of game loop and shutdown
        if (CurrentTime > 30000 || GetAsyncKeyState(VK_ESCAPE)) 
          BStartRunning = TRUE;

        dt = CurrentTime / (1000.0f * 20.0f);

        UpdateMu( &MuT, MuA, MuB );
        Interpolate( MuC, MuT, MuA, MuB );

        UpdateColor( &ColorT, ColorA, ColorB );
        Interpolate(ColorC, ColorT, ColorA, ColorB );

        // Fill constant buffer
        D3D11_MAPPED_SUBRESOURCE msr;
        pImmediateContext->Map((ID3D11Resource *)pcbFractal, 0, D3D11_MAP_WRITE_DISCARD, 0,  &msr);

        static MainConstantBuffer* mc;
        mc = (MainConstantBuffer*) msr.pData;

        mc->c_height = (int)WINHEIGHT;
        mc->c_width  = (int)WINWIDTH;
        mc->diffuse[0] = ColorC[0];
        mc->diffuse[1] = ColorC[1];
        mc->diffuse[2] = ColorC[2];
        mc->diffuse[3] = ColorC[3];
        mc->epsilon = Epsilon;
        mc->mu[0] = MuC[0];
        mc->mu[1] = MuC[1];
        mc->mu[2] = MuC[2];
        mc->mu[3] = MuC[3];
        mc->orientation[0] = 1.0;
        //		mc->orientation[1] = 0.0;
        //		mc->orientation[2] = 0.0;
        //		mc->orientation[3] = 0.0;
        //		mc->orientation[4] = 0.0;
        mc->orientation[5] = 1.0;
        //		mc->orientation[6] = 0.0;
        //		mc->orientation[7] = 0.0;
        //		mc->orientation[8] = 0.0;
        //		mc->orientation[9] = 0.0;
        mc->orientation[10] = 1.0;
        //		mc->orientation[11] = 0.0;
        //		mc->orientation[12] = 0.0;
        //		mc->orientation[13] = 0.0;
        //		mc->orientation[14] = 0.0;
        mc->orientation[15] = 1.0;

        mc->selfShadow = selfShadow;
        mc->zoom = zoom;

        pImmediateContext->Unmap((ID3D11Resource *)pcbFractal,0);

        // Set compute shader
        pImmediateContext->CSSetShader(pCompiledComputeShader, NULL, 0 );

        // For CS output
        pImmediateContext->CSSetUnorderedAccessViews(0, 1, &pComputeOutput, NULL );

        // For CS constant buffer
        pImmediateContext->CSSetConstantBuffers(0, 1, &pcbFractal );

        // Run the CS
        pImmediateContext->Dispatch((WINWIDTH + 3) / 4, (WINHEIGHT + 63) / 64, 1 );

        // make it visible
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
	  return 0;// (int)msg.wParam;
#else
    }

    ExitProcess(0);
#endif
  }
